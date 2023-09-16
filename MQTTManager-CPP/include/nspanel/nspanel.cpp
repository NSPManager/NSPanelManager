#include "nspanel.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
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

  this->_mqtt_command_topic = "nspanel/";
  this->_mqtt_command_topic.append(this->_name);
  this->_mqtt_command_topic.append("/command");

  MQTT_Manager::attach_observer(this);
}

uint NSPanel::get_id() {
  return this->_id;
}

std::string NSPanel::get_mac() {
  return this->_mac;
}

bool NSPanel::mqtt_callback(const std::string &topic, const std::string &payload) {
  if (topic.compare(this->_mqtt_log_topic) == 0) {
    // Split log message by semicolon to extract MAC, log level and message.
    std::string message = payload;
    std::vector<std::string> message_parts;
    size_t pos = 0;
    uint8_t count = 0;
    std::string token;
    while ((pos = message.find(";")) != std::string::npos && count < 2) {
      token = message.substr(0, pos);
      message_parts.push_back(token);
      message.erase(0, pos + 1); // Remove current part from beginning of topic string (including delimiter)
      count++;
    }
    message_parts.push_back(message);

    if (message_parts.size() == 3) {
      std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      std::tm tm = *std::localtime(&now);
      std::stringstream buffer;
      if (!MqttManagerConfig::clock_us_style) {
        buffer << std::put_time(&tm, "%H:%M:%S");
      } else {
        buffer << std::put_time(&tm, "%I:%M:%S %p");
      }

      nlohmann::json log_data;
      log_data["type"] = "log";
      log_data["time"] = buffer.str();
      log_data["panel"] = this->_name;
      log_data["mac"] = message_parts[0];
      log_data["level"] = message_parts[1];
      log_data["message"] = message_parts[2];
      WebsocketServer::broadcast_json(log_data);
      return true;
    } else {
      SPDLOG_ERROR("Received message on log topic {} with wrong format. Message: {}", topic, payload);
    }
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
      nlohmann::json status_reps;
      status_reps["type"] = "status";
      status_reps["payload"] = this->get_websocket_json_representation();
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
      nlohmann::json status_reps;
      status_reps["type"] = "status";
      status_reps["payload"] = this->get_websocket_json_representation();
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

void NSPanel::send_command(nlohmann::json &command) {
  std::string buffer = command.dump();
  MQTT_Manager::publish(this->_mqtt_command_topic, buffer);
}

std::string NSPanel::get_name() {
  return this->_name;
}
