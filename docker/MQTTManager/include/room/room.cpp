#include "command_manager/command_manager.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf/protobuf_general.pb.h"
#include "protobuf/protobuf_nspanel.pb.h"
#include "protobuf_mqttmanager.pb.h"
#include "scenes/scene.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <chrono>
#include <cstdint>
#include <iterator>
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
  this->_mqtt_status_topic = fmt::format("nspanel/mqttmanager_{}/room/{}/status", MqttManagerConfig::get_settings().manager_address(), this->_id);

  SPDLOG_TRACE("Room {}::{} initialized with status topic '{}'.", this->_id, this->_name, this->_mqtt_status_topic);

  this->_publish_protobuf_status();
}

MQTT_MANAGER_ENTITY_TYPE Room::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::ROOM;
}

MQTT_MANAGER_ENTITY_CONTROLLER Room::get_controller() {
  return MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
}

uint16_t Room::get_id() {
  return this->_id;
}

std::string Room::get_name() {
  return this->_name;
}

void Room::post_init() {
  // No post init tasks for room.
}

void Room::attach_entity(MqttManagerEntity *entity) {
  this->_entities.push_back(entity);
  entity->attach_entity_changed_callback(boost::bind(&Room::entity_changed_callback, this, _1));
}

void Room::detach_entity(MqttManagerEntity *entity) {
  this->_entities.remove(entity);
  entity->detach_entity_changed_callback(boost::bind(&Room::entity_changed_callback, this, _1));
}

void Room::entity_changed_callback(MqttManagerEntity *entity) {
  SPDLOG_TRACE("Entity with ID {} changed in room {}::{}, resending protobuf status.", entity->get_id(), this->_id, this->_name);
  if (this->_send_status_updates) {
    switch (entity->get_type()) {

    case LIGHT:
      this->_publish_protobuf_status();
      break;
    case SCENE:
      this->_publish_protobuf_status();
      break;
    case NSPANEL_RELAY_GROUP:
    case ROOM:
      break;
    }
  }
}

void Room::_publish_protobuf_status() {
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
  uint16_t num_lights_table = 0;

  bool any_light_entity_on = std::find_if(this->_entities.begin(), this->_entities.end(), [](MqttManagerEntity *e) {
                               return e->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT && ((Light *)e)->get_state();
                             }) != this->_entities.end();

  for (auto entity : this->_entities) {
    if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT) {
      Light *light = (Light *)entity;
      if ((any_light_entity_on && light->get_state()) || !any_light_entity_on) {
        total_light_level_all += light->get_brightness();
        total_kelvin_level_all += light->get_color_temperature();
        num_lights_total++;
      }
      switch (light->get_light_type()) {
      case MQTT_MANAGER_LIGHT_TYPE::TABLE:
        SPDLOG_TRACE("Found table light {}::{}, state: {}", light->get_id(), light->get_name(), light->get_state() ? "ON" : "OFF");
        if (light->get_state()) {
          total_light_level_table += light->get_brightness();
          total_kelvin_table += light->get_color_temperature();
          num_lights_table++;
        }
        break;
      case MQTT_MANAGER_LIGHT_TYPE::CEILING:
        SPDLOG_TRACE("Found ceiling light {}::{}, state: {}", light->get_id(), light->get_name(), light->get_state() ? "ON" : "OFF");
        if (light->get_state()) {
          total_light_level_ceiling += light->get_brightness();
          total_kelvin_ceiling += light->get_color_temperature();
          num_lights_ceiling++;
        }
        break;
      }

      // Build light status object
      NSPanelLightStatus *light_status = status.add_lights();
      light_status->set_id(light->get_id());
      light_status->set_name(light->get_name());
      light_status->set_can_dim(light->can_dim());
      light_status->set_can_color_temperature(light->can_color_temperature());
      light_status->set_can_rgb(light->can_rgb());
      light_status->set_light_level(light->get_brightness());
      light_status->set_color_temp(light->get_color_temperature());
      light_status->set_hue(light->get_hue());
      light_status->set_saturation(light->get_saturation());
    } else if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::SCENE) {
      Scene *scene = (Scene *)entity;
      NSPanelScene *scene_status = status.add_scenes();
      scene_status->set_scene_id(scene->get_id());
      scene_status->set_name(scene->get_name());
      scene_status->set_can_save(scene->can_save());
    }
  }

  if (num_lights_total > 0) {
    float average_kelvin = total_kelvin_level_all / num_lights_total;
    average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
    uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;

    status.set_average_dim_level(total_light_level_all / num_lights_total);
    status.set_average_color_temperature(kelvin_pct);
  } else {
    status.set_average_dim_level(0);
    status.set_average_color_temperature(0);
  }
  if (num_lights_table > 0) {
    float average_kelvin = total_kelvin_table / num_lights_table;
    average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
    uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;

    status.set_table_lights_dim_level(total_light_level_table / num_lights_table);
    status.set_table_lights_color_temperature_value(kelvin_pct);
  } else {
    SPDLOG_TRACE("No table lights found, setting value to 0.");
    status.set_table_lights_dim_level(0);
    status.set_table_lights_color_temperature_value(0);
  }
  if (num_lights_ceiling > 0) {
    float average_kelvin = total_kelvin_ceiling / num_lights_ceiling;
    average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
    uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;

    status.set_ceiling_lights_dim_level(total_light_level_ceiling / num_lights_ceiling);
    status.set_ceiling_lights_color_temperature_value(total_kelvin_ceiling / num_lights_ceiling);
  } else {
    SPDLOG_TRACE("No ceiling lights found, setting value to 0.");
    status.set_ceiling_lights_dim_level(0);
    status.set_ceiling_lights_color_temperature_value(0);
  }

  SPDLOG_TRACE("Kelvin all: {}", status.average_color_temperature());

  // Format and send
  std::string data;
  status.SerializeToString(&data);
  MQTT_Manager::publish(this->_mqtt_status_topic, data, true);
  this->_last_status_update = std::chrono::system_clock::now();
}

