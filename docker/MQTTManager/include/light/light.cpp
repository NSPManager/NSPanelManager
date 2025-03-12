#include "light.hpp"
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
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

Light::Light(uint32_t light_id) {
  this->_id = light_id;
  this->reload_config();

  // Build MQTT Topics
  std::string mqtt_base_topic = fmt::format("nspanel/entities/light/{}/", this->_id);
  this->_current_state = false;
  this->_current_brightness = 50;
  this->_current_color_temperature = 2500;
  this->_current_saturation = 50;
  this->_current_hue = 50;

  this->_requested_state = false;
  this->_requested_brightness = 50;
  this->_requested_color_temperature = 2500;
  this->_requested_saturation = 50;
  this->_requested_hue = 50;
  CommandManager::attach_callback(boost::bind(&Light::command_callback, this, _1));

  SPDLOG_DEBUG("Light {}::{} base loaded, can dim: {}, can color temp: {}, can_rgb: {}. Controlled from main page? {}. Entities page ID {} in slot {}.", this->_id, this->_name, this->_can_dim ? "yes" : "no", this->_can_color_temperature ? "yes" : "no", this->_can_rgb ? "yes" : "no", this->_controlled_from_main_page ? "Yes" : "No", this->_entity_page_id, this->_entity_page_slot);
}

MQTT_MANAGER_LIGHT_TYPE Light::get_light_type() {
  return this->_light_type;
}

uint16_t Light::get_room_id() {
  return this->_room_id;
}

void Light::reload_config() {
  auto light = database_manager::database.get<database_manager::Light>(this->_id);
  this->_name = light.friendly_name;
  SPDLOG_DEBUG("Loading light {}::{}.", this->_id, this->_name);

  this->_room_id = light.room_id;
  this->_entity_page_id = light.entities_page_id;
  this->_entity_page_slot = light.room_view_position;
  this->_controlled_from_main_page = light.controlled_by_nspanel_main_page;

  if (light.is_ceiling_light) {
    this->_light_type = MQTT_MANAGER_LIGHT_TYPE::CEILING;
  } else {
    this->_light_type = MQTT_MANAGER_LIGHT_TYPE::TABLE;
  }

  if (std::string(light.type).compare("home_assistant") == 0) {
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  } else if (std::string(light.type).compare("openhab") == 0) {
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
  } else {
    SPDLOG_ERROR("Got unknown type ({}) for light {}::{}. Will default to HOME_ASSISTANT.", std::string(light.type), this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }

  this->_can_dim = light.can_dim;
  this->_can_color_temperature = light.can_color_temperature;
  this->_can_rgb = light.can_rgb;

  if (this->_can_dim) {
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
    this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
  } else if (this->_can_rgb) {
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
    this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
  } else {
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT; // Normal mode. Don't do anything special with light change request.
    this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
  }
}

MQTT_MANAGER_ENTITY_TYPE Light::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::LIGHT;
}

MQTT_MANAGER_ENTITY_CONTROLLER Light::get_controller() {
  return this->_controller;
}

uint16_t Light::get_id() {
  return this->_id;
}

std::string Light::get_name() {
  return this->_name;
}

bool Light::get_controlled_from_main_page() {
  return this->_controlled_from_main_page;
}

uint32_t Light::get_entity_page_id() {
  return this->_entity_page_id;
}

uint8_t Light::get_entity_page_slot() {
  return this->_entity_page_slot;
}

MQTT_MANAGER_LIGHT_MODE Light::get_mode() {
  return this->_current_mode;
}

void Light::turn_on(bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} be turned on.", this->_id, this->_name);
  this->_requested_state = true;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

void Light::turn_off(bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} be turned off.", this->_id, this->_name);
  this->_requested_state = false;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

bool Light::get_state() {
  return this->_current_state;
}

void Light::set_brightness(uint8_t brightness, bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} to be brightness {}.", this->_id, this->_name, brightness);
  if (brightness < 0) {
    brightness = 0;
  } else if (brightness > 100) {
    brightness = 100;
  }
  this->_requested_brightness = brightness;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

