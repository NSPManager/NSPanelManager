#include "openhab_light.hpp"
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>

OpenHabLight::OpenHabLight(nlohmann::json &init_data) : Light(init_data) {
  // Process OpenHAB specific details. General light data is loaded in the "Light" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB) {
    spdlog::error("OpenHabLight has not been recognized as controlled by OPENHAB. Will stop processing light.");
    return;
  }

  this->_openhab_name = init_data["openhab_name"];
  std::string openhab_control_mode = init_data["openhab_control_mode"];
  if (openhab_control_mode.compare("dimmer") == 0) {
    this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::DIMMER;
    this->_openhab_on_off_item = init_data["openhab_item_dimmer"];
  } else if (openhab_control_mode.compare("switch") == 0) {
    this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH;
    this->_openhab_on_off_item = init_data["openhab_item_switch"];
  } else {
    this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH;
    this->_openhab_on_off_item = init_data["openhab_item_switch"];
    spdlog::error("Got unknown OpenHAB control mode ({}) for light {}::{}. Will assume switch.", openhab_control_mode, this->_id, this->_name);
  }

  if (this->_can_color_temperature) {
    this->_openhab_item_color_temperature = init_data["openhab_item_color_temp"];
  }
  if (this->_can_rgb) {
    this->_openhab_item_rgb = init_data["openhab_itemrgb"];
  }

  spdlog::debug("Loaded light {}::{}.", this->_id, this->_name);
}

void OpenHabLight::turn_on() {
  spdlog::debug("Turning on HA light {}::{}", this->_id, this->_name);
}

void OpenHabLight::turn_off() {
  spdlog::debug("Turning off HA light {}::{}", this->_id, this->_name);
}

void OpenHabLight::set_brightness(uint8_t brightness) {
  spdlog::debug("Setting dim level for light {}::{} to {}", this->_id, this->_name, brightness);
}

void OpenHabLight::set_color_temperature(uint color_temperature) {
  spdlog::debug("Setting color temperatre for light {}::{} to {}", this->_id, this->_name, color_temperature);
}

void OpenHabLight::set_hsb(uint8_t hue, uint8_t saturation, uint8_t brightness) {
  spdlog::debug("Setting HSB for light {}::{} to {},{},{}", this->_id, this->_name, hue, saturation, brightness);
}
