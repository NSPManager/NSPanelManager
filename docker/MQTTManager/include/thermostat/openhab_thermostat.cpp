#include "openhab_thermostat.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "thermostat/thermostat.hpp"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <cstdint>
#include <gtest/gtest.h>
#include <nlohmann/json_fwd.hpp>
#include <openhab_manager/openhab_manager.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

uint64_t CurrentTimeMilliseconds() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

OpenhabThermostat::OpenhabThermostat(uint32_t thermostat_id) : ThermostatEntity(thermostat_id) {
  // Process Home Assistant specific details. General thermostat data is loaded in the "ThermostatEntity" constructor.
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB) {
    SPDLOG_ERROR("OpenhabThermostat has not been recognized as controlled by OPENHAB. Will stop processing thermostat.");
    return;
  }

  SPDLOG_DEBUG("Loaded thermostat {}::{}", this->_id, this->_name);
  this->reload_config();                // Reload configuration actually load all openhab items and attach event observers.
  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

OpenhabThermostat::~OpenhabThermostat() {
  OpenhabManager::detach_event_observer(this->_openhab_target_temperature_item, boost::bind(&OpenhabThermostat::openhab_target_temperature_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_fan_mode_item, boost::bind(&OpenhabThermostat::openhab_fan_mode_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_mode_item, boost::bind(&OpenhabThermostat::openhab_mode_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_preset_item, boost::bind(&OpenhabThermostat::openhab_preset_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_swing_item, boost::bind(&OpenhabThermostat::openhab_swing_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_swingh_item, boost::bind(&OpenhabThermostat::openhab_swingh_event_callback, this, _1));
}

void OpenhabThermostat::reload_config() {
  ThermostatEntity::reload_config();

  // Detach existing event observers
  OpenhabManager::detach_event_observer(this->_openhab_target_temperature_item, boost::bind(&OpenhabThermostat::openhab_target_temperature_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_fan_mode_item, boost::bind(&OpenhabThermostat::openhab_fan_mode_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_mode_item, boost::bind(&OpenhabThermostat::openhab_mode_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_preset_item, boost::bind(&OpenhabThermostat::openhab_preset_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_swing_item, boost::bind(&OpenhabThermostat::openhab_swing_event_callback, this, _1));
  OpenhabManager::detach_event_observer(this->_openhab_swingh_item, boost::bind(&OpenhabThermostat::openhab_swingh_event_callback, this, _1));

  nlohmann::json entity_data;
  try {
    auto thermostat = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = thermostat.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load thermostat {}: {}", this->_id, e.what());
    return;
  }

  if (entity_data.contains("openhab_temperature_item")) {
    this->_openhab_target_temperature_item = entity_data["openhab_temperature_item"];
  } else {
    SPDLOG_ERROR("No openhab_temperature_item name defined for Thermostat {}::{}", this->_id, this->_name);
  }

  if (entity_data.contains("openhab_fan_mode_item")) {
    this->_openhab_fan_mode_item = entity_data["openhab_fan_mode_item"];
  } else {
    SPDLOG_WARN("No openhab_fan_mode_item name defined for Thermostat {}::{}", this->_id, this->_name);
  }

  if (entity_data.contains("openhab_hvac_mode_item")) {
    this->_openhab_mode_item = entity_data["openhab_hvac_mode_item"];
  } else {
    SPDLOG_WARN("No openhab_hvac_mode_item name defined for Thermostat {}::{}", this->_id, this->_name);
  }

  if (entity_data.contains("openhab_preset_item")) {
    this->_openhab_preset_item = entity_data["openhab_preset_item"];
  } else {
    SPDLOG_WARN("No openhab_preset_item name defined for Thermostat {}::{}", this->_id, this->_name);
  }

  if (entity_data.contains("openhab_swing_item")) {
    this->_openhab_swing_item = entity_data["openhab_swing_item"];
  } else {
    SPDLOG_WARN("No openhab_swing_item name defined for Thermostat {}::{}", this->_id, this->_name);
  }

  if (entity_data.contains("openhab_swingh_item")) {
    this->_openhab_swingh_item = entity_data["openhab_swingh_item"];
  } else {
    SPDLOG_WARN("No openhab_swingh_item name defined for Thermostat {}::{}", this->_id, this->_name);
  }

  SPDLOG_DEBUG("Thermostat {}::{} Attaching openhab target temp item: {}", this->_id, this->_name, this->_openhab_target_temperature_item);

  // Attach new event observers
  OpenhabManager::attach_event_observer(this->_openhab_target_temperature_item, boost::bind(&OpenhabThermostat::openhab_target_temperature_event_callback, this, _1));
  OpenhabManager::attach_event_observer(this->_openhab_fan_mode_item, boost::bind(&OpenhabThermostat::openhab_fan_mode_event_callback, this, _1));
  OpenhabManager::attach_event_observer(this->_openhab_mode_item, boost::bind(&OpenhabThermostat::openhab_mode_event_callback, this, _1));
  OpenhabManager::attach_event_observer(this->_openhab_preset_item, boost::bind(&OpenhabThermostat::openhab_preset_event_callback, this, _1));
  OpenhabManager::attach_event_observer(this->_openhab_swing_item, boost::bind(&OpenhabThermostat::openhab_swing_event_callback, this, _1));
  OpenhabManager::attach_event_observer(this->_openhab_swingh_item, boost::bind(&OpenhabThermostat::openhab_swingh_event_callback, this, _1));
}

void OpenhabThermostat::send_state_update_to_controller() {
  nlohmann::json service_data;
  service_data["type"] = "ItemCommandEvent";

  if (this->_requested_mode != this->_current_mode) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_mode_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_mode.value;
    payload_data["type"] = "String";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_mode = this->_requested_mode;
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
  }

  if (this->_requested_fan_mode != this->_current_fan_mode) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_fan_mode_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_fan_mode.value;
    payload_data["type"] = "String";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_fan_mode = this->_requested_fan_mode;
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
  }

  if (this->_requested_preset != this->_current_preset) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_preset_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_preset.value;
    payload_data["type"] = "String";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_preset = this->_requested_preset;
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
  }

  if (this->_requested_swing_mode != this->_current_swing_mode) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_swing_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_swing_mode.value;
    payload_data["type"] = "String";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_swing_mode = this->_requested_swing_mode;
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
  }

  if (this->_requested_swingh_mode != this->_current_swingh_mode) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_swingh_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_swingh_mode.value;
    payload_data["type"] = "String";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_swingh_mode = this->_requested_swingh_mode;
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
  }

  if (this->_requested_temperature != this->_current_temperature) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_target_temperature_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_temperature;
    payload_data["type"] = "Number";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_temperature = this->_requested_temperature;
      this->_entity_changed_callbacks(this);
    }
    OpenhabManager::send_json(service_data);
  }
}

void OpenhabThermostat::openhab_target_temperature_event_callback(nlohmann::json data) {
  SPDLOG_DEBUG("Thermostat {}::{} get temperature callback!", this->_id, this->_name);

  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    std::string topic_item = topic_parts[2];

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_target_temperature_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_target_temperature_change + 1000) {
        SPDLOG_DEBUG("Thermostat {}::{}, payload: {}", this->_id, this->_name, payload.dump());
        if (payload["value"].is_null()) { // Got state but state is NULL, ignore.
          return;
        } else if (payload["value"].is_string() && std::string(payload["value"]).compare("NULL") == 0) {
          return;
        } else if (payload["value"].is_object()) {
          return;
        }

        float target_temperature = std::round(atof(std::string(payload["value"]).c_str()));
        SPDLOG_DEBUG("Thermostat {}::{} got new temperature {}, current temperature: {}.", this->_id, this->_name, target_temperature, this->_current_temperature);
        if (target_temperature != this->_current_temperature) {
          this->_current_temperature = target_temperature;
          this->_requested_temperature = target_temperature;
          this->_last_target_temperature_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
        }
      }
    }
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB thermostat {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_target_temperature_item.compare(data["payload"]["name"]) == 0) {
      nlohmann::json payload = data["payload"];
      if (payload["state"].is_null()) { // Got state but state is NULL, ignore.
        return;
      } else if (payload["state"].is_string() && std::string(payload["state"]).compare("NULL") == 0) {
        return;
      } else if (payload["state"].is_object()) {
        return;
      }

      float target_temperature = std::round(atof(std::string(payload["state"]).c_str()));

      SPDLOG_DEBUG("Thermostat {}::{} got new temperature {}, current temperature: {}.", this->_id, this->_name, target_temperature, this->_current_temperature);
      if (target_temperature != this->_current_temperature) {
        this->_current_temperature = target_temperature;
        this->_requested_temperature = target_temperature;
        this->_last_target_temperature_change = CurrentTimeMilliseconds();
        this->send_state_update_to_nspanel();
        this->_signal_entity_changed();
      }
    }
  }
}

