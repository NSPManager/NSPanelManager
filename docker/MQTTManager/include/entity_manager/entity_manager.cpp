#include "entity/entity.hpp"
#include "ipc_handler/ipc_handler.hpp"
#include "light/home_assistant_light.hpp"
#include "light/openhab_light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "protobuf_general.pb.h"
#include "room/room.hpp"
#include "scenes/home_assistant_scene.hpp"
#include "scenes/nspm_scene.hpp"
#include "scenes/openhab_scene.hpp"
#include "scenes/scene.hpp"
#include "web_helper/WebHelper.hpp"
#include "websocket_server/websocket_server.hpp"
#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/frame.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <cstdint>
#include <cstdlib>
#include <database_manager/database_manager.hpp>
#include <entity_manager/entity_manager.hpp>
#include <exception>
#include <ixwebsocket/IXWebSocket.h>
#include <memory>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nspanel/nspanel.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>
#include <sys/types.h>
#include <vector>

#define ITEM_IN_LIST(list, item) (std::find(list.cbegin(), list.cend(), item) != list.end());

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void EntityManager::init() {
  // MQTT_Manager::attach_observer(EntityManager::mqtt_callback);
  WebsocketServer::attach_message_callback(EntityManager::websocket_callback);
  MqttManagerConfig::attach_config_loaded_listener(EntityManager::post_init_entities);
  MQTT_Manager::subscribe("nspanel/mqttmanager/command", &EntityManager::mqtt_topic_callback);
  MQTT_Manager::subscribe("nspanel/+/status", &EntityManager::mqtt_topic_callback);

  IPCHandler::attach_callback("entity_manager/add_light", &EntityManager::ipc_callback_add_light);

  EntityManager::load_rooms();
}

void EntityManager::attach_entity_added_listener(void (*listener)(std::shared_ptr<MqttManagerEntity>)) {
  EntityManager::_entity_added_signal.connect(listener);
}

void EntityManager::detach_entity_added_listener(void (*listener)(std::shared_ptr<MqttManagerEntity>)) {
  EntityManager::_entity_added_signal.disconnect(listener);
}

void EntityManager::load_rooms() {
  auto room_ids = database_manager::database.select(&database_manager::Room::id, sqlite_orm::from<database_manager::Room>());
  SPDLOG_INFO("Loading {} rooms.", room_ids.size());

  // Check if any existing room has been removed.
  {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
    for (auto it = EntityManager::_rooms.begin(); it != EntityManager::_rooms.end(); it++) {
      auto room_id_slot = std::find_if(room_ids.begin(), room_ids.end(), [&it](auto id) {
        return (*it)->get_id() == id;
      });
      if (room_id_slot == room_ids.end()) {
        // Room was not found in list of IDs in the DB, remove the loaded room.
        SPDLOG_INFO("Room {}::{} was found in config but not in database. Removing room.", (*it)->get_id(), (*it)->get_name());
        EntityManager::_rooms.erase(it);
      }
    }
  }

  // Cause existing room to reload config or add a new room if it does not exist.
  for (auto &room_id : room_ids) {
    auto existing_room = EntityManager::get_room(room_id);
    if (existing_room != nullptr) [[likely]] {
      existing_room->reload_config();
    } else {
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
      auto room = std::shared_ptr<Room>(new Room(room_id));
      SPDLOG_INFO("Room {}::{} was found in database but not in config. Creating room.", room->get_id(), room->get_name());
      EntityManager::_rooms.push_back(room);
    }
  }
}

