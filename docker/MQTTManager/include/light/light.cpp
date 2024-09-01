#include "light.hpp"
#include "entity_manager/entity_manager.hpp"
#include <cstdint>
#include <entity/entity.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

Light::Light(nlohmann::json &init_data) {
  this->_id = init_data["id"];
  this->_room_id = init_data["room_id"];

  // Build MQTT Topics
  std::string mqtt_base_topic = "nspanel/entities/light/";
  mqtt_base_topic.append(std::to_string(this->_id));
  mqtt_base_topic.append("/");
  this->_mqtt_brightness_topic = std::string(mqtt_base_topic);
  this->_mqtt_brightness_topic.append("state_brightness_pct");
  this->_mqtt_kelvin_topic = std::string(mqtt_base_topic);
  this->_mqtt_kelvin_topic.append("state_kelvin");
  this->_mqtt_hue_topic = std::string(mqtt_base_topic);
  this->_mqtt_hue_topic.append("state_hue");
  this->_mqtt_saturation_topic = std::string(mqtt_base_topic);
  this->_mqtt_saturation_topic.append("state_sat");

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

  this->update_config(init_data);
  SPDLOG_DEBUG("Light {}::{} base loaded, can dim: {}, can color temp: {}, can_rgb: {}.", this->_id, this->_name, this->_can_dim ? "yes" : "no", this->_can_color_temperature ? "yes" : "no", this->_can_rgb ? "yes" : "no");
}

void Light::update_config(nlohmann::json &init_data) {
  if (this->_name.compare(init_data["name"]) != 0) {
    this->_name = init_data["name"];
    SPDLOG_DEBUG("Loading light {}::{}.", this->_id, this->_name);

    if (std::string(init_data["light_type"]).compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else if (std::string(init_data["light_type"]).compare("openhab") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
    } else {
      SPDLOG_ERROR("Got unknown type ({}) for light {}::{}. Will default to HOME_ASSISTANT.", std::string(init_data["type"]), this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }

    this->_can_dim = init_data["can_dim"];
    this->_can_color_temperature = init_data["can_color_temperature"];
    this->_can_rgb = init_data["can_rgb"];

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

void Light::post_init() {
  Room *room_entity = EntityManager::get_entity_by_id<Room>(MQTT_MANAGER_ENTITY_TYPE::ROOM, this->_room_id);
  if (room_entity != nullptr) {
    this->_room = room_entity;
    this->_room->attach_entity(this);
    SPDLOG_INFO("Attaching light {}::{} to room {}::{}", this->_id, this->_name, this->_room->get_id(), this->_room->get_name());
  } else {
    SPDLOG_ERROR("Found no room with ID: {}", this->_room_id);
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
}

void Light::reset_requests() {
  this->_requested_hue = this->_current_hue;
  this->_requested_state = this->_current_state;
  this->_requested_brightness = this->_current_brightness;
  this->_requested_saturation = this->_current_saturation;
  this->_requested_color_temperature = this->_current_color_temperature;
}
