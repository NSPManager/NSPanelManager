#include "cover/home_assistant_cover.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <cmath>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

HomeAssistantCover::HomeAssistantCover(uint32_t cover_id) : CoverEntity(cover_id) {
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantCover has not been recognized as controlled by HOME_ASSISTANT. Will stop processing cover.");
    return;
  }

  if (!this->_load_home_assistant_config()) {
    return;
  }

  SPDLOG_DEBUG("Loaded cover {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantCover::home_assistant_event_callback, this, _1));

  this->send_state_update_to_nspanel();
}

HomeAssistantCover::~HomeAssistantCover() {
  if (!this->_home_assistant_name.empty()) {
    HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantCover::home_assistant_event_callback, this, _1));
  }
}

bool HomeAssistantCover::_load_home_assistant_config() {
  auto cover = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = cover.get_entity_data_json();

  if (entity_data.contains("home_assistant_name") && entity_data["home_assistant_name"].is_string()) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No Home Assistant name defined for cover {}::{}", this->_id, this->_name);
    this->_home_assistant_name.clear();
    return false;
  }

  if (this->_home_assistant_name.rfind("cover.", 0) != 0) {
    SPDLOG_ERROR("Unknown type of Home Assistant entity '{}' for cover {}::{}. Expected cover.*.", this->_home_assistant_name, this->_id, this->_name);
    this->_home_assistant_name.clear();
    return false;
  }

  return true;
}

void HomeAssistantCover::reload_config() {
  CoverEntity::reload_config();

  std::string previous_name = this->_home_assistant_name;
  if (!this->_load_home_assistant_config()) {
    if (!previous_name.empty()) {
      HomeAssistantManager::detach_event_observer(previous_name, boost::bind(&HomeAssistantCover::home_assistant_event_callback, this, _1));
    }
    this->send_state_update_to_nspanel();
    return;
  }

  if (previous_name != this->_home_assistant_name) {
    if (!previous_name.empty()) {
      HomeAssistantManager::detach_event_observer(previous_name, boost::bind(&HomeAssistantCover::home_assistant_event_callback, this, _1));
    }
    HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantCover::home_assistant_event_callback, this, _1));
  }

  this->send_state_update_to_nspanel();
}

void HomeAssistantCover::_call_service(const std::string &service, nlohmann::json service_data) {
  nlohmann::json command = {
      {"type", "call_service"},
      {"domain", "cover"},
      {"target", {{"entity_id", this->_home_assistant_name}}},
      {"service", service},
  };
  if (!service_data.empty()) {
    command["service_data"] = service_data;
  }
  HomeAssistantManager::send_json(command);
}

