#include "button/button.hpp"
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
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <unistd.h>

ButtonEntity::ButtonEntity(uint32_t light_id) {
  this->_id = light_id;
  this->reload_config();

  CommandManager::attach_callback(boost::bind(&ButtonEntity::command_callback, this, _1));

  SPDLOG_DEBUG("Button {}::{} base loaded.", this->_id, this->_name);
}

uint16_t ButtonEntity::get_room_id() {
  return this->_room_id;
}

void ButtonEntity::reload_config() {
  std::lock_guard<std::mutex> lock_guard(this->_entity_data_mutex);
  auto button_entity = database_manager::database.get<database_manager::Entity>(this->_id);
  this->_name = button_entity.friendly_name;
  SPDLOG_DEBUG("Loading button {}::{}.", this->_id, this->_name);

  this->_room_id = button_entity.room_id;
  this->_entity_page_id = button_entity.entities_page_id;
  this->_entity_page_slot = button_entity.room_view_position;

  nlohmann::json entity_data = button_entity.get_entity_data_json();
  this->_entity_data = entity_data;
  if (entity_data.contains("controller")) {
    std::string controller = entity_data["controller"];
    if (controller.compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else if (controller.compare("nspm") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
    } else {
      SPDLOG_ERROR("Got unknown controller ({}) for light {}::{}. Will default to HOME_ASSISTANT.", std::string(controller), this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }
  } else {
    SPDLOG_ERROR("No controller defined for light {}::{}. Will default to HOME_ASSISTANT.", this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }
}

MQTT_MANAGER_ENTITY_TYPE ButtonEntity::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::BUTTON;
}

MQTT_MANAGER_ENTITY_CONTROLLER ButtonEntity::get_controller() {
  return this->_controller;
}

uint16_t ButtonEntity::get_id() {
  return this->_id;
}

std::string ButtonEntity::get_name() {
  return this->_name;
}

uint32_t ButtonEntity::get_entity_page_id() {
  return this->_entity_page_id;
}

uint8_t ButtonEntity::get_entity_page_slot() {
  return this->_entity_page_slot;
}

void ButtonEntity::attach_delete_callback(void (*callback)(ButtonEntity *)) {
  this->_button_destroyed_callbacks.connect(callback);
}

void ButtonEntity::detach_delete_callback(void (*callback)(ButtonEntity *)) {
  this->_button_destroyed_callbacks.disconnect(callback);
}

ButtonEntity::~ButtonEntity() {
  SPDLOG_DEBUG("Destructor for button {}::{} called.", this->_id, this->_name);
  this->_button_destroyed_callbacks(this);
  this->_signal_entity_destroyed();
  CommandManager::detach_callback(boost::bind(&ButtonEntity::command_callback, this, _1));
}

void ButtonEntity::command_callback(NSPanelMQTTManagerCommand &command) {
}

bool ButtonEntity::can_toggle() {
  return true;
}

void ButtonEntity::toggle() {
  this->send_state_update_to_controller();
}

std::string_view ButtonEntity::get_icon() {
  return ""; // Show no icon
}

uint16_t ButtonEntity::get_icon_color() {
  return GUI_Colors::icon_color_off;
}

uint16_t ButtonEntity::get_icon_active_color() {
  return GUI_Colors::icon_color_off;
}

std::string ButtonEntity::get_mqtt_state_topic() {
  return "";
}
