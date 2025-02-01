#include "entity/entity.hpp"
#include "ipc_handler/ipc_handler.hpp"
#include "light/home_assistant_light.hpp"
#include "light/light.hpp"
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
  // TODO: On 'reload config signal', reload the config.
  // MqttManagerConfig::attach_config_loaded_listener(EntityManager::post_init_entities);
  MQTT_Manager::subscribe("nspanel/mqttmanager/command", &EntityManager::mqtt_topic_callback);
  MQTT_Manager::subscribe("nspanel/+/status", &EntityManager::mqtt_topic_callback);

  EntityManager::load_entities();
}

void EntityManager::load_entities() {
  SPDLOG_INFO("Loading config...");
  EntityManager::_weather_manager.update_config();

  EntityManager::load_rooms();
  EntityManager::load_scenes();
  EntityManager::load_nspanels();
  EntityManager::load_lights();

  SPDLOG_INFO("Total loaded NSPanels: {}", EntityManager::_nspanels.size());
  SPDLOG_INFO("Total loaded Rooms: {}", EntityManager::_rooms.size());
  SPDLOG_INFO("Total loaded Entities: {}", EntityManager::_entities.size());
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

void EntityManager::load_nspanels() {
  auto nspanel_ids = database_manager::database.select(&database_manager::NSPanel::id, sqlite_orm::from<database_manager::NSPanel>());
  SPDLOG_INFO("Loading {} NSPanels.", nspanel_ids.size());

  // Check if any existing NSPanel has been removed.
  {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
    for (auto it = EntityManager::_nspanels.begin(); it != EntityManager::_nspanels.end(); it++) {
      auto room_id_slot = std::find_if(nspanel_ids.begin(), nspanel_ids.end(), [&it](auto id) {
        return (*it)->get_id() == id;
      });
      if (room_id_slot == nspanel_ids.end()) {
        // Room was not found in list of IDs in the DB, remove the loaded room.
        SPDLOG_INFO("NSPanel {}::{} was found in config but not in database. Removing panel.", (*it)->get_id(), (*it)->get_name());
        EntityManager::_nspanels.erase(it);
      }
    }
  }

  // Cause existing NSPanel to reload config or add a new NSPanel if it does not exist.
  for (auto &nspanel_id : nspanel_ids) {
    auto existing_room = EntityManager::get_room(nspanel_id);
    if (existing_room != nullptr) [[likely]] {
      existing_room->reload_config();
    } else {
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
      auto panel = std::shared_ptr<NSPanel>(new NSPanel(nspanel_id));
      SPDLOG_INFO("NSPanel {}::{} was found in database but not in config. Creating panel.", panel->get_id(), panel->get_name());
      EntityManager::_nspanels.push_back(panel);
    }
  }
}

void EntityManager::load_lights() {
  auto light_ids = database_manager::database.select(&database_manager::Light::id, sqlite_orm::from<database_manager::Light>());
  SPDLOG_INFO("Loading {} lights.", light_ids.size());

  // Check if any existing light has been removed.
  {
    auto existing_lights = EntityManager::get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    for (auto it = existing_lights.begin(); it != existing_lights.end(); it++) {
      auto room_id_slot = std::find_if(light_ids.begin(), light_ids.end(), [&it](auto id) {
        return (*it)->get_id() == id;
      });
      if (room_id_slot == light_ids.end()) {
        // Room was not found in list of IDs in the DB, remove the loaded room.
        SPDLOG_INFO("Light {}::{} was found in config but not in database. Removing light.", (*it)->get_id(), (*it)->get_name());
        EntityManager::remove_entity((*it));
      }
    }
  }

  // Cause existing lights to reload config or add a new light if it does not exist.
  for (auto &light_id : light_ids) {
    auto existing_light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, light_id);
    if (existing_light != nullptr) [[likely]] {
      existing_light->reload_config();
    } else {
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);

      try {
        auto light_settings = database_manager::database.get<database_manager::Light>(light_id);
        if (light_settings.type.compare("home_assistant") == 0) {
          std::shared_ptr<HomeAssistantLight> light = std::shared_ptr<HomeAssistantLight>(new HomeAssistantLight(light_settings.id));
          SPDLOG_INFO("Light {}::{} was found in database but not in config. Creating light.", light->get_id(), light->get_name());
          EntityManager::_entities.push_back(light);
        } else if (light_settings.type.compare("openhab") == 0) {
          std::shared_ptr<OpenhabLight> light = std::shared_ptr<OpenhabLight>(new OpenhabLight(light_settings.id));
          SPDLOG_INFO("Light {}::{} was found in database but not in config. Creating light.", light->get_id(), light->get_name());
          EntityManager::_entities.push_back(light);
        } else {
          SPDLOG_ERROR("Unknown light type '{}'. Will ignore entity.", light_settings.type);
        }
      } catch (std::exception &e) {
        SPDLOG_ERROR("Caught exception: {}", e.what());
        SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
      }
    }
  }
}

