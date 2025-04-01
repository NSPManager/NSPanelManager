#include "command_manager/command_manager.hpp"
#include "entity/entity.hpp"
#include "ipc_handler/ipc_handler.hpp"
#include "light/home_assistant_light.hpp"
#include "light/light.hpp"
#include "light/openhab_light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
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
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <database_manager/database_manager.hpp>
#include <entity_manager/entity_manager.hpp>
#include <iterator>
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
#include <thread>
#include <vector>

#define ITEM_IN_LIST(list, item) (std::find(list.cbegin(), list.cend(), item) != list.end());

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void EntityManager::init() {
  if (!EntityManager::_update_all_rooms_status_thread.joinable()) {
    SPDLOG_INFO("No thread to handle 'All rooms' status exists, starting...");
    EntityManager::_last_room_update_time = std::chrono::system_clock::now();
    EntityManager::_update_all_rooms_status_thread = std::thread(EntityManager::update_all_rooms_status);
    EntityManager::_update_all_rooms_status_thread.detach();
  }

  CommandManager::attach_callback(&EntityManager::_command_callback);

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
  EntityManager::_weather_manager.reload_config();

  EntityManager::load_scenes();
  EntityManager::load_lights();
  EntityManager::load_rooms();    // Rooms are loaded last as to make all room components be able to find entities of other types.
  EntityManager::load_nspanels(); // Loads panels after rooms are loaded so that they can find all availables entities and rooms for the panel config.

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

  // Remove room if it does no longer exist
  EntityManager::_rooms.erase(std::remove_if(EntityManager::_rooms.begin(), EntityManager::_rooms.end(), [&room_ids](auto room) {
                                return std::find_if(room_ids.begin(), room_ids.end(), [&room](auto id) { return id == room->get_id(); }) == room_ids.end();
                              }),
                              EntityManager::_rooms.end());

  // Cause existing room to reload config or add a new room if it does not exist.
  for (auto &room_id : room_ids) {
    auto existing_room = EntityManager::get_room(room_id);
    if (existing_room != nullptr) [[likely]] {
      existing_room->reload_config();
      existing_room->post_init();
    } else {
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
      auto room = std::shared_ptr<Room>(new Room(room_id));
      SPDLOG_INFO("Room {}::{} was found in database but not in config. Creating room.", room->get_id(), room->get_name());
      room->post_init();
      room->attach_room_changed_callback(&EntityManager::_room_updated_callback);
      EntityManager::_rooms.push_back(room);
    }
  }
}

