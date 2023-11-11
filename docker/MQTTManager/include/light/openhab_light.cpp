#include "openhab_light.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
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

  if (this->_can_color_temperature) {
    this->_openhab_item_color_temperature = init_data["openhab_item_color_temp"];
  }
  if (this->_can_rgb) {
    this->_openhab_item_rgb = init_data["openhab_item_rgb"];
  }

  this->_update_item_values_from_openhab_rest();

  OpenhabManager::attach_event_observer(this);

  SPDLOG_DEBUG("Loaded light {}::{}.", this->_id, this->_name);
}

void OpenhabLight::send_state_update_to_controller() {
  nlohmann::json service_data;
  service_data["type"] = "ItemCommandEvent";
  service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_on_off_item);
  nlohmann::json payload_data;
  payload_data["type"] = "Percent";

  SPDLOG_DEBUG("--- Sending light {}::{} event state ---", this->_id, this->_name);
  SPDLOG_DEBUG("Current mode: {}", this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB ? "RGB" : "DEFAULT");
  // SPDLOG_DEBUG("Requested state: {}, current: {}", this->_requested_state, this->_current_state);
  // SPDLOG_DEBUG("Requested brightness: {}, current: {}", this->_requested_brightness, this->_current_brightness);
  SPDLOG_DEBUG("Requested color_temperature: {}, current: {}", this->_requested_color_temperature, this->_current_color_temperature);
  SPDLOG_DEBUG("Requested hue: {}, current: {}", this->_requested_hue, this->_current_hue);
  SPDLOG_DEBUG("Requested saturation: {}, current: {}", this->_requested_saturation, this->_current_saturation);

  // Do not send brightness seperatly for rgb, this is included in the HSB values.
  if (this->_requested_state) {
    SPDLOG_DEBUG("Setting light {}::{} to level: {}", this->_id, this->_name, this->_requested_brightness);
    payload_data["value"] = this->_requested_brightness;
    service_data["payload"] = payload_data.dump();
    OpenhabManager::send_json(service_data);
  } else if (!this->_requested_state) {
    SPDLOG_DEBUG("Setting light {}::{} to level: 0", this->_id, this->_name);
    payload_data["value"] = 0;
    service_data["payload"] = payload_data.dump();
    OpenhabManager::send_json(service_data);
  }

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
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
    OpenhabManager::send_json(service_data);
  } else if (this->_can_rgb && this->_requested_state && (this->_requested_hue != this->_current_hue || this->_requested_saturation != this->_current_saturation)) {
    SPDLOG_DEBUG("Setting light {}::{} to HSB: {},{},{}", this->_id, this->_name, this->_requested_hue, this->_requested_saturation, this->_requested_brightness);
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_item_rgb);
    payload_data["type"] = "HSB";
    payload_data["value"] = fmt::format("{},{},{}", this->_requested_hue, this->_requested_saturation, this->_requested_brightness);
    service_data["payload"] = payload_data.dump();
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
    OpenhabManager::send_json(service_data);
  }
}

bool OpenhabLight::openhab_event_callback(nlohmann::json &data) {
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
      return false;
    }

    std::string topic_item = topic_parts[2];
    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_on_off_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_brightness_change + 250) {
        double brightness = atof(std::string(payload["value"]).c_str());
        if (brightness < 0) {
          brightness = 0;
        } else if (brightness > 100) {
          brightness = 100;
        }
        this->_current_brightness = std::round(brightness);
        this->_requested_brightness = this->_current_brightness;

        if (this->_current_brightness <= 0) {
          this->_requested_state = false;
        } else {
          this->_requested_state = true;
        }

        SPDLOG_DEBUG("Light {}::{} got new brightness from Openhab, new brightness: {}", this->_id, this->_name, this->_current_brightness);
        MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
        this->_last_brightness_change = CurrentTimeMilliseconds();
      }
      return true;
    } else if (topic_item.compare(this->_openhab_item_color_temperature) == 0 && (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT || CurrentTimeMilliseconds() >= this->_last_light_mode_change + 1000)) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_color_temp_change + 250) {
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
      return true;
    } else if (topic_item.compare(this->_openhab_item_rgb) == 0 && (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB || CurrentTimeMilliseconds() >= this->_last_light_mode_change + 1000)) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_brightness_change + 250 || CurrentTimeMilliseconds() >= this->_last_rgb_change + 250) {
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
      return true;
    }
    return false;
  } else {
    return false;
  }
}