void Room::command_callback(NSPanelMQTTManagerCommand &command) {
  if (command.has_first_page_turn_on() && command.first_page_turn_on().selected_room() == this->_id && !command.first_page_turn_on().global()) {
    if (MqttManagerConfig::get_settings().optimistic_mode()) {
      this->_send_status_updates = false;
    }

    SPDLOG_DEBUG("Room {}:{} got command to turn lights on from first page.", this->_id, this->_name);
    std::vector<MqttManagerEntity *> lights_list;
    // Get all lights that are on
    std::copy_if(this->_entities.begin(), this->_entities.end(), std::back_inserter(lights_list), [command](MqttManagerEntity *e) {
      return e->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT && ((Light *)e)->get_state() &&
             (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_ALL ||
              (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_CEILING_LIGHTS && ((Light *)e)->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) ||
              (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_TABLE_LIGHTS && ((Light *)e)->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE));
    });
    if (lights_list.size() == 0) {
      // No lights that were on were found, get all lights in room.
      std::copy_if(this->_entities.begin(), this->_entities.end(), std::back_inserter(lights_list), [command](MqttManagerEntity *e) {
        return e->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT &&
               (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_ALL ||
                (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_CEILING_LIGHTS && ((Light *)e)->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) ||
                (command.first_page_turn_on().affect_lights() == NSPanelMQTTManagerCommand_AffectLightsOptions::NSPanelMQTTManagerCommand_AffectLightsOptions_TABLE_LIGHTS && ((Light *)e)->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE));
      });
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
      light_cmd->add_light_ids(((Light *)lights_list[i])->get_id());
      ((Light *)lights_list[i])->command_callback(cmd);
    }

    if (MqttManagerConfig::get_settings().optimistic_mode()) {
      this->_publish_protobuf_status();
      this->_send_status_updates = true;
    }
  } else if (command.has_first_page_turn_off()) {
    SPDLOG_DEBUG("Room {}:{} got command to turn lights off from first page.", this->_id, this->_name);
    std::vector<MqttManagerEntity *> lights_list;
    // Get all lights that are on
    std::copy_if(this->_entities.begin(), this->_entities.end(), std::back_inserter(lights_list), [](MqttManagerEntity *e) {
      return e->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT && ((Light *)e)->get_state();
    });

    for (int i = 0; i < lights_list.size(); i++) {
      // TODO: Rework so that the light can handle to command via command callback.
      ((Light *)lights_list[i])->turn_off(true);
    }
  }
}
