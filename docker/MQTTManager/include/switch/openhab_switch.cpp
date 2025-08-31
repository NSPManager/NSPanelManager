#include "openhab_switch.hpp"
#include "database_manager/database_manager.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include "protobuf_general.pb.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

OpenhabSwitch::OpenhabSwitch(uint32_t switch_id) : SwitchEntity(switch_id) {
  // Process OpenHAB specific details. General light data is loaded in the "Light" constructor.
  //
  this->_last_on_off_change = 0;

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB) {
    SPDLOG_ERROR("OpenhabSwitch has not been recognized as controlled by OPENHAB. Will stop processing light.");
    return;
  }

  auto switch_config = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = switch_config.get_entity_data_json();

  if (entity_data.contains("openhab_name")) {
    this->_openhab_name = entity_data["openhab_name"];
  } else {
    SPDLOG_ERROR("No OpenHAB name defined for switch {}::{}", this->_id, this->_name);
  }
  if (entity_data.contains("openhab_item_switch")) {
    this->_openhab_on_off_item = entity_data["openhab_item_switch"];
  } else {
    SPDLOG_ERROR("No OpenHAB on/off item defined for switch {}::{}", this->_id, this->_name);
  }

  OpenhabManager::attach_event_observer(this->_openhab_on_off_item, boost::bind(&OpenhabSwitch::openhab_event_callback, this, _1));

  SPDLOG_DEBUG("Loaded light {}::{}.", this->_id, this->_name);
}

OpenhabSwitch::~OpenhabSwitch() {
  OpenhabManager::detach_event_observer(this->_openhab_on_off_item, boost::bind(&OpenhabSwitch::openhab_event_callback, this, _1));
}

void OpenhabSwitch::send_state_update_to_controller() {
  std::lock_guard<std::mutex> lock_guard(this->_openhab_items_mutex);
  // SPDLOG_DEBUG("--- Sending light {}::{} event state ---", this->_id, this->_name);
  // SPDLOG_DEBUG("Current mode: {}", this->_current_mode == MQTT_MANAGER_LIGHT_MODE::RGB ? "RGB" : "DEFAULT");
  // SPDLOG_DEBUG("Requested state: {}, current: {}", this->_requested_state, this->_current_state);
  // SPDLOG_DEBUG("Requested brightness: {}, current: {}", this->_requested_brightness, this->_current_brightness);
  // SPDLOG_DEBUG("Requested color_temperature: {}, current: {}", this->_requested_color_temperature, this->_current_color_temperature);
  // SPDLOG_DEBUG("Requested hue: {}, current: {}", this->_requested_hue, this->_current_hue);
  // SPDLOG_DEBUG("Requested saturation: {}, current: {}", this->_requested_saturation, this->_current_saturation);

  nlohmann::json service_data;
  service_data["type"] = "ItemCommandEvent";
  service_data["topic"] = fmt::format("openhab/items/{}/command", this->_openhab_on_off_item);
  nlohmann::json payload_data;
  payload_data["type"] = "OnOff";
  payload_data["value"] = this->_requested_state ? "ON" : "OFF";
  service_data["payload"] = payload_data.dump();

  if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
    this->_current_state = this->_requested_state;
    this->_entity_changed_callbacks(this);
  }
  OpenhabManager::send_json(service_data);
}

void OpenhabSwitch::openhab_event_callback(nlohmann::json data) {
  std::lock_guard<std::mutex> lock_guard(this->_openhab_items_mutex);
  if (std::string(data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));

    if (topic_parts.size() < 3) {
      SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enought parts, topic: {}", std::string(data["topic"]));
      return;
    }

    std::string topic_item = topic_parts[2];
    nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
    if (topic_item.compare(this->_openhab_on_off_item) == 0) {
      // We only care about the first event from Openhab, ignore the rest
      if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() >= this->_last_on_off_change + 1000) {
        bool state = std::string(payload["value"]).compare("ON") == 0;
        SPDLOG_DEBUG("Switch {}::{} got new state {}, current state: {}.", this->_id, this->_name, state, this->_current_state);
        if (state != this->_current_state) {
          this->_current_state = state;
          this->_requested_state = this->_current_state;

          this->_last_on_off_change = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
          this->_signal_entity_changed();
        }
      }
    }
    return;
  } else if (std::string(data["type"]).compare("ItemStateFetched") == 0) {
    SPDLOG_TRACE("OpenHAB light {}::{} Got initial data from OpenHAB via custom ItemStateFetched event.", this->_id, this->_name);
    if (this->_openhab_on_off_item.compare(data["payload"]["name"]) == 0) {
      if (data["payload"]["state"].is_string()) {
        nlohmann::json payload = nlohmann::json::parse(std::string(data["payload"]));
        bool state = std::string(payload["value"]).compare("ON") == 0;
        SPDLOG_DEBUG("Switch {}::{} got new state {}, current state: {}.", this->_id, this->_name, state, this->_current_state);
        if (state != this->_current_state) {
          this->_current_state = state;
          this->_requested_state = this->_current_state;

          this->_last_on_off_change = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
          SPDLOG_ERROR("Light {}::{} got OpenHAB data from OpenHAB via initial ItemStateFetched event for OpenHAB item {} though current on/off state was neither on or off.", this->_id, this->_name, this->_openhab_on_off_item);
        }
      }
    }
  } else {
    return;
  }
}