void EntityManager::load_scenes() {
  auto scene_ids = database_manager::database.select(&database_manager::Scene::id, sqlite_orm::from<database_manager::Scene>());
  SPDLOG_INFO("Loading {} scenes.", scene_ids.size());

  // Check if any existing scene has been removed.
  {
    auto existing_scenes = EntityManager::get_all_entities_by_type<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE);
    for (auto it = existing_scenes.begin(); it != existing_scenes.end(); it++) {
      auto scene_id_slot = std::find_if(scene_ids.begin(), scene_ids.end(), [&it](auto id) {
        return (*it)->get_id() == id;
      });
      if (scene_id_slot == scene_ids.end()) {
        // Room was not found in list of IDs in the DB, remove the loaded room.
        SPDLOG_INFO("Scene {}::{} was found in config but not in database. Removing scene.", (*it)->get_id(), (*it)->get_name());
        EntityManager::remove_entity((*it));
      }
    }
  }

  // Cause existing NSPanel to reload config or add a new NSPanel if it does not exist.
  for (auto &scene_id : scene_ids) {
    auto existing_scene = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, scene_id);
    if (existing_scene != nullptr) [[likely]] {
      existing_scene->reload_config();
    } else {
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);

      try {
        auto scene_settings = database_manager::database.get<database_manager::Scene>(scene_id);
        if (scene_settings.scene_type.compare("home_assistant") == 0) {
          std::shared_ptr<HomeAssistantScene> scene = std::shared_ptr<HomeAssistantScene>(new HomeAssistantScene(scene_settings.id));
          SPDLOG_INFO("Scene {}::{} was found in database but not in config. Creating scene.", scene->get_id(), scene->get_name());
          EntityManager::_entities.push_back(scene);
        } else if (scene_settings.scene_type.compare("openhab") == 0) {
          std::shared_ptr<OpenhabScene> scene = std::shared_ptr<OpenhabScene>(new OpenhabScene(scene_settings.id));
          SPDLOG_INFO("Scene {}::{} was found in database but not in config. Creating scene.", scene->get_id(), scene->get_name());
          EntityManager::_entities.push_back(scene);
        } else if (scene_settings.scene_type.compare("nspm_scene") == 0) {
          std::shared_ptr<NSPMScene> scene = std::shared_ptr<NSPMScene>(new NSPMScene(scene_settings.id));
          SPDLOG_INFO("Scene {}::{} was found in database but not in config. Creating scene.", scene->get_id(), scene->get_name());
          EntityManager::_entities.push_back(scene);
        } else {
          SPDLOG_ERROR("Unknown scene type '{}'. Will ignore entity.", scene_settings.scene_type);
        }
      } catch (std::exception &e) {
        SPDLOG_ERROR("Caught exception: {}", e.what());
        SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
      }
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
        auto panel = EntityManager::get_nspanel_by_mac(mac_origin);
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
  auto panel = EntityManager::get_nspanel_by_mac(mac_address);
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

    // TODO: Handle register request
    // NSPanel *new_nspanel = new NSPanel(settings);
    // EntityManager::_nspanels.push_back(new_nspanel);
    // new_nspanel->register_to_manager(data);
  }
}

std::shared_ptr<NSPanel> EntityManager::get_nspanel_by_id(uint id) {
  SPDLOG_TRACE("Trying to find NSPanel by ID {}", id);
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
  for (auto nspanel : EntityManager::_nspanels) {
    if (nspanel->get_id() == id) {
      SPDLOG_TRACE("Found NSPanel by ID {}", id);
      return nspanel;
    }
  }
  SPDLOG_TRACE("Did not find NSPanel by ID {}", id);
  return nullptr;
}

std::shared_ptr<NSPanel> EntityManager::get_nspanel_by_mac(std::string mac) {
  SPDLOG_TRACE("Trying to find NSPanel by MAC {}", mac);
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
  for (auto nspanel : EntityManager::_nspanels) {
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
      auto nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
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
      auto nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
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
      auto nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
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
    auto nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
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
    auto panel = EntityManager::get_nspanel_by_mac(mac);
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
    auto panel = EntityManager::get_nspanel_by_mac(mac);
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
    auto panel = EntityManager::get_nspanel_by_mac(mac);
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
        EntityManager::load_nspanels();

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
