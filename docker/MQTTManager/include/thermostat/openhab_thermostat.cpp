#include "openhab_thermostat.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "thermostat/thermostat.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <charconv>
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
  OpenhabManager::detach_event_observer(this->_openhab_current_temperature_item, boost::bind(&OpenhabThermostat::openhab_current_temperature_event_callback, this, _1));
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

  if (!this->_use_current_temperature) {
    SPDLOG_INFO("Thermostat {}::{} is configured to not use temperature current temperature item.");
  } else if (entity_data.contains("openhab_current_temperature_item")) {
    this->_openhab_current_temperature_item = entity_data["openhab_current_temperature_item"];
  } else {
    SPDLOG_ERROR("No openhab_current_temperature_item name defined for Thermostat {}::{}", this->_id, this->_name);
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
  bool send_state_update = false;

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
      send_state_update = true;
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
      send_state_update = true;
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
      send_state_update = true;
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
      send_state_update = true;
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
      send_state_update = true;
    }
    OpenhabManager::send_json(service_data);
  }

  if (this->_requested_temperature != this->_current_temperature) {
    service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_target_temperature_item);
    // payload_data["type"] = "string";
    nlohmann::json payload_data;
    payload_data["value"] = this->_requested_temperature;
    payload_data["type"] = "Decimal";
    service_data["payload"] = payload_data.dump();
    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE)) {
      this->_current_temperature = this->_requested_temperature;
      this->_entity_changed_callbacks(this);
      send_state_update = true;
    }
    OpenhabManager::send_json(service_data);
  }

  if (send_state_update) {
    this->send_state_update_to_nspanel();
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

        std::string payload_string = payload["value"].get<std::string>();
        float target_temperature = 0;
        if (!payload_string.empty()) {
          auto [ptr, ec] = std::from_chars(payload_string.data(), payload_string.data() + payload_string.size(), target_temperature);
          if (ec == std::errc()) {
            target_temperature = std::round(target_temperature * 10) / 10; // Round to 1 decimal
          }
        }
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

void OpenhabThermostat::openhab_current_temperature_event_callback(nlohmann::json data) {
  SPDLOG_DEBUG("Thermostat {}::{} get current temperature callback!", this->_id, this->_name);

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

        std::string payload_string = payload["value"].get<std::string>();
        float current_temperature = 0;
        if (!payload_string.empty()) {
          auto [ptr, ec] = std::from_chars(payload_string.data(), payload_string.data() + payload_string.size(), current_temperature);
          if (ec == std::errc()) {
            current_temperature = std::round(current_temperature * 10) / 10; // Round to 1 decimal
          }
        }
        SPDLOG_DEBUG("Thermostat {}::{} got new temperature sensor value {}, current temperature: {}.", this->_id, this->_name, current_temperature, this->_current_temperature_sensor);
        if (current_temperature != this->_current_temperature_sensor) {
          this->_current_temperature_sensor = current_temperature;
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

      std::string payload_string = payload["value"].get<std::string>();
      float current_temperature = 0;
      if (!payload_string.empty()) {
        auto [ptr, ec] = std::from_chars(payload_string.data(), payload_string.data() + payload_string.size(), current_temperature);
        if (ec == std::errc()) {
          current_temperature = std::round(current_temperature * 10) / 10; // Round to 1 decimal
        }
      }
      SPDLOG_DEBUG("Thermostat {}::{} got new temperature {}, current temperature: {}.", this->_id, this->_name, current_temperature, this->_current_temperature_sensor);
      if (current_temperature != this->_current_temperature_sensor) {
        this->_current_temperature_sensor = current_temperature;
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
          this->_last_swing_change = CurrentTimeMilliseconds();
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
      if (CurrentTimeMilliseconds() >= this->_last_swingh_change + 1000) {
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
            this->_last_swingh_change = CurrentTimeMilliseconds();
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
    if (data.contains("payload") && data.at("payload").contains("name") && this->_openhab_swing_item.compare(data["payload"]["name"]) == 0) {
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
          this->_last_swingh_change = CurrentTimeMilliseconds();
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