void OpenhabLight::_update_item_values_from_openhab_rest() {
  std::string brightness;
  std::string color_temperature;
  std::string rgb;

  brightness = this->_openhab_rest_get(this->_openhab_on_off_item);
  if (this->_can_color_temperature) {
    color_temperature = this->_openhab_rest_get(this->_openhab_item_color_temperature);
  }
  if (this->_can_rgb) {
    rgb = this->_openhab_rest_get(this->_openhab_item_rgb);
  }

  if (!brightness.empty()) {
    nlohmann::json data = nlohmann::json::parse(brightness);
    if (data.contains("state")) {
      this->_current_brightness = atoi(std::string(data["state"]).c_str());
      this->_current_state = this->_current_brightness > 0;

      SPDLOG_DEBUG("Got state {} and brightness {} from OpenHAB.", this->_current_state ? "ON" : "OFF", this->_current_brightness);
    } else {
      SPDLOG_ERROR("Brightness state from OpenHAB does not contain state attribute. JSON: {}", brightness);
    }
  } else {
    SPDLOG_ERROR("Failed to get current brightness of item {}. Will assume 0.", this->_openhab_on_off_item);
    this->_current_state = false;
    this->_current_brightness = 0;
  }

  if (this->_can_color_temperature && !color_temperature.empty()) {
    nlohmann::json data = nlohmann::json::parse(color_temperature);
    if (data.contains("state")) {
      this->_current_color_temperature = atoi(std::string(data["state"]).c_str());
      SPDLOG_DEBUG("Got color temperature {} from OpenHAB.", this->_current_color_temperature);
    } else {
      SPDLOG_ERROR("Color temperature state from OpenHAB does not contain state attribute. JSON: {}", color_temperature);
    }
  } else if (this->_can_color_temperature && color_temperature.empty()) {
    SPDLOG_ERROR("Failed to get current color temperature of item {}. Will assume 0.", this->_openhab_item_color_temperature);
    this->_current_color_temperature = 0;
  }

  if (this->_can_rgb && !rgb.empty()) {
    nlohmann::json data = nlohmann::json::parse(rgb);
    if (data.contains("state")) {
      std::string rgb_data = data["state"];

      // Split log message by colon to extract Hue, Saturation and Brightness
      std::vector<std::string> rgb_data_parts;
      size_t pos = 0;
      uint8_t count = 0;
      std::string token;
      while ((pos = rgb_data.find(",")) != std::string::npos && count < 2) {
        token = rgb_data.substr(0, pos);
        rgb_data_parts.push_back(token);
        rgb_data.erase(0, pos + 1); // Remove current part from beginning of topic string (including delimiter)
        count++;
      }
      rgb_data_parts.push_back(rgb_data);

      if (rgb_data_parts.size() >= 3) {
        this->_current_hue = atoi(rgb_data_parts[0].c_str());
        SPDLOG_DEBUG("Got hue {} from OpenHAB.", this->_current_hue);
        this->_current_saturation = atoi(rgb_data_parts[1].c_str());
        SPDLOG_DEBUG("Got saturation {} from OpenHAB.", this->_current_color_temperature);
      } else {
        SPDLOG_ERROR("Couldn't decode HSB value for light {}::{}. Got value: {}", this->_id, this->_name, rgb);
      }
    } else {
      SPDLOG_ERROR("HSB state from OpenHAB does not contain state attribute. JSON: {}", brightness);
    }
  } else if (this->_can_rgb && rgb.empty()) {
    SPDLOG_ERROR("Failed to get current HSB of item {}. Will assume 0.", this->_openhab_item_rgb);
    this->_current_hue = 0;
    this->_current_saturation = 0;
  }

  this->_requested_state = this->_current_state;
  this->_requested_brightness = this->_current_brightness;
  this->_requested_saturation = this->_current_saturation;
  this->_requested_color_temperature = this->_current_color_temperature;
  this->_requested_hue = this->_current_hue;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string OpenhabLight::_openhab_rest_get(std::string &item) {
  CURL *curl = curl_easy_init();
  CURLcode res;

  std::string response_data;
  std::string request_url = MqttManagerConfig::openhab_address;
  request_url.append("/rest/items/");
  request_url.append(item);
  SPDLOG_TRACE("Requesting openhab item state from: {}", request_url);

  std::string bearer_token = "Bearer ";
  bearer_token.append(MqttManagerConfig::openhab_access_token);

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, bearer_token.c_str());
  if (headers == NULL) {
    SPDLOG_ERROR("Failed to set bearer token header for OpenHAB light rest request.");
    return response_data;
  }
  headers = curl_slist_append(headers, "Content-type: application/json");
  if (headers == NULL) {
    SPDLOG_ERROR("Failed to set content-type header for OpenHAB light rest request.");
    return response_data;
  }

  curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  /* Perform the request, res will get the return code */
  res = curl_easy_perform(curl);
  long http_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  /* Check for errors */
  if (res != CURLE_OK || http_code != 200) {
    SPDLOG_ERROR("Failed to get data from OpenHAB api at '{}'. Got response: {}", request_url, response_data);
    return response_data;
  }

  /* always cleanup */
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  return response_data;
}

OpenhabLight::~OpenhabLight() {}
