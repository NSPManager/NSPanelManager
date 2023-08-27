#include "openhab_light.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

OpenhabLight::OpenhabLight(nlohmann::json &init_data) : Light(init_data) {
  // Process OpenHAB specific details. General light data is loaded in the "Light" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB) {
    SPDLOG_ERROR("OpenhabLight has not been recognized as controlled by OPENHAB. Will stop processing light.");
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
    SPDLOG_ERROR("Got unknown OpenHAB control mode ({}) for light {}::{}. Will assume switch.", openhab_control_mode, this->_id, this->_name);
  }

  if (this->_can_color_temperature) {
    this->_openhab_item_color_temperature = init_data["openhab_item_color_temp"];
  }
  if (this->_can_rgb) {
    this->_openhab_item_rgb = init_data["openhab_itemrgb"];
  }

  SPDLOG_DEBUG("Loaded light {}::{}.", this->_id, this->_name);
}

void OpenhabLight::send_state_update_to_controller() {
  nlohmann::json service_data;
  service_data["type"] = "ItemCommandEvent";
  service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_on_off_item);
  nlohmann::json payload_data;
  payload_data["type"] = "Percent";
  if (this->_requested_state) {
    SPDLOG_DEBUG("Setting light {}::{} to level: {}", this->_id, this->_name, this->_requested_brightness);
    payload_data["value"] = this->_requested_brightness;
  } else {
    SPDLOG_DEBUG("Setting light {}::{} to level: 0", this->_id, this->_name);
    payload_data["value"] = 0;
  }
  service_data["payload"] = payload_data.dump();
  OpenhabManager::send_json(service_data);

  if (this->_can_color_temperature && this->_requested_state && this->_requested_color_temperature != this->_current_color_temperature) {
    // Calculate color temp percentage
    uint16_t kelvin_max_floored = MqttManagerConfig::color_temp_max - MqttManagerConfig::color_temp_min;
    uint16_t kelvin_floored = this->_requested_color_temperature - MqttManagerConfig::color_temp_min;
    uint8_t color_temp_percentage = 100 - int(((float)kelvin_floored / (float)kelvin_max_floored) * 100);
    if (color_temp_percentage > 100) {
      color_temp_percentage = 100;
    } else if (color_temp_percentage < 0) {
      color_temp_percentage = 0;
    }

    SPDLOG_DEBUG("Setting light {}::{} to color temp: {}%", this->_id, this->_name, color_temp_percentage);
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_item_color_temperature);
    payload_data["value"] = color_temp_percentage;
    service_data["payload"] = payload_data.dump();
    OpenhabManager::send_json(service_data);
  }
}

bool OpenhabLight::openhab_event_callback(nlohmann::json &data) {
  SPDLOG_DEBUG("Got openhab event: {}", data.dump());
  return false;
}