void OpenhabThermostat::openhab_fan_mode_event_callback(nlohmann::json data) {
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    std::string topic_item = topic_parts[2];

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_fan_mode_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_fan_mode_change + 1000) {
        if (payload["value"].is_null()) { // Got state but state is NULL, ignore.
          return;
        } else if (payload["value"].is_string() && std::string(payload["value"]).compare("NULL") == 0) {
          return;
        } else if (payload["value"].is_object()) {
          return;
        }

        std::string mode = std::string(payload["value"]);
        SPDLOG_DEBUG("Thermostat {}::{} got new fan mode {}, current fan mode: {}.", this->_id, this->_name, mode, this->_current_fan_mode.value);
        if (this->_current_fan_mode.value.compare(mode) != 0) {
          auto new_fan_mode = std::find_if(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), [&](const ThermostatOptionHolder &option) {
            return option.value.compare(mode) == 0;
          });
          if (new_fan_mode != this->_supported_fan_modes.end()) {
            this->_current_fan_mode = *new_fan_mode;
            this->_requested_fan_mode = *new_fan_mode;
            this->_last_fan_mode_change = CurrentTimeMilliseconds();
            this->send_state_update_to_nspanel();
            this->_signal_entity_changed();
            SPDLOG_DEBUG("Thermostat {}::{} got new fan mode: {}", this->_id, this->_name, new_fan_mode->value);
          } else {
            SPDLOG_WARN("Thermostat {}::{} got new fan mode '{}' from openhab but that fan mode is not supported.", this->_id, this->_name, mode);
          }
        }
      }
    }
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB thermostat {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_fan_mode_item.compare(data["payload"]["name"]) == 0) {
      nlohmann::json payload = data["payload"];
      if (payload["state"].is_null()) { // Got state but state is NULL, ignore.
        return;
      } else if (payload["state"].is_string() && std::string(payload["state"]).compare("NULL") == 0) {
        return;
      } else if (payload["state"].is_object()) {
        return;
      }

      std::string mode = std::string(payload["state"]);
      SPDLOG_DEBUG("Thermostat {}::{} got new fan mode {}, current fan mode: {}.", this->_id, this->_name, mode, this->_current_fan_mode.value);
      if (this->_current_fan_mode.value.compare(mode) != 0) {
        auto new_fan_mode = std::find_if(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), [&](const ThermostatOptionHolder &option) {
          return option.value.compare(mode) == 0;
        });
        if (new_fan_mode != this->_supported_fan_modes.end()) {
          this->_current_fan_mode = *new_fan_mode;
          this->_requested_fan_mode = *new_fan_mode;
          this->_last_fan_mode_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
          SPDLOG_DEBUG("Thermostat {}::{} got new fan mode: {}", this->_id, this->_name, new_fan_mode->value);
        } else {
          SPDLOG_WARN("Thermostat {}::{} got new fan mode '{}' from openhab but that fan mode is not supported.", this->_id, this->_name, mode);
        }
      }
    }
  }
}

