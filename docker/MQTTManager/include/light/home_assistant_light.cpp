#include "home_assistant_light.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include <cstdint>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

HomeAssistantLight::HomeAssistantLight(nlohmann::json &init_data) : Light(init_data) {
  // Process Home Assistant specific details. General light data is loaded in the "Light" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantLight has not been rekognized as controlled by HOME_ASSISTANT. Will stop processing light.");
    return;
  }

  this->_home_assistant_name = init_data["home_assistant_name"];
  SPDLOG_DEBUG("Loaded light {}::{}.", this->_id, this->_name);

  if (this->_home_assistant_name.rfind("light.", 0) == 0) {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT;
  } else if (this->_home_assistant_name.rfind("switch.", 0) == 0) {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_SWITCH;
  } else {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT;
    SPDLOG_ERROR("Unknown type of home assistant entity '{}'. Will assume light is a light.", this->_home_assistant_name);
  }

  HomeAssistantManager::attach_event_observer(this);
}

void HomeAssistantLight::send_state_update_to_controller() {
  nlohmann::json service_data;
  service_data["type"] = "call_service";
  service_data["target"]["entity_id"] = this->_home_assistant_name;
  if (this->_home_assistant_light_type == MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT) {
    service_data["domain"] = "light";
    if (this->_requested_state) {
      service_data["service"] = "turn_on";

      if (this->_requested_brightness != this->_current_brightness) {
        service_data["service_data"]["brightness_pct"] = this->_requested_brightness;
      }

      if (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT && this->_requested_color_temperature != this->_current_color_temperature) {
        service_data["service_data"]["kelvin"] = this->_requested_color_temperature;
      } else if (this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB && this->_requested_hue != this->_current_hue || this->_requested_saturation != this->_current_saturation) {
        service_data["service_data"]["hs_color"] = {this->_requested_hue, this->_requested_saturation};
      }
    } else {
      service_data["service"] = "turn_off";
    }
  } else if (this->_home_assistant_light_type == MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_SWITCH) {
    service_data["domain"] = "switch";
    if (this->_requested_state) {
      service_data["service"] = "switch_on";
    } else {
      service_data["service"] = "switch_off";
    }
  }
  HomeAssistantManager::send_json(service_data);
}

bool HomeAssistantLight::home_assistant_event_callback(nlohmann::json &data) {
  if (std::string(data["event"]["event_type"]).compare("state_changed") == 0) {
    if (std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) == 0) {
      SPDLOG_DEBUG("Got event update for HA light {}::{}.", this->_id, this->_name);
      nlohmann::json new_state_data = data["event"]["data"]["new_state"];
      nlohmann::json new_state_attributes = new_state_data["attributes"];

      if (new_state_data.contains("state")) {
        std::string new_state = new_state_data["state"];
        if (new_state.compare("on") == 0) {
          this->_current_state = true;
          this->_requested_state = true;
          if (this->_can_dim) {
            if (new_state_attributes.contains("brightness")) {
              uint8_t new_brightness = new_state_attributes["brightness"];
              new_brightness = ((float)new_brightness / 255.0) * 100; // Home assistant sends brightness as 0 to 255. We want percentage (0-100)
              this->_current_brightness = new_brightness;
              this->_requested_brightness = new_brightness;
              MQTT_Manager::publish(this->_mqtt_brightness_topic, std::to_string(this->_current_brightness));
            } else {
              // Light can dim but no brightness was given in update. Fallback to 100%.
              this->_current_brightness = 100;
              this->_requested_brightness = 100;
              MQTT_Manager::publish(this->_mqtt_brightness_topic, "100", true);
            }
          } else {
            // We will never get a state_changed event from HA that the requested brightness has been set as this is a switch.
            this->_current_brightness = 100;
            this->_requested_brightness = 100;
            MQTT_Manager::publish(this->_mqtt_brightness_topic, "100", true);
          }
        } else if (new_state.compare("off") == 0) {
          this->_current_state = false;
          this->_requested_state = false;
          this->_requested_brightness = 0;
          MQTT_Manager::publish(this->_mqtt_brightness_topic, "0", true);
        } else {
          SPDLOG_ERROR("Unknown entity state: {}", new_state);
        }
      }

      if (new_state_attributes.contains("color_mode")) {
        std::string new_color_mode = new_state_attributes["color_mode"];
        if (this->_can_color_temperature && new_color_mode.compare("color_temp") == 0) {
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
          if (new_state_attributes.contains("color_temp_kelvin")) {
            this->_current_color_temperature = new_state_attributes["color_temp_kelvin"];
            this->_requested_color_temperature = this->_current_color_temperature;
            MQTT_Manager::publish(this->_mqtt_kelvin_topic, std::to_string(this->_current_color_temperature), true);
          }
        } else if (new_color_mode.compare("onoff") == 0) {
          // Light does not support colors. Do nothing.
        } else if (this->_can_rgb) {
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
          std::vector<float> hs_color = new_state_attributes["hs_color"];
          this->_current_hue = (uint16_t)hs_color[0];
          this->_requested_hue = this->_current_hue;
          this->_current_saturation = (uint8_t)hs_color[1];
          this->_requested_saturation = this->_current_saturation;
          MQTT_Manager::publish(this->_mqtt_hue_topic, std::to_string(this->_current_hue), true);
          MQTT_Manager::publish(this->_mqtt_saturation_topic, std::to_string(this->_current_saturation), true);
        }
      }
      return true;
    }
  }
  return false;
}

HomeAssistantLight::~HomeAssistantLight() {}