uint8_t Light::get_brightness() {
  return this->_current_brightness;
}

void Light::set_color_temperature(uint color_temperature, bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} to be color temperature {}.", this->_id, this->_name, color_temperature);
  this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
  this->_requested_color_temperature = color_temperature;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

uint Light::get_color_temperature() {
  return this->_current_color_temperature;
}

void Light::set_hue(uint16_t hue, bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} to be hue {}.", this->_id, this->_name, hue);
  this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
  this->_requested_hue = hue;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

uint16_t Light::get_hue() {
  return this->_current_hue;
}

void Light::set_saturation(uint8_t saturation, bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} to be saturation {}.", this->_id, this->_name, saturation);
  this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
  this->_requested_saturation = saturation;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

uint8_t Light::get_saturation() {
  return this->_current_saturation;
}

void Light::set_hsb(uint16_t hue, uint8_t saturation, uint8_t brightness, bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} to be HSB {},{},{}.", this->_id, this->_name, hue, saturation, brightness);
  this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
  this->_requested_hue = hue;
  this->_requested_saturation = saturation;
  this->_requested_brightness = brightness;
  if (send_update) {
    this->send_state_update_to_controller();
  }
}

void Light::attach_delete_callback(void (*callback)(Light *)) {
  this->_light_destroyed_callbacks.connect(callback);
}

void Light::detach_delete_callback(void (*callback)(Light *)) {
  this->_light_destroyed_callbacks.disconnect(callback);
}

Light::~Light() {
  SPDLOG_DEBUG("Destructor for light {}::{} called.", this->_id, this->_name);
  this->_light_destroyed_callbacks(this);
  this->_signal_entity_destroyed();
  CommandManager::detach_callback(boost::bind(&Light::command_callback, this, _1));
}

void Light::reset_requests() {
  this->_requested_hue = this->_current_hue;
  this->_requested_state = this->_current_state;
  this->_requested_brightness = this->_current_brightness;
  this->_requested_saturation = this->_current_saturation;
  this->_requested_color_temperature = this->_current_color_temperature;
}

bool Light::can_dim() {
  return this->_can_dim;
}

bool Light::can_color_temperature() {
  return this->_can_color_temperature;
}

bool Light::can_rgb() {
  return this->_can_rgb;
}

void Light::command_callback(NSPanelMQTTManagerCommand &command) {
  if (command.has_light_command()) {
    // Check if this light ID is in command
    auto light_id = std::find_if(command.light_command().light_ids().begin(), command.light_command().light_ids().end(), [this](int32_t id) {
      return id == this->_id;
    });

    if (light_id != command.light_command().light_ids().end()) {
      NSPanelMQTTManagerCommand_LightCommand cmd = command.light_command();
      if (cmd.has_brightness()) {
        this->set_brightness(cmd.brightness(), false);
        if (cmd.brightness() > 0) {
          this->turn_on(false);
        } else {
          this->turn_off(false);
        }
      }
      if (cmd.has_color_temperature()) {
        // Convert color temperature (0-100) to actual color temperature in kelvin.
        uint32_t color_temperature_kelvin = cmd.color_temperature() * ((MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min) / 100);
        if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
          color_temperature_kelvin = MqttManagerConfig::get_settings().color_temp_max - color_temperature_kelvin;
        } else {
          color_temperature_kelvin += MqttManagerConfig::get_settings().color_temp_min;
        }
        this->set_color_temperature(color_temperature_kelvin, false);
      }
      if (cmd.has_hue()) {
        this->set_hue(cmd.hue(), false);
      }
      if (cmd.has_saturation()) {
        this->set_saturation(cmd.saturation(), false);
      }
      this->send_state_update_to_controller();
    }
  }
}

bool Light::can_toggle() {
  return true;
}

void Light::toggle() {
  if (this->_current_state) {
    this->turn_off(true);
  } else {
    this->turn_on(true);
  }
}