void OpenhabThermostat::openhab_mode_event_callback(nlohmann::json data) {
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    std::string topic_item = topic_parts[2];

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_mode_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_mode_change + 1000) {
        if (payload["value"].is_null()) { // Got state but state is NULL, ignore.
          return;
        } else if (payload["value"].is_string() && std::string(payload["value"]).compare("NULL") == 0) {
          return;
        } else if (payload["value"].is_object()) {
          return;
        }

        std::string mode = std::string(payload["value"]);
        SPDLOG_DEBUG("Thermostat {}::{} got new mode {}, current mode: {}.", this->_id, this->_name, mode, this->_current_mode.value);
        if (this->_current_mode.value.compare(mode) != 0) {
          auto new_mode = std::find_if(this->_supported_modes.begin(), this->_supported_modes.end(), [&](const ThermostatOptionHolder &option) {
            return option.value.compare(mode) == 0;
          });
          if (new_mode != this->_supported_modes.end()) {
            this->_current_mode = *new_mode;
            this->_requested_mode = *new_mode;
            this->_last_mode_change = CurrentTimeMilliseconds();
            this->send_state_update_to_nspanel();
            this->_signal_entity_changed();
            SPDLOG_DEBUG("Thermostat {}::{} got new mode: {}", this->_id, this->_name, new_mode->value);
          } else {
            SPDLOG_WARN("Thermostat {}::{} got new mode '{}' from openhab but that HVAC mode is not supported.", this->_id, this->_name, mode);
          }
        }
      }
    }
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB thermostat {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_mode_item.compare(data["payload"]["name"]) == 0) {
      nlohmann::json payload = data["payload"];
      if (payload["state"].is_null()) { // Got state but state is NULL, ignore.
        return;
      } else if (payload["state"].is_string() && std::string(payload["state"]).compare("NULL") == 0) {
        return;
      } else if (payload["state"].is_object()) {
        return;
      }

      std::string mode = std::string(payload["state"]);
      SPDLOG_DEBUG("Thermostat {}::{} got new mode {}, current mode: {}.", this->_id, this->_name, mode, this->_current_mode.value);
      if (this->_current_mode.value.compare(mode) != 0) {
        auto new_mode = std::find_if(this->_supported_modes.begin(), this->_supported_modes.end(), [&](const ThermostatOptionHolder &option) {
          return option.value.compare(mode) == 0;
        });
        if (new_mode != this->_supported_modes.end()) {
          this->_current_mode = *new_mode;
          this->_requested_mode = *new_mode;
          this->_last_mode_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
          SPDLOG_DEBUG("Thermostat {}::{} got new mode: {}", this->_id, this->_name, new_mode->value);
        } else {
          SPDLOG_WARN("Thermostat {}::{} got new mode '{}' from openhab but that HVAC mode is not supported.", this->_id, this->_name, mode);
        }
      }
    }
  }
}

