#include "nspanel.hpp"
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <curl/curl.h>
#include <exception>
#include <fmt/core.h>
#include <iomanip>
#include <list>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <websocket_server/websocket_server.hpp>

NSPanelRelayGroup::NSPanelRelayGroup(nlohmann::json &config) {
  this->update_config(config);
}

void NSPanelRelayGroup::update_config(nlohmann::json &config) {
  this->_id = config["id"];
  this->_name = config["name"];

  this->_nspanel_relays.clear();
  for (nlohmann::json nspanel_relay : config["relays"]) {
    this->_nspanel_relays.insert(std::make_pair<int, int>(int(nspanel_relay["nspanel_id"]), int(nspanel_relay["relay_num"])));
  }
}

NSPanelRelayGroup::~NSPanelRelayGroup() {
}

uint16_t NSPanelRelayGroup::get_id() {
  return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE NSPanelRelayGroup::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::NSPANEL_RELAY_GROUP;
}

MQTT_MANAGER_ENTITY_CONTROLLER NSPanelRelayGroup::get_controller() {
  return MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
}

void NSPanelRelayGroup::post_init() {
}

bool NSPanelRelayGroup::contains(int nspanel_id, int relay_num) {
  for (auto pair : this->_nspanel_relays) {
    if (pair.first == nspanel_id && pair.second == relay_num) {
      return true;
    }
  }
  return false;
}

void NSPanelRelayGroup::turn_on() {
  SPDLOG_DEBUG("Turning on NSPanelRelayGroup {}::{}", this->_id, this->_name);
  for (auto pair : this->_nspanel_relays) {
    NSPanel *panel = EntityManager::get_nspanel_by_id(pair.first);
    if (panel != nullptr) {
      panel->set_relay_state(pair.second, true);
    } else {
      SPDLOG_ERROR("Did not find NSPanel with ID {}.", pair.first);
    }
  }
}

void NSPanelRelayGroup::turn_off() {
  SPDLOG_DEBUG("Turning off NSPanelRelayGroup {}::{}", this->_id, this->_name);
  for (auto pair : this->_nspanel_relays) {
    NSPanel *panel = EntityManager::get_nspanel_by_id(pair.first);
    if (panel != nullptr) {
      panel->set_relay_state(pair.second, false);
    } else {
      SPDLOG_ERROR("Did not find NSPanel with ID {}.", pair.first);
    }
  }
}

NSPanel::NSPanel(nlohmann::json &init_data) {
  // If this panel is just a panel in waiting (ie. not accepted the request yet) it won't have an id.
  this->update_config(init_data);
}

