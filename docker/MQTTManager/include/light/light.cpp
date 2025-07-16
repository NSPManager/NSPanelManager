#include "light.hpp"
#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <entity/entity_icons.hpp>
#include <google/protobuf/util/message_differencer.h>
#include <nlohmann/json.hpp>
#include <protobuf/protobuf_nspanel_entity.pb.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unistd.h>

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
  auto light = database_manager::database.get<database_manager::Entity>(this->_id);
  this->_name = light.friendly_name;
  SPDLOG_DEBUG("Loading light {}::{}.", this->_id, this->_name);

  this->_room_id = light.room_id;
  this->_entity_page_id = light.entities_page_id;
  this->_entity_page_slot = light.room_view_position;
  nlohmann::json entity_data = light.get_entity_data_json();

  if (entity_data.contains("controlled_from_main_page")) {
    this->_controlled_from_main_page = entity_data["controlled_from_main_page"];
  } else {
    this->_controlled_from_main_page = true;
  }

  if (entity_data.contains("is_ceiling_light") && entity_data["is_ceiling_light"] == true) {
    this->_light_type = MQTT_MANAGER_LIGHT_TYPE::CEILING;
  } else {
    this->_light_type = MQTT_MANAGER_LIGHT_TYPE::TABLE;
  }

  if (entity_data.contains("controller")) {
    std::string controller = entity_data["controller"];
    if (controller.compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else if (controller.compare("openhab") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
    } else {
      SPDLOG_ERROR("Got unknown type ({}) for light {}::{}. Will default to HOME_ASSISTANT.", controller, this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }
  } else {
    SPDLOG_ERROR("No controller defined for light {}::{}. Will default to HOME_ASSISTANT.", this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }

  if (entity_data.contains("can_dim")) {
    this->_can_dim = entity_data["can_dim"] == true;
  }
  if (entity_data.contains("can_color_temperature")) {
    this->_can_color_temperature = entity_data["can_color_temperature"] == true;
  }
  if (entity_data.contains("can_rgb")) {
    this->_can_rgb = entity_data["can_rgb"] == true;
  }

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

void Light::send_state_update_to_nspanel() {
  SPDLOG_TRACE("Sending state update to MQTT for light {}::{}", this->_id, this->_name);

  NSPanelEntityState state;
  NSPanelEntityState_Light *light_state = state.mutable_light();
  light_state->set_light_id(this->_id);
  light_state->set_name(this->_name);
  light_state->set_can_color_temp(this->_can_color_temperature);
  light_state->set_can_color(this->_can_rgb);

  light_state->set_brightness(this->_current_brightness);
  light_state->set_hue(this->_current_hue);
  light_state->set_saturation(this->_current_saturation);

  if (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT) {
    light_state->set_current_light_mode(NSPanelEntityState_Light_LightMode::NSPanelEntityState_Light_LightMode_COLOR_TEMP);
  } else if (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB) {
    light_state->set_current_light_mode(NSPanelEntityState_Light_LightMode::NSPanelEntityState_Light_LightMode_RGB);
  } else {
    light_state->set_current_light_mode(NSPanelEntityState_Light_LightMode::NSPanelEntityState_Light_LightMode_COLOR_TEMP);
    SPDLOG_ERROR("Unknown light mode! Will assume default mode.");
  }

  float kelvin_pct = (((float)this->_current_color_temperature - MqttManagerConfig::get_settings().color_temp_min) / (MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min)) * 100;
  if (MqttManagerConfig::get_settings().reverse_color_temperature_slider) {
    kelvin_pct = 100 - kelvin_pct;
  }
  light_state->set_color_temp(kelvin_pct);

  google::protobuf::util::MessageDifferencer differencer;
  if (!differencer.Equals(this->_last_entity_state, state)) {
    MQTT_Manager::publish_protobuf(this->get_mqtt_state_topic(), state, true);
    this->_last_entity_state = state;
  }
}

MQTT_MANAGER_LIGHT_MODE Light::get_mode() {
  return this->_current_mode;
}

void Light::turn_on(bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} be turned on.", this->_id, this->_name);
  this->_requested_state = true;
  if (send_update) {
    this->send_state_update_to_controller();

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_state = true;
    }
  }
}

void Light::turn_off(bool send_update) {
  SPDLOG_TRACE("Requested light {}::{} be turned off.", this->_id, this->_name);
  this->_requested_state = false;
  if (send_update) {
    this->send_state_update_to_controller();

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_state = false;
    }
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

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_brightness = brightness;
    }
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

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_color_temperature = color_temperature;
    }
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

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_hue = hue;
    }
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

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_saturation = saturation;
    }
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

    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_hue = hue;
      this->_current_saturation = saturation;
      this->_current_brightness = brightness;
    }
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
    // Apply default "turn on brightness" if it is requested to become 0 as that won't work.
    if (this->_requested_brightness == 0) {
      this->_requested_brightness = std::stoi(MqttManagerConfig::get_setting_with_default("light_turn_on_brightness", "50"));
    }
    this->turn_on(true);
  }
}

std::string_view Light::get_icon() {
  if (this->get_state()) {
    return EntityIcons::entity_icon_switch_on;
  } else {
    return EntityIcons::entity_icon_switch_off;
  }
}

uint16_t Light::get_icon_color() {
  if (this->get_state()) {
    return GUI_Colors::icon_color_on;
  } else {
    return GUI_Colors::icon_color_off;
  }
}

uint16_t Light::get_icon_active_color() {
  if (this->get_state()) {
    return GUI_Colors::icon_color_on;
  } else {
    return GUI_Colors::icon_color_off;
  }
}

std::string Light::get_mqtt_state_topic() {
  std::string manager_address = MqttManagerConfig::get_settings().manager_address;
  return fmt::format("nspanel/mqttmanager_{}/entities/lights/{}/state", manager_address, this->get_id());
}