void OpenhabThermostat::openhab_preset_event_callback(nlohmann::json data) {
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    std::string topic_item = topic_parts[2];

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_preset_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_preset_change + 1000) {
        if (payload["value"].is_null()) { // Got state but state is NULL, ignore.
          return;
        } else if (payload["value"].is_string() && std::string(payload["value"]).compare("NULL") == 0) {
          return;
        } else if (payload["value"].is_object()) {
          return;
        }

        std::string mode = std::string(payload["value"]);
        SPDLOG_DEBUG("Thermostat {}::{} got new preset {}, current preset: {}.", this->_id, this->_name, mode, this->_current_preset.value);
        if (this->_current_preset.value.compare(mode) != 0) {
          auto new_preset = std::find_if(this->_supported_presets.begin(), this->_supported_presets.end(), [&](const ThermostatOptionHolder &option) {
            return option.value.compare(mode) == 0;
          });
          if (new_preset != this->_supported_presets.end()) {
            this->_current_preset = *new_preset;
            this->_requested_preset = *new_preset;
            this->_last_preset_change = CurrentTimeMilliseconds();
            this->send_state_update_to_nspanel();
            this->_signal_entity_changed();
            SPDLOG_DEBUG("Thermostat {}::{} got new preset: {}", this->_id, this->_name, new_preset->value);
          } else {
            SPDLOG_WARN("Thermostat {}::{} got new preset '{}' from HA but that preset is not supported.", this->_id, this->_name, mode);
          }
        }
      }
    }
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB thermostat {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_preset_item.compare(data["payload"]["name"]) == 0) {
      nlohmann::json payload = data["payload"];
      if (payload["state"].is_null()) { // Got state but state is NULL, ignore.
        return;
      } else if (payload["state"].is_string() && std::string(payload["state"]).compare("NULL") == 0) {
        return;
      } else if (payload["state"].is_object()) {
        return;
      }

      std::string mode = std::string(payload["state"]);
      SPDLOG_DEBUG("Thermostat {}::{} got new mode {}, current mode: {}.", this->_id, this->_name, mode, this->_current_mode.value);
      if (this->_current_mode.value.compare(mode) != 0) {
        auto new_preset = std::find_if(this->_supported_presets.begin(), this->_supported_presets.end(), [&](const ThermostatOptionHolder &option) {
          return option.value.compare(mode) == 0;
        });
        if (new_preset != this->_supported_presets.end()) {
          this->_current_preset = *new_preset;
          this->_requested_preset = *new_preset;
          this->_last_preset_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
          SPDLOG_DEBUG("Thermostat {}::{} got new preset: {}", this->_id, this->_name, new_preset->value);
        } else {
          SPDLOG_WARN("Thermostat {}::{} got new preset '{}' from openhab but that preset is not supported.", this->_id, this->_name, mode);
        }
      }
    }
  }
}