void NSPanel::update_config(nlohmann::json &init_data) {
  if (init_data.contains("id")) {
    this->_id = init_data["id"];
  }

  bool rebuilt_mqtt = false; // Wether or not to rebuild mqtt topics and subscribe to the new topics.
  if (init_data.contains("name")) {
    if (this->_name.compare(init_data["name"]) != 0) {
      rebuilt_mqtt = true;
    }
    this->_name = init_data["name"];
  } else if (init_data.contains("friendly_name")) {
    if (this->_name.compare(init_data["friendly_name"]) != 0) {
      rebuilt_mqtt = true;
    }
    this->_name = init_data["friendly_name"];
  }

  if (init_data.contains("mac")) {
    this->_mac = init_data["mac"];
  } else if (init_data.contains("mac_origin")) {
    this->_mac = init_data["mac_origin"];
  } else {
    SPDLOG_ERROR("Creating new NSPanel with no known MAC!");
  }
  this->_ip_address = init_data["address"];
  if (init_data.contains(("is_us_panel"))) {
    this->_is_us_panel = init_data["is_us_panel"];
  } else {
    this->_is_us_panel = false;
  }
  if (!init_data.contains("id")) {
    this->_state = MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT;
  }
  this->_has_registered_to_manager = init_data.contains("id");
  this->_is_register_accepted = init_data.contains("id");
  if (this->_state == MQTT_MANAGER_NSPANEL_STATE::OFFLINE || this->_state == MQTT_MANAGER_NSPANEL_STATE::UNKNOWN) {
    this->_rssi = -255;
    this->_heap_used_pct = 0;
    this->_nspanel_warnings = "";
    this->_temperature = -255;
    this->_update_progress = 0;
  }
  if (init_data.contains("id")) {
    SPDLOG_DEBUG("Loaded NSPanel {}::{}.", this->_id, this->_name);
  } else {
    SPDLOG_DEBUG("Loaded NSPanel {} with no ID.", this->_name);
  }
  if (init_data.contains("relay1_is_light")) {
    this->_relay1_is_mqtt_light = init_data["relay1_is_light"];
  } else {
    this->_relay1_is_mqtt_light = false;
  }
  if (init_data.contains("relay2_is_light")) {
    this->_relay2_is_mqtt_light = init_data["relay2_is_light"];
  } else {
    this->_relay2_is_mqtt_light = false;
  }

  if (rebuilt_mqtt) {
    this->reset_mqtt_topics();
    // Convert stored MAC to MAC used in MQTT, ex. AA:AA:AA:BB:BB:BB to aa_aa_aa_bb_bb_bb
    std::string mqtt_register_mac = this->_mac;
    std::replace(mqtt_register_mac.begin(), mqtt_register_mac.end(), ':', '_');
    std::transform(mqtt_register_mac.begin(), mqtt_register_mac.end(), mqtt_register_mac.begin(), [](unsigned char c) {
      return std::tolower(c);
    });
    this->_mqtt_register_mac = mqtt_register_mac;

    this->_mqtt_log_topic = fmt::format("nspanel/{}/log", this->_name);
    this->_mqtt_command_topic = fmt::format("nspanel/{}/command", this->_name);
    this->_mqtt_sensor_temperature_topic = fmt::format("homeassistant/sensor/nspanelmanager/{}_temperature/config", mqtt_register_mac);
    this->_mqtt_switch_relay1_topic = fmt::format("homeassistant/switch/nspanelmanager/{}_relay1/config", mqtt_register_mac);
    this->_mqtt_light_relay1_topic = fmt::format("homeassistant/light/nspanelmanager/{}_relay1/config", mqtt_register_mac);
    this->_mqtt_switch_relay2_topic = fmt::format("homeassistant/switch/nspanelmanager/{}_relay2/config", mqtt_register_mac);
    this->_mqtt_light_relay2_topic = fmt::format("homeassistant/light/nspanelmanager/{}_relay2/config", mqtt_register_mac);
    this->_mqtt_switch_screen_topic = fmt::format("homeassistant/switch/nspanelmanager/{}_screen/config", mqtt_register_mac);
    this->_mqtt_number_screen_brightness_topic = fmt::format("homeassistant/number/nspanelmanager/{}_screen_brightness/config", mqtt_register_mac);
    this->_mqtt_number_screensaver_brightness_topic = fmt::format("homeassistant/number/nspanelmanager/{}_screensaver_brightness/config", mqtt_register_mac);
    this->_mqtt_relay1_command_topic = fmt::format("nspanel/{}/r1_cmd", this->_name);
    this->_mqtt_relay1_state_topic = fmt::format("nspanel/{}/r1_state", this->_name);
    this->_mqtt_relay2_command_topic = fmt::format("nspanel/{}/r2_cmd", this->_name);
    this->_mqtt_relay2_state_topic = fmt::format("nspanel/{}/r2_state", this->_name);
    this->_mqtt_status_topic = fmt::format("nspanel/{}/status", this->_name);
    this->_mqtt_status_report_topic = fmt::format("nspanel/{}/status_report", this->_name);
  }

  if (this->_has_registered_to_manager) {
    // If this NSPanel is registered to manager, listen to state topics.
    MQTT_Manager::subscribe(this->_mqtt_relay1_state_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_relay2_state_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_log_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_status_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_status_report_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    this->register_to_home_assistant();
  }
}

NSPanel::~NSPanel() {
  this->reset_mqtt_topics();
}

void NSPanel::reset_mqtt_topics() {
  MQTT_Manager::detach_callback(this->_mqtt_relay1_state_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
  MQTT_Manager::detach_callback(this->_mqtt_relay2_state_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
  MQTT_Manager::detach_callback(this->_mqtt_log_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
  MQTT_Manager::detach_callback(this->_mqtt_status_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
  MQTT_Manager::detach_callback(this->_mqtt_status_report_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));

  // This nspanel was removed. Clear any retain on any MQTT topic.
  MQTT_Manager::clear_retain(this->_mqtt_status_topic);
  MQTT_Manager::clear_retain(this->_mqtt_command_topic);
  MQTT_Manager::clear_retain(this->_mqtt_light_relay1_topic);
  MQTT_Manager::clear_retain(this->_mqtt_light_relay2_topic);
  MQTT_Manager::clear_retain(this->_mqtt_switch_relay1_topic);
  MQTT_Manager::clear_retain(this->_mqtt_switch_relay2_topic);
  MQTT_Manager::clear_retain(this->_mqtt_switch_screen_topic);
  MQTT_Manager::clear_retain(this->_mqtt_sensor_temperature_topic);
  MQTT_Manager::clear_retain(this->_mqtt_number_screen_brightness_topic);
  MQTT_Manager::clear_retain(this->_mqtt_number_screensaver_brightness_topic);
}

uint NSPanel::get_id() {
  return this->_id;
}

std::string NSPanel::get_mac() {
  return this->_mac;
}

MQTT_MANAGER_NSPANEL_STATE NSPanel::get_state() {
  return this->_state;
}

void NSPanel::mqtt_callback(std::string topic, std::string payload) {
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
        SPDLOG_DEBUG("NSPanel {}::{} became ONLINE.", this->_id, this->_name);
      } else if (state.compare("offline") == 0) {
        this->_state = MQTT_MANAGER_NSPANEL_STATE::OFFLINE;
        SPDLOG_DEBUG("NSPanel {}::{} became OFFLINE.", this->_id, this->_name);
      } else {
        SPDLOG_ERROR("Received unknown state for nspanel {}::{}. State: {}", this->_id, this->_name, state);
      }

      this->send_websocket_update();
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
        if (this->_state == MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT) {
          // Do nothing, simply block state change to something else.
        } else if (state.compare("updating_tft") == 0) {
          this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT;
        } else if (state.compare("updating_fw") == 0) {
          this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE;
        } else if (state.compare("updating_fs") == 0) {
          this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA;
        } else {
          SPDLOG_ERROR("Received unknown state from nspanel {}::{}. State: {}", this->_id, this->_name, state);
        }
      } else {
        if (this->_state == MQTT_MANAGER_NSPANEL_STATE::WAITING) {
          // We were waiting for a new status report. Set panel to online.
          this->_state = MQTT_MANAGER_NSPANEL_STATE::ONLINE;
        }
      }

      if (data.contains("progress")) {
        this->_update_progress = data["progress"];
      } else {
        this->_update_progress = 0;
      }

      this->send_websocket_update();
    }
  } else if (topic.compare(this->_mqtt_relay1_state_topic) == 0) {
    this->_relay1_state = (payload.compare("1") == 0);
    SPDLOG_DEBUG("NSPanel {}::{} relay1 state: {}", this->_id, this->_name, this->_relay1_state ? "ON" : "OFF");
    std::list<NSPanelRelayGroup *> relay_groups = EntityManager::get_all_entities_by_type<NSPanelRelayGroup>(MQTT_MANAGER_ENTITY_TYPE::NSPANEL_RELAY_GROUP);
    for (NSPanelRelayGroup *relay_group : relay_groups) {
      if (relay_group->contains(this->_id, 1)) {
        if (payload.compare("1") == 0) {
          relay_group->turn_on();
        } else {
          relay_group->turn_off();
        }
      }
    }
  } else if (topic.compare(this->_mqtt_relay2_state_topic) == 0) {
    this->_relay2_state = (payload.compare("1") == 0);
    SPDLOG_DEBUG("NSPanel {}::{} relay2 state: {}", this->_id, this->_name, this->_relay2_state ? "ON" : "OFF");
    std::list<NSPanelRelayGroup *> relay_groups = EntityManager::get_all_entities_by_type<NSPanelRelayGroup>(MQTT_MANAGER_ENTITY_TYPE::NSPANEL_RELAY_GROUP);
    for (NSPanelRelayGroup *relay_group : relay_groups) {
      if (relay_group->contains(this->_id, 2)) {
        if (payload.compare("1") == 0) {
          relay_group->turn_on();
        } else {
          relay_group->turn_off();
        }
      }
    }
  }
}