void HomeAssistantCover::open() {
  if (!this->_can_open) {
    SPDLOG_WARN("Asked to open cover {}::{} but Home Assistant does not report the open feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("open_cover");
}

void HomeAssistantCover::close() {
  if (!this->_can_close) {
    SPDLOG_WARN("Asked to close cover {}::{} but Home Assistant does not report the close feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("close_cover");
}

void HomeAssistantCover::stop() {
  if (!this->_can_stop) {
    SPDLOG_WARN("Asked to stop cover {}::{} but Home Assistant does not report the stop feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("stop_cover");
}

void HomeAssistantCover::set_position(uint8_t position) {
  if (!this->_can_set_position) {
    SPDLOG_WARN("Asked to set position of cover {}::{} but Home Assistant does not report the set position feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("set_cover_position", {{"position", position}});
}

void HomeAssistantCover::open_tilt() {
  if (!this->_can_open_tilt) {
    SPDLOG_WARN("Asked to open tilt of cover {}::{} but Home Assistant does not report the open tilt feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("open_cover_tilt");
}

void HomeAssistantCover::close_tilt() {
  if (!this->_can_close_tilt) {
    SPDLOG_WARN("Asked to close tilt of cover {}::{} but Home Assistant does not report the close tilt feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("close_cover_tilt");
}

void HomeAssistantCover::stop_tilt() {
  if (!this->_can_stop_tilt) {
    SPDLOG_WARN("Asked to stop tilt of cover {}::{} but Home Assistant does not report the stop tilt feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("stop_cover_tilt");
}

void HomeAssistantCover::set_tilt_position(uint8_t position) {
  if (!this->_can_set_tilt_position) {
    SPDLOG_WARN("Asked to set tilt position of cover {}::{} but Home Assistant does not report the set tilt position feature.", this->_id, this->_name);
    return;
  }
  this->_call_service("set_cover_tilt_position", {{"tilt_position", position}});
}

void HomeAssistantCover::home_assistant_event_callback(nlohmann::json data) {
  if (!data.contains("event") || !data["event"].contains("event_type") || !data["event"].contains("data") || !data["event"]["data"].contains("entity_id")) {
    SPDLOG_ERROR("Cover {}::{} received malformed event data from Home Assistant.", this->_id, this->_name);
    return;
  }
  if (std::string(data["event"]["event_type"]).compare("state_changed") != 0 || std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) != 0) {
    return;
  }

  uint16_t previous_icon_color = this->get_icon_color();

  try {
    nlohmann::json new_state_data = data["event"]["data"]["new_state"];
    if (new_state_data.is_null()) {
      // Entity was removed from Home Assistant.
      this->_state = NSPanelEntityState_Cover_CoverState_UNAVAILABLE;
      this->_has_position = false;
      this->_has_tilt_position = false;
      this->_can_open = false;
      this->_can_close = false;
      this->_can_stop = false;
      this->_can_set_position = false;
      this->_can_open_tilt = false;
      this->_can_close_tilt = false;
      this->_can_stop_tilt = false;
      this->_can_set_tilt_position = false;
      this->_device_class.clear();
    } else {
      std::string new_state = new_state_data.contains("state") && new_state_data["state"].is_string() ? new_state_data["state"].get<std::string>() : "";
      if (new_state.compare("open") == 0) {
        this->_state = NSPanelEntityState_Cover_CoverState_OPEN;
      } else if (new_state.compare("closed") == 0) {
        this->_state = NSPanelEntityState_Cover_CoverState_CLOSED;
      } else if (new_state.compare("opening") == 0) {
        this->_state = NSPanelEntityState_Cover_CoverState_OPENING;
      } else if (new_state.compare("closing") == 0) {
        this->_state = NSPanelEntityState_Cover_CoverState_CLOSING;
      } else if (new_state.compare("unavailable") == 0) {
        this->_state = NSPanelEntityState_Cover_CoverState_UNAVAILABLE;
      } else {
        this->_state = NSPanelEntityState_Cover_CoverState_UNKNOWN;
      }

      if (new_state_data.contains("attributes") && new_state_data["attributes"].is_object()) {
        nlohmann::json attributes = new_state_data["attributes"];

        if (attributes.contains("current_position") && attributes["current_position"].is_number()) {
          double position = attributes["current_position"].get<double>();
          this->_position = static_cast<uint8_t>(std::clamp<long>(std::lround(position), 0, 100));
          this->_has_position = true;
        }
        if (attributes.contains("current_tilt_position") && attributes["current_tilt_position"].is_number()) {
          double position = attributes["current_tilt_position"].get<double>();
          this->_tilt_position = static_cast<uint8_t>(std::clamp<long>(std::lround(position), 0, 100));
          this->_has_tilt_position = true;
        }

        uint32_t supported_features = 0;
        if (attributes.contains("supported_features") && attributes["supported_features"].is_number()) {
          int64_t value = attributes["supported_features"].get<int64_t>();
          supported_features = static_cast<uint32_t>(std::max<int64_t>(value, 0));
        }
        this->_can_open = supported_features & HomeAssistantCover::feature_open;
        this->_can_close = supported_features & HomeAssistantCover::feature_close;
        this->_can_stop = supported_features & HomeAssistantCover::feature_stop;
        this->_can_set_position = supported_features & HomeAssistantCover::feature_set_position;
        this->_can_open_tilt = supported_features & HomeAssistantCover::feature_open_tilt;
        this->_can_close_tilt = supported_features & HomeAssistantCover::feature_close_tilt;
        this->_can_stop_tilt = supported_features & HomeAssistantCover::feature_stop_tilt;
        this->_can_set_tilt_position = supported_features & HomeAssistantCover::feature_set_tilt_position;

        if (attributes.contains("device_class") && attributes["device_class"].is_string()) {
          this->_device_class = attributes["device_class"];
        } else {
          this->_device_class.clear();
        }
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception while processing Home Assistant cover {}::{} event: {}", this->_id, this->_name, e.what());
    return;
  }

  this->send_state_update_to_nspanel();
  if (previous_icon_color != this->get_icon_color()) {
    this->_signal_entity_changed();
  }
}
