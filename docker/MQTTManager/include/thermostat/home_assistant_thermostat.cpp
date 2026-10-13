#include "home_assistant_thermostat.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include "thermostat/thermostat.hpp"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <charconv>
#include <cstdint>
#include <gtest/gtest.h>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

HomeAssistantThermostat::HomeAssistantThermostat(uint32_t thermostat_id) : ThermostatEntity(thermostat_id) {
  // Process Home Assistant specific details. General thermostat data is loaded in the "ThermostatEntity" constructor.
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantThermostat has not been recognized as controlled by HOME_ASSISTANT. Will stop processing thermostat.");
    return;
  }

  nlohmann::json entity_data;
  try {
    auto thermostat = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = thermostat.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load thermostat {}: {}", this->_id, e.what());
    return;
  }

  if (entity_data.contains("home_assistant_name")) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No home assistant name defined for Thermostat {}::{}", this->_id, this->_name);
    return;
  }
  SPDLOG_DEBUG("Loaded thermostat {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantThermostat::home_assistant_event_callback, this, _1));

  if (this->_home_assistant_name.rfind("climate.", 0) != 0) {
    SPDLOG_ERROR("Unknown type of home assistant entity '{}'. Expected climate.", this->_home_assistant_name);
  }

  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

HomeAssistantThermostat::~HomeAssistantThermostat() {
  HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantThermostat::home_assistant_event_callback, this, _1));
}

void HomeAssistantThermostat::reload_config() {
  ThermostatEntity::reload_config();
  HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantThermostat::home_assistant_event_callback, this, _1));

  nlohmann::json entity_data;
  try {
    auto thermostat = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = thermostat.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load thermostat {}: {}", this->_id, e.what());
    return;
  }

  if (entity_data.contains("home_assistant_name")) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No home assistant name defined for Thermostat {}::{}", this->_id, this->_name);
    return;
  }

  // Reattach event observer in case entity has changed.
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantThermostat::home_assistant_event_callback, this, _1));
}