void OpenhabThermostat::openhab_swing_event_callback(nlohmann::json data) {
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    std::string topic_item = topic_parts[2];

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_swing_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_swing_change + 1000) {
        if (payload["value"].is_null()) { // Got state but state is NULL, ignore.
          return;
        } else if (payload["value"].is_string() && std::string(payload["value"]).compare("NULL") == 0) {
          return;
        } else if (payload["value"].is_object()) {
          return;
        }

        std::string mode = std::string(payload["value"]);
        SPDLOG_DEBUG("Thermostat {}::{} got new swing {}, current swing: {}.", this->_id, this->_name, mode, this->_current_swing_mode.value);
        if (this->_current_swing_mode.value.compare(mode) != 0) {
          auto new_swing = std::find_if(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), [&](const ThermostatOptionHolder &option) {
            return option.value.compare(mode) == 0;
          });
          if (new_swing != this->_supported_swing_modes.end()) {
            this->_current_swing_mode = *new_swing;
            this->_requested_swing_mode = *new_swing;
            this->_last_swing_change = CurrentTimeMilliseconds();
            this->send_state_update_to_nspanel();
            this->_signal_entity_changed();
            SPDLOG_DEBUG("Thermostat {}::{} got new swing mode: {}", this->_id, this->_name, new_swing->value);
          } else {
            SPDLOG_WARN("Thermostat {}::{} got new swing mode '{}' from openhab but that swing mode is not supported.", this->_id, this->_name, mode);
          }
        }
      }
    }
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB thermostat {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_swing_item.compare(data["payload"]["name"]) == 0) {
      nlohmann::json payload = data["payload"];
      if (payload["state"].is_null()) { // Got state but state is NULL, ignore.
        return;
      } else if (payload["state"].is_string() && std::string(payload["state"]).compare("NULL") == 0) {
        return;
      } else if (payload["state"].is_object()) {
        return;
      }

      std::string mode = std::string(payload["state"]);
      SPDLOG_DEBUG("Thermostat {}::{} got new swing mode {}, current swing mode: {}.", this->_id, this->_name, mode, this->_current_swing_mode.value);
      if (this->_current_swing_mode.value.compare(mode) != 0) {
        auto new_swing_mode = std::find_if(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), [&](const ThermostatOptionHolder &option) {
          return option.value.compare(mode) == 0;
        });
        if (new_swing_mode != this->_supported_swing_modes.end()) {
          this->_current_swing_mode = *new_swing_mode;
          this->_requested_swing_mode = *new_swing_mode;
          this->_last_preset_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
          SPDLOG_DEBUG("Thermostat {}::{} got new swing mode: {}", this->_id, this->_name, new_swing_mode->value);
        } else {
          SPDLOG_WARN("Thermostat {}::{} got new swing mode '{}' from openhab but that swing mode is not supported.", this->_id, this->_name, mode);
        }
      }
    }
  }
}