void NSPanel::send_websocket_update() {
  // Send status over to web interface:
  nlohmann::json status_reps;
  status_reps["type"] = "status";
  status_reps["payload"] = this->get_websocket_json_representation();
  WebsocketServer::broadcast_json(status_reps);
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
  case MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT:
    data["state"] = "awaiting_accept";
    break;
  default:
    data["state"] = "unknown";
    break;
  }

  std::string send_mac = this->_mac;
  send_mac.erase(std::remove(send_mac.begin(), send_mac.end(), ':'), send_mac.end());

  if (this->_has_registered_to_manager) {
    data["id"] = this->_id;
  }
  data["name"] = this->_name;
  data["rssi"] = this->_rssi;
  data["heap_used_pct"] = this->_heap_used_pct;
  data["mac_address"] = send_mac;
  data["ip_address"] = this->_ip_address;
  data["temperature"] = this->_temperature;
  std::string warnings_string = this->_nspanel_warnings;
  if (warnings_string.length() > 0) {
    warnings_string.append("\\n");
  }
  warnings_string.append(this->_nspanel_warnings_from_manager);
  data["warnings"] = warnings_string;

  return data;
}

void NSPanel::reboot() {
  SPDLOG_INFO("Sending reboot command to nspanel {}::{}.", this->_id, this->_name);
  nlohmann::json cmd;
  cmd["command"] = "reboot";
  this->send_command(cmd);

  if (this->_state != MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT) {
    this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
  }
}

