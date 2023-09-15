#include "nspanel.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include <spdlog/spdlog.h>
#include <websocket_server/websocket_server.hpp>

NSPanel::NSPanel(nlohmann::json &init_data) {
  this->_id = init_data["id"];
  this->_name = init_data["name"];
  this->_mac = init_data["mac"];
  this->_ip_address = init_data["address"];
  this->_is_us_panel = std::string(init_data["is_us_panel"]).compare("True") == 0;
  this->_state = MQTT_MANAGER_NSPANEL_STATE::UNKNOWN;
  this->_rssi = -100;
  this->_heap_used_pct = 0;
  this->_nspanel_warnings = "";
  this->_temperature = -255;
  SPDLOG_DEBUG("Loaded NSPanel {}::{}.", this->_id, this->_name);

  this->_mqtt_log_topic = "nspanel/";
  this->_mqtt_log_topic.append(this->_name);
  this->_mqtt_log_topic.append("/log");

  this->_mqtt_status_topic = "nspanel/";
  this->_mqtt_status_topic.append(this->_name);
  this->_mqtt_status_topic.append("/status");

  this->_mqtt_status_report_topic = "nspanel/";
  this->_mqtt_status_report_topic.append(this->_name);
  this->_mqtt_status_report_topic.append("/status_report");

  MQTT_Manager::attach_observer(this);
}

uint NSPanel::get_id() {
  return this->_id;
}

std::string NSPanel::get_mac() {
  return this->_mac;
}

bool NSPanel::mqtt_callback(const std::string &topic, const std::string &payload) {
  // TODO: When sending updated data to websocket, use internal filtered data instead of sending raw MQTT json to websocket.
  if (topic.compare(this->_mqtt_log_topic) == 0) {
    // TODO: Handle log
    return true;
  } else if (topic.compare(this->_mqtt_status_topic) == 0) {
    nlohmann::json data = nlohmann::json::parse(payload);
    if (std::string(data["mac"]).compare(this->_mac) == 0) {
      // Update internal state.
      std::string state = data["state"];
      if (state.compare("online") == 0) {
        this->_state = MQTT_MANAGER_NSPANEL_STATE::ONLINE;
      } else if (state.compare("offline") == 0) {
        this->_state = MQTT_MANAGER_NSPANEL_STATE::OFFLINE;
      } else {
        SPDLOG_ERROR("Received unknown state for nspanel {}::{}. State: {}", this->_id, this->_name, state);
      }

      // Send status over to web interface:
      nlohmann::json status_reps = this->get_websocket_json_representation();
      WebsocketServer::broadcast_json(status_reps);
      return true;
    }
  } else if (topic.compare(this->_mqtt_status_report_topic) == 0) {
    nlohmann::json data = nlohmann::json::parse(payload);
    if (std::string(data["mac"]).compare(this->_mac) == 0) {
      // Update internal status
      this->_rssi = data["rssi"];
      this->_heap_used_pct = data["heap_used_pct"];
      this->_temperature = data["temperature"];
      this->_ip_address = data["ip"];
      this->_nspanel_warnings = data["warnings"];

      // Send status over to web interface:
      nlohmann::json status_reps = this->get_websocket_json_representation();
      WebsocketServer::broadcast_json(status_reps);
      return true;
    }
    return true;
  }
  return false;
}

nlohmann::json NSPanel::get_websocket_json_representation() {
  nlohmann::json data;
  switch (this->_state) {
  case MQTT_MANAGER_NSPANEL_STATE::ONLINE:
    data["state"] = "online";
    break;
  case MQTT_MANAGER_NSPANEL_STATE::OFFLINE:
    data["state"] = "offline";
    break;
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE:
    data["state"] = "update_fw";
    break;
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT:
    data["state"] = "update_tft";
    break;
  default:
    data["state"] = "unknown";
    break;
  }
  data["rssi"] = this->_rssi;
  data["heap_used_pct"] = this->_heap_used_pct;
  data["mac"] = this->_mac;
  data["temperature"] = this->_temperature;
  data["ip"] = this->_ip_address;
  data["warnings"] = this->_nspanel_warnings;

  return data;
}
