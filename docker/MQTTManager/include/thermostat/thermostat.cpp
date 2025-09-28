#include "thermostat/thermostat.hpp"
#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include "protobuf_nspanel_entity.pb.h"
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
}

void ThermostatEntity::set_mode(std::string mode) {
  if (std::find(this->_supported_modes.begin(), this->_supported_modes.end(), mode) != this->_supported_modes.end()) {
    this->_requested_mode = mode;
  } else {
    SPDLOG_ERROR("Invalid mode ({}) for thermostat {}::{}. Will default to 'off'.", mode, this->_id, this->_name);
    this->_requested_mode = "off";
  }
  this->send_state_update_to_controller();
}

std::string ThermostatEntity::get_mode() {
  return this->_requested_mode;
}

std::vector<std::string> ThermostatEntity::get_supported_modes() {
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
  if (std::find(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), swing_mode) != this->_supported_swing_modes.end()) {
    this->_requested_swing_mode = swing_mode;
  } else {
    SPDLOG_ERROR("Invalid swing mode ({}) for thermostat {}::{}. Will default to 'off'.", swing_mode, this->_id, this->_name);
    this->_requested_swing_mode = "off";
  }
  this->send_state_update_to_controller();
}

std::string ThermostatEntity::get_swing_mode() {
  return this->_requested_swing_mode;
}

std::vector<std::string> ThermostatEntity::get_supported_swing_modes() {
  return this->_supported_swing_modes;
}

void ThermostatEntity::send_state_update_to_nspanel() {
  NSPanelEntityState state;
  NSPanelEntityState_Thermostat *th_status = state.mutable_thermostat();
  th_status->set_thermostat_id(this->_id);
  th_status->set_name(this->_name);
  th_status->set_current_temperature(this->_current_temperature);
  th_status->set_step_size(this->_step_size);

  if (!this->_supported_modes.empty()) {
    auto mode_options = th_status->add_options();
    mode_options->set_name("Mode");
    mode_options->set_current_value(this->_current_mode);
    mode_options->set_icon(EntityIcons::thermostat_auto); // TODO: Enable user selectable icons for each mode.
    for (const auto &mode : this->_supported_modes) {
      SPDLOG_DEBUG("Adding HVAC option '{}' to thermostat {}::{} state message.", mode, this->_id, this->_name);
      mode_options->add_options(mode);
    }
  }

  if (!this->_supported_fan_modes.empty()) {
    auto fan_options = th_status->add_options();
    fan_options->set_name("Fan");
    fan_options->set_current_value(this->_current_fan_mode);
    fan_options->set_icon(EntityIcons::fan_auto); // TODO: Enable user selectable icons for each mode.
    for (const auto &mode : this->_supported_fan_modes) {
      SPDLOG_DEBUG("Adding fan option '{}' to thermostat {}::{} state message.", mode, this->_id, this->_name);
      fan_options->add_options(mode);
    }
  }

  if (!this->_supported_presets.empty()) {
    auto preset_options = th_status->add_options();
    preset_options->set_name("Preset"); // TODO: Enable user selectable icons for each mode.
    preset_options->set_current_value(this->_current_preset);
    preset_options->set_icon(EntityIcons::eco);
    for (const auto &preset : this->_supported_presets) {
      SPDLOG_DEBUG("Adding preset option '{}' to thermostat {}::{} state message.", preset, this->_id, this->_name);
      preset_options->add_options(preset);
    }
  }

  if (!this->_supported_swing_modes.empty()) {
    auto swing_options = th_status->add_options();
    swing_options->set_name("Swing");
    swing_options->set_current_value(this->_current_swing_mode);
    swing_options->set_icon(EntityIcons::swing_vertical); // TODO: Enable user selectable icons for each mode.
    for (const auto &mode : this->_supported_swing_modes) {
      SPDLOG_DEBUG("Adding swing option '{}' to thermostat {}::{} state message.", mode, this->_id, this->_name);
      swing_options->add_options(mode);
    }
  }

  if (!this->_supported_swing_modes.empty()) {
    auto swing_options = th_status->add_options();
    swing_options->set_name("Swing Horizontal");
    swing_options->set_current_value(this->_current_swingh_mode);
    swing_options->set_icon(EntityIcons::swing_horizontal); // TODO: Enable user selectable icons for each mode.
    for (const auto &mode : this->_supported_swingh_modes) {
      SPDLOG_DEBUG("Adding swingh option '{}' to thermostat {}::{} state message.", mode, this->_id, this->_name);
      swing_options->add_options(mode);
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
}

void ThermostatEntity::set_fan_mode(std::string fan_mode) {
  if (std::find(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), fan_mode) != this->_supported_fan_modes.end()) {
    this->_requested_fan_mode = fan_mode;
  } else {
    SPDLOG_ERROR("Invalid fan mode ({}) for thermostat {}::{}. Will default to 'off'.", fan_mode, this->_id, this->_name);
    this->_requested_fan_mode = "off";
  }
  this->send_state_update_to_controller();
}

std::string ThermostatEntity::get_fan_mode() {
  return this->_requested_fan_mode;
}

std::vector<std::string> ThermostatEntity::get_supported_fan_modes() {
  return this->_supported_fan_modes;
}

void ThermostatEntity::set_preset(std::string preset) {
  if (std::find(this->_supported_presets.begin(), this->_supported_presets.end(), preset) != this->_supported_presets.end()) {
    this->_requested_preset = preset;
  } else {
    SPDLOG_ERROR("Invalid preset ({}) for thermostat {}::{}. Will default to 'off'.", preset, this->_id, this->_name);
    this->_requested_preset = "off";
  }
  this->send_state_update_to_controller();
}

std::string ThermostatEntity::get_preset() {
  return this->_requested_preset;
}

std::vector<std::string> ThermostatEntity::get_supported_presets() {
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