void OpenhabThermostat::openhab_swingh_event_callback(nlohmann::json data) {
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    std::string topic_item = topic_parts[2];

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", std::string(data["topic"]));
      return;
    }

    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_swingh_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest but still indicate that event was handled so the manager stops looping over all entities.
      if (CurrentTimeMilliseconds() >= this->_last_swing_change + 1000) {
        if (payload["value"].is_null()) { // Got state but state is NULL, ignore.
          return;
        } else if (payload["value"].is_string() && std::string(payload["value"]).compare("NULL") == 0) {
          return;
        } else if (payload["value"].is_object()) {
          return;
        }

        std::string mode = std::string(payload["value"]);
        SPDLOG_DEBUG("Thermostat {}::{} got new horizontal swing {}, current horizontal swing: {}.", this->_id, this->_name, mode, this->_current_swingh_mode.value);
        if (this->_current_swingh_mode.value.compare(mode) != 0) {
          auto new_horizontal_swing = std::find_if(this->_supported_swingh_modes.begin(), this->_supported_swingh_modes.end(), [&](const ThermostatOptionHolder &option) {
            return option.value.compare(mode) == 0;
          });
          if (new_horizontal_swing != this->_supported_swingh_modes.end()) {
            this->_current_swingh_mode = *new_horizontal_swing;
            this->_requested_swingh_mode = *new_horizontal_swing;
            this->_last_swing_change = CurrentTimeMilliseconds();
            this->send_state_update_to_nspanel();
            this->_signal_entity_changed();
            SPDLOG_DEBUG("Thermostat {}::{} got new horizontal swing mode: {}", this->_id, this->_name, new_horizontal_swing->value);
          } else {
            SPDLOG_WARN("Thermostat {}::{} got new horizontal swing mode '{}' from openhab but that mode is not supported.", this->_id, this->_name, mode);
          }
        }
      }
    }
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB thermostat {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_swing_item.compare(data["payload"]["name"]) == 0) {
      nlohmann::json payload = data["payload"];
      if (payload["state"].is_null()) { // Got state but state is NULL, ignore.
        return;
      } else if (payload["state"].is_string() && std::string(payload["state"]).compare("NULL") == 0) {
        return;
      } else if (payload["state"].is_object()) {
        return;
      }

      std::string mode = std::string(payload["state"]);
      SPDLOG_DEBUG("Thermostat {}::{} got new horizontal swing mode {}, current horizontal swing mode: {}.", this->_id, this->_name, mode, this->_current_swingh_mode.value);
      if (this->_current_swingh_mode.value.compare(mode) != 0) {
        auto new_swing_mode = std::find_if(this->_supported_swingh_modes.begin(), this->_supported_swingh_modes.end(), [&](const ThermostatOptionHolder &option) {
          return option.value.compare(mode) == 0;
        });
        if (new_swing_mode != this->_supported_swingh_modes.end()) {
          this->_current_swingh_mode = *new_swing_mode;
          this->_requested_swingh_mode = *new_swing_mode;
          this->_last_preset_change = CurrentTimeMilliseconds();
          this->send_state_update_to_nspanel();
          this->_signal_entity_changed();
          SPDLOG_DEBUG("Thermostat {}::{} got new horizontal swing mode: {}", this->_id, this->_name, new_swing_mode->value);
        } else {
          SPDLOG_WARN("Thermostat {}::{} got new horizontal swing mode '{}' from openhab but that mode is not supported.", this->_id, this->_name, mode);
        }
      }
    }
  }
}

// TESTING
#if defined(TEST_MODE) && TEST_MODE == 1
#include <gtest/gtest.h>

class HomeAssistantLightTest : public ::testing::Test {
protected:
  HomeAssistantLightTest() {
    // Initialize any necessary resources or setup for the tests
  }

  void SetUp() override {

    // Initialize any necessary resources or setup for the tests
    database_manager::Entity light_entity;
    light_entity.entity_type = "light";
    light_entity.friendly_name = "Unit test HA light (light)";
    light_entity.room_id = 1;
    light_entity.room_view_position = 2;
    light_entity.set_entity_data_json({{"can_color_temperature", true},
                                       {"can_dim", true},
                                       {"can_rgb", true},
                                       {"controlled_by_nspanel_main_page", true},
                                       {"controller", "home_assistant"},
                                       {"home_assistant_name", "light.office_ceiling_light"},
                                       {"is_ceiling_light", true},
                                       {"openhab_control_mode", "dimmer"},
                                       {"openhab_item_color_temp", ""},
                                       {"openhab_item_dimmer", ""},
                                       {"openhab_item_rgb", ""},
                                       {"openhab_item_switch", ""},
                                       {"openhab_name", ""}});
    ha_light_id = database_manager::database.insert(light_entity);
    // light_entity = database_manager::database.get<database_manager::Entity>(ha_light_id);

    SPDLOG_INFO("HA Light created in DB. Creating instance of light object.");
    light = new HomeAssistantLight(ha_light_id);
  }

  void TearDown() override {
    // Clean up any resources or teardown after the tests

    database_manager::database.remove<database_manager::Entity>(ha_light_id);
    // database_manager::database.remove<database_manager::Entity>(ha_light_switch_id);
  }

  HomeAssistantLight *light = nullptr;

  int32_t ha_light_id;
  int32_t ha_light_switch_id;
};

TEST_F(HomeAssistantLightTest, is_off_by_default) {
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
}

TEST_F(HomeAssistantLightTest, is_not_on_after_turn_on_in_nonoptimistic_mode) {
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "false");
  light->turn_on(false);
  light->set_brightness(50, true);
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
}

TEST_F(HomeAssistantLightTest, is_on_after_turn_on_in_optimistic_mode) {
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
  light->turn_on(false);
  light->set_brightness(50, true);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
}

