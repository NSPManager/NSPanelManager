#include "cover/openhab_cover.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <cmath>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <vector>

OpenhabCover::OpenhabCover(uint32_t cover_id) : CoverEntity(cover_id) {
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB) {
    SPDLOG_ERROR("OpenhabCover has not been recognized as controlled by OPENHAB. Will stop processing cover.");
    return;
  }

  if (!this->_load_openhab_config()) {
    return;
  }

  SPDLOG_DEBUG("Loaded cover {}::{}, OpenHAB item: {}", this->_id, this->_name, this->_openhab_item);
  OpenhabManager::attach_event_observer(this->_openhab_item, boost::bind(&OpenhabCover::openhab_event_callback, this, _1));

  this->send_state_update_to_nspanel();
}

OpenhabCover::~OpenhabCover() {
  if (!this->_openhab_item.empty()) {
    OpenhabManager::detach_event_observer(this->_openhab_item, boost::bind(&OpenhabCover::openhab_event_callback, this, _1));
  }
}

bool OpenhabCover::_load_openhab_config() {
  auto cover = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = cover.get_entity_data_json();

  if (entity_data.contains("openhab_item") && entity_data["openhab_item"].is_string()) {
    this->_openhab_item = entity_data["openhab_item"];
  } else {
    SPDLOG_ERROR("No OpenHAB item defined for cover {}::{}", this->_id, this->_name);
    this->_openhab_item.clear();
    return false;
  }

  if (this->_openhab_item.empty()) {
    SPDLOG_ERROR("Empty OpenHAB item defined for cover {}::{}", this->_id, this->_name);
    return false;
  }

  return true;
}

void OpenhabCover::reload_config() {
  CoverEntity::reload_config();

  std::string previous_item = this->_openhab_item;
  if (!this->_load_openhab_config()) {
    if (!previous_item.empty()) {
      OpenhabManager::detach_event_observer(previous_item, boost::bind(&OpenhabCover::openhab_event_callback, this, _1));
    }
    this->send_state_update_to_nspanel();
    return;
  }

  if (previous_item != this->_openhab_item) {
    if (!previous_item.empty()) {
      OpenhabManager::detach_event_observer(previous_item, boost::bind(&OpenhabCover::openhab_event_callback, this, _1));
    }
    OpenhabManager::attach_event_observer(this->_openhab_item, boost::bind(&OpenhabCover::openhab_event_callback, this, _1));
  }

  this->send_state_update_to_nspanel();
}

void OpenhabCover::_send_command(const std::string &type, const std::string &value) {
  nlohmann::json event_data;
  event_data["type"] = "ItemCommandEvent";
  event_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_item);
  nlohmann::json payload;
  payload["type"] = type;
  payload["value"] = value;
  event_data["payload"] = payload.dump();
  OpenhabManager::send_json(event_data);
}

void OpenhabCover::open() {
  // OpenHAB Rollershutter percentages run from 0 (open) to 100 (closed).
  this->_send_command("Percent", "0");
}

void OpenhabCover::close() {
  this->_send_command("Percent", "100");
}

void OpenhabCover::stop() {
  this->_send_command("StopMove", "STOP");
}

void OpenhabCover::set_position(uint8_t position) {
  // The panel uses 0 = closed, 100 = open; OpenHAB uses the reverse.
  this->_send_command("Percent", std::to_string(100 - position));
}

void OpenhabCover::open_tilt() {
  SPDLOG_WARN("OpenHAB cover {}::{} does not support tilt.", this->_id, this->_name);
}

void OpenhabCover::close_tilt() {
  SPDLOG_WARN("OpenHAB cover {}::{} does not support tilt.", this->_id, this->_name);
}

void OpenhabCover::stop_tilt() {
  SPDLOG_WARN("OpenHAB cover {}::{} does not support tilt.", this->_id, this->_name);
}

void OpenhabCover::set_tilt_position(uint8_t position) {
  SPDLOG_WARN("OpenHAB cover {}::{} does not support tilt.", this->_id, this->_name);
}

void OpenhabCover::_update_from_state(const nlohmann::json &state) {
  uint16_t previous_icon_color = this->get_icon_color();

  if (state.is_string()) {
    std::string value = state;
    if (value.compare("NULL") == 0 || value.compare("UNDEF") == 0) {
      this->_state = NSPanelEntityState_Cover_CoverState_UNAVAILABLE;
      this->_has_position = false;
      this->_has_tilt_position = false;
      this->send_state_update_to_nspanel();
      if (previous_icon_color != this->get_icon_color()) {
        this->_signal_entity_changed();
      }
      return;
    }

    try {
      double percentage = std::stod(value);
      percentage = std::clamp(percentage, 0.0, 100.0);
      uint8_t openness = static_cast<uint8_t>(std::clamp<long>(std::lround(100.0 - percentage), 0, 100));

      this->_has_position = true;
      this->_position = openness;
      if (percentage <= 0.0) {
        this->_state = NSPanelEntityState_Cover_CoverState_OPEN;
      } else if (percentage >= 100.0) {
        this->_state = NSPanelEntityState_Cover_CoverState_CLOSED;
      } else {
        this->_state = NSPanelEntityState_Cover_CoverState_OPEN;
      }
      this->_can_open = true;
      this->_can_close = true;
      this->_can_stop = true;
      this->_can_set_position = true;
      this->_can_open_tilt = false;
      this->_can_close_tilt = false;
      this->_can_stop_tilt = false;
      this->_can_set_tilt_position = false;
      this->_device_class = "shutter";
    } catch (std::exception &e) {
      SPDLOG_ERROR("Could not parse OpenHAB cover state '{}' for {}::{}: {}", value, this->_id, this->_name, e.what());
      return;
    }
  } else if (state.is_number()) {
    this->_update_from_state(nlohmann::json(std::to_string(state.get<double>())));
    return;
  } else {
    SPDLOG_WARN("Unsupported OpenHAB cover state for {}::{}: {}", this->_id, this->_name, state.dump());
    return;
  }

  this->send_state_update_to_nspanel();
  if (previous_icon_color != this->get_icon_color()) {
    this->_signal_entity_changed();
  }
}

void OpenhabCover::openhab_event_callback(nlohmann::json data) {
  if (!data.contains("type")) {
    return;
  }

  std::string type = data["type"];
  if (type.compare("ItemStateChangedEvent") == 0) {
    if (!data.contains("topic") || !data.contains("payload")) {
      return;
    }
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    if (topic_parts.size() < 3 || topic_parts[2].compare(this->_openhab_item) != 0) {
      return;
    }

    try {
      nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
      if (payload.contains("value")) {
        this->_update_from_state(payload["value"]);
      }
    } catch (std::exception &e) {
      SPDLOG_ERROR("Could not parse OpenHAB state event for cover {}::{}: {}", this->_id, this->_name, e.what());
    }
  } else if (type.compare("ItemStateFetched") == 0) {
    if (!data.contains("payload") || !data["payload"].contains("name") || !data["payload"].contains("state")) {
      return;
    }
    if (std::string(data["payload"]["name"]).compare(this->_openhab_item) != 0) {
      return;
    }
    this->_update_from_state(data["payload"]["state"]);
  }
}
