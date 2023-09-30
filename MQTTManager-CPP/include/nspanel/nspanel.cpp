#include "nspanel.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <list>
#include <nlohmann/json_fwd.hpp>
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
  this->_update_progress = 0;
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

      std::string send_mac = message_parts[0];
      send_mac.erase(std::remove(send_mac.begin(), send_mac.end(), ':'), send_mac.end());

      nlohmann::json log_data;
      log_data["type"] = "log";
      log_data["time"] = buffer.str();
      log_data["panel"] = this->_name;
      log_data["mac"] = message_parts[0];
      log_data["level"] = message_parts[1];
      log_data["message"] = message_parts[2];
      WebsocketServer::broadcast_json(log_data);

      // Save log message in backtrace for when (if) the log interface requests it.
      NSPanelLogMessage message;
      message.time = buffer.str();
      message.level = message_parts[1];
      message.message = message_parts[2];
      this->_log_messages.push_front(message);
      // Remove older messages from backtrace.
      while (this->_log_messages.size() > MqttManagerConfig::max_log_buffer_size) {
        this->_log_messages.pop_back();
      }
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
        SPDLOG_DEBUG("NSPanel {}::{} become ONLINE.", this->_id, this->_name);
      } else if (state.compare("offline") == 0) {
        this->_state = MQTT_MANAGER_NSPANEL_STATE::OFFLINE;
        SPDLOG_DEBUG("NSPanel {}::{} become OFFLINE.", this->_id, this->_name);
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

      if (data.contains("state")) {
        std::string state = data["state"];
        if (state.compare("updating_tft") == 0) {
          this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT;
        } else if (state.compare("updating_fw") == 0) {
          this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE;
        } else if (state.compare("updating_fs") == 0) {
          this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA;
        } else {
          SPDLOG_ERROR("Received unknown state from nspanel {}::{}. State: {}", this->_id, this->_name, state);
        }
      }

      if (data.contains("progress")) {
        this->_update_progress = data["progress"];
      } else {
        this->_update_progress = 0;
      }

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
    data["state"] = "updating_fw";
    data["progress"] = this->_update_progress;
    break;
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA:
    data["state"] = "updating_fs";
    data["progress"] = this->_update_progress;
    break;
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT:
    data["state"] = "updating_tft";
    data["progress"] = this->_update_progress;
    break;
  case MQTT_MANAGER_NSPANEL_STATE::WAITING:
    data["state"] = "waiting";
    break;
  default:
    data["state"] = "unknown";
    break;
  }

  std::string send_mac = this->_mac;
  send_mac.erase(std::remove(send_mac.begin(), send_mac.end(), ':'), send_mac.end());

  data["id"] = this->_id;
  data["name"] = this->_name;
  data["rssi"] = this->_rssi;
  data["heap_used_pct"] = this->_heap_used_pct;
  data["mac_address"] = send_mac;
  data["ip_address"] = this->_ip_address;
  data["temperature"] = this->_temperature;
  data["warnings"] = this->_nspanel_warnings;

  return data;
}

void NSPanel::reboot() {
  SPDLOG_INFO("Sending reboot command to nspanel {}::{}.", this->_id, this->_name);
  nlohmann::json cmd;
  cmd["command"] = "reboot";
  this->send_command(cmd);

  this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
}

void NSPanel::firmware_update() {
  SPDLOG_INFO("Sending firmware update command to nspanel {}::{}.", this->_id, this->_name);
  nlohmann::json cmd;
  cmd["command"] = "firmware_update";
  this->send_command(cmd);

  this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
}

void NSPanel::tft_update() {
  SPDLOG_INFO("Sending TFT update command to nspanel {}::{}.", this->_id, this->_name);
  nlohmann::json cmd;
  cmd["command"] = "tft_update";
  this->send_command(cmd);

  this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
}

void NSPanel::send_command(nlohmann::json &command) {
  std::string buffer = command.dump();
  MQTT_Manager::publish(this->_mqtt_command_topic, buffer);
}

std::string NSPanel::get_name() {
  return this->_name;
}

nlohmann::json NSPanel::get_websocket_json_logs() {
  std::list<nlohmann::json> logs;
  for (NSPanelLogMessage log : this->_log_messages) {
    nlohmann::json log_message;
    log_message["time"] = log.time;
    log_message["level"] = log.level;
    log_message["message"] = log.message;
    logs.push_front(log_message);
  }

  nlohmann::json response;
  response["logs"] = logs;
  return response;
}