TEST_F(HomeAssistantLightTest, light_reacts_to_state_changes_from_home_assistant) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");

  nlohmann::json event_data = nlohmann::json::parse(R"(
      {

          "event":{
              "context":{
                  "id":"01K2WNR8WT8VS6B5HTQ7ZEKSAP",
                  "parent_id":null,
                  "user_id":"d28bd745ad714c108c27c31a90406189"
              },
              "data":{
                  "entity_id":"light.office_ceiling_light",
                  "new_state":{
                      "attributes":{
                          "brightness":204,
                          "color_mode":"color_temp",
                          "color_temp":443,
                          "color_temp_kelvin":2257,
                          "effect":null,
                          "effect_list":[
                              "blink",
                              "breathe",
                              "okay",
                              "channel_change",
                              "finish_effect",
                              "stop_effect"
                          ],
                          "friendly_name":"office_ceiling_light",
                          "hs_color":[
                              20,
                              80
                          ],
                          "max_color_temp_kelvin":4000,
                          "max_mireds":454,
                          "min_color_temp_kelvin":2202,
                          "min_mireds":250,
                          "rgb_color":[
                              255,
                              149,
                              46
                          ],
                          "supported_color_modes":[
                              "color_temp"
                          ],
                          "supported_features":44,
                          "xy_color":[
                              0.572,
                              0.389
                          ]
                      },
                      "context":{
                          "id":"01K2WNR8WT8VS6B5HTQ7ZEKSAP",
                          "parent_id":null,
                          "user_id":"d28bd745ad714c108c27c31a90406189"
                      },
                      "entity_id":"light.office_ceiling_light",
                      "last_changed":"2025-08-17T18:45:35.203504+00:00",
                      "last_reported":"2025-08-17T18:48:00.253850+00:00",
                      "last_updated":"2025-08-17T18:48:00.253850+00:00",
                      "state":"on"
                  },
                  "old_state": "removed_as_it_is_not_used"
              },
              "event_type":"state_changed",
              "origin":"LOCAL",
              "time_fired":"2025-08-17T18:48:00.253850+00:00"
          },
          "id":3,
          "type":"event"

      }  )");

  light->home_assistant_event_callback(event_data);

  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 80); // We sent brightness 204, ie. 80%
  EXPECT_EQ(light->get_color_temperature(), 2257);
  EXPECT_EQ(light->get_color_temperature(), 2257);

  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153;
  event_data["event"]["data"]["new_state"]["attributes"]["color_temp_kelvin"] = 5000;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60); // We sent brightness 153, ie. 60%
  EXPECT_EQ(light->get_color_temperature(), 5000);

  event_data["event"]["data"]["new_state"]["state"] = "off";
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 0;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 60); // Remember last brightness value

  // Change to RGB mode
  event_data["event"]["data"]["new_state"]["state"] = "on";
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153;  // 60%
  event_data["event"]["data"]["new_state"]["attributes"]["color_mode"] = "xy"; // TODO: Update with correct color mode as sent from HA.
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60);
  EXPECT_EQ(light->get_hue(), 20);
  EXPECT_EQ(light->get_saturation(), 80);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that changes to RGB/HSB values as correctly interpreted
  event_data["event"]["data"]["new_state"]["attributes"]["hs_color"][0] = 30;
  event_data["event"]["data"]["new_state"]["attributes"]["hs_color"][1] = 60;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60);
  EXPECT_EQ(light->get_hue(), 30);
  EXPECT_EQ(light->get_saturation(), 60);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that we can revert back to color temp mode
  event_data["event"]["data"]["new_state"]["state"] = "on";
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153; // 60%
  event_data["event"]["data"]["new_state"]["attributes"]["color_mode"] = "color_temp";
  event_data["event"]["data"]["new_state"]["attributes"]["color_temp_kelvin"] = 4000;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60);
  EXPECT_EQ(light->get_color_temperature(), 4000);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);
}

