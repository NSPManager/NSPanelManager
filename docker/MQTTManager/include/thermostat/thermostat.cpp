#include "thermostat/thermostat.hpp"
#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include "protobuf_nspanel_entity.pb.h"
#include "room/room.hpp"
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <entity/entity_icons.hpp>
#include <google/protobuf/util/message_differencer.h>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <unistd.h>

ThermostatEntity::ThermostatEntity(uint32_t light_id) {
  this->_id = light_id;
  this->_step_size = 1.0; // Set default step size to only whole numbers.
  this->reload_config();

  // Build MQTT Topics
  std::string mqtt_base_topic = fmt::format("nspanel/entities/light/{}/", this->_id);
  CommandManager::attach_callback(boost::bind(&ThermostatEntity::command_callback, this, _1));

  SPDLOG_DEBUG("Switch {}::{} base loaded.", this->_id, this->_name);
}

uint16_t ThermostatEntity::get_room_id() {
  return this->_room_id;
}

void ThermostatEntity::reload_config() {
  auto switch_entity = database_manager::database.get<database_manager::Entity>(this->_id);
  this->_name = switch_entity.friendly_name;
  SPDLOG_DEBUG("Loading switch {}::{}.", this->_id, this->_name);

  this->_room_id = switch_entity.room_id;
  this->_entity_page_id = switch_entity.entities_page_id;
  this->_entity_page_slot = switch_entity.room_view_position;

  nlohmann::json entity_data = switch_entity.get_entity_data_json();
  if (entity_data.contains("controller")) {
    std::string controller = entity_data["controller"];
    if (controller.compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else if (controller.compare("openhab") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
    } else {
      SPDLOG_ERROR("Got unknown controller ({}) for light {}::{}. Will default to HOME_ASSISTANT.", std::string(controller), this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }
  } else {
    SPDLOG_ERROR("No controller defined for light {}::{}. Will default to HOME_ASSISTANT.", this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }

  this->_supported_fan_modes.clear();
  this->_supported_modes.clear();
  this->_supported_swing_modes.clear();
  this->_supported_swingh_modes.clear();
  this->_supported_presets.clear();

  SPDLOG_DEBUG("Loading fan modes for {}::{}.", this->_id, this->_name);
  if (entity_data.contains("fan_modes")) {
    for (auto &mode : entity_data["fan_modes"]) {
      if (mode.contains("value")) {
        ThermostatOptionHolder holder;
        holder.value = mode["value"];
        if (mode.contains("icon")) {
          holder.icon = mode["icon"];
        } else [[unlikely]] {
          SPDLOG_ERROR("Mode does not contain an icon key.");
        }
        if (mode.contains("label")) {
          holder.label = mode["label"];
        } else [[unlikely]] {
          SPDLOG_WARN("Thermostat entry for mode {} does not give a label. Will use raw mode value.", holder.value);
          holder.label = mode;
        }
        this->_supported_fan_modes.push_back(holder);
      } else {
        SPDLOG_ERROR("Mode does not contain a value key.");
      }
    }
  }

  SPDLOG_DEBUG("Loading HVAC modes for {}::{}.", this->_id, this->_name);
  if (entity_data.contains("hvac_modes")) {
    for (auto &mode : entity_data["hvac_modes"]) {
      if (mode.contains("value")) {
        ThermostatOptionHolder holder;
        holder.value = mode["value"];
        if (mode.contains("icon")) {
          holder.icon = mode["icon"];
        } else [[unlikely]] {
          SPDLOG_ERROR("Mode does not contain an icon key.");
        }
        if (mode.contains("label")) {
          holder.label = mode["label"];
        } else [[unlikely]] {
          SPDLOG_WARN("Thermostat entry for mode {} does not give a label. Will use raw mode value.", holder.value);
          holder.label = mode;
        }
        this->_supported_modes.push_back(holder);
      } else {
        SPDLOG_ERROR("Mode does not contain a value key.");
      }
    }
  }

  SPDLOG_DEBUG("Loading presets for {}::{}.", this->_id, this->_name);
  if (entity_data.contains("preset_modes")) {
    for (auto &mode : entity_data["preset_modes"]) {
      if (mode.contains("value")) {
        ThermostatOptionHolder holder;
        holder.value = mode["value"];
        if (mode.contains("icon")) {
          holder.icon = mode["icon"];
        } else [[unlikely]] {
          SPDLOG_ERROR("Mode does not contain an icon key.");
        }
        if (mode.contains("label")) {
          holder.label = mode["label"];
        } else [[unlikely]] {
          SPDLOG_WARN("Thermostat entry for mode {} does not give a label. Will use raw mode value.", holder.value);
          holder.label = mode;
        }
        this->_supported_presets.push_back(holder);
      } else {
        SPDLOG_ERROR("Mode does not contain a value key.");
      }
    }
  }

  SPDLOG_DEBUG("Loading swing modes for {}::{}.", this->_id, this->_name);
  if (entity_data.contains("swing_modes")) {
    for (auto &mode : entity_data["swing_modes"]) {
      if (mode.contains("value")) {
        ThermostatOptionHolder holder;
        holder.value = mode["value"];
        if (mode.contains("icon")) {
          holder.icon = mode["icon"];
        } else [[unlikely]] {
          SPDLOG_ERROR("Mode does not contain an icon key.");
        }
        if (mode.contains("label")) {
          holder.label = mode["label"];
        } else [[unlikely]] {
          SPDLOG_WARN("Thermostat entry for mode {} does not give a label. Will use raw mode value.", holder.value);
          holder.label = mode;
        }
        this->_supported_swing_modes.push_back(holder);
      } else {
        SPDLOG_ERROR("Mode does not contain a value key.");
      }
    }
  }

  SPDLOG_DEBUG("Loading horizontal swing modes for {}::{}.", this->_id, this->_name);
  if (entity_data.contains("swingh_modes")) {
    for (auto &mode : entity_data["swingh_modes"]) {
      if (mode.contains("value")) {
        ThermostatOptionHolder holder;
        holder.value = mode["value"];
        if (mode.contains("icon")) {
          holder.icon = mode["icon"];
        } else [[unlikely]] {
          SPDLOG_ERROR("Mode does not contain an icon key.");
        }
        if (mode.contains("label")) {
          holder.label = mode["label"];
        } else [[unlikely]] {
          SPDLOG_WARN("Thermostat entry for mode {} does not give a label. Will use raw mode value.", holder.value);
          holder.label = mode;
        }
        this->_supported_swingh_modes.push_back(holder);
      } else {
        SPDLOG_ERROR("Mode does not contain a value key.");
      }
    }
  }
}

void ThermostatEntity::set_mode(std::string mode) {
  auto request_mode = std::find_if(this->_supported_modes.begin(), this->_supported_modes.end(), [&](const ThermostatOptionHolder &holder) {
    return holder.value.compare(mode) == 0;
  });
  if (request_mode != this->_supported_modes.end()) {
    this->_requested_mode = *request_mode;
    this->send_state_update_to_controller();
  } else {
    SPDLOG_ERROR("Invalid mode ({}) for thermostat {}::{}.", mode, this->_id, this->_name);
  }
}

ThermostatOptionHolder ThermostatEntity::get_mode() {
  return this->_requested_mode;
}

std::vector<ThermostatOptionHolder> ThermostatEntity::get_supported_modes() {
  return this->_supported_modes;
}

void ThermostatEntity::set_temperature(float temperature) {
  this->_requested_temperature = temperature;
  this->send_state_update_to_controller();
}

float ThermostatEntity::get_temperature() {
  return this->_current_temperature;
}

void ThermostatEntity::set_swing_mode(std::string swing_mode) {
  auto request_swing_mode = std::find_if(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), [&](const ThermostatOptionHolder &holder) {
    return holder.value.compare(swing_mode) == 0;
  });
  if (request_swing_mode != this->_supported_swing_modes.end()) {
    this->_requested_swing_mode = *request_swing_mode;
    this->send_state_update_to_controller();
  } else {
    SPDLOG_ERROR("Invalid swing mode ({}) for thermostat {}::{}.", swing_mode, this->_id, this->_name);
  }
}

ThermostatOptionHolder ThermostatEntity::get_swing_mode() {
  return this->_requested_swing_mode;
}

std::vector<ThermostatOptionHolder> ThermostatEntity::get_supported_swing_modes() {
  return this->_supported_swing_modes;
}

void ThermostatEntity::set_swing_horizontal_mode(std::string swingh_mode) {
  auto request_swingh_mode = std::find_if(this->_supported_swingh_modes.begin(), this->_supported_swingh_modes.end(), [&](const ThermostatOptionHolder &holder) {
    return holder.value.compare(swingh_mode) == 0;
  });
  if (request_swingh_mode != this->_supported_swingh_modes.end()) {
    this->_requested_swingh_mode = *request_swingh_mode;
    this->send_state_update_to_controller();
  } else {
    SPDLOG_ERROR("Invalid swing (horizontal) mode ({}) for thermostat {}::{}.", swingh_mode, this->_id, this->_name);
  }
}

ThermostatOptionHolder ThermostatEntity::get_swing_horizontal_mode() {
  return this->_requested_swingh_mode;
}

std::vector<ThermostatOptionHolder> ThermostatEntity::get_supported_swing_horizontal_modes() {
  return this->_supported_swingh_modes;
}

void ThermostatEntity::send_state_update_to_nspanel() {
  auto room = EntityManager::get_room(this->_room_id);
  if (room) {
    NSPanelEntityState state;
    NSPanelEntityState_Thermostat *th_status = state.mutable_thermostat();
    th_status->set_thermostat_id(this->_id);
    th_status->set_name(this->_name);
    th_status->set_set_temperature(this->_current_temperature);
    th_status->set_step_size(this->_step_size);
    th_status->set_has_current_temperature(false);

    if ((*room)->has_temperature_sensor()) {
      auto room_temp = (*room)->get_temperature();
      if (room_temp) {
        th_status->set_current_temperature(*room_temp);
        th_status->set_has_current_temperature(true);
      } else {
        SPDLOG_ERROR("Failed to get room temperature from room {}::{} while trying to send state update for thermostat {}::{}. Will send temperature thermostat set temp as room temp. Got message: {}", (*room)->get_id(), (*room)->get_name(), this->_id, this->_name, room_temp.error());
        th_status->set_current_temperature(this->_current_temperature);
        th_status->set_has_current_temperature(true);
      }
    } else {
      SPDLOG_ERROR("Failed to get room {}::{} has not temperature sensor. Will send state update for thermostat {}::{} with current room temperature set temp as thermostat set point.", (*room)->get_id(), (*room)->get_name(), this->_id, this->_name);
      th_status->set_current_temperature(this->_current_temperature);
      th_status->set_has_current_temperature(true);
    }

    if (!this->_supported_modes.empty()) {
      auto mode_options = th_status->add_options();
      mode_options->set_name("Mode");
      mode_options->set_current_value(this->_current_mode.label);
      mode_options->set_current_icon(this->_current_mode.icon);
      for (const auto &mode : this->_supported_modes) {
        auto option = mode_options->add_options();
        option->set_value(mode.label);
        option->set_icon(mode.icon);
      }
    }

    if (!this->_supported_fan_modes.empty()) {
      auto fan_options = th_status->add_options();
      fan_options->set_name(ThermostatEntity::fan_label);
      fan_options->set_current_value(this->_current_fan_mode.label);
      fan_options->set_current_icon(this->_current_fan_mode.icon);
      for (const auto &mode : this->_supported_fan_modes) {
        auto option = fan_options->add_options();
        option->set_value(mode.label);
        option->set_icon(mode.icon);
      }
    }

    if (!this->_supported_presets.empty()) {
      auto preset_options = th_status->add_options();
      preset_options->set_name(ThermostatEntity::preset_label);
      preset_options->set_current_value(this->_current_preset.label);
      preset_options->set_current_icon(this->_current_preset.icon);
      for (const auto &preset : this->_supported_presets) {
        auto option = preset_options->add_options();
        option->set_value(preset.label);
        option->set_icon(preset.icon);
      }
    }

    if (!this->_supported_swing_modes.empty()) {
      auto swing_options = th_status->add_options();
      swing_options->set_name(ThermostatEntity::swing_label);
      swing_options->set_current_value(this->_current_swing_mode.label);
      swing_options->set_current_icon(this->_current_swing_mode.icon);
      for (const auto &mode : this->_supported_swing_modes) {
        auto option = swing_options->add_options();
        option->set_value(mode.label);
        option->set_icon(mode.icon);
      }
    }

    if (!this->_supported_swingh_modes.empty()) {
      auto swingh_options = th_status->add_options();
      swingh_options->set_name(ThermostatEntity::swingh_label);
      swingh_options->set_current_value(this->_current_swingh_mode.label);
      swingh_options->set_current_icon(this->_current_swingh_mode.icon);
      for (const auto &mode : this->_supported_swingh_modes) {
        auto option = swingh_options->add_options();
        option->set_value(mode.label);
        option->set_icon(mode.icon);
      }
    }

    google::protobuf::util::MessageDifferencer differencer;
    if (!differencer.Compare(this->_last_thermostat_state, state)) {
      SPDLOG_DEBUG("Sending updated state for thermostat {}::{} over MQTT.", this->_id, this->_name);
      this->_last_thermostat_state = state;

      MQTT_Manager::publish_protobuf(this->get_mqtt_state_topic(), state, true);
    } else {
      SPDLOG_DEBUG("Did not send state update for thermostat {}::{} as there were no changes.", this->_id, this->_name);
    }
  } else {
    SPDLOG_ERROR("Failed to get room from EntityManager while trying to send state update for thermostat {}::{} to NSPanel.", this->_id, this->_name);
  }
}

void ThermostatEntity::set_fan_mode(std::string fan_mode) {
  auto request_fan_mode = std::find_if(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), [fan_mode](const ThermostatOptionHolder &holder) {
    return holder.value.compare(fan_mode) == 0;
  });
  if (request_fan_mode != this->_supported_fan_modes.end()) {
    this->_requested_fan_mode = *request_fan_mode;
    this->send_state_update_to_controller();
  } else {
    SPDLOG_ERROR("Invalid fan mode ({}) for thermostat {}::{}. Will default to 'off'.", fan_mode, this->_id, this->_name);
  }
}

ThermostatOptionHolder ThermostatEntity::get_fan_mode() {
  return this->_requested_fan_mode;
}

std::vector<ThermostatOptionHolder> ThermostatEntity::get_supported_fan_modes() {
  return this->_supported_fan_modes;
}

void ThermostatEntity::set_preset(std::string preset) {
  auto request_preset = std::find_if(this->_supported_presets.begin(), this->_supported_presets.end(), [preset](const ThermostatOptionHolder &holder) {
    return holder.value.compare(preset) == 0;
  });
  if (request_preset != this->_supported_presets.end()) {
    this->_requested_preset = *request_preset;
    this->send_state_update_to_controller();
  } else {
    SPDLOG_ERROR("Invalid preset ({}) for thermostat {}::{}. Will default to 'off'.", preset, this->_id, this->_name);
  }
}

ThermostatOptionHolder ThermostatEntity::get_preset() {
  return this->_requested_preset;
}

std::vector<ThermostatOptionHolder> ThermostatEntity::get_supported_presets() {
  return this->_supported_presets;
}

MQTT_MANAGER_ENTITY_TYPE ThermostatEntity::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::THERMOSTAT;
}

