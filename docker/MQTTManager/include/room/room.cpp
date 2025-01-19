#include "command_manager/command_manager.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf/protobuf_general.pb.h"
#include "protobuf/protobuf_nspanel.pb.h"
#include "protobuf_mqttmanager.pb.h"
#include "room/room_entities_page.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <chrono>
#include <cstdint>
#include <memory>
#include <netdb.h>
#include <nlohmann/json.hpp>
#include <room/room.hpp>
#include <spdlog/spdlog.h>
#include <string>

Room::Room(RoomSettings &config) {
  this->update_config(config);
  this->_send_status_updates = true;
  CommandManager::attach_callback(boost::bind(&Room::command_callback, this, _1));
}

Room::~Room() {
  CommandManager::detach_callback(boost::bind(&Room::command_callback, this, _1));
}

void Room::update_config(RoomSettings &config) {
  this->_id = config.id();
  this->_name = config.name();
  this->_mqtt_state_topic = fmt::format("nspanel/mqttmanager_{}/room/{}/status", MqttManagerConfig::get_settings().manager_address(), this->_id);

  SPDLOG_TRACE("Room {}::{} initialized with status topic '{}'.", this->_id, this->_name, this->_mqtt_state_topic);

  {
    std::lock_guard<std::mutex> mutex_guard(this->_entities_pages_mutex);
    for(uint32_t entity_page_id : config.entity_page_ids()) {
        this->_entity_pages.push_back(std::shared_ptr<RoomEntitiesPage>(new RoomEntitiesPage(entity_page_id, this)));
    }
    SPDLOG_DEBUG("Created {} RoomEntitiesPages for room {}::{}.", this->_entity_pages.size(), this->_id, this->_name);
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
        for(std::shared_ptr<RoomEntitiesPage> &page : this->_entity_pages) {
            page->post_init();
            page->attach_state_change_callback(boost::bind(&Room::page_changed_callback, this, _1));
            SPDLOG_DEBUG("Attached callbacks for all entities in RoomEntitiesPage {}.", page->get_id());
        }
    }
    this->_room_changed_callbacks(this);

    // if(this->_send_status_updates) {
        // this->_publish_protobuf_status();
    // }
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

void Room::page_changed_callback(RoomEntitiesPage *page) {
  if (this->_send_status_updates) {
      this->_send_room_state_update();
  }
}

void Room::command_callback(NSPanelMQTTManagerCommand &command) {
  if (command.has_first_page_turn_on() && (command.first_page_turn_on().selected_room() == this->_id || command.first_page_turn_on().global())) {
    if (MqttManagerConfig::get_settings().optimistic_mode()) {
      this->_send_status_updates = false;
    }

    std::vector<std::shared_ptr<Light>> all_room_lights = this->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    // Remove any light entities that are not controlled by first page.
    all_room_lights.erase(std::remove_if(all_room_lights.begin(), all_room_lights.end(), [](std::shared_ptr<Light> light) {
        return !light->get_controlled_from_main_page();
    }), all_room_lights.end());

    SPDLOG_DEBUG("Room {}:{} got command to turn lights on from first page.", this->_id, this->_name);
    std::vector<std::shared_ptr<Light>> lights_list;
    // Get all lights that are on
    for(auto light : all_room_lights) {
        if (light->get_state() && (
               (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_ALL ||
                (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_CEILING_LIGHTS && light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) ||
                (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_TABLE_LIGHTS && light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE)))) {
                    lights_list.push_back(light);
                }
    }
    if (lights_list.size() == 0) {
      // No lights that were on were found, get all lights in room.
      SPDLOG_DEBUG("No lights were on in room {}::{}. Will affect all lights in room instead.", this->_id, this->_name);
      for(auto light : all_room_lights) {
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

    if (MqttManagerConfig::get_settings().optimistic_mode()) {
        this->_send_room_state_update();
        this->_send_status_updates = true;
    }
  } else if (command.has_first_page_turn_off()) {
    SPDLOG_DEBUG("Room {}:{} got command to turn lights off from first page.", this->_id, this->_name);
    std::vector<std::shared_ptr<Light>> all_room_lights = this->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    // Remove any light entities that are not controlled by first page.
    all_room_lights.erase(std::remove_if(all_room_lights.begin(), all_room_lights.end(), [](std::shared_ptr<Light> light) {
        return !light->get_controlled_from_main_page();
    }), all_room_lights.end());

    for(auto light : all_room_lights) {
        if(light->get_state()) {
            light->turn_off(true);
        }
    }
  }
}

void Room::_send_room_state_update() {
    NSPanelRoomStatus status;
    status.set_id(this->_id);
    status.set_name(this->_name);

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
    for(std::shared_ptr<RoomEntitiesPage> &entity_page : this->_entity_pages) {
        std::vector<std::shared_ptr<Light>> entities = entity_page->get_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
        for(auto light : entities) {
            if(light->get_state() && light->get_controlled_from_main_page()) {
                any_light_entity_on = true;
                break;
            }
        }
    }

    for (std::shared_ptr<RoomEntitiesPage> &entities_page : this->_entity_pages) {
      for (auto &light : entities_page->get_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT)) {
          // Light is not controlled from main page, exclude it from calculations.
          if(!light->get_controlled_from_main_page()) {
              continue;
          }

          if ((any_light_entity_on && light->get_state()) || !any_light_entity_on) {
              total_light_level_all += light->get_brightness();
              total_kelvin_level_all += light->get_color_temperature();
              num_lights_total++;
          }
          if (light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE) {
              SPDLOG_TRACE("Room {}::{} found table light {}::{}, state: {}", this->_id, this->_name, light->get_id(), light->get_name(), light->get_state() ? "ON" : "OFF");
              num_lights_table++;
              if (light->get_state()) {
                  total_light_level_table += light->get_brightness();
                  total_kelvin_table += light->get_color_temperature();
                  num_lights_table_on++;
              }
          } else if (light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) {
              SPDLOG_TRACE("Room {}::{} found ceiling light {}::{}, state: {}", this->_id, this->_name, light->get_id(), light->get_name(), light->get_state() ? "ON" : "OFF");
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
      average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
      uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;
      if(MqttManagerConfig::get_settings().reverse_color_temperature_slider()) {
          kelvin_pct = 100 - kelvin_pct;
      }

      status.set_average_dim_level(total_light_level_all / num_lights_total);
      status.set_average_color_temperature(kelvin_pct);
    } else {
      status.set_average_dim_level(0);
      status.set_average_color_temperature(0);
    }

    SPDLOG_DEBUG("Room {}::{} average dim level: {}, average color temperature: {}.", this->_id, this->_name, status.average_dim_level(), status.average_color_temperature());

    if (num_lights_table_on > 0) {
      float average_kelvin = (float)total_kelvin_table / num_lights_table_on;
      average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
      uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;
      if(MqttManagerConfig::get_settings().reverse_color_temperature_slider()) {
          kelvin_pct = 100 - kelvin_pct;
      }

      status.set_table_lights_dim_level(total_light_level_table / num_lights_table_on);
      status.set_table_lights_color_temperature_value(kelvin_pct);
    } else {
      SPDLOG_TRACE("No table lights found, setting value to 0.");
      status.set_table_lights_dim_level(0);
      status.set_table_lights_color_temperature_value(0);
    }

    if (num_lights_ceiling_on > 0) {
      float average_kelvin = (float)total_kelvin_ceiling / num_lights_ceiling_on;
      average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
      uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;
      if(MqttManagerConfig::get_settings().reverse_color_temperature_slider()) {
          kelvin_pct = 100 - kelvin_pct;
      }

      status.set_ceiling_lights_dim_level(total_light_level_ceiling / num_lights_ceiling_on);
      status.set_ceiling_lights_color_temperature_value(kelvin_pct);
    } else {
      SPDLOG_TRACE("No ceiling lights found, setting value to 0.");
      status.set_ceiling_lights_dim_level(0);
      status.set_ceiling_lights_color_temperature_value(0);
    }

    SPDLOG_TRACE("Kelvin all lights in room {}::{}: {}", this->_id, this->_name, status.average_color_temperature());

    std::string protobuf_str_buffer;
    if(status.SerializeToString(&protobuf_str_buffer)) {
        MQTT_Manager::publish(this->_mqtt_state_topic, protobuf_str_buffer, true);
    } else {
        SPDLOG_ERROR("Failed to serialize room {}::{} protobuf state object. Will not publish.", this->_id, this->_name);
    }
}