void HomeAssistantThermostat::send_state_update_to_controller() {
  if (this->_requested_mode != this->_current_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_hvac_mode"},
        {"service_data", {{"hvac_mode", this->_requested_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_fan_mode != this->_current_fan_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_fan_mode"},
        {"service_data", {{"fan_mode", this->_requested_fan_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_preset != this->_current_preset) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_preset_mode"},
        {"service_data", {{"preset_mode", this->_requested_preset.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_swing_mode != this->_current_swing_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_swing_mode"},
        {"service_data", {{"swing_mode", this->_requested_swing_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_swingh_mode != this->_current_swingh_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_swing_horizontal_mode"},
        {"service_data", {{"swing_horizontal_mode", this->_requested_swingh_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_temperature != this->_current_temperature) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_temperature"},
        {"service_data", {{"temperature", this->_requested_temperature}}}};
    HomeAssistantManager::send_json(command);
  }
}

void HomeAssistantThermostat::home_assistant_event_callback(nlohmann::json data) {
  if (!data.contains("event")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event' key.", this->_id, this->_name);
    return;
  } else if (!data["event"].contains("event_type")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event.event_type' key.", this->_id, this->_name);
    return;
  } else if (!data["event"].contains("data")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event.data' key.", this->_id, this->_name);
    return;
  } else if (!data["event"]["data"].contains("entity_id")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event.data.entity_id' key.", this->_id, this->_name);
    return;
  }

  if (std::string(data["event"]["event_type"]).compare("state_changed") == 0) {
    if (std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) == 0) {
      SPDLOG_DEBUG("Got event update for HA thermostat {}::{}.", this->_id, this->_name);
      nlohmann::json new_state_data = data["event"]["data"]["new_state"];
      nlohmann::json new_state_attributes = new_state_data["attributes"];
      bool changed_attribute = false;
      // SPDLOG_DEBUG("Event data new_state: {}", new_state_data.dump(4));

      try {
        if (new_state_data.contains("state") && !new_state_data["state"].is_null()) {
          std::string new_state = new_state_data["state"].get<std::string>();
          if (new_state.compare(this->_current_mode.value) != 0) {
            auto new_mode = std::find_if(this->_supported_modes.begin(), this->_supported_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_state) == 0;
            });
            if (new_mode != this->_supported_modes.end()) {
              this->_current_mode = *new_mode;
              this->_requested_mode = *new_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new state: {}", this->_id, this->_name, new_mode->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new state '{}' from HA but that HVAC mode is not supported.", this->_id, this->_name, new_state_data.at("state").get<std::string>());
            }
          }
        }

        if (new_state_attributes.contains("fan_mode") && !new_state_attributes["fan_mode"].is_null()) {
          std::string new_fan_mode = new_state_attributes.at("fan_mode").get<std::string>();
          if (new_fan_mode.compare(this->_current_fan_mode.value) != 0) {
            auto new_mode = std::find_if(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_fan_mode) == 0;
            });
            if (new_mode != this->_supported_fan_modes.end()) {
              this->_current_fan_mode = *new_mode;
              this->_requested_fan_mode = *new_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new fan mode: {}", this->_id, this->_name, new_mode->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new fan mode '{}' from HA but that fan mode is not supported.", this->_id, this->_name, new_fan_mode);
            }
          }
        }

        if (new_state_attributes.contains("preset_mode") && !new_state_attributes["preset_mode"].is_null()) {
          std::string new_preset_mode = new_state_attributes.at("preset_mode").get<std::string>();
          if (new_preset_mode.compare(this->_current_preset.value) != 0) {
            auto request_preset = std::find_if(this->_supported_presets.begin(), this->_supported_presets.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_preset_mode) == 0;
            });
            if (request_preset != this->_supported_presets.end()) {
              this->_current_preset = *request_preset;
              this->_requested_preset = *request_preset;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new preset mode: {}", this->_id, this->_name, request_preset->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new preset mode '{}' from HA but that preset mode is not supported.", this->_id, this->_name, new_preset_mode);
            }
          }
        }

        if (new_state_attributes.contains("swing_mode") && !new_state_attributes["swing_mode"].is_null()) {
          std::string new_swing_mode = new_state_attributes.at("swing_mode").get<std::string>();
          if (new_swing_mode.compare(this->_current_swing_mode.value) != 0) {
            auto request_swing_mode = std::find_if(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_swing_mode) == 0;
            });
            if (request_swing_mode != this->_supported_swing_modes.end()) {
              this->_current_swing_mode = *request_swing_mode;
              this->_requested_swing_mode = *request_swing_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new swing mode: {}", this->_id, this->_name, request_swing_mode->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new swing mode '{}' from HA but that swing mode is not supported.", this->_id, this->_name, new_swing_mode);
            }
          }
        }

        if (new_state_attributes.contains("swing_horizontal_mode") && !new_state_attributes["swing_horizontal_mode"].is_null()) {
          std::string new_swingh_mode = new_state_attributes.at("swing_horizontal_mode").get<std::string>();
          if (new_swingh_mode.compare(this->_current_swingh_mode.value) != 0) {
            auto request_swingh_mode = std::find_if(this->_supported_swingh_modes.begin(), this->_supported_swingh_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_swingh_mode) == 0;
            });
            if (request_swingh_mode != this->_supported_swingh_modes.end()) {
              this->_current_swingh_mode = *request_swingh_mode;
              this->_requested_swingh_mode = *request_swingh_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new swing mode: {}", this->_id, this->_name, new_swingh_mode);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new swing mode '{}' from HA but that swing mode is not supported.", this->_id, this->_name, new_swingh_mode);
            }
          }
        }

        if (this->_use_current_temperature && new_state_attributes.contains("current_temperature") && !new_state_attributes["current_temperature"].is_null()) {
          float temperature = new_state_attributes.at("current_temperature").get<float>();
          if (temperature != this->_current_temperature_sensor) {
            this->_current_temperature_sensor = temperature;
            this->_current_temperature_sensor_available = true;
            changed_attribute = true;
            SPDLOG_DEBUG("Thermostat {}::{} got new temperature sensor reading: {}", this->_id, this->_name, temperature);
          }
        }

        if (new_state_attributes.contains("temperature") && !new_state_attributes["temperature"].is_null()) {
          float temperature = new_state_attributes.at("temperature").get<float>();
          if (temperature != this->_current_temperature) {
            this->_current_temperature = temperature;
            this->_requested_temperature = temperature;
            changed_attribute = true;
            SPDLOG_DEBUG("Thermostat {}::{} got new target temperature: {}", this->_id, this->_name, temperature);
          }
        } else {
          SPDLOG_WARN("Received state update for {}::{} but update has no valid set temperature.", this->_id, this->_name);
        }
      } catch (std::exception &e) {
        SPDLOG_ERROR("Caught exception when trying to update state for light {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
      }

      if (changed_attribute) {
        this->reset_requests();
        this->send_state_update_to_nspanel();
        this->_signal_entity_changed();
      }
    }
  }
}