MQTT_MANAGER_ENTITY_CONTROLLER ThermostatEntity::get_controller() {
  return this->_controller;
}

uint16_t ThermostatEntity::get_id() {
  return this->_id;
}

std::string ThermostatEntity::get_name() {
  return this->_name;
}

uint32_t ThermostatEntity::get_entity_page_id() {
  return this->_entity_page_id;
}

uint8_t ThermostatEntity::get_entity_page_slot() {
  return this->_entity_page_slot;
}

void ThermostatEntity::attach_delete_callback(void (*callback)(ThermostatEntity *)) {
  this->_thermostat_destroyed_callbacks.connect(callback);
}

void ThermostatEntity::detach_delete_callback(void (*callback)(ThermostatEntity *)) {
  this->_thermostat_destroyed_callbacks.disconnect(callback);
}

void ThermostatEntity::reset_requests() {
  this->_requested_preset = this->_current_preset;
  this->_requested_temperature = this->_current_temperature;
  this->_requested_mode = this->_current_mode;
  this->_requested_fan_mode = this->_current_fan_mode;
  this->_requested_swing_mode = this->_current_swing_mode;
}

void ThermostatEntity::command_callback(NSPanelMQTTManagerCommand &command) {
  if (command.has_thermostat_command() && command.thermostat_command().thermostat_id() == this->_id) {
    auto thermostat_command = command.thermostat_command();
    SPDLOG_DEBUG("Received command from NSPanel to set thermostat {}::{} setting {} to value {}", this->_id, this->_name, thermostat_command.option(), thermostat_command.new_value());

    if (thermostat_command.option().compare(ThermostatEntity::mode_label) == 0) {
      auto new_mode = std::find_if(this->_supported_modes.begin(), this->_supported_modes.end(), [thermostat_command](const ThermostatOptionHolder &mode) {
        return mode.label.compare(thermostat_command.new_value()) == 0;
      });
      if (new_mode != this->_supported_modes.end()) [[likely]] {
        this->set_mode(new_mode->value);
      } else {
        SPDLOG_ERROR("Thermostat {}::{} received command to set set option {} to value {} but that value is not valid.", this->_id, this->_name, thermostat_command.option(), thermostat_command.new_value());
      }
    } else if (thermostat_command.option().compare(ThermostatEntity::fan_label) == 0) {
      auto new_mode = std::find_if(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), [thermostat_command](const ThermostatOptionHolder &mode) {
        return mode.label.compare(thermostat_command.new_value()) == 0;
      });
      if (new_mode != this->_supported_fan_modes.end()) [[likely]] {
        this->set_fan_mode(new_mode->value);
      } else {
        SPDLOG_ERROR("Thermostat {}::{} received command to set set option {} to value {} but that value is not valid.", this->_id, this->_name, thermostat_command.option(), thermostat_command.new_value());
      }
    } else if (thermostat_command.option().compare(ThermostatEntity::preset_label) == 0) {
      auto new_preset = std::find_if(this->_supported_presets.begin(), this->_supported_presets.end(), [thermostat_command](const ThermostatOptionHolder &mode) {
        return mode.label.compare(thermostat_command.new_value()) == 0;
      });
      if (new_preset != this->_supported_presets.end()) [[likely]] {
        this->set_preset(new_preset->value);
      } else {
        SPDLOG_ERROR("Thermostat {}::{} received command to set set option {} to value {} but that value is not valid.", this->_id, this->_name, thermostat_command.option(), thermostat_command.new_value());
      }
    } else if (thermostat_command.option().compare(ThermostatEntity::swing_label) == 0) {
      auto new_mode = std::find_if(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), [thermostat_command](const ThermostatOptionHolder &mode) {
        return mode.label.compare(thermostat_command.new_value()) == 0;
      });
      if (new_mode != this->_supported_swing_modes.end()) [[likely]] {
        this->set_swing_mode(new_mode->value);
      } else {
        SPDLOG_ERROR("Thermostat {}::{} received command to set set option {} to value {} but that value is not valid.", this->_id, this->_name, thermostat_command.option(), thermostat_command.new_value());
      }
    } else if (thermostat_command.option().compare(ThermostatEntity::swingh_label) == 0) {
      auto new_mode = std::find_if(this->_supported_swingh_modes.begin(), this->_supported_swingh_modes.end(), [thermostat_command](const ThermostatOptionHolder &mode) {
        return mode.label.compare(thermostat_command.new_value()) == 0;
      });
      if (new_mode != this->_supported_swingh_modes.end()) [[likely]] {
        this->set_swing_horizontal_mode(new_mode->value);
      } else {
        SPDLOG_ERROR("Thermostat {}::{} received command to set set option {} to value {} but that value is not valid.", this->_id, this->_name, thermostat_command.option(), thermostat_command.new_value());
      }
    }
  } else if (command.has_thermostat_temperature_command() && command.thermostat_temperature_command().thermostat_id() == this->_id) {
    this->set_temperature(command.thermostat_temperature_command().temperature());
  }
}

ThermostatEntity::~ThermostatEntity() {
  SPDLOG_DEBUG("Destructor for thermostat {}::{} called.", this->_id, this->_name);
  this->_thermostat_destroyed_callbacks(this);
  this->_signal_entity_destroyed();
  CommandManager::detach_callback(boost::bind(&ThermostatEntity::command_callback, this, _1));
}

bool ThermostatEntity::can_toggle() {
  return true;
}

void ThermostatEntity::toggle() {
}

std::string_view ThermostatEntity::get_icon() {
  return EntityIcons::entity_icon_switch_off; // TODO: Implement correct thermostat icon.
}

uint16_t ThermostatEntity::get_icon_color() {
  return GUI_Colors::icon_color_off;
}

uint16_t ThermostatEntity::get_icon_active_color() {
  return GUI_Colors::icon_color_off;
}

std::string ThermostatEntity::get_mqtt_state_topic() {
  std::string manager_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS);
  return fmt::format("nspanel/mqttmanager_{}/entities/thermostats/{}/state", manager_address, this->get_id());
}
