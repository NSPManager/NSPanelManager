#include "openhab_light.hpp"
#include "database_manager/database_manager.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include "protobuf_general.pb.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

uint64_t CurrentTimeMilliseconds() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

OpenhabLight::OpenhabLight(uint32_t light_id) : Light(light_id) {
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

  auto light = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = light.get_entity_data_json();

  if (entity_data.contains("openhab_name")) {
    this->_openhab_name = entity_data["openhab_name"];
  } else {
    SPDLOG_ERROR("No openhab_name defined for light {}::{}", this->_id, this->_name);
  }

  if (entity_data.contains("openhab_control_mode")) {
    std::string openhab_control_mode = entity_data["openhab_control_mode"];
    if (openhab_control_mode.compare("dimmer") == 0) {
      this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::DIMMER;
      if (entity_data.contains("openhab_item_dimmer")) {
        this->_openhab_on_off_item = entity_data["openhab_item_dimmer"];
      } else {
        SPDLOG_ERROR("No openhab on/off/dimmer item defined for light {}::{}", this->_id, this->_name);
      }
    } else if (openhab_control_mode.compare("switch") == 0) {
      this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH;
      if (entity_data.contains("openhab_item_switch")) {
        this->_openhab_on_off_item = entity_data["openhab_item_switch"];
      } else {
        SPDLOG_ERROR("No openhab on/off item defined for light {}::{}", this->_id, this->_name);
      }
    } else {
      SPDLOG_ERROR("Got unknown OpenHAB control mode ({}) for light {}::{}. Will assume switch.", openhab_control_mode, this->_id, this->_name);
      this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH;
      if (entity_data.contains("openhab_item_switch")) {
        this->_openhab_on_off_item = entity_data["openhab_item_switch"];
      } else {
        SPDLOG_ERROR("No openhab on/off item defined for light {}::{}", this->_id, this->_name);
      }
    }
  } else {
    SPDLOG_ERROR("No OpenHAB control mode defined for light {}::{}. Will assume switch.", this->_id, this->_name);
    this->_openhab_control_mode = MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH;
    if (entity_data.contains("openhab_item_switch")) {
      this->_openhab_on_off_item = entity_data["openhab_item_switch"];
    } else {
      SPDLOG_ERROR("No openhab on/off item defined for light {}::{}", this->_id, this->_name);
    }
  }

  OpenhabManager::attach_event_observer(this->_openhab_on_off_item, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  if (this->_can_color_temperature) {
    if (entity_data.contains("openhab_item_color_temp")) {
      this->_openhab_item_color_temperature = entity_data["openhab_item_color_temp"];
      OpenhabManager::attach_event_observer(this->_openhab_item_color_temperature, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
    } else {
      SPDLOG_ERROR("No openhab color temperature item defined for light {}::{}", this->_id, this->_name);
    }
  }
  if (this->_can_rgb) {
    if (entity_data.contains("openhab_item_rgb")) {
      this->_openhab_item_rgb = entity_data["openhab_item_rgb"];
      OpenhabManager::attach_event_observer(this->_openhab_item_rgb, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
    } else {
      SPDLOG_ERROR("No openhab color item defined for light {}::{}", this->_id, this->_name);
    }
  }

  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
  SPDLOG_DEBUG("Loaded light {}::{}.", this->_id, this->_name);
}

OpenhabLight::~OpenhabLight() {
  OpenhabManager::detach_event_observer(this->_openhab_on_off_item, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  if (this->_can_color_temperature) {
    OpenhabManager::detach_event_observer(this->_openhab_item_color_temperature, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  }
  if (this->_can_rgb) {
    OpenhabManager::detach_event_observer(this->_openhab_item_rgb, boost::bind(&OpenhabLight::openhab_event_callback, this, _1));
  }
}

void OpenhabLight::send_state_update_to_controller() {
  std::lock_guard<std::mutex> lock_guard(this->_openhab_items_mutex);
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
    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_current_state = this->_requested_state;
      this->_last_on_off_change = CurrentTimeMilliseconds();
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
    return; // The light is a switch, it can't do anything more than ON/OFF. Exit function early.
  }

  // If the light is off but in RGB mode and the user has configured the lights to turn on in "color temp" mode, force it back to color temp mode.
  bool force_send_kelvin = false;
  if (this->_requested_state && !this->_current_state && MqttManagerConfig::get_settings().light_turn_on_behaviour == LightTurnOnBehaviour::COLOR_TEMPERATURE && this->_can_color_temperature && (this->_requested_mode != this->_current_mode && this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT)) {
    this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
    force_send_kelvin = true;
  }

  if (!this->_requested_state) {
    if (this->_current_state) {
      SPDLOG_DEBUG("Setting light {}::{} to level: 0", this->_id, this->_name);
      payload_data["value"] = 0;
      service_data["payload"] = payload_data.dump();
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_current_state = false;
        this->_last_on_off_change = CurrentTimeMilliseconds();
      }
      OpenhabManager::send_json(service_data);
    } else {
      SPDLOG_DEBUG("Light {}::{} is already off. Will not send update to openhab.", this->_id, this->_name);
    }
  } else if (this->_requested_state && this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT) { // Send new brightness and/or kelvin if they have changed.
    if (this->_requested_brightness != this->_current_brightness || this->_requested_state != this->_current_state) {
      SPDLOG_DEBUG("Setting light {}::{} to level: {}", this->_id, this->_name, this->_requested_brightness);
      payload_data["value"] = this->_requested_brightness;
      service_data["payload"] = payload_data.dump();
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_last_on_off_change = CurrentTimeMilliseconds();
        this->_last_brightness_change = CurrentTimeMilliseconds();
        this->_current_state = true;
        this->_current_brightness = this->_requested_brightness;
      }
      OpenhabManager::send_json(service_data);
    }

    if ((this->_can_color_temperature && this->_requested_color_temperature != this->_current_color_temperature) || force_send_kelvin) {
      // Calculate color temp percentage
      uint16_t kelvin_max_floored = MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min;
      uint16_t kelvin_floored = this->_requested_color_temperature - MqttManagerConfig::get_settings().color_temp_min;
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
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_last_color_temp_change = CurrentTimeMilliseconds();
        this->_last_light_mode_change = CurrentTimeMilliseconds(); // Make sure we do not go to "RGB" mode when Zigbee2Mqtt sends updated HSB values to reflect color temp value
        this->_current_color_temperature = this->_requested_color_temperature;
      }
      OpenhabManager::send_json(service_data);
    }
  } else if (this->_can_rgb && this->_requested_state && this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::RGB) {
    SPDLOG_DEBUG("Setting light {}::{} to HSB: {},{},{}", this->_id, this->_name, this->_requested_hue, this->_requested_saturation, this->_requested_brightness);
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_item_rgb);
    payload_data["type"] = "HSB";
    payload_data["value"] = fmt::format("{},{},{}", this->_requested_hue, this->_requested_saturation, this->_requested_brightness);
    service_data["payload"] = payload_data.dump();
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
    if (MqttManagerConfig::get_settings().optimistic_mode) {
      this->_last_rgb_change = CurrentTimeMilliseconds();
      this->_last_light_mode_change = CurrentTimeMilliseconds(); // Make sure we do not go to "color temp" mode when Zigbee2Mqtt sends updated color temp value to reflect HSB value
      this->_current_hue = this->_requested_hue;
      this->_current_saturation = this->_requested_saturation;
      this->_current_brightness = this->_requested_brightness;
    }
    OpenhabManager::send_json(service_data);
  }

  if (MqttManagerConfig::get_settings().optimistic_mode) {
    this->send_state_update_to_nspanel();
    this->_entity_changed_callbacks(this);
  }
}

