#include "home_assistant_light.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <cstdint>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

HomeAssistantLight::HomeAssistantLight(uint32_t light_id) : Light(light_id) {
  // Process Home Assistant specific details. General light data is loaded in the "Light" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantLight has not been rekognized as controlled by HOME_ASSISTANT. Will stop processing light.");
    return;
  }

  auto light = database_manager::database.get<database_manager::Light>(this->_id);
  this->_home_assistant_name = light.home_assistant_name;
  SPDLOG_DEBUG("Loaded light {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantLight::home_assistant_event_callback, this, _1));

  if (this->_home_assistant_name.rfind("light.", 0) == 0) {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT;
  } else if (this->_home_assistant_name.rfind("switch.", 0) == 0) {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_SWITCH;
  } else {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT;
    SPDLOG_ERROR("Unknown type of home assistant entity '{}'. Will assume light is a light and not a switch.", this->_home_assistant_name);
  }
}

void HomeAssistantLight::send_state_update_to_controller() {
  // SPDLOG_TRACE("Light {}::{} current mode  : {}", this->_id, this->_name, (int)this->_current_mode);
  // SPDLOG_TRACE("Light {}::{} requested mode: {}", this->_id, this->_name, (int)this->_requested_mode);
  // SPDLOG_TRACE("Light {}::{} current kelvin  : {}", this->_id, this->_name, this->_current_color_temperature);
  // SPDLOG_TRACE("Light {}::{} requested kelvin: {}", this->_id, this->_name, this->_requested_color_temperature);
  // SPDLOG_TRACE("Light {}::{} requested state: {}", this->_id, this->_name, this->_requested_state ? "on" : "off");
  // SPDLOG_TRACE("Light {}::{} current state: {}", this->_id, this->_name, this->_current_state ? "on" : "off");

  nlohmann::json service_data;
  service_data["type"] = "call_service";
  service_data["target"]["entity_id"] = this->_home_assistant_name;
  if (this->_home_assistant_light_type == MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT) {
    service_data["domain"] = "light";
    if (this->_requested_state) {
      service_data["service"] = "turn_on";
      if (MqttManagerConfig::get_settings().optimistic_mode()) {
        this->_current_state = true;
      }

      if (this->_requested_brightness != this->_current_brightness) {
        service_data["service_data"]["brightness_pct"] = this->_requested_brightness;
        if (MqttManagerConfig::get_settings().optimistic_mode()) {
          this->_current_brightness = this->_requested_brightness;
        }
      }

      // This is a turn on event and it currently off. Send kelvin if turn on behavior is to use color temp.
      if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT || (!this->_current_state && MqttManagerConfig::get_settings().light_turn_on_behavior() == MQTTManagerSettings_turn_on_behavior_color_temperature)) {
        service_data["service_data"]["kelvin"] = this->_requested_color_temperature;
        if (MqttManagerConfig::get_settings().optimistic_mode()) {
          this->_current_color_temperature = this->_requested_color_temperature;
        }
      }

      if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT && this->_requested_color_temperature != this->_current_color_temperature) {
        service_data["service_data"]["kelvin"] = this->_requested_color_temperature;
        if (MqttManagerConfig::get_settings().optimistic_mode()) {
          this->_current_color_temperature = this->_requested_color_temperature;
        }
      } else if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::RGB && this->_requested_hue != this->_current_hue || this->_requested_saturation != this->_current_saturation) {
        service_data["service_data"]["hs_color"] = {this->_requested_hue, this->_requested_saturation};
        if (MqttManagerConfig::get_settings().optimistic_mode()) {
          this->_current_hue = this->_requested_hue;
          this->_current_saturation = this->_requested_saturation;
        }
      }
    } else {
      service_data["service"] = "turn_off";
      if (MqttManagerConfig::get_settings().optimistic_mode()) {
        this->_current_state = false;
      }
    }
  } else if (this->_home_assistant_light_type == MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_SWITCH) {
    service_data["domain"] = "switch";
    if (this->_requested_state) {
      service_data["service"] = "switch_on";
      if (MqttManagerConfig::get_settings().optimistic_mode()) {
        this->_current_state = true;
      }
    } else {
      service_data["service"] = "switch_off";
      if (MqttManagerConfig::get_settings().optimistic_mode()) {
        this->_current_state = false;
      }
    }
  }
  HomeAssistantManager::send_json(service_data);

  if (MqttManagerConfig::get_settings().optimistic_mode()) {
    this->_entity_changed_callbacks(this);
  }
}