void NSPanel::firmware_update() {
  SPDLOG_INFO("Sending firmware update command to nspanel {}::{}.", this->_id, this->_name);
  nlohmann::json cmd;
  cmd["command"] = "firmware_update";
  this->send_command(cmd);

  if (this->_state != MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT) {
    this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
  }
}

void NSPanel::tft_update() {
  SPDLOG_INFO("Sending TFT update command to nspanel {}::{}.", this->_id, this->_name);
  nlohmann::json cmd;
  cmd["command"] = "tft_update";
  this->send_command(cmd);

  if (this->_state != MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT) {
    this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
  }
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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void NSPanel::update_warnings_from_manager() {
  // If this NSPanel hasn't been registered to the manager, then there is no need to fetch warnings from the manager.
  if (!this->_has_registered_to_manager) {
    return;
  }
  try {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
      std::string response_data;
      SPDLOG_DEBUG("Requesting NSPanel warnings from: http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/get_nspanels_warnings");
      curl_easy_setopt(curl, CURLOPT_URL, "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/get_nspanels_warnings");
      /* example.com is redirected, so we tell libcurl to follow redirection */
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      long http_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      /* Check for errors */
      if (res == CURLE_OK && !response_data.empty() && http_code == 200) {
        nlohmann::json data = nlohmann::json::parse(response_data);
        for (nlohmann::json panel : data["panels"]) {
          if (std::string(panel["nspanel"]["mac"]).compare(this->_mac) == 0) {
            SPDLOG_DEBUG("Found warnings from manager matching MAC {}", this->_mac);
            this->_nspanel_warnings_from_manager = panel["warnings"];
            break;
          }
        }
      } else {
        SPDLOG_ERROR("curl_easy_perform() failed, got code: '{}' with status code: {}. Will retry.", curl_easy_strerror(res), http_code);
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
    } else {
      SPDLOG_ERROR("Failed to curl_easy_init(). Will try again.");
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::diagnostic_information(e, true));
  }
}

void NSPanel::accept_register_request() {
  this->_is_register_accepted = true;
  this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
}

bool NSPanel::has_registered_to_manager() {
  return this->_has_registered_to_manager;
}

bool NSPanel::register_to_manager(const nlohmann::json &register_request_payload) {
  try {

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
      std::string response_data;
      std::string payload_data = register_request_payload.dump();
      SPDLOG_INFO("Sending registration data to Django for database management.");
      curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8000/api/register_nspanel");
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_data.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      long http_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      /* Check for errors */
      if (res == CURLE_OK && http_code == 200) {
        SPDLOG_INFO("Panel registration OK. Checking for new warnings.");
        this->update_warnings_from_manager();
        SPDLOG_INFO("Panel registration OK. Updating internal data.");
        nlohmann::json data = nlohmann::json::parse(response_data);
        this->update_config(data); // Returned data from registration request is the same as data from the global /api/get_mqtt_manager_config
        this->register_to_home_assistant();
        SPDLOG_INFO("Panel registration completed. Sending accept command to panel.");

        // Everything was successfull, send registration accept to panel:
        nlohmann::json response;
        response["command"] = "register_accept";
        response["address"] = MqttManagerConfig::manager_address.c_str();
        response["port"] = MqttManagerConfig::manager_port;
        MQTT_Manager::publish(this->_mqtt_command_topic, response.dump());

        this->send_websocket_update();
        curl_easy_cleanup(curl);
        return true;
      } else {
        SPDLOG_ERROR("curl_easy_perform() when registring panel failed, got code: {}.", curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to register NSPanel: {}", boost::diagnostic_information(e, true));
  }

  return false;
}

void NSPanel::register_to_home_assistant() {
  SPDLOG_DEBUG("Registring NSPanel {}::{} entities to Home Assistant via MQTT AutoReg.", this->_id, this->_name);

  std::list<std::string> identifiers;
  identifiers.push_back(fmt::format("mac_{}", this->_mqtt_register_mac));

  nlohmann::json base_json;
  base_json["device"] = nlohmann::json();
  base_json["device"]["identifiers"] = identifiers;
  base_json["device"]["manufacturer"] = "Sonoff";
  base_json["device"]["model"] = "NSPanel";
  base_json["device"]["name"] = this->_name;
  base_json["availability"] = nlohmann::json();
  base_json["availability"]["topic"] = this->_mqtt_status_topic;
  base_json["availability"]["value_template"] = "{{ value_json.state }}";

  // Register temperature sensor
  nlohmann::json temperature_sensor_data = nlohmann::json(base_json);
  temperature_sensor_data["device_class"] = "temperature";
  if (MqttManagerConfig::use_farenheit) {
    temperature_sensor_data["unit_of_measurement"] = "°F";
  } else {
    temperature_sensor_data["unit_of_measurement"] = "°C";
  }
  temperature_sensor_data["name"] = "Temperature";
  temperature_sensor_data["state_topic"] = fmt::format("nspanel/{}/temperature_state", this->_name);
  temperature_sensor_data["unique_id"] = fmt::format("{}_temperature", this->_name);
  std::string temperature_sensor_data_str = temperature_sensor_data.dump();
  SPDLOG_DEBUG("Registring temp sensor for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_sensor_temperature_topic, temperature_sensor_data_str, true);

  // Register relay1
  if (!this->_relay1_is_mqtt_light) {
    nlohmann::json relay1_data = nlohmann::json(base_json);
    relay1_data["device_class"] = "switch";
    relay1_data["name"] = "Relay 1";
    relay1_data["state_topic"] = this->_mqtt_relay1_state_topic;
    relay1_data["command_topic"] = this->_mqtt_relay1_command_topic;
    relay1_data["state_on"] = "1";
    relay1_data["state_off"] = "0";
    relay1_data["payload_on"] = "1";
    relay1_data["payload_off"] = "0";
    relay1_data["unique_id"] = fmt::format("{}_relay1", this->_mqtt_register_mac);
    std::string relay1_data_str = relay1_data.dump();
    MQTT_Manager::clear_retain(this->_mqtt_light_relay1_topic);
    SPDLOG_DEBUG("Registring relay1 as relay for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
    MQTT_Manager::publish(this->_mqtt_switch_relay1_topic, relay1_data_str, true);
  } else {
    // TODO
    SPDLOG_ERROR("Relay1 as light is not implemented yet!");
  }

  // Register relay2
  if (!this->_relay1_is_mqtt_light) {
    nlohmann::json relay2_data = nlohmann::json(base_json);
    relay2_data["device_class"] = "switch";
    relay2_data["name"] = "Relay 2";
    relay2_data["state_topic"] = this->_mqtt_relay2_state_topic;
    relay2_data["command_topic"] = this->_mqtt_relay2_command_topic;
    relay2_data["state_on"] = "1";
    relay2_data["state_off"] = "0";
    relay2_data["payload_on"] = "1";
    relay2_data["payload_off"] = "0";
    relay2_data["unique_id"] = fmt::format("{}_relay2", this->_mqtt_register_mac);
    std::string relay2_data_str = relay2_data.dump();
    MQTT_Manager::clear_retain(this->_mqtt_light_relay2_topic);
    SPDLOG_DEBUG("Registring relay2 as relay for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
    MQTT_Manager::publish(this->_mqtt_switch_relay2_topic, relay2_data_str, true);
  } else {
    SPDLOG_ERROR("Relay2 as light is not implemented yet!");
  }

  // Register screen switch
  nlohmann::json screen_data = nlohmann::json(base_json);
  screen_data["device_class"] = "switch";
  screen_data["name"] = "Screen power";
  screen_data["state_topic"] = fmt::format("nspanel/{}/screen_state", this->_name);
  screen_data["command_topic"] = fmt::format("nspanel/{}/screen_cmd", this->_name);
  screen_data["state_on"] = "1";
  screen_data["state_off"] = "0";
  screen_data["payload_on"] = "1";
  screen_data["payload_off"] = "0";
  screen_data["unique_id"] = fmt::format("{}_screen_state", this->_mqtt_register_mac);
  std::string screen_data_str = screen_data.dump();
  SPDLOG_DEBUG("Registring screenstate switch for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_switch_screen_topic, screen_data_str, true);

  // Register screen brightness
  nlohmann::json screen_brightness_data = nlohmann::json(base_json);
  screen_brightness_data["name"] = "Screen brightness";
  screen_brightness_data["command_topic"] = fmt::format("nspanel/{}/brightness", this->_name);
  screen_brightness_data["min"] = "1";
  screen_brightness_data["max"] = "100";
  screen_brightness_data["unique_id"] = fmt::format("{}_screen_brightness", this->_mqtt_register_mac);
  std::string screen_brightness_data_str = screen_brightness_data.dump();
  SPDLOG_DEBUG("Registring screen brightness for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_number_screen_brightness_topic, screen_brightness_data_str, true);

  // Register screensaver brightness
  nlohmann::json screensaver_brightness_data = nlohmann::json(base_json);
  screensaver_brightness_data["name"] = "Screen brightness";
  screensaver_brightness_data["command_topic"] = fmt::format("nspanel/{}/brightness_screensaver", this->_name);
  screensaver_brightness_data["min"] = "0";
  screensaver_brightness_data["max"] = "100";
  screensaver_brightness_data["unique_id"] = fmt::format("{}_screensaver_brightness", this->_mqtt_register_mac);
  std::string screensaver_brightness_data_str = screensaver_brightness_data.dump();
  SPDLOG_DEBUG("Registring screensaver brightness for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_number_screensaver_brightness_topic, screensaver_brightness_data_str, true);
}

void NSPanel::set_relay_state(uint8_t relay, bool state) {
  if (relay == 1 && this->_relay1_state != state) {
    MQTT_Manager::publish(this->_mqtt_relay1_command_topic, state ? "1" : "0");
  } else if (relay == 2 && this->_relay2_state != state) {
    MQTT_Manager::publish(this->_mqtt_relay2_command_topic, state ? "1" : "0");
  }
}