#define COLOR_TEMP_PERCENT_TO_KELVIN(min, max, kelvin_pct) (((max - min) / 100) * kelvin_pct + min)
TEST_F(HomeAssistantLightTest, verify_nspanel_command_compliance) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MIN);
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MAX);
  // spdlog::set_level(spdlog::level::trace);

  NSPanelMQTTManagerCommand cmd;
  cmd.set_nspanel_id(0);
  NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
  std::vector<uint32_t> light_ids = {light->get_id()};
  light_cmd->add_light_ids(light->get_id());
  light_cmd->set_brightness(50);
  light_cmd->set_has_brightness(true);
  light_cmd->set_color_temperature(50);
  light_cmd->set_has_color_temperature(true);
  light_cmd->set_has_hue(false);
  light_cmd->set_has_saturation(false);

  light->command_callback(cmd);

  // Check that light turns on, sets correct brightness and color temperature.
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 50));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  // Verify that only color temperature can be change.
  light_cmd->set_has_brightness(false);
  light_cmd->set_color_temperature(70);
  light_cmd->set_has_color_temperature(true);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  // Verify that only brightness can be change.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(true);
  light_cmd->set_brightness(70);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  // Verify that hue and saturation can be changed and the lights goes into RGB mode.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(false);
  light_cmd->set_hue(30);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(60);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);                                                                      // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 30);
  EXPECT_EQ(light->get_saturation(), 60);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that only hue can be changed.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(false);
  light_cmd->set_hue(40);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(60);
  light_cmd->set_has_saturation(false);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);                                                                      // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 40);
  EXPECT_EQ(light->get_saturation(), 60); // Verify that value is unchanged.
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that only saturation can be changed.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(false);
  light_cmd->set_hue(40);
  light_cmd->set_has_hue(false);
  light_cmd->set_saturation(70);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);                                                                      // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 40);                                                                             // Verify that value is unchanged.
  EXPECT_EQ(light->get_saturation(), 70);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that brightness can be changed and RGB is kept.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_brightness(100);
  light_cmd->set_has_brightness(true);
  light_cmd->set_hue(40);
  light_cmd->set_has_hue(false);
  light_cmd->set_saturation(70);
  light_cmd->set_has_saturation(false);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 100);                                                                     // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 40);                                                                             // Verify that value is unchanged.
  EXPECT_EQ(light->get_saturation(), 70);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
}

TEST_F(HomeAssistantLightTest, verify_optimistic_mode_compliance) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MIN);
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MAX);
  // spdlog::set_level(spdlog::level::trace);

  NSPanelMQTTManagerCommand cmd;
  cmd.set_nspanel_id(0);
  NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
  std::vector<uint32_t> light_ids = {light->get_id()};
  light_cmd->add_light_ids(light->get_id());
  light_cmd->set_brightness(50);
  light_cmd->set_has_brightness(true);
  light_cmd->set_color_temperature(50);
  light_cmd->set_has_color_temperature(true);
  light_cmd->set_has_hue(false);
  light_cmd->set_has_saturation(false);

  light->command_callback(cmd);

  // Check that light turns on, sets correct brightness and color temperature.
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 50));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  light_cmd->set_has_brightness(false);
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_hue(30);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(50);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);

  EXPECT_EQ(light->get_state(), true);                                                                         // Verify light is still on
  EXPECT_EQ(light->get_brightness(), 50);                                                                      // Verify light is still set to 50% brightness
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 50)); // Verify color temp has not changed.
  EXPECT_EQ(light->get_hue(), 30);
  EXPECT_EQ(light->get_saturation(), 50);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB); // Light should have switched to RGB mode.
}

TEST_F(HomeAssistantLightTest, verify_non_optimistic_mode_compliance) {
  // Turn off optimistic mode and verify that no values change.
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "false");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MIN);
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MAX);
  // spdlog::set_level(spdlog::level::trace);

  NSPanelMQTTManagerCommand cmd;
  cmd.set_nspanel_id(0);
  NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
  std::vector<uint32_t> light_ids = {light->get_id()};
  light_cmd->add_light_ids(light->get_id());
  light_cmd->set_brightness(50);
  light_cmd->set_has_brightness(true);
  light_cmd->set_color_temperature(50);
  light_cmd->set_has_color_temperature(true);
  light_cmd->set_has_hue(false);
  light_cmd->set_has_saturation(false);
  light->command_callback(cmd);

  // Verify light is still off, brightness is still 0 and hue and saturation did not change.
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
  EXPECT_EQ(light->get_color_temperature(), 0);
  EXPECT_EQ(light->get_hue(), 0);
  EXPECT_EQ(light->get_saturation(), 0);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT); // Light should have switched to RGB mode.

  light_cmd->set_has_brightness(false);
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_hue(30);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(30);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);

  // Verify light is still off, brightness is still 0 and hue and saturation did not change.
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
  EXPECT_EQ(light->get_color_temperature(), 0);
  EXPECT_EQ(light->get_hue(), 0);
  EXPECT_EQ(light->get_saturation(), 0);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT); // Light should have switched to RGB mode.
}

#endif // !MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TEST
