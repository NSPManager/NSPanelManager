#include "switch/switch.hpp"
#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <entity/entity_icons.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <unistd.h>

SwitchEntity::SwitchEntity(uint32_t light_id) {
  this->_id = light_id;
  this->reload_config();

  // Build MQTT Topics
  std::string mqtt_base_topic = fmt::format("nspanel/entities/light/{}/", this->_id);
  this->_current_state = false;
  this->_requested_state = false;
  CommandManager::attach_callback(boost::bind(&SwitchEntity::command_callback, this, _1));

  SPDLOG_DEBUG("Switch {}::{} base loaded.", this->_id, this->_name);
}

uint16_t SwitchEntity::get_room_id() {
  return this->_room_id;
}

void SwitchEntity::reload_config() {
  auto switch_entity = database_manager::database.get<database_manager::Entity>(this->_id);
  this->_name = switch_entity.friendly_name;
  SPDLOG_DEBUG("Loading switch {}::{}.", this->_id, this->_name);

  this->_room_id = switch_entity.room_id;
  this->_entity_page_id = switch_entity.entities_page_id;
  this->_entity_page_slot = switch_entity.room_view_position;

  nlohmann::json entity_data = switch_entity.get_entity_data_json();
  if (entity_data.contains("controller")) {
    std::string controller = entity_data["controller"];
    if (controller.compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else if (controller.compare("openhab") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
    }
    else if (controller.compare("homey") == 0)
    {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY;
    }
    else
    {
      SPDLOG_ERROR("Got unknown controller ({}) for light {}::{}. Will default to HOME_ASSISTANT.", std::string(controller), this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }
  } else {
    SPDLOG_ERROR("No controller defined for light {}::{}. Will default to HOME_ASSISTANT.", this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }
}

MQTT_MANAGER_ENTITY_TYPE SwitchEntity::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY;
}

MQTT_MANAGER_ENTITY_CONTROLLER SwitchEntity::get_controller() {
  return this->_controller;
}

uint16_t SwitchEntity::get_id() {
  return this->_id;
}

std::string SwitchEntity::get_name() {
  return this->_name;
}

uint32_t SwitchEntity::get_entity_page_id() {
  return this->_entity_page_id;
}

uint8_t SwitchEntity::get_entity_page_slot() {
  return this->_entity_page_slot;
}

void SwitchEntity::turn_on(bool send_update) {
  SPDLOG_TRACE("Requested switch {}::{} be turned on.", this->_id, this->_name);
  this->_requested_state = true;
  if (send_update) {
    this->send_state_update_to_controller();

    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_state = true;
    }
  }
}

void SwitchEntity::turn_off(bool send_update) {
  SPDLOG_TRACE("Requested switch {}::{} be turned off.", this->_id, this->_name);
  this->_requested_state = false;
  if (send_update) {
    this->send_state_update_to_controller();

    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_state = false;
    }
  }
}

bool SwitchEntity::get_state() {
  return this->_current_state;
}

void SwitchEntity::attach_delete_callback(void (*callback)(SwitchEntity *)) {
  this->_switch_destroyed_callbacks.connect(callback);
}

void SwitchEntity::detach_delete_callback(void (*callback)(SwitchEntity *)) {
  this->_switch_destroyed_callbacks.disconnect(callback);
}

SwitchEntity::~SwitchEntity() {
  SPDLOG_DEBUG("Destructor for switch {}::{} called.", this->_id, this->_name);
  this->_switch_destroyed_callbacks(this);
  this->_signal_entity_destroyed();
  CommandManager::detach_callback(boost::bind(&SwitchEntity::command_callback, this, _1));
}

void SwitchEntity::command_callback(NSPanelMQTTManagerCommand &command) {
}

bool SwitchEntity::can_toggle() {
  return true;
}

void SwitchEntity::toggle() {
  if (this->_current_state) {
    this->turn_off(true);
  } else {
    this->turn_on(true);
  }
}

std::string_view SwitchEntity::get_icon() {
  if (this->get_state()) {
    return EntityIcons::entity_icon_switch_on;
  } else {
    return EntityIcons::entity_icon_switch_off;
  }
}

uint16_t SwitchEntity::get_icon_color() {
  if (this->get_state()) {
    return GUI_Colors::icon_color_on;
  } else {
    return GUI_Colors::icon_color_off;
  }
}

uint16_t SwitchEntity::get_icon_active_color() {
  if (this->get_state()) {
    return GUI_Colors::icon_color_on;
  } else {
    return GUI_Colors::icon_color_off;
  }
}

std::string SwitchEntity::get_mqtt_state_topic() {
  return "";
}
