#include "command_manager/command_manager.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf/protobuf_nspanel.pb.h"
#include "room/room_entities_page.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/stacktrace.hpp>
#include <cstdint>
#include <database_manager/database_manager.hpp>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <nlohmann/json.hpp>
#include <room/room.hpp>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>
#include <system_error>

Room::Room(uint32_t room_id) {
  SPDLOG_INFO("Initializing room with ID {}.", room_id);
  this->_id = room_id;
  this->reload_config();
  this->_send_status_updates = true;
  CommandManager::attach_callback(boost::bind(&Room::command_callback, this, _1));

  this->_send_room_state_update();
}

Room::~Room() {
  CommandManager::detach_callback(boost::bind(&Room::command_callback, this, _1));
  MQTT_Manager::clear_retain(this->_mqtt_state_topic); // Clear old state topic.
  SPDLOG_INFO("Room {}::{} destroyed.", this->_id, this->_name);
}

void Room::reload_config() {
  try {
    SPDLOG_DEBUG("Loading room {}, trying to get settings from DB.", this->_id);
    try {
      auto db_room = database_manager::database.get<database_manager::Room>(this->_id);
      this->_name = db_room.friendly_name;
      this->_mqtt_state_topic = fmt::format("nspanel/mqttmanager_{}/room/{}/state", MqttManagerConfig::get_settings().manager_address, this->_id);

      {
        std::lock_guard<std::mutex> mutex_guard(this->_entities_pages_mutex);
        SPDLOG_DEBUG("Loaded config for room {}::{}, loading room entity pages.", this->_id, this->_name);
        auto room_entities_pages = database_manager::database.get_all<database_manager::RoomEntitiesPage>(sqlite_orm::where(sqlite_orm::c(&database_manager::RoomEntitiesPage::room_id) == this->_id and sqlite_orm::c(&database_manager::RoomEntitiesPage::is_scenes_page) == false), sqlite_orm::order_by(&database_manager::RoomEntitiesPage::display_order).asc().collate_nocase());
        // Look for existing but removed RoomEntitiesPages and remove them from the list
        this->_entity_pages.erase(std::remove_if(this->_entity_pages.begin(), this->_entity_pages.end(), [&room_entities_pages](auto entity_page) {
                                    return std::find_if(room_entities_pages.begin(), room_entities_pages.end(), [&entity_page](auto room_entity_page) {
                                             return room_entity_page.id == entity_page->get_id();
                                           }) == room_entities_pages.end();
                                  }),
                                  this->_entity_pages.end());

        // Load new RoomEntitiesPages
        for (auto &entity_page : room_entities_pages) {
          bool found = false;
          for (auto &existing_entity_page : this->_entity_pages) {
            if (existing_entity_page->get_id() == entity_page.id) {
              found = true;
              break;
            }
          }

          if (!found) {
            this->_entity_pages.push_back(std::shared_ptr<RoomEntitiesPage>(new RoomEntitiesPage(entity_page.id, this)));
            SPDLOG_DEBUG("Created {} RoomEntitiesPages for room {}::{}.", this->_entity_pages.size(), this->_id, this->_name);
          }
        }

        // Sort the room entities pages by display order and then resend config.
        std::sort(this->_entity_pages.begin(), this->_entity_pages.end(), [](const std::shared_ptr<RoomEntitiesPage> &a, const std::shared_ptr<RoomEntitiesPage> &b) {
          return a->get_display_order() < b->get_display_order();
        });
        // Send state update for all entity pages so that they display the correct order.
        for (auto &entity_page : this->_entity_pages) {
          entity_page->reload_config(true);
        }

        auto scenes_pages = database_manager::database.get_all<database_manager::RoomEntitiesPage>(sqlite_orm::where(sqlite_orm::c(&database_manager::RoomEntitiesPage::room_id) == this->_id and sqlite_orm::c(&database_manager::RoomEntitiesPage::is_scenes_page) == true), sqlite_orm::order_by(&database_manager::RoomEntitiesPage::display_order).asc().collate_nocase());
        // Look for existing but removed RoomEntitiesPages that hold scenes and remove them from the list.
        this->_scene_pages.erase(std::remove_if(this->_scene_pages.begin(), this->_scene_pages.end(), [&scenes_pages](auto scene_page) {
                                   return std::find_if(scenes_pages.begin(), scenes_pages.end(), [&scene_page](auto room_entity_page) {
                                            return room_entity_page.id == scene_page->get_id();
                                          }) == scenes_pages.end();
                                 }),
                                 this->_scene_pages.end());

        // Load new RoomEntitiesPages for scenes
        for (auto &scene_page : scenes_pages) {
          bool found = false;
          for (auto &existing_scene_page : this->_entity_pages) {
            if (existing_scene_page->get_id() == scene_page.id) {
              found = true;
              break;
            }
          }

          if (!found) {
            this->_scene_pages.push_back(std::shared_ptr<RoomEntitiesPage>(new RoomEntitiesPage(scene_page.id, this)));
            SPDLOG_DEBUG("Created {} RoomEntitiesPages (for scenes) for room {}::{}.", this->_scene_pages.size(), this->_id, this->_name);
          }
        }

        // Sort the room entities pages by display order and then resend config.
        std::sort(this->_scene_pages.begin(), this->_scene_pages.end(), [](const std::shared_ptr<RoomEntitiesPage> &a, const std::shared_ptr<RoomEntitiesPage> &b) {
          return a->get_display_order() < b->get_display_order();
        });
        // Send state update for all entity pages so that they display the correct order.
        for (auto &scene_page : this->_scene_pages) {
          scene_page->reload_config(true);
        }
      }

      SPDLOG_TRACE("Room {}::{} initialized with status topic '{}'. Room has {} RoomEntitiesPages", this->_id, this->_name, this->_mqtt_state_topic, this->_entity_pages.size());
    } catch (std::system_error &ex) {
      SPDLOG_ERROR("Failed to get room with ID {} from database! Will cancel config reload.", this->_id);
      return;
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

uint16_t Room::get_id() {
  return this->_id;
}

std::string Room::get_name() {
  return this->_name;
}

void Room::post_init() {
  {
    std::lock_guard<std::mutex> mutex_guard(this->_entities_pages_mutex);
    for (std::shared_ptr<RoomEntitiesPage> &page : this->_entity_pages) {
      page->post_init();
      page->attach_state_change_callback(boost::bind(&Room::page_changed_callback, this, _1));
      SPDLOG_DEBUG("Attached callbacks for all entities in RoomEntitiesPage {}.", page->get_id());
    }

    std::lock_guard<std::mutex> scene_mutex_guard(this->_scene_entities_pages_mutex);
    for (std::shared_ptr<RoomEntitiesPage> &page : this->_scene_pages) {
      page->post_init();
      page->attach_state_change_callback(boost::bind(&Room::page_changed_callback, this, _1));
      SPDLOG_DEBUG("Attached callbacks for all scenes in RoomEntitiesPage {}.", page->get_id());
    }
  }

  this->_room_changed_callbacks(this);
  this->_send_room_state_update();
}

std::vector<std::shared_ptr<MqttManagerEntity>> Room::get_all_entities() {
  std::lock_guard<std::mutex> lock_guard(this->_entities_pages_mutex);
  std::vector<std::shared_ptr<MqttManagerEntity>> entities;
  for (std::shared_ptr<RoomEntitiesPage> &page : this->_entity_pages) {
    std::vector<std::shared_ptr<MqttManagerEntity>> page_entities = page->get_entities();
    entities.insert(entities.end(), page_entities.begin(), page_entities.end());
  }
  return entities;
}

uint16_t Room::get_number_of_entity_pages() {
  return this->_entity_pages.size();
}

std::vector<std::shared_ptr<RoomEntitiesPage>> Room::get_all_entities_pages() {
  return this->_entity_pages;
}

void Room::page_changed_callback(RoomEntitiesPage *page) {
  if (this->_send_status_updates) {
    this->_send_room_state_update();
  }
}

void Room::command_callback(NSPanelMQTTManagerCommand &command) {
  if (command.has_first_page_turn_on() && (command.first_page_turn_on().selected_room() == this->_id || command.first_page_turn_on().global())) {
    if (MqttManagerConfig::get_settings().optimistic_mode) {
      SPDLOG_DEBUG("Optimistic mode enabled, will not send state updates via callbacks in room.");
      this->_send_status_updates = false;
    }

    std::vector<std::shared_ptr<Light>> all_room_lights = this->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    // Remove any light entities that are not controlled by first page.
    all_room_lights.erase(std::remove_if(all_room_lights.begin(), all_room_lights.end(), [](std::shared_ptr<Light> light) {
                            return !light->get_controlled_from_main_page();
                          }),
                          all_room_lights.end());

    SPDLOG_DEBUG("Room {}:{} got command to turn lights on from first page.", this->_id, this->_name);
    std::vector<std::shared_ptr<Light>> lights_list;
    // Get all lights that are on
    for (auto light : all_room_lights) {
      if (light->get_state() && ((command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_ALL ||
                                  (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_CEILING_LIGHTS && light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) ||
                                  (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_TABLE_LIGHTS && light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE)))) {
        lights_list.push_back(light);
      }
    }
    if (lights_list.size() == 0) {
      // No lights that were on were found, get all lights in room.
      SPDLOG_DEBUG("No lights were on in room {}::{}. Will affect all lights in room instead.", this->_id, this->_name);
      for (auto light : all_room_lights) {
        if (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_ALL ||
            (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_CEILING_LIGHTS && light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) ||
            (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_TABLE_LIGHTS && light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE)) {
          lights_list.push_back(light);
        }
      }
    }

    // Build the individual commands that are to be sent to each light.
    NSPanelMQTTManagerCommand cmd;
    NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
    light_cmd->set_has_brightness(command.first_page_turn_on().has_brightness_value());
    light_cmd->set_brightness(command.first_page_turn_on().brightness_slider_value());
    light_cmd->set_has_color_temperature(command.first_page_turn_on().has_kelvin_value());
    light_cmd->set_color_temperature(command.first_page_turn_on().kelvin_slider_value());
    light_cmd->set_has_hue(false);
    light_cmd->set_has_saturation(false);

    for (int i = 0; i < lights_list.size(); i++) {
      light_cmd->clear_light_ids();
      light_cmd->add_light_ids(lights_list[i]->get_id());
      lights_list[i]->command_callback(cmd);
    }

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_send_room_state_update();
      this->_send_status_updates = true;
      SPDLOG_DEBUG("Optimistic mode enabled, reenabled send state updates via callbacks in room.");
    }
  } else if (command.has_first_page_turn_off()) {
    SPDLOG_DEBUG("Room {}:{} got command to turn lights off from first page.", this->_id, this->_name);
    std::vector<std::shared_ptr<Light>> all_room_lights = this->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    // Remove any light entities that are not controlled by first page.
    all_room_lights.erase(std::remove_if(all_room_lights.begin(), all_room_lights.end(), [](std::shared_ptr<Light> light) {
                            return !light->get_controlled_from_main_page();
                          }),
                          all_room_lights.end());

    for (auto light : all_room_lights) {
      if (light->get_state()) {
        light->turn_off(true);
      }
    }
  }
}

void Room::_send_room_state_update() {
  std::lock_guard<std::mutex> lock_guard(this->_send_room_status_update_mutex);
  NSPanelRoomStatus status;
  status.set_id(this->_id);
  status.set_name(this->_name);

  {
    std::lock_guard<std::mutex> lock_guard(this->_entities_pages_mutex);
    for (auto &entity_page : this->_entity_pages) {
      status.add_entity_page_ids(entity_page->get_id());
    }
  }

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

  bool any_light_entity_on = false;
  std::lock_guard<std::mutex> mutex_guard(this->_entities_pages_mutex);
  for (std::shared_ptr<RoomEntitiesPage> &entity_page : this->_entity_pages) {
    std::vector<std::shared_ptr<Light>> entities = entity_page->get_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    for (auto light : entities) {
      if (light->get_state() && light->get_controlled_from_main_page()) {
        any_light_entity_on = true;
        break;
      }
    }
  }

  for (std::shared_ptr<RoomEntitiesPage> &entities_page : this->_entity_pages) {
    for (auto &light : entities_page->get_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT)) {
      // Light is not controlled from main page, exclude it from calculations.
      if (!light->get_controlled_from_main_page()) {
        continue;
      }

      if ((any_light_entity_on && light->get_state()) || !any_light_entity_on) {
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
  status.set_num_table_lights(num_lights_table);
  status.set_num_ceiling_lights(num_lights_ceiling);
  status.set_num_table_lights_on(num_lights_table_on);
  status.set_num_ceiling_lights_on(num_lights_ceiling_on);

  if (num_lights_total > 0) {
    float average_kelvin = (float)total_kelvin_level_all / num_lights_total;
    average_kelvin -= MqttManagerConfig::get_settings().color_temp_min;
    uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
    if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
      kelvin_pct = 100 - kelvin_pct;
    }

    status.set_average_dim_level(total_light_level_all / num_lights_total);
    status.set_average_color_temperature(kelvin_pct);
  } else {
    status.set_average_dim_level(0);
    status.set_average_color_temperature(0);
  }

  if (num_lights_table_on > 0) {
    float average_kelvin = (float)total_kelvin_table / num_lights_table_on;
    average_kelvin -= MqttManagerConfig::get_settings().color_temp_min;
    uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
    if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
      kelvin_pct = 100 - kelvin_pct;
    }

    status.set_table_lights_dim_level(total_light_level_table / num_lights_table_on);
    status.set_table_lights_color_temperature_value(kelvin_pct);
  } else {
    // SPDLOG_TRACE("No table lights found, setting value to 0.");
    status.set_table_lights_dim_level(0);
    status.set_table_lights_color_temperature_value(0);
  }

  if (num_lights_ceiling_on > 0) {
    float average_kelvin = (float)total_kelvin_ceiling / num_lights_ceiling_on;
    average_kelvin -= MqttManagerConfig::get_settings().color_temp_min;
    uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
    if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
      kelvin_pct = 100 - kelvin_pct;
    }

    status.set_ceiling_lights_dim_level(total_light_level_ceiling / num_lights_ceiling_on);
    status.set_ceiling_lights_color_temperature_value(kelvin_pct);
  } else {
    // SPDLOG_TRACE("No ceiling lights found, setting value to 0.");
    status.set_ceiling_lights_dim_level(0);
    status.set_ceiling_lights_color_temperature_value(0);
  }

  SPDLOG_DEBUG("Room {}::{} average dim level: {}, average color temperature: {}. Num ceiling lights on: {}, Ceiling lights brightness: {}, Num table lights on: {}, Table lights brightness: {}", this->_id, this->_name, status.average_dim_level(), status.average_color_temperature(), num_lights_ceiling_on, status.ceiling_lights_dim_level(), num_lights_table_on, status.table_lights_dim_level());

  std::string protobuf_str_buffer;
  if (status.SerializeToString(&protobuf_str_buffer)) {
    MQTT_Manager::publish(this->_mqtt_state_topic, protobuf_str_buffer, true);
  } else {
    SPDLOG_ERROR("Failed to serialize room {}::{} protobuf state object. Will not publish.", this->_id, this->_name);
  }
}