void OpenhabLight::openhab_event_callback(nlohmann::json data) {
  std::lock_guard<std::mutex> lock_guard(this->_openhab_items_mutex);
  if (std::string(data["type"]).compare("GroupItemStateChangedEvent") == 0) {
    if (CurrentTimeMilliseconds() < this->_last_brightness_change + 1000 ||
        CurrentTimeMilliseconds() < this->_last_color_temp_change + 1000 ||
        CurrentTimeMilliseconds() < this->_last_rgb_change + 1000) {
      // Update received inside ignore time from last change. Simply ignore it.
      return;
    }

    // TODO: Set event in local variable and start thread to wait 1 second from last GroupItemStateChangedEvent before processing event.
    std::lock_guard<std::mutex> lock_guard(this->_openhab_group_item_state_changed_event_mutex);
    this->_last_group_item_state_changed_event_ms = CurrentTimeMilliseconds();
    nlohmann::json event_data = data;
    event_data["type"] = "GroupItemStateChangedEventFinal";
    this->_last_group_item_state_changed_event_data = event_data;

    if (!this->_openhab_group_item_state_changed_event_thread.joinable()) {
      this->_openhab_group_item_state_changed_event_thread = std::thread(&OpenhabLight::_openhab_group_item_state_changed_event_thread_func, this);
    }
  } else if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0 || std::string(data["type"]).compare("GroupItemStateChangedEventFinal") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent or GroupItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    std::string topic_item = topic_parts[2];
    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_on_off_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_brightness_change + 1000) {
        double brightness = 0;
        if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::DIMMER) {
          brightness = std::round(atof(std::string(payload["value"]).c_str()));
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
        SPDLOG_DEBUG("Light {}::{} got new brightness {}, current brightness: {}.", this->_id, this->_name, brightness, this->_current_brightness);
        if (brightness != this->_current_brightness || (!this->_current_state && brightness > 0)) {
          if (brightness != 0) { // Only update internal brightness if not zero as to remember last brightness when toggling light.
            this->_current_brightness = brightness;
            this->_requested_brightness = this->_current_brightness;
          }

          if (brightness <= 0) {
            this->_requested_state = false;
            this->_current_state = false;
          } else {
            this->_requested_state = true;
            this->_current_state = true;
          }

          SPDLOG_DEBUG("Light {}::{} got new brightness from Openhab, new brightness: {}", this->_id, this->_name, this->_current_brightness);
          this->_last_brightness_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
        }
      }
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
        unsigned long kelvin_max_floored = MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min;
        uint16_t kelvin = std::round(MqttManagerConfig::get_settings().color_temp_min + int((color_temperature / (double)100) * kelvin_max_floored));

        if (kelvin != this->_current_color_temperature) {
          this->_current_color_temperature = kelvin;
          this->_requested_color_temperature = this->_current_color_temperature;
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
          this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
          this->_last_color_temp_change = CurrentTimeMilliseconds();

          SPDLOG_DEBUG("Light {}::{} got new color temperature from Openhab, new value: {}", this->_id, this->_name, this->_current_color_temperature);
          this->_last_light_mode_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
        }
      }
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

        bool has_changed = false;
        uint16_t hue = std::round(atof(payload_parts[0].c_str()));
        uint16_t saturation = std::round(atof(payload_parts[1].c_str()));
        uint16_t brightness = std::round(atof(payload_parts[2].c_str()));

        if (hue != this->_current_hue || hue != this->_requested_hue) {
          this->_current_hue = hue;
          this->_requested_hue = this->_current_hue;
          has_changed = true;
        }

        if (saturation != this->_current_saturation || saturation != this->_requested_saturation) {
          this->_current_saturation = saturation;
          this->_requested_saturation = this->_current_saturation;
          has_changed = true;
        }

        if (brightness != this->_current_brightness || brightness != this->_requested_brightness) {
          this->_current_brightness = brightness;
          this->_requested_brightness = this->_current_brightness;
          has_changed = true;
        }

        if (has_changed) {
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
          this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
          this->_last_rgb_change = CurrentTimeMilliseconds();
          this->_last_brightness_change = CurrentTimeMilliseconds();

          SPDLOG_DEBUG("Light {}::{} got new HSB from Openhab, new values: {},{},{}", this->_id, this->_name, this->_current_hue, this->_current_saturation, this->_current_brightness);
          this->_last_light_mode_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
        }
      }
      return;
    }
    return;
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB light {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_on_off_item.compare(data["payload"]["name"]) == 0) {
      if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::DIMMER) {
        if (data["payload"]["state"].is_string()) { // OpenHAB always sends values as strings. We convert this to integer below.
          this->_current_brightness = std::stoi(data["payload"]["state"].get<std::string>());
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
        } else {
          SPDLOG_ERROR("Light {}::{} got OpenHAB data from OpenHAB via initial ItemStateFetched event for OpenHAB item {} though current state was not a string.", this->_id, this->_name, this->_openhab_on_off_item);
        }
      } else if (this->_openhab_control_mode == MQTT_MANAGER_OPENHAB_CONTROL_MODE::SWITCH) {
        if (data["payload"]["state"].is_string()) {
          this->_current_state = std::string(data["payload"]["state"]).compare("ON") == 0;
          if (this->_current_state) {
            this->_current_brightness = 100;
          } else {
            this->_current_brightness = 0;
          }

          this->_requested_state = this->_current_state;
          this->_requested_brightness = this->_current_brightness;
        } else {
          SPDLOG_ERROR("Light {}::{} got OpenHAB data from OpenHAB via initial ItemStateFetched event for OpenHAB item {} though current on/off state was neither on or off.", this->_id, this->_name, this->_openhab_on_off_item);
        }
      }
    } else if (this->_openhab_item_color_temperature.compare(data["payload"]["name"]) == 0) {
      if (data["payload"]["state"].is_number()) {
        double color_temperature = 100 - atof(std::string(data["payload"]["state"]).c_str());
        if (color_temperature < 0) {
          color_temperature = 0;
        } else if (color_temperature > 100) {
          color_temperature = 100;
        }
        // Convert from percentage to actual color temp.
        unsigned long kelvin_max_floored = MqttManagerConfig::get_settings().color_temp_max - MqttManagerConfig::get_settings().color_temp_min;
        uint16_t kelvin = MqttManagerConfig::get_settings().color_temp_min + int((color_temperature / (double)100) * kelvin_max_floored);

        this->_current_color_temperature = kelvin;
        this->_requested_color_temperature = this->_current_color_temperature;
      } else {
        SPDLOG_ERROR("Light {}::{} got OpenHAB data from OpenHAB via initial ItemStateFetched event for OpenHAB item {} though current color temperature state was not a string.", this->_id, this->_name, this->_openhab_item_color_temperature);
      }
    } else if (this->_openhab_item_rgb.compare(data["payload"]["name"]) == 0) {
      if (data["payload"]["state"].is_string()) {
        std::vector<std::string> hsb_parts;
        boost::split(hsb_parts, std::string(data["payload"]["state"]), boost::is_any_of(","));
        if (hsb_parts.size() >= 3) {
          this->_current_hue = atoi(hsb_parts[0].c_str());
          this->_current_saturation = atoi(hsb_parts[1].c_str());
          this->_requested_state = atoi(hsb_parts[2].c_str()) > 0;
          this->_current_state = this->_requested_state;
          // this->_current_brightness = atoi(hsb_parts[2].c_str());

          this->_requested_hue = this->_current_hue;
          this->_requested_saturation = this->_current_saturation;
          // Don't set/send out brightness from HSB when initially loading the light state.
          // this->_requested_brightness = this->_current_brightness;
          // MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness), true);
        } else {
          SPDLOG_ERROR("Failed to decode HSB value '{}' into 3 or more parts.", std::string(data["payload"]["state"]));
        }
      } else {
        SPDLOG_ERROR("Light {}::{} got OpenHAB data from OpenHAB via initial ItemStateFetched event for OpenHAB item {} though current color temperature state was not a string.", this->_id, this->_name, this->_openhab_item_color_temperature);
      }
    }
  } else {
    return;
  }
}

void OpenhabLight::_openhab_group_item_state_changed_event_thread_func() {
  for (;;) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> lock(this->_openhab_group_item_state_changed_event_mutex);
    if (CurrentTimeMilliseconds() - this->_last_group_item_state_changed_event_ms > 1000) {
      break;
    }
  }

  // Process event:
  SPDLOG_TRACE("Light {}::{} group event state changes has settled. Starting to process event...", this->_id, this->_name);
  std::lock_guard<std::mutex> lock(this->_openhab_group_item_state_changed_event_mutex);
  this->openhab_event_callback(this->_last_group_item_state_changed_event_data);
}