std::shared_ptr<Room> EntityManager::get_room(uint32_t room_id) {
  try {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
    for (auto room = EntityManager::_rooms.begin(); room != EntityManager::_rooms.end(); room++) {
      if ((*room)->get_id() == room_id) {
        return std::shared_ptr<Room>((*room));
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
  return nullptr;
}

std::vector<std::shared_ptr<Room>> EntityManager::get_all_rooms() {
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
  return EntityManager::_rooms;
}

void EntityManager::remove_room(uint32_t room_id) {
  try {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
    for (auto room = EntityManager::_rooms.begin(); room != EntityManager::_rooms.end(); room++) {
      if ((*room)->get_id() == room_id) {
        SPDLOG_INFO("Removing room room {}::{}", (*room)->get_id(), (*room)->get_name());
        EntityManager::_rooms.erase(room);
        break;
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::add_light(LightSettings &config) {
  try {
    if (EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, config.id()) == nullptr) {
      std::string light_type = config.type();
      if (light_type.compare("home_assistant") == 0) {
        std::shared_ptr<HomeAssistantLight> light = std::shared_ptr<HomeAssistantLight>(new HomeAssistantLight(config));
        EntityManager::_entities.push_back(light);
      } else if (light_type.compare("openhab") == 0) {
        std::shared_ptr<OpenhabLight> light = std::shared_ptr<OpenhabLight>(new OpenhabLight(config));
        EntityManager::_entities.push_back(light);
      } else {
        SPDLOG_ERROR("Unknown light type '{}'. Will ignore entity.", light_type);
      }
    } else {
      SPDLOG_ERROR("A light with ID {} already exists.", config.id());
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

bool EntityManager::ipc_callback_add_light(nlohmann::json message, nlohmann::json *response) {
  try {
    SPDLOG_DEBUG("Received IPC callback for new light, creating new light.");
    LightSettings setting;
    setting.ParseFromString(std::string(message["data"]));
    EntityManager::add_light(setting);
    // Everything else is already initialized, post_init light directly after adding it.
    auto light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, setting.id());
    if (light != nullptr) {
      light->post_init();
    } else {
      SPDLOG_ERROR("Failed to find light with id {}. Will not post_init()!", setting.id());
    }

    // TODO: Send update to panels about new light
    (*response)["status"] = "ok";
    return true;
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
    (*response)["status"] = "error";
    return true;
  }
}

void EntityManager::add_scene(nlohmann::json &config) {
  try {
    std::shared_ptr<Scene> scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, config.at("scene_id"));
    if (scene == nullptr) {
      std::string scene_type = config["scene_type"];
      if (scene_type.compare("nspm_scene") == 0) {
        std::shared_ptr<Scene> scene = std::shared_ptr<Scene>(new NSPMScene(config));
        EntityManager::_entities.push_back(scene);
      } else if (scene_type.compare("home_assistant") == 0) {
        std::shared_ptr<Scene> scene = std::shared_ptr<Scene>(new HomeAssistantScene(config));
        EntityManager::_entities.push_back(scene);
      } else if (scene_type.compare("openhab") == 0) {
        std::shared_ptr<Scene> scene = std::shared_ptr<Scene>(new OpenhabScene(config));
        EntityManager::_entities.push_back(scene);
      }
    } else {
      scene->update_config(config);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::add_nspanel_relay_group(nlohmann::json &config) {
  if (!config.contains("id") || !config.at("id").is_number()) {
    SPDLOG_ERROR("Tried to create NSPanel relay group but 'id' was not present in config!");
    return;
  }

  try {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanel_relay_groups_mutex);
    for (auto relay_group = EntityManager::_nspanel_relay_groups.begin(); relay_group != EntityManager::_nspanel_relay_groups.end(); relay_group++) {
      if ((*relay_group)->get_id() == config.at("id")) {
        (*relay_group)->update_config(config);
        return;
      }
    }

    EntityManager::_nspanel_relay_groups.push_back(std::shared_ptr<NSPanelRelayGroup>(new NSPanelRelayGroup(config)));
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

std::shared_ptr<NSPanelRelayGroup> EntityManager::get_relay_group(uint32_t relay_group_id) {
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanel_relay_groups_mutex);
  for (auto relay_group = EntityManager::_nspanel_relay_groups.begin(); relay_group != EntityManager::_nspanel_relay_groups.end(); relay_group++) {
    if ((*relay_group)->get_id() == relay_group_id) {
      return (*relay_group);
    }
  }
  return nullptr;
}

std::vector<std::shared_ptr<NSPanelRelayGroup>> EntityManager::get_all_relay_groups() {
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanel_relay_groups_mutex);
  return EntityManager::_nspanel_relay_groups;
}

void EntityManager::add_nspanel(NSPanelSettings &config) {
  try {
    int panel_id = config.id();
    std::string panel_mac = config.mac_address();
    NSPanel *panel = EntityManager::get_nspanel_by_id(panel_id);
    if (panel == nullptr) {
      panel = EntityManager::get_nspanel_by_mac(panel_mac);
    }
    if (panel != nullptr) {
      SPDLOG_DEBUG("Found existing NSPanel {}::{}. Updating config for existing panel.", panel->get_id(), panel->get_name());
      panel->update_config(config);
    } else {
      NSPanel *panel = new NSPanel(config);
      panel->send_websocket_update();
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
      EntityManager::_nspanels.push_back(panel);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::post_init_entities() {
  SPDLOG_INFO("New config loaded, processing changes.");
  EntityManager::_weather_manager.update_config();

  {
    // Process any loaded lights
    SPDLOG_DEBUG("Updating lights.");
    std::list<int> light_ids;
    for (LightSettings &config : MqttManagerConfig::light_configs) {
      light_ids.push_back(config.id());
      std::shared_ptr<Light> light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, config.id());
      if (light != nullptr) {
        light->update_config(config);
      } else {
        EntityManager::add_light(config);
      }
    }
    SPDLOG_DEBUG("Existing lights updated.");

    // Check for any removed lights
    SPDLOG_DEBUG("Checking for removed lights.");
    for (int i = 0; i < EntityManager::_entities.size(); i++) {
      auto rit = EntityManager::_entities[i];
      if (rit->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT) {
        bool exists = false;
        for (int light_id : light_ids) {
          if (light_id == rit->get_id()) {
            exists = true;
            break;
          }
        }

        if (!exists) {
          std::shared_ptr<Light> light = std::static_pointer_cast<Light>(rit);
          SPDLOG_DEBUG("Removing Light {}::{} as it doesn't exist in config anymore.", light->get_id(), light->get_name());
          EntityManager::_entities.erase(EntityManager::_entities.begin() + i);
          SPDLOG_DEBUG("Light removed successfully.");
        }
      }
    }
  }

  {
    // Process any loaded NSPanels
    SPDLOG_DEBUG("Updating NSPanels.");
    std::list<int> nspanel_ids;
    for (NSPanelSettings &config : MqttManagerConfig::nspanel_configs) {
      EntityManager::add_nspanel(config); // add_nspanel takes care to check if it exists before adding it. IF it exists, update it instead.
      nspanel_ids.push_back(config.id());
    }

    // Check for any removed nspanels
    SPDLOG_DEBUG("Checking for removed NSPanels.");
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
    for (int i = 0; i < EntityManager::_nspanels.size(); i++) {
      auto rit = EntityManager::_nspanels[i];
      if (rit->has_registered_to_manager()) {
        bool exists = ITEM_IN_LIST(nspanel_ids, rit->get_id());
        if (!exists) {
          NSPanel *panel = rit;
          rit->erase();
          SPDLOG_DEBUG("Removing NSPanel {}::{} as it doesn't exist in config anymore.", panel->get_id(), panel->get_name());
          EntityManager::_nspanels.erase(EntityManager::_nspanels.begin() + i);
          delete panel;
        }
      }
    }
  }

  {
    // Process any loaded NSPanel Relay Groups
    SPDLOG_DEBUG("Updating NSPanel relay groups.");
    std::list<int> relay_group_ids;
    for (nlohmann::json &config : MqttManagerConfig::nspanel_relay_group_configs) {
      EntityManager::add_nspanel_relay_group(config);
      relay_group_ids.push_back(config["relay_group_id"]);
    }
    SPDLOG_DEBUG("Existing relay groups updated.");

    // Check for any removed lights
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanel_relay_groups_mutex);
    SPDLOG_DEBUG("Checking for removed relay groups.");
    for (int i = 0; i < EntityManager::_nspanel_relay_groups.size(); i++) {
      auto rit = EntityManager::_nspanel_relay_groups[i];
      bool exists = false;
      for (int rg_id : relay_group_ids) {
        if (rg_id == rit->get_id()) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        SPDLOG_DEBUG("Removing relay group with id {} as it doesn't exist in config anymore.", rit->get_id());
        // MqttManagerEntity *rg = rit;
        EntityManager::_nspanel_relay_groups.erase(EntityManager::_nspanel_relay_groups.begin() + i);
        // delete rg;
        SPDLOG_DEBUG("Relay group removed successfully.");
      }
    }
  }

  {
    // Process any loaded Scenes
    SPDLOG_DEBUG("Updating scenes.");
    std::list<int> scene_ids;
    for (nlohmann::json &config : MqttManagerConfig::scenes_configs) {
      EntityManager::add_scene(config);
      scene_ids.push_back(config["scene_id"]);
    }
    SPDLOG_DEBUG("Existing scenes updated.");

    // Check for any removed lights
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    SPDLOG_DEBUG("Checking for removed scenes.");
    for (int i = 0; i < EntityManager::_entities.size(); i++) {
      auto rit = EntityManager::_entities[i];
      if (rit->get_type() == MQTT_MANAGER_ENTITY_TYPE::SCENE) {
        bool exists = false;
        for (int scene_id : scene_ids) {
          if (scene_id == rit->get_id()) {
            exists = true;
            break;
          }
        }

        if (!exists) {
          SPDLOG_DEBUG("Removing scene with id {} as it doesn't exist in config anymore.", rit->get_id());
          EntityManager::_entities.erase(EntityManager::_entities.begin() + i);
          SPDLOG_DEBUG("Relay group removed successfully.");
        }
      }
    }
  }

  {
    SPDLOG_INFO("Performing post init on {} entities.", EntityManager::_entities.size());
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    for (auto entity : EntityManager::_entities) {
      SPDLOG_DEBUG("Performing PostInit on entity type {} with id {}", static_cast<int>(entity->get_type()), entity->get_id());
      entity->post_init();
    }
  }

  {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
    for (auto room : EntityManager::_rooms) {
      SPDLOG_DEBUG("Performing PostInit on Room {}::{}", room->get_id(), room->get_name());
      room->post_init();
    }
  }

  SPDLOG_INFO("Total loaded NSPanels: {}", EntityManager::_nspanels.size());
  SPDLOG_INFO("Total loaded Rooms: {}", EntityManager::_rooms.size());
  SPDLOG_INFO("Total loaded Entities: {}", EntityManager::_entities.size());
}

void EntityManager::remove_entity(std::shared_ptr<MqttManagerEntity> entity) {
  SPDLOG_DEBUG("Removing entity with ID {}.", entity->get_id());
  {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    EntityManager::_entities.erase(std::find(EntityManager::_entities.cbegin(), EntityManager::_entities.cend(), entity));
  }
  EntityManager::_entity_removed_signal(std::static_pointer_cast<MqttManagerEntity>(entity));
}

void EntityManager::mqtt_topic_callback(const std::string &topic, const std::string &payload) {
  EntityManager::_process_message(topic, payload);
}

bool EntityManager::mqtt_callback(const std::string &topic, const std::string &payload) {
  SPDLOG_TRACE("Processing message on topic: {}, payload: {}", topic, payload);
  try {
    return EntityManager::_process_message(topic, payload);
  } catch (const std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing message. Exception: ", ex.what());
  } catch (...) {
    SPDLOG_ERROR("Caught unknown exception while processing message.");
  }

  return false;
}

bool EntityManager::_process_message(const std::string &topic, const std::string &payload) {
  try {
    if (topic.compare("nspanel/mqttmanager/command") == 0) {
      SPDLOG_TRACE("Received command payload: {}", payload);
      nlohmann::json data = nlohmann::json::parse(payload);
      if (!data.contains("mac_origin")) {
        SPDLOG_ERROR("Command payload did not contain a 'mac_origin' attribute. Will cancel processing.");
        return true;
      }
      std::string mac_origin = data["mac_origin"];
      if (data.contains("method")) {
        NSPanel *panel = EntityManager::get_nspanel_by_mac(mac_origin);
        if (panel == nullptr) {
          SPDLOG_TRACE("Received command from an unknown NSPanel. Will ignore it.");
          return true;
        } else if (panel != nullptr && !panel->has_registered_to_manager()) {
          SPDLOG_TRACE("Received command from an NSPanel that hasn't registered to the manager yet. Will ignore it.");
          return true;
        }
        std::string command_method = data["method"];
        if (command_method.compare("set") == 0) {
          std::string command_set_attribute = data["attribute"];
          if (command_set_attribute.compare("brightness") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint8_t new_brightness = data["brightness"];
            for (uint entity_id : entity_ids) {
              std::shared_ptr<Light> light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, entity_id);
              if (light != nullptr) {
                if (new_brightness != 0) {
                  light->set_brightness(new_brightness, false);
                  light->turn_on(true);
                } else {
                  light->turn_off(true);
                }
              }
            }
          } else if (command_set_attribute.compare("kelvin") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint16_t new_kelvin = data["kelvin"];
            for (uint entity_id : entity_ids) {
              std::shared_ptr<Light> light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, entity_id);
              if (light != nullptr) {
                light->set_color_temperature(new_kelvin, true);
              }
            }
          } else if (command_set_attribute.compare("hue") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint16_t new_hue = data["hue"];
            for (uint entity_id : entity_ids) {
              std::shared_ptr<Light> light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, entity_id);
              if (light != nullptr) {
                light->set_hue(new_hue, true);
              }
            }
          } else if (command_set_attribute.compare("saturation") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint8_t new_saturation = data["saturation"];
            for (uint entity_id : entity_ids) {
              std::shared_ptr<Light> light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, entity_id);
              if (light != nullptr) {
                light->set_saturation(new_saturation, true);
              }
            }
          } else {
            SPDLOG_ERROR("Unknown attribute '{}' in set-command request.", command_set_attribute);
          }
        } else {
          SPDLOG_ERROR("Unknown method. Payload: {}", payload);
        }
      } else if (data.contains("command")) {
        std::string command = data["command"];
        if (command.compare("register_request") == 0) {
          EntityManager::_handle_register_request(data);
        } else if (command.compare("activate_scene") == 0) {
          int scene_id = data["scene_id"];
          std::shared_ptr<Scene> scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, scene_id);
          if (scene != nullptr) {
            scene->activate();
          } else {
            SPDLOG_ERROR("No scene with ID {} exists.", scene_id);
          }
        } else if (command.compare("save_scene") == 0) {
          int scene_id = data["scene_id"];
          std::shared_ptr<Scene> scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, scene_id);
          if (scene != nullptr) {
            scene->save();
          } else {
            SPDLOG_ERROR("No scene with ID {} exists.", scene_id);
          }

        } else {
          SPDLOG_ERROR("Got command but no handler for it exists. Command: {}", command);
        }
      } else {
        SPDLOG_ERROR("Received unknown message on command topic. Message: {}", payload);
      }

      return true;
    }

  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::diagnostic_information(e, true));
  }

  return false; // Message was not processed by us, keep looking.
}

void EntityManager::_handle_register_request(const nlohmann::json &data) {
  std::string mac_address = data["mac_origin"];
  std::string name = data["friendly_name"];
  SPDLOG_INFO("Got register request from NSPanel with name {} and MAC: {}", name, mac_address);
  NSPanel *panel = EntityManager::get_nspanel_by_mac(mac_address);
  if (panel != nullptr && panel->get_state() != MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT && panel->get_state() != MQTT_MANAGER_NSPANEL_STATE::DENIED) {
    SPDLOG_TRACE("Has registered to manager? {}", panel->has_registered_to_manager() ? "TRUE" : "FALSE");
    if (panel->get_state() == MQTT_MANAGER_NSPANEL_STATE::WAITING) {
      SPDLOG_TRACE("State: WAITING");
    } else if (panel->get_state() == MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT) {
      SPDLOG_TRACE("State: AWAITING_ACCEPT");
    } else {
      SPDLOG_DEBUG("State: something else, {}.", int(panel->get_state()));
    }
    panel->register_to_manager(data);
  }
  if (panel == nullptr) {
    nlohmann::json init_data = data;
    SPDLOG_INFO("Panel is not registered to manager, adding panel but as 'pending accept' status.");

    // TODO: Convert JSON to NSPanelSettings object
    NSPanelSettings settings;
    settings.set_accepted(false);
    settings.set_denied(false);
    if (data.contains("mac")) {
      settings.set_mac_address(data.at("mac"));
    } else if (data.contains("mac_origin")) {
      settings.set_mac_address(data.at("mac_origin"));
    }
    if (data.contains("address")) {
      settings.set_ip_address(data.at("address"));
    } else if (data.contains("ip_address")) {
      settings.set_ip_address(data.at("ip_address"));
    }
    if (data.contains("name")) {
      settings.set_name(data.at("name"));
    } else if (data.contains("friendly_name")) {
      settings.set_name(data.at("friendly_name"));
    }

    NSPanel *new_nspanel = new NSPanel(settings);
    EntityManager::_nspanels.push_back(new_nspanel);
    new_nspanel->register_to_manager(data);
  }
}

NSPanel *EntityManager::get_nspanel_by_id(uint id) {
  SPDLOG_TRACE("Trying to find NSPanel by ID {}", id);
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
  for (NSPanel *nspanel : EntityManager::_nspanels) {
    if (nspanel->get_id() == id) {
      SPDLOG_TRACE("Found NSPanel by ID {}", id);
      return nspanel;
    }
  }
  SPDLOG_TRACE("Did not find NSPanel by ID {}", id);
  return nullptr;
}

NSPanel *EntityManager::get_nspanel_by_mac(std::string mac) {
  SPDLOG_TRACE("Trying to find NSPanel by MAC {}", mac);
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
  for (NSPanel *nspanel : EntityManager::_nspanels) {
    SPDLOG_TRACE("Found NSPanel with mac '{}'. Searching for mac '{}'.", nspanel->get_mac(), mac);
    if (nspanel->get_mac().compare(mac) == 0) {
      SPDLOG_TRACE("Found NSPanel by MAC {}", mac);
      return nspanel;
    }
  }
  SPDLOG_TRACE("Did not find NSPanel by MAC {}", mac);
  return nullptr;
}

bool EntityManager::websocket_callback(std::string &message, std::string *response_buffer) {
  nlohmann::json data = nlohmann::json::parse(message);
  nlohmann::json args = data["args"];

  uint64_t command_id = data["cmd_id"];
  std::string command = data["command"];

  if (command.compare("get_nspanels_status") == 0) {
    SPDLOG_DEBUG("Processing request for NSPanels status.");
    std::vector<nlohmann::json> panel_responses;
    for (auto it = EntityManager::_nspanels.cbegin(); it != EntityManager::_nspanels.cend(); it++) {
      if ((*it)->get_state() == MQTT_MANAGER_NSPANEL_STATE::DENIED) {
        continue; // Skip any panel that is denied.
      }
      if ((*it)->has_registered_to_manager()) {
        SPDLOG_DEBUG("Requesting state from NSPanel {}::{}", (*it)->get_id(), (*it)->get_name());
      } else {
        SPDLOG_DEBUG("Requesting state from NSPanel ??::{}", (*it)->get_name());
      }
      if (args.contains("nspanel_id")) {
        if ((*it)->get_id() == atoi(std::string(args["nspanel_id"]).c_str())) {
          panel_responses.push_back((*it)->get_websocket_json_representation());
          break;
        }
      } else {
        // In case no ID was specified, send status for all panels.
        panel_responses.push_back((*it)->get_websocket_json_representation());
      }
    }
    nlohmann::json response;
    response["nspanels"] = panel_responses;
    response["cmd_id"] = command_id;
    (*response_buffer) = response.dump();
    return true;
  } else if (command.compare("reboot_nspanels") == 0) {
    nlohmann::json args = data["args"];
    nlohmann::json nspanels = args["nspanels"];
    for (std::string nspanel_id_str : nspanels) {
      uint16_t nspanel_id = atoi(nspanel_id_str.c_str());
      NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
      if (nspanel != nullptr) {
        SPDLOG_INFO("Sending reboot command to nspanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        nlohmann::json cmd;
        cmd["command"] = "reboot";
        nspanel->send_command(cmd);
      } else {
        SPDLOG_ERROR("Received command to reboot NSPanel with ID {} but no panel with that ID is loaded.", nspanel_id);
      }
    }
    return true;
  } else if (command.compare("firmware_update_nspanels") == 0) {
    nlohmann::json args = data["args"];
    nlohmann::json nspanels = args["nspanels"];
    for (std::string nspanel_id_str : nspanels) {
      uint16_t nspanel_id = atoi(nspanel_id_str.c_str());
      NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
      if (nspanel != nullptr) {
        SPDLOG_INFO("Sending firmware update command to nspanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        nlohmann::json cmd;
        cmd["command"] = "firmware_update";
        nspanel->send_command(cmd);
      } else {
        SPDLOG_ERROR("Received command to firmware update NSPanel with ID {} but no panel with that ID is loaded.");
      }
    }
    return true;
  } else if (command.compare("tft_update_nspanels") == 0) {
    nlohmann::json args = data["args"];
    nlohmann::json nspanels = args["nspanels"];
    for (std::string nspanel_id_str : nspanels) {
      uint16_t nspanel_id = atoi(nspanel_id_str.c_str());
      NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
      if (nspanel != nullptr) {
        SPDLOG_INFO("Sending TFT update command to nspanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        nlohmann::json cmd;
        cmd["command"] = "tft_update";
        nspanel->send_command(cmd);
      } else {
        SPDLOG_ERROR("Received command to TFT update NSPanel with ID {} but no panel with that ID is loaded.", nspanel_id);
      }
    }
    return true;
  } else if (command.compare("get_nspanel_logs") == 0) {
    SPDLOG_DEBUG("Processing request for NSPanel logs.");
    nlohmann::json args = data["args"];
    uint16_t nspanel_id = atoi(std::string(args["nspanel_id"]).c_str());
    NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
    if (nspanel != nullptr) {
      nlohmann::json response = nspanel->get_websocket_json_logs();
      response["cmd_id"] = command_id;
      (*response_buffer) = response.dump();
    } else {
      SPDLOG_ERROR("Received request for logs from NSPanel with ID {} but no panel with that ID is loaded.");
    }
    return true;
  } else if (command.compare("nspanel_accept") == 0) {
    nlohmann::json args = data["args"];
    std::string mac = args["mac_address"];
    NSPanel *panel = EntityManager::get_nspanel_by_mac(mac);
    if (panel != nullptr) {
      SPDLOG_INFO("Accepting reqister request for NSPanel with MAC {} as per user request from websocket.", mac);
      panel->accept_register_request();
      nlohmann::json response;
      response["cmd_id"] = command_id;
      response["success"] = true;
      response["mac_address"] = mac;
      (*response_buffer) = response.dump();
      panel->send_websocket_update();
      return true;
    } else {
      SPDLOG_DEBUG("Received NSPanel accept request for a panel we could not find. Ignoring request.");
    }
  } else if (command.compare("nspanel_deny") == 0) {
    nlohmann::json args = data["args"];
    std::string mac = args["mac_address"];
    NSPanel *panel = EntityManager::get_nspanel_by_mac(mac);
    if (panel != nullptr) {
      SPDLOG_INFO("Accepting reqister request for NSPanel with MAC {} as per user request from websocket.", mac);
      panel->deny_register_request();
      nlohmann::json response;
      response["cmd_id"] = command_id;
      response["success"] = true;
      response["mac_address"] = mac;
      (*response_buffer) = response.dump();
      panel->send_websocket_update();
      return true;
    } else {
      SPDLOG_DEBUG("Received NSPanel deny request for a panel we could not find. Ignoring request.");
    }
  } else if (command.compare("nspanel_delete") == 0) {
    nlohmann::json args = data["args"];
    std::string mac = args["mac_address"];
    NSPanel *panel = EntityManager::get_nspanel_by_mac(mac);
    if (panel != nullptr) {
      SPDLOG_INFO("Received command to delete NSPanel {}::{}.", panel->get_id(), panel->get_name());
      std::string url = fmt::format("http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/delete_nspanel/{}", panel->get_id()).c_str();
      std::string response_data;
      if (WebHelper::perform_get_request(&url, &response_data, nullptr) && !response_data.empty()) {
        panel->reboot();
        nlohmann::json response;
        response["cmd_id"] = command_id;
        response["success"] = true;
        response["mac_address"] = mac;
        (*response_buffer) = response.dump();

        // Instantly delete NSPanel from manager.
        std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
        for (int i = 0; i < EntityManager::_nspanels.size(); i++) {
          if (EntityManager::_nspanels[i] == panel) {
            EntityManager::_nspanels.erase(EntityManager::_nspanels.begin() + i);
            panel->reset_mqtt_topics();
            delete panel;
            SPDLOG_INFO("Deleted NSPanel instance from EntityManager.");
            break;
          }
        }

        SPDLOG_DEBUG("Panel with MAC {} delete call completed.", mac);
        return true;
      } else {
        SPDLOG_ERROR("Failed to delete NSPanel with given MAC.");
      }
    } else {
      SPDLOG_ERROR("Received request to delete NSPanel but no NSPanel with MAC {} is register to this manager.", mac);
    }
  }

  return false;
}