void EntityManager::load_nspanels() {
  auto nspanel_ids = database_manager::database.select(&database_manager::NSPanel::id, sqlite_orm::from<database_manager::NSPanel>());
  SPDLOG_INFO("Loading {} NSPanels.", nspanel_ids.size());

  // Check if any existing NSPanel has been removed.
  EntityManager::_nspanels.erase(std::remove_if(EntityManager::_nspanels.begin(), EntityManager::_nspanels.end(), [&nspanel_ids](auto nspanel) {
                                   return std::find_if(nspanel_ids.begin(), nspanel_ids.end(), [&nspanel](auto id) { return id == nspanel->get_id(); }) == nspanel_ids.end();
                                 }),
                                 EntityManager::_nspanels.end());

  // Cause existing NSPanel to reload config or add a new NSPanel if it does not exist.
  for (auto &nspanel_id : nspanel_ids) {
    auto existing_nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
    if (existing_nspanel != nullptr) [[likely]] {
      existing_nspanel->reload_config();
      existing_nspanel->send_config();
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
  EntityManager::_entities.erase(std::remove_if(EntityManager::_entities.begin(), EntityManager::_entities.end(), [&light_ids](auto entity) {
                                   return entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT && std::find_if(light_ids.begin(), light_ids.end(), [&entity](auto id) { return id == entity->get_id(); }) == light_ids.end();
                                 }),
                                 EntityManager::_entities.end());

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
  SPDLOG_DEBUG("Loaded {} lights", light_ids.size());
}

void EntityManager::load_scenes() {
  auto scene_ids = database_manager::database.select(&database_manager::Scene::id, sqlite_orm::from<database_manager::Scene>());
  SPDLOG_INFO("Loading {} scenes.", scene_ids.size());

  // Check if any existing scene has been removed.
  EntityManager::_entities.erase(std::remove_if(EntityManager::_entities.begin(), EntityManager::_entities.end(), [&scene_ids](auto entity) {
                                   return entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::SCENE && std::find_if(scene_ids.begin(), scene_ids.end(), [&entity](auto id) { return entity->get_id() == id; }) == scene_ids.end();
                                 }),
                                 EntityManager::_entities.end());

  // Cause existing NSPanel to reload config or add a new NSPanel if it does not exist.
  for (auto &scene_id : scene_ids) {
    auto existing_scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, scene_id);
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
  SPDLOG_DEBUG("Loaded {} scenes", scene_ids.size());
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

void EntityManager::update_all_rooms_status() {
  SPDLOG_INFO("Started thread to handle 'All rooms' status updates.");
  for (;;) {
    // Wait for notification that a room has been updated
    {
      std::unique_lock<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
      EntityManager::_room_update_condition_variable.wait(mutex_guard, [&]() {
        return !EntityManager::_all_rooms_status_updated;
      });
    }
    // Wait until changes has settled as when a user changes light states in "All rooms" mode a burst of changes will occur from all rooms.
    uint32_t backoff_time = std::stoi(MqttManagerConfig::get_setting_with_default("all_rooms_status_backoff_time", "250"));
    while (EntityManager::_last_room_update_time.load() + std::chrono::milliseconds(backoff_time) > std::chrono::system_clock::now()) {
      std::this_thread::sleep_for(EntityManager::_last_room_update_time.load() + std::chrono::milliseconds(backoff_time) - std::chrono::system_clock::now());
    }

    SPDLOG_DEBUG("Updating 'All rooms' status.");
    EntityManager::_all_rooms_status_updated = true;

    NSPanelRoomStatus all_rooms_status;

    // Calculate average light level
    uint64_t total_light_level_all = 0;
    uint64_t total_light_level_ceiling = 0;
    uint64_t total_light_level_table = 0;
    uint64_t total_kelvin_level_all = 0;
    uint64_t total_kelvin_ceiling = 0;
    uint64_t total_kelvin_table = 0;
    uint16_t num_lights_total = 0;
    uint16_t num_lights_ceiling = 0;
    uint16_t num_lights_ceiling_on = 0;
    uint16_t num_lights_table = 0;
    uint16_t num_lights_table_on = 0;

    // Determine if any light is on in any of the rooms
    bool any_light_on = false;
    for (auto room : EntityManager::_rooms) {
      std::vector<std::shared_ptr<Light>> entities = room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
      for (auto light : entities) {
        if (light->get_state() && light->get_controlled_from_main_page()) {
          any_light_on = true;
          break;
        }
      }
    }

    for (auto room : EntityManager::_rooms) {
      for (auto &light : room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT)) {
        // Light is not controlled from main page, exclude it from calculations.
        if (!light->get_controlled_from_main_page()) {
          continue;
        }

        if ((any_light_on && light->get_state()) || !any_light_on) {
          total_light_level_all += light->get_brightness();
          total_kelvin_level_all += light->get_color_temperature();
          num_lights_total++;
        }
        if (light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE) {
          // SPDLOG_TRACE("Room {}::{} found table light {}::{}, state: {}", this->_id, this->_name, light->get_id(), light->get_name(), light->get_state() ? "ON" : "OFF");
          num_lights_table++;
          if (light->get_state()) {
            total_light_level_table += light->get_brightness();
            total_kelvin_table += light->get_color_temperature();
            num_lights_table_on++;
          }
        } else if (light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) {
          // SPDLOG_TRACE("Room {}::{} found ceiling light {}::{}, state: {}", this->_id, this->_name, light->get_id(), light->get_name(), light->get_state() ? "ON" : "OFF");
          num_lights_ceiling++;
          if (light->get_state()) {
            total_light_level_ceiling += light->get_brightness();
            total_kelvin_ceiling += light->get_color_temperature();
            num_lights_ceiling_on++;
          }
        }
      }
    }
    // Update result if a ceiling or table light is found.
    all_rooms_status.set_num_table_lights(num_lights_table);
    all_rooms_status.set_num_ceiling_lights(num_lights_ceiling);
    all_rooms_status.set_num_table_lights_on(num_lights_table_on);
    all_rooms_status.set_num_ceiling_lights_on(num_lights_ceiling_on);

    if (num_lights_total > 0) {
      float average_kelvin = (float)total_kelvin_level_all / num_lights_total;
      average_kelvin -= MqttManagerConfig::get_settings().color_temp_min;
      uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
      if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
        kelvin_pct = 100 - kelvin_pct;
      }

      all_rooms_status.set_average_dim_level(total_light_level_all / num_lights_total);
      all_rooms_status.set_average_color_temperature(kelvin_pct);
    } else {
      all_rooms_status.set_average_dim_level(0);
      all_rooms_status.set_average_color_temperature(0);
    }

    if (num_lights_table_on > 0) {
      float average_kelvin = (float)total_kelvin_table / num_lights_table_on;
      average_kelvin -= MqttManagerConfig::get_settings().color_temp_min;
      uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
      if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
        kelvin_pct = 100 - kelvin_pct;
      }

      all_rooms_status.set_table_lights_dim_level(total_light_level_table / num_lights_table_on);
      all_rooms_status.set_table_lights_color_temperature_value(kelvin_pct);
    } else {
      // SPDLOG_TRACE("No table lights found, setting value to 0.");
      all_rooms_status.set_table_lights_dim_level(0);
      all_rooms_status.set_table_lights_color_temperature_value(0);
    }

    if (num_lights_ceiling_on > 0) {
      float average_kelvin = (float)total_kelvin_ceiling / num_lights_ceiling_on;
      average_kelvin -= MqttManagerConfig::get_settings().color_temp_min;
      uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
      if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
        kelvin_pct = 100 - kelvin_pct;
      }

      all_rooms_status.set_ceiling_lights_dim_level(total_light_level_ceiling / num_lights_ceiling_on);
      all_rooms_status.set_ceiling_lights_color_temperature_value(kelvin_pct);
    } else {
      // SPDLOG_TRACE("No ceiling lights found, setting value to 0.");
      all_rooms_status.set_ceiling_lights_dim_level(0);
      all_rooms_status.set_ceiling_lights_color_temperature_value(0);
    }

    std::string all_rooms_status_string;
    if (all_rooms_status.SerializeToString(&all_rooms_status_string)) {
      SPDLOG_DEBUG("All rooms status updated. Waiting for next notify.");
      MQTT_Manager::publish(fmt::format("nspanel/mqttmanager_{}/all_rooms_status", MqttManagerConfig::get_settings().manager_address), all_rooms_status_string, true);
    } else {
      SPDLOG_ERROR("Failed to serialize 'All rooms' status. Will try again next time there is a room status change.");
    }
  }
}

