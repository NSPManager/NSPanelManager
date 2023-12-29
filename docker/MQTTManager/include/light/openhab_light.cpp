#include "openhab_light.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

uint64_t CurrentTimeMilliseconds() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

OpenhabLight::OpenhabLight(nlohmann::json &init_data) : Light(init_data) {
  // Process OpenHAB specific details. General light data is loaded in the "Light" constructor.
  //
  this->_last_light_mode_change = 0;
  this->_last_rgb_change = 0;
  this->_last_on_off_change = 0;
  this->_last_brightness_change = 0;
  this->_last_color_temp_change = 0;

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
    SPDLOG_ERROR("Got unknown OpenHAB control mode ({}) for light {}::{}. Will assume switch.", openhab_control_mode, this->_id, this->_name);
    this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH;
    this->_openhab_on_off_item = init_data["openhab_item_switch"];
  }

  OpenhabManager::attach_event_observer(this->_openhab_on_off_item, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  if (this->_can_color_temperature) {
    this->_openhab_item_color_temperature = init_data["openhab_item_color_temp"];
    OpenhabManager::attach_event_observer(this->_openhab_item_color_temperature, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  }
  if (this->_can_rgb) {
    this->_openhab_item_rgb = init_data["openhab_item_rgb"];
    OpenhabManager::attach_event_observer(this->_openhab_item_rgb, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  }

  SPDLOG_DEBUG("Loaded light {}::{}.", this->_id, this->_name);
}

void OpenhabLight::send_state_update_to_controller() {
  // SPDLOG_DEBUG("--- Sending light {}::{} event state ---", this->_id, this->_name);
  // SPDLOG_DEBUG("Current mode: {}", this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB ? "RGB" : "DEFAULT");
  // SPDLOG_DEBUG("Requested state: {}, current: {}", this->_requested_state, this->_current_state);
  // SPDLOG_DEBUG("Requested brightness: {}, current: {}", this->_requested_brightness, this->_current_brightness);
  // SPDLOG_DEBUG("Requested color_temperature: {}, current: {}", this->_requested_color_temperature, this->_current_color_temperature);
  // SPDLOG_DEBUG("Requested hue: {}, current: {}", this->_requested_hue, this->_current_hue);
  // SPDLOG_DEBUG("Requested saturation: {}, current: {}", this->_requested_saturation, this->_current_saturation);

  nlohmann::json service_data;
  service_data["type"] = "ItemCommandEvent";
  service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_on_off_item);
  nlohmann::json payload_data;
  payload_data["type"] = "Percent";

  if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH) {
    payload_data["type"] = "OnOff";
    payload_data["value"] = this->_requested_state ? "ON" : "OFF";
    service_data["payload"] = payload_data.dump();
    OpenhabManager::send_json(service_data);
    return; // The light is a switch, it can't do anything more than ON/OFF. Exit function early.
  }

  // Do not send brightness seperatly for rgb, this is included in the HSB values.
  if (!this->_requested_state) {
    if (this->_current_state) {
      SPDLOG_DEBUG("Setting light {}::{} to level: 0", this->_id, this->_name);
      payload_data["value"] = 0;
      service_data["payload"] = payload_data.dump();
      OpenhabManager::send_json(service_data);
    } else {
      SPDLOG_DEBUG("Light {}::{} is already off. Will not send update to openhab.");
    }
  } else if (this->_requested_state && this->_current_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT) {
    // Send light level:
    SPDLOG_DEBUG("Setting light {}::{} to level: {}", this->_id, this->_name, this->_requested_brightness);
    payload_data["value"] = this->_requested_brightness;
    service_data["payload"] = payload_data.dump();
    OpenhabManager::send_json(service_data);

    if (this->_can_color_temperature) {
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
      this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
      OpenhabManager::send_json(service_data);
    }
  } else if (this->_can_rgb && this->_requested_state && this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB) {
    SPDLOG_DEBUG("Setting light {}::{} to HSB: {},{},{}", this->_id, this->_name, this->_requested_hue, this->_requested_saturation, this->_requested_brightness);
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_item_rgb);
    payload_data["type"] = "HSB";
    payload_data["value"] = fmt::format("{},{},{}", this->_requested_hue, this->_requested_saturation, this->_requested_brightness);
    service_data["payload"] = payload_data.dump();
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
    OpenhabManager::send_json(service_data);
  }
}