void HomeAssistantLight::home_assistant_event_callback(nlohmann::json data) {
  if (std::string(data["event"]["event_type"]).compare("state_changed") == 0) {
    if (std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) == 0) {
      SPDLOG_DEBUG("Got event update for HA light {}::{}.", this->_id, this->_name);
      nlohmann::json new_state_data = data["event"]["data"]["new_state"];
      nlohmann::json new_state_attributes = new_state_data["attributes"];
      bool changed_attribute = false;

      if (new_state_data.contains("state") && !new_state_data["state"].is_null()) {
        try {
          std::string new_state = new_state_data["state"];
          if (new_state.compare("on") == 0) {
            if (!this->_current_state) {
              changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
            }
            this->_current_state = true;
            this->_requested_state = true;
            if (this->_can_dim) {
              if (new_state_attributes.contains("brightness")) {
                uint8_t new_brightness = 0;
                if (!new_state_attributes["brightness"].is_null()) {
                  new_brightness = new_state_attributes["brightness"];
                }
                new_brightness = ((float)new_brightness / 255.0) * 100; // Home assistant sends brightness as 0 to 255. We want percentage (0-100)
                if (new_brightness != this->_current_brightness) {
                  changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
                }
                this->_current_brightness = new_brightness;
                this->_requested_brightness = new_brightness;
                MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness), true);
              } else {
                // Light can dim but no brightness was given in update. Fallback to 100%.
                SPDLOG_ERROR("Got new event data for light {}::{}. Light is configured for brightness (dimmable) but no 'brightness' value was available in entity payload.", this->_id, this->_name);
                if (this->_current_brightness != 100) {
                  changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
                }
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
                this->_current_brightness = 100;
                this->_requested_brightness = 100;
                MQTT_Manager::publish(this->_mqtt_brightness_topic, "100", true);
              }
            } else {
              // We will never get a state_changed event from HA that the requested brightness has been set as this is a switch.
              if (this->_current_state) {
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
              }
              this->_current_brightness = 100;
              this->_requested_brightness = 100;
              MQTT_Manager::publish(this->_mqtt_brightness_topic, "100", true);
            }
          } else if (new_state.compare("off") == 0) {
            if (this->_current_state) {
              changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
            }
            this->_current_state = false;
            this->_requested_state = false;
            this->_requested_brightness = 0;
            MQTT_Manager::publish(this->_mqtt_brightness_topic, "0", true);
          } else {
            SPDLOG_ERROR("Unknown entity state: {}", new_state);
          }
        } catch (std::exception &e) {
          SPDLOG_ERROR("Caught exception when trying to update state for light {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
        }
      }

      if (new_state_attributes.contains("color_mode") && !new_state_attributes["color_mode"].is_null()) {
        try {
          std::string new_color_mode = new_state_attributes["color_mode"];
          if (this->_can_color_temperature && new_color_mode.compare("color_temp") == 0) {
            this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
            this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
            if (new_state_attributes.contains("color_temp_kelvin") && !new_state_attributes["color_temp_kelvin"].is_null()) {
              if (new_state_attributes["color_temp_kelvin"] != this->_current_color_temperature) {
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
              }
              this->_current_color_temperature = new_state_attributes["color_temp_kelvin"];
              this->_requested_color_temperature = this->_current_color_temperature;
              MQTT_Manager::publish(this->_mqtt_kelvin_topic, std::to_string(this->_current_color_temperature), true);
            }
          } else if (new_color_mode.compare("onoff") == 0) {
            // Light does not support colors. Do nothing.
          } else if (this->_can_rgb) {
            this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
            this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
            if (new_state_attributes.contains("hs_color") && !new_state_attributes["hs_color"].is_null()) {
              std::vector<float> hs_color = new_state_attributes["hs_color"];

              if ((uint16_t)hs_color[0] != this->_current_hue || (uint8_t)hs_color[1]) {
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
              }

              this->_current_hue = (uint16_t)hs_color[0];
              this->_requested_hue = this->_current_hue;
              this->_current_saturation = (uint8_t)hs_color[1];
              this->_requested_saturation = this->_current_saturation;
              MQTT_Manager::publish(this->_mqtt_hue_topic, std::to_string(this->_current_hue), true);
              MQTT_Manager::publish(this->_mqtt_saturation_topic, std::to_string(this->_current_saturation), true);
            }
          }
        } catch (std::exception &e) {
          SPDLOG_ERROR("Caught exception when trying to update color mode for light {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
        }
      }

      if (changed_attribute) {
        this->_signal_entity_changed();
      }
    }
  }
}

HomeAssistantLight::~HomeAssistantLight() {}
