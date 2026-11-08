#include "cover/cover.hpp"
#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity_icons.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <fmt/format.h>
#include <google/protobuf/util/message_differencer.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

CoverEntity::CoverEntity(uint32_t cover_id) {
  this->_id = cover_id;
  this->_load_common_config();

  CommandManager::attach_callback(boost::bind(&CoverEntity::command_callback, this, _1));

  SPDLOG_DEBUG("Cover {}::{} base loaded.", this->_id, this->_name);
}

void CoverEntity::_load_common_config() {
  auto cover = database_manager::database.get<database_manager::Entity>(this->_id);
  this->_name = cover.friendly_name;
  this->_room_id = cover.room_id;
  this->_entity_page_id = cover.entities_page_id;
  this->_entity_page_slot = cover.room_view_position;

  nlohmann::json entity_data = cover.get_entity_data_json();
  if (entity_data.contains("controller") && entity_data["controller"].is_string()) {
    std::string controller = entity_data["controller"];
    if (controller.compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else if (controller.compare("openhab") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
    } else {
      SPDLOG_ERROR("Got unknown controller ({}) for cover {}::{}. Will default to HOME_ASSISTANT.", controller, this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }
  } else {
    SPDLOG_ERROR("No controller defined for cover {}::{}. Will default to HOME_ASSISTANT.", this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }
}

void CoverEntity::reload_config() {
  this->_load_common_config();
}

uint16_t CoverEntity::get_room_id() {
  return this->_room_id;
}

std::string CoverEntity::get_name() {
  return this->_name;
}

uint16_t CoverEntity::get_id() {
  return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE CoverEntity::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::COVER;
}

MQTT_MANAGER_ENTITY_CONTROLLER CoverEntity::get_controller() {
  return this->_controller;
}

uint32_t CoverEntity::get_entity_page_id() {
  return this->_entity_page_id;
}

uint8_t CoverEntity::get_entity_page_slot() {
  return this->_entity_page_slot;
}

NSPanelEntityState_Cover_CoverState CoverEntity::get_state() {
  return this->_state;
}

bool CoverEntity::has_position() {
  return this->_has_position;
}

uint8_t CoverEntity::get_position() {
  return this->_position;
}

bool CoverEntity::has_tilt_position() {
  return this->_has_tilt_position;
}

uint8_t CoverEntity::get_tilt_position() {
  return this->_tilt_position;
}

bool CoverEntity::can_open() {
  return this->_can_open;
}

bool CoverEntity::can_close() {
  return this->_can_close;
}

bool CoverEntity::can_stop() {
  return this->_can_stop;
}

bool CoverEntity::can_set_position() {
  return this->_can_set_position;
}

std::string CoverEntity::get_device_class() {
  return this->_device_class;
}

void CoverEntity::send_state_update_to_nspanel() {
  NSPanelEntityState state;
  NSPanelEntityState_Cover *cover_state = state.mutable_cover();
  cover_state->set_cover_id(this->_id);
  cover_state->set_name(this->_name);
  cover_state->set_state(this->_state);
  cover_state->set_has_position(this->_has_position);
  cover_state->set_position(this->_has_position ? this->_position : 0);
  cover_state->set_has_tilt_position(this->_has_tilt_position);
  cover_state->set_tilt_position(this->_has_tilt_position ? this->_tilt_position : 0);
  cover_state->set_can_open(this->_can_open);
  cover_state->set_can_close(this->_can_close);
  cover_state->set_can_stop(this->_can_stop);
  cover_state->set_can_set_position(this->_can_set_position);
  cover_state->set_can_open_tilt(this->_can_open_tilt);
  cover_state->set_can_close_tilt(this->_can_close_tilt);
  cover_state->set_can_stop_tilt(this->_can_stop_tilt);
  cover_state->set_can_set_tilt_position(this->_can_set_tilt_position);
  cover_state->set_device_class(this->_device_class);

  google::protobuf::util::MessageDifferencer differencer;
  if (!differencer.Equals(this->_last_cover_state, state)) {
    SPDLOG_DEBUG("Sending updated state for cover {}::{} over MQTT.", this->_id, this->_name);
    this->_last_cover_state = state;
    MQTT_Manager::publish_protobuf(this->get_mqtt_state_topic(), state, true);
  } else {
    SPDLOG_TRACE("Did not send state update for cover {}::{} as there were no changes.", this->_id, this->_name);
  }
}

void CoverEntity::command_callback(NSPanelMQTTManagerCommand &command) {
  if (!command.has_cover_command() || static_cast<uint32_t>(command.cover_command().cover_id()) != this->_id) {
    return;
  }

  auto cover_command = command.cover_command();
  SPDLOG_DEBUG("Received command from NSPanel for cover {}::{}.", this->_id, this->_name);

  switch (cover_command.action()) {
  case NSPanelMQTTManagerCommand_CoverCommand_Action_OPEN:
    this->open();
    break;
  case NSPanelMQTTManagerCommand_CoverCommand_Action_CLOSE:
    this->close();
    break;
  case NSPanelMQTTManagerCommand_CoverCommand_Action_STOP:
    this->stop();
    break;
  case NSPanelMQTTManagerCommand_CoverCommand_Action_OPEN_TILT:
    this->open_tilt();
    break;
  case NSPanelMQTTManagerCommand_CoverCommand_Action_CLOSE_TILT:
    this->close_tilt();
    break;
  case NSPanelMQTTManagerCommand_CoverCommand_Action_STOP_TILT:
    this->stop_tilt();
    break;
  case NSPanelMQTTManagerCommand_CoverCommand_Action_NONE:
    // The command only changes a position.
    break;
  default:
    SPDLOG_WARN("Received unknown action {} for cover {}::{}.", static_cast<int>(cover_command.action()), this->_id, this->_name);
    break;
  }

  if (cover_command.has_position()) {
    this->set_position(static_cast<uint8_t>(std::clamp(cover_command.position(), 0, 100)));
  }
  if (cover_command.has_tilt_position()) {
    this->set_tilt_position(static_cast<uint8_t>(std::clamp(cover_command.tilt_position(), 0, 100)));
  }
}

bool CoverEntity::can_toggle() {
  // A dedicated cover page is the primary UI. Until that firmware page is installed, allow the
  // generic entities-page icon to open/close blinds and shutters. Never quick-toggle a garage
  // door, gate or door from a single tap.
  if (this->_state == NSPanelEntityState_Cover_CoverState_UNKNOWN || this->_state == NSPanelEntityState_Cover_CoverState_UNAVAILABLE) {
    return false;
  }
  if (this->_device_class.compare("garage") == 0 || this->_device_class.compare("gate") == 0 || this->_device_class.compare("door") == 0) {
    return false;
  }
  if (this->_state == NSPanelEntityState_Cover_CoverState_OPENING || this->_state == NSPanelEntityState_Cover_CoverState_CLOSING) {
    return this->_can_stop;
  }
  bool open = this->_has_position ? this->_position > 0 : this->_state == NSPanelEntityState_Cover_CoverState_OPEN;
  return open ? this->_can_close : this->_can_open;
}

void CoverEntity::toggle() {
  if (!this->can_toggle()) {
    return;
  }
  if (this->_state == NSPanelEntityState_Cover_CoverState_OPENING || this->_state == NSPanelEntityState_Cover_CoverState_CLOSING) {
    this->stop();
  } else if (this->_has_position ? this->_position == 0 : this->_state == NSPanelEntityState_Cover_CoverState_CLOSED) {
    this->open();
  } else {
    this->close();
  }
}

std::string_view CoverEntity::get_icon() {
  bool open = this->_state != NSPanelEntityState_Cover_CoverState_UNAVAILABLE && this->_state != NSPanelEntityState_Cover_CoverState_UNKNOWN &&
              (this->_has_position ? this->_position > 0 : this->_state == NSPanelEntityState_Cover_CoverState_OPEN);
  return open ? EntityIcons::entity_icon_cover_open : EntityIcons::entity_icon_cover_closed;
}

uint16_t CoverEntity::get_icon_color() {
  bool open = this->_state != NSPanelEntityState_Cover_CoverState_UNAVAILABLE && this->_state != NSPanelEntityState_Cover_CoverState_UNKNOWN &&
              (this->_has_position ? this->_position > 0 : this->_state == NSPanelEntityState_Cover_CoverState_OPEN);
  return open ? GUI_Colors::icon_color_on : GUI_Colors::icon_color_off;
}

uint16_t CoverEntity::get_icon_active_color() {
  return this->get_icon_color();
}

std::string CoverEntity::get_mqtt_state_topic() {
  std::string manager_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS);
  return fmt::format("nspanel/mqttmanager_{}/entities/covers/{}/state", manager_address, this->get_id());
}

CoverEntity::~CoverEntity() {
  SPDLOG_DEBUG("Destructor for cover {}::{} called.", this->_id, this->_name);
  this->_signal_entity_destroyed();
  CommandManager::detach_callback(boost::bind(&CoverEntity::command_callback, this, _1));
}