void OpenhabLight::openhab_event_callback(nlohmann::json data) {
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    size_t pos = 0;
    std::string token;
    while ((pos = topic.find("/")) != std::string::npos) {
      token = topic.substr(0, pos);
      topic_parts.push_back(token);
      topic.erase(0, pos + 1); // Remove current part from beginning of topic string (including delimiter)
    }

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enought parts, topic: {}", std::string(data["topic"]));
      return;
    }

    std::string topic_item = topic_parts[2];
    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_on_off_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_brightness_change + 1000) {
        double brightness = 0;
        if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::DIMMER) {
          brightness = atof(std::string(payload["value"]).c_str());
          if (brightness < 0) {
            brightness = 0;
          } else if (brightness > 100) {
            brightness = 100;
          }
        } else if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH) {
          if (std::string(payload["value"]).compare("ON") == 0) {
            brightness = 100;
          } else {
            brightness = 0;
          }
        } else {
          SPDLOG_ERROR("Unknown openhab control mode for light {}::{}.", this->_id, this->_name);
        }
        this->_current_brightness = std::round(brightness);
        this->_requested_brightness = this->_current_brightness;

        if (this->_current_brightness <= 0) {
          this->_requested_state = false;
          this->_current_state = false;
        } else {
          this->_requested_state = true;
          this->_current_state = true;
        }

        SPDLOG_DEBUG("Light {}::{} got new brightness from Openhab, new brightness: {}", this->_id, this->_name, this->_current_brightness);
        MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
        this->_last_brightness_change = CurrentTimeMilliseconds();
      }
      this->_signal_entity_changed();
      return;
    } else if (topic_item.compare(this->_openhab_item_color_temperature) == 0 && (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT || CurrentTimeMilliseconds() >= this->_last_light_mode_change + 1000)) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_color_temp_change + 1000) {
        double color_temperature = 100 - atof(std::string(payload["value"]).c_str());
        if (color_temperature < 0) {
          color_temperature = 0;
        } else if (color_temperature > 100) {
          color_temperature = 100;
        }
        // Convert from percentage to actual color temp.
        double kelvin_max_floored = MqttManagerConfig::color_temp_max - MqttManagerConfig::color_temp_min;
        uint16_t kelvin = MqttManagerConfig::color_temp_min + int((color_temperature / (double)100) * kelvin_max_floored);

        this->_current_color_temperature = std::round(kelvin);
        this->_requested_color_temperature = this->_current_color_temperature;
        this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
        this->_last_color_temp_change = CurrentTimeMilliseconds();

        SPDLOG_DEBUG("Light {}::{} got new color temperature from Openhab, new value: {}", this->_id, this->_name, this->_current_color_temperature);
        MQTT_Manager::publish(this->_mqtt_kelvin_topic, std::to_string(this->_current_color_temperature));
      }
      this->_last_light_mode_change = CurrentTimeMilliseconds();
      this->_signal_entity_changed();
      return;
    } else if (topic_item.compare(this->_openhab_item_rgb) == 0 && (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB || CurrentTimeMilliseconds() >= this->_last_light_mode_change + 1000)) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_brightness_change + 1000 && CurrentTimeMilliseconds() >= this->_last_rgb_change + 1000) {
        std::string values = payload["value"];
        std::vector<std::string> payload_parts;
        size_t pos = 0;
        std::string token;
        while ((pos = values.find(",")) != std::string::npos) {
          token = values.substr(0, pos);
          payload_parts.push_back(token);
          values.erase(0, pos + 1); // Remove current part from beginning of topic string (including delimiter)
        }
        payload_parts.push_back(values); // Add last part of values to the vector manually as the string doesn't end with ,

        uint16_t hue = std::round(atof(payload_parts[0].c_str()));
        uint16_t saturation = std::round(atof(payload_parts[1].c_str()));
        uint16_t brightness = std::round(atof(payload_parts[2].c_str()));

        if (hue != this->_current_hue || hue != this->_requested_hue) {
          this->_current_hue = std::round(atof(payload_parts[0].c_str()));
          this->_requested_hue = this->_current_hue;
          MQTT_Manager::publish(this->_mqtt_hue_topic, std::to_string(this->_current_hue));
        }

        if (saturation != this->_current_saturation || saturation != this->_requested_saturation) {
          this->_current_saturation = std::round(atof(payload_parts[1].c_str()));
          this->_requested_saturation = this->_current_saturation;
          MQTT_Manager::publish(this->_mqtt_saturation_topic, std::to_string(this->_current_saturation));
        }

        if (brightness != this->_current_brightness || brightness != this->_requested_brightness) {
          this->_current_brightness = std::round(atof(payload_parts[2].c_str()));
          this->_requested_brightness = this->_current_brightness;
          MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
        }

        this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
        this->_last_rgb_change = CurrentTimeMilliseconds();
        this->_last_brightness_change = CurrentTimeMilliseconds();

        SPDLOG_DEBUG("Light {}::{} got new HSB from Openhab, new values: {},{},{}", this->_id, this->_name, this->_current_hue, this->_current_saturation, this->_current_brightness);
      }
      this->_last_light_mode_change = CurrentTimeMilliseconds();
      this->_signal_entity_changed();
      return;
    }
    return;
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_DEBUG("Light {}::{} Received new REST API state: {}", this->_id, this->_name, data.dump());
    if (this->_openhab_on_off_item.compare(data["payload"]["name"]) == 0) {
      if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::DIMMER) {
        this->_current_brightness = atoi(std::string(data["payload"]["state"]).c_str());
        if (this->_current_brightness > 100) {
          this->_current_brightness = 100;
        }
        if (this->_current_brightness > 0) {
          this->_current_state = true;
        } else {
          this->_current_state = false;
        }

        this->_requested_state = this->_current_state;
        this->_requested_brightness = this->_current_brightness;
        MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
      } else if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH) {
        this->_current_state = std::string(data["payload"]["state"]).compare("ON") == 0;
        if (this->_current_state) {
          this->_current_brightness = 100;
        } else {
          this->_current_brightness = 0;
        }

        this->_requested_state = this->_current_state;
        this->_requested_brightness = this->_current_brightness;
        MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
      }
    } else if (this->_openhab_item_color_temperature.compare(data["payload"]["name"]) == 0) {
      this->_current_color_temperature = atoi(std::string(data["payload"]["state"]).c_str());
      this->_requested_color_temperature = this->_current_color_temperature;
      MQTT_Manager::publish(this->_mqtt_kelvin_topic, std::to_string(this->_current_color_temperature));
    } else if (this->_openhab_item_rgb.compare(data["payload"]["name"]) == 0) {
      std::vector<std::string> hsb_parts;
      boost::split(hsb_parts, std::string(data["payload"]["state"]), boost::is_any_of(","));
      if (hsb_parts.size() >= 3) {
        this->_current_hue = atoi(hsb_parts[0].c_str());
        this->_current_saturation = atoi(hsb_parts[1].c_str());
        this->_current_brightness = atoi(hsb_parts[1].c_str());

        this->_requested_hue = this->_current_hue;
        this->_requested_saturation = this->_current_saturation;
        this->_requested_brightness = this->_current_brightness;
        MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
        MQTT_Manager::publish(this->_mqtt_hue_topic, std::to_string(this->_current_hue));
        MQTT_Manager::publish(this->_mqtt_saturation_topic, std::to_string(this->_current_saturation));
      } else {
        SPDLOG_ERROR("Failed to decode HSB value '{}' into 3 or more parts.", std::string(data["payload"]["state"]));
      }
    }
  } else {
    return;
  }
}

OpenhabLight::~OpenhabLight() {}