void EntityManager::_room_updated_callback(Room *room) {
  SPDLOG_DEBUG("Updating last room changed time.");
  {
    std::unique_lock<std::mutex> mutex_guard(EntityManager::_rooms_mutex);
    EntityManager::_last_room_update_time = std::chrono::system_clock::now();
    EntityManager::_all_rooms_status_updated = false;
  }
  SPDLOG_DEBUG("Last room changed time updated. Calling notify_all.");
  EntityManager::_room_update_condition_variable.notify_all();
}

void EntityManager::_command_callback(NSPanelMQTTManagerCommand &command) {
  if (command.has_first_page_turn_on() && command.first_page_turn_on().global()) {
    std::vector<std::shared_ptr<Room>> rooms;
    {
      std::lock_guard<std::mutex> lock_guard(EntityManager::_rooms_mutex);
      rooms = EntityManager::_rooms;
    }
    NSPanelMQTTManagerCommand base_command;
    base_command.CopyFrom(command);
    auto *turn_on_command = base_command.mutable_first_page_turn_on();

    // Check if ANY table or ceiling light is turned on.
    bool any_ceiling_light_on = false;
    bool any_table_light_on = false;
    for (auto &room : rooms) {
      for (auto &entity : room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT)) {
        if (entity->get_controlled_from_main_page() && entity->get_state()) {
          if (entity->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) {
            any_ceiling_light_on = true;
          } else if (entity->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE) {
            any_table_light_on = true;
          }
        }
        if (any_ceiling_light_on && any_table_light_on) {
          break;
        }
      }
      if (any_ceiling_light_on && any_table_light_on) {
        break;
      }
    }

    if (!any_ceiling_light_on && !any_table_light_on) {
      // Turn on all lights in all the room
      SPDLOG_DEBUG("No lights are turned on, will send command to ALL rooms while processing 'All rooms' command");
      for (auto &room : rooms) {
        turn_on_command->set_global(false);
        turn_on_command->set_selected_room(room->get_id());
        room->command_callback(base_command);
      }
    } else {
      // Lights are turned on in any/some rooms, send command to rooms where lights are turned on to change brightness of those lights
      SPDLOG_DEBUG("Lights are turned on, will send command to all rooms with lights on while processing 'All rooms' command. Ceiling lights on: {}, table lights on: {}", any_ceiling_light_on ? "Yes" : "No", any_table_light_on ? "Yes" : "No");
      for (auto &room : rooms) {
        std::vector<std::shared_ptr<Light>> lights = room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
        for (auto &light : lights) {
          if (light->get_state() && light->get_controlled_from_main_page()) {
            turn_on_command->set_global(false);
            turn_on_command->set_selected_room(room->get_id());
            room->command_callback(base_command);
            break;
          }
        }
      }
    }
  } else if (command.has_toggle_entity_from_entities_page()) {
    auto entity = EntityManager::get_entity_by_page_id_and_slot(command.toggle_entity_from_entities_page().entity_page_id(), command.toggle_entity_from_entities_page().entity_slot());
    if (entity && entity->can_toggle()) {
      SPDLOG_DEBUG("Will toggle entity in slot {} in page with ID {}.", command.toggle_entity_from_entities_page().entity_slot(), command.toggle_entity_from_entities_page().entity_page_id());
      // Handle light seperatly as they reequires some special handling in regards to what brightness to turn on to.
      if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT) {
        auto light_entity = std::dynamic_pointer_cast<Light>(entity);
        if (light_entity) {
          if (light_entity->get_state()) {
            light_entity->turn_off(true);
          } else {
            // Calculate average brightness of all lights in room and turn on light to that level, if lights are off then turn on to default brightness
            uint16_t room_id = light_entity->get_room_id();
            auto room = EntityManager::get_room(room_id);
            if (room) {
              auto room_entities = room->get_all_entities();
              // Remove any entities that are not lights
              room_entities.erase(std::remove_if(room_entities.begin(), room_entities.end(), [](auto entity) {
                                    return entity->get_type() != MQTT_MANAGER_ENTITY_TYPE::LIGHT;
                                  }),
                                  room_entities.end());

              bool any_light_entity_on = std::find_if(room_entities.begin(), room_entities.end(), [](auto &entity) {
                                           auto light = std::dynamic_pointer_cast<Light>(entity);
                                           return light && light->get_state();
                                         }) != room_entities.end();

              // Remove any lights that are not on if a light is on
              if (any_light_entity_on) {
                room_entities.erase(std::remove_if(room_entities.begin(), room_entities.end(), [](auto entity) {
                                      auto light = std::dynamic_pointer_cast<Light>(entity);
                                      return light && !light->get_state();
                                    }),
                                    room_entities.end());
              }

              uint64_t total_light_brightness = std::accumulate(room_entities.begin(), room_entities.end(), 0, [](uint64_t sum, auto &entity) {
                auto light = std::dynamic_pointer_cast<Light>(entity);
                return sum + (light ? light->get_brightness() : 0);
              });

              uint16_t average_light_brightness = total_light_brightness / room_entities.size();
              if (average_light_brightness == 0) {
                average_light_brightness = std::stoi(MqttManagerConfig::get_setting_with_default("light_turn_on_brightness", "50"));
              }

              light_entity->set_brightness(average_light_brightness, false);
              light_entity->turn_on(true);
            } else {
              SPDLOG_ERROR("Failed to get room that light resides in. Will not be able to turn on light from toggle command.");
            }
          }
        } else {
          SPDLOG_ERROR("Received command to toggle light entity in slot {} in page with ID {} but entity could not be cast to a light.", command.toggle_entity_from_entities_page().entity_slot(), command.toggle_entity_from_entities_page().entity_page_id());
        }
      } else {
        entity->toggle();
      }
    } else {
      SPDLOG_DEBUG("Received command to toggle entity in slot {} in page with ID {} bot did not find such an entity.", command.toggle_entity_from_entities_page().entity_slot(), command.toggle_entity_from_entities_page().entity_page_id());
    }
  }
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
      SPDLOG_DEBUG("Received command payload: {}", payload);
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

    std::shared_ptr<NSPanel> new_panel = NSPanel::create_from_discovery_request(init_data);
    if (new_panel != nullptr) {
      std::lock_guard<std::mutex> lock_guard(EntityManager::_nspanels_mutex);
      EntityManager::_nspanels.push_back(new_panel);
      nlohmann::json data = {
          {"event_type", "register_request"},
          {"nspanel_id", new_panel->get_id()}};
      WebsocketServer::broadcast_json(data);
    }
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
