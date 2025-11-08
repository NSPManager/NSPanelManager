#include "home_assistant_switch.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <cstdint>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <switch/switch.hpp>

HomeAssistantSwitch::HomeAssistantSwitch(uint32_t switch_id) : SwitchEntity(switch_id) {
  // Process Home Assistant specific details. General switch data is loaded in the "Switch" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantSwitch has not been recognized as controlled by HOME_ASSISTANT. Will stop processing switch.");
    return;
  }

  auto db_switch = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = db_switch.get_entity_data_json();
  if (entity_data.contains("home_assistant_name")) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No Home Assistant name defined for switch {}::{}", this->_id, this->_name);
  }
  SPDLOG_DEBUG("Loaded Switch {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantSwitch::home_assistant_event_callback, this, _1));
}

HomeAssistantSwitch::~HomeAssistantSwitch() {
  HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantSwitch::home_assistant_event_callback, this, _1));
}

void HomeAssistantSwitch::send_state_update_to_controller() {
  // SPDLOG_TRACE("Switch {}::{} current mode  : {}", this->_id, this->_name, (int)this->_current_mode);
  // SPDLOG_TRACE("Switch {}::{} requested mode: {}", this->_id, this->_name, (int)this->_requested_mode);
  // SPDLOG_TRACE("Switch {}::{} current kelvin  : {}", this->_id, this->_name, this->_current_color_temperature);
  // SPDLOG_TRACE("Switch {}::{} requested kelvin: {}", this->_id, this->_name, this->_requested_color_temperature);
  // SPDLOG_TRACE("Switch {}::{} requested state: {}", this->_id, this->_name, this->_requested_state ? "on" : "off");
  // SPDLOG_TRACE("Switch {}::{} current state: {}", this->_id, this->_name, this->_current_state ? "on" : "off");

  nlohmann::json service_data;
  service_data["type"] = "call_service";
  service_data["target"]["entity_id"] = this->_home_assistant_name;
  if (boost::starts_with(this->_home_assistant_name, "switch.")) {
    service_data["domain"] = "switch";
    if (this->_requested_state) {
      service_data["service"] = "turn_on";
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_current_state = true;
      }
    } else {
      service_data["service"] = "turn_off";
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_current_state = false;
      }
    }
    HomeAssistantManager::send_json(service_data);
  } else if (boost::starts_with(this->_home_assistant_name, "input_boolean.")) {
    service_data["domain"] = "input_boolean";
    if (this->_requested_state) {
      service_data["service"] = "turn_on";
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_current_state = true;
      }
    } else {
      service_data["service"] = "turn_off";
      if (MqttManagerConfig::get_settings().optimistic_mode) {
        this->_current_state = false;
      }
    }
    HomeAssistantManager::send_json(service_data);
  } else {
    SPDLOG_ERROR("Could not determin domain for switch entity {}::{}. HA Name: {}", this->_id, this->_name, this->_home_assistant_name);
  }

  if (MqttManagerConfig::get_settings().optimistic_mode) {
    this->_entity_changed_callbacks(this);
  }
}

void HomeAssistantSwitch::home_assistant_event_callback(nlohmann::json data) {
  if (std::string(data["event"]["event_type"]).compare("state_changed") == 0) {
    if (std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) == 0) {
      SPDLOG_DEBUG("Got event update for Home Assistant Switch {}::{}.", this->_id, this->_name);
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
          } else if (new_state.compare("off") == 0) {
            if (this->_current_state) {
              changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
            }
            this->_current_state = false;
            this->_requested_state = false;
          } else {
            SPDLOG_ERROR("Unknown entity state: {}", new_state);
          }
        } catch (std::exception &e) {
          SPDLOG_ERROR("Caught exception when trying to update state for home Assistant {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
        }
      }

      if (changed_attribute) {
        this->_signal_entity_changed();
      }
    }
  }
}
