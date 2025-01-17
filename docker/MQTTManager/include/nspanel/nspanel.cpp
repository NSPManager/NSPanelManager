#include "nspanel.hpp"
#include <command_manager/command_manager.hpp>
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "ipc_handler/ipc_handler.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include "web_helper/WebHelper.hpp"
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/write.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <curl/curl.h>
#include <exception>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <iomanip>
#include <list>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <websocket_server/websocket_server.hpp>

NSPanelRelayGroup::NSPanelRelayGroup(nlohmann::json &config) {
  this->update_config(config);
}

void NSPanelRelayGroup::update_config(nlohmann::json &config) {
  this->_id = config["relay_group_id"];
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

NSPanel::NSPanel(NSPanelSettings &init_data) {
  // Assume panel to be offline until proven otherwise
  this->_state = MQTT_MANAGER_NSPANEL_STATE::OFFLINE;
  this->_last_state_update_sent = true;
  // If this panel is just a panel in waiting (ie. not accepted the request yet) it won't have an id.
  this->_has_registered_to_manager = false;
  this->update_config(init_data);

  CommandManager::attach_callback(boost::bind(&NSPanel::command_callback, this, _1));

  IPCHandler::attach_callback(fmt::format("nspanel/{}/status", this->_id), boost::bind(&NSPanel::handle_ipc_request_status, this, _1, _2));
  IPCHandler::attach_callback(fmt::format("nspanel/{}/reboot", this->_id), boost::bind(&NSPanel::handle_ipc_request_reboot, this, _1, _2));
  IPCHandler::attach_callback(fmt::format("nspanel/{}/update_screen", this->_id), boost::bind(&NSPanel::handle_ipc_request_update_screen, this, _1, _2));
  IPCHandler::attach_callback(fmt::format("nspanel/{}/update_firmware", this->_id), boost::bind(&NSPanel::handle_ipc_request_update_firmware, this, _1, _2));
  IPCHandler::attach_callback(fmt::format("nspanel/{}/accept_register_request", this->_id), boost::bind(&NSPanel::handle_ipc_request_accept_register_request, this, _1, _2));
  IPCHandler::attach_callback(fmt::format("nspanel/{}/deny_register_request", this->_id), boost::bind(&NSPanel::handle_ipc_request_deny_register_request, this, _1, _2));
  IPCHandler::attach_callback(fmt::format("nspanel/{}/logs", this->_id), boost::bind(&NSPanel::handle_ipc_request_get_logs, this, _1, _2));
}

void NSPanel::update_config(NSPanelSettings &settings) {
  SPDLOG_TRACE("NSPanel {}::{} received config update.", this->_id, this->_name);

  // TODO: Remove all "if mac or mac_address" (and similar) and decide on ONE name for all things.
  if (settings.has_id()) {
    this->_id = settings.id();
    this->_has_registered_to_manager = true; // Data contained an ID which it got from the manager config.
  } else {
    this->_state = MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT;
  }

  bool rebuilt_mqtt = false; // Wether or not to rebuild mqtt topics and subscribe to the new topics.
  if (this->_name.compare(settings.name()) != 0) {
    rebuilt_mqtt = true;
    if (this->_name.size() > 0) {
      this->reboot();
    }
    this->_name = settings.name();
  }

  this->_mac = settings.mac_address();
  this->_ip_address = settings.ip_address();
  this->_is_us_panel = settings.is_us_panel();
  this->_relay1_register_type = settings.relay1_register_type();
  this->_relay2_register_type = settings.relay2_register_type();

  if (this->_state == MQTT_MANAGER_NSPANEL_STATE::OFFLINE || this->_state == MQTT_MANAGER_NSPANEL_STATE::UNKNOWN) {
    this->_rssi = 0;
    this->_heap_used_pct = 0;
    this->_nspanel_warnings.clear();
    this->_temperature = 0;
    this->_update_progress = 0;
  }

  // Last thing to do, check if the panel as actually accepted into our manager.
  if (settings.denied()) {
    SPDLOG_INFO("Loaded denied NSPanel {}::{}.", this->_id, this->_name);
    this->_is_register_denied = true;
    this->_is_register_accepted = false;
    this->_state = MQTT_MANAGER_NSPANEL_STATE::DENIED;
    rebuilt_mqtt = false;
  }

  if (settings.accepted() == true) {
    SPDLOG_INFO("Loaded accepted NSPanel {}::{}.", this->_id, this->_name);
    this->_is_register_denied = false;
    this->_is_register_accepted = true;
    this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
    rebuilt_mqtt = true;
  }

  if (!this->_is_register_denied && !this->_is_register_accepted) {
    // No decission has been made on wether ot accept or deny panel. It is therefore awaiting_accept
    this->_state = MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT;
    rebuilt_mqtt = true;
  }

  if (settings.has_id()) {
    SPDLOG_DEBUG("Loaded NSPanel {}::{}, type: {}.", this->_id, this->_name, this->_is_us_panel ? "US" : "EU");
  } else {
    SPDLOG_DEBUG("Loaded NSPanel {} with no ID.", this->_name, this->_is_us_panel ? "US" : "EU");
  }

  if (rebuilt_mqtt) {
    SPDLOG_DEBUG("Building MQTT topics for NSPanel {}::{}", this->_id, this->_name);
    this->reset_mqtt_topics();
    // Convert stored MAC to MAC used in MQTT, ex. AA:AA:AA:BB:BB:BB to aa_aa_aa_bb_bb_bb
    std::string mqtt_register_mac = this->_mac;
    std::replace(mqtt_register_mac.begin(), mqtt_register_mac.end(), ':', '_');
    std::transform(mqtt_register_mac.begin(), mqtt_register_mac.end(), mqtt_register_mac.begin(), [](unsigned char c) {
      return std::tolower(c);
    });
    this->_mqtt_register_mac = mqtt_register_mac;

    this->_mqtt_config_topic = fmt::format("nspanel/{}/config", this->_mac);
    this->_mqtt_log_topic = fmt::format("nspanel/{}/log", this->_name); // TODO: Remove as this is the old log topic. Use the new based on MAC-address instead.
    this->_mqtt_command_topic = fmt::format("nspanel/{}/command", this->_mac);
    this->_mqtt_sensor_temperature_topic = fmt::format("homeassistant/sensor/nspanelmanager/{}_temperature/config", mqtt_register_mac);
    this->_mqtt_switch_relay1_topic = fmt::format("homeassistant/switch/nspanelmanager/{}_relay1/config", mqtt_register_mac);
    this->_mqtt_light_relay1_topic = fmt::format("homeassistant/light/nspanelmanager/{}_relay1/config", mqtt_register_mac);
    this->_mqtt_switch_relay2_topic = fmt::format("homeassistant/switch/nspanelmanager/{}_relay2/config", mqtt_register_mac);
    this->_mqtt_light_relay2_topic = fmt::format("homeassistant/light/nspanelmanager/{}_relay2/config", mqtt_register_mac);
    this->_mqtt_switch_screen_topic = fmt::format("homeassistant/switch/nspanelmanager/{}_screen/config", mqtt_register_mac);
    this->_mqtt_number_screen_brightness_topic = fmt::format("homeassistant/number/nspanelmanager/{}_screen_brightness/config", mqtt_register_mac);
    this->_mqtt_number_screensaver_brightness_topic = fmt::format("homeassistant/number/nspanelmanager/{}_screensaver_brightness/config", mqtt_register_mac);
    this->_mqtt_select_screensaver_topic = fmt::format("homeassistant/select/nspanelmanager/{}_screensaver_select/config", mqtt_register_mac);
    this->_mqtt_relay1_command_topic = fmt::format("nspanel/{}/r1_cmd", this->_mac);
    this->_mqtt_relay1_state_topic = fmt::format("nspanel/{}/r1_state", this->_mac);
    this->_mqtt_relay2_command_topic = fmt::format("nspanel/{}/r2_cmd", this->_mac);
    this->_mqtt_relay2_state_topic = fmt::format("nspanel/{}/r2_state", this->_mac);
    this->_mqtt_status_topic = fmt::format("nspanel/{}/state", this->_mac);
    this->_mqtt_status_report_topic = fmt::format("nspanel/{}/status_report", this->_mac);
    this->_mqtt_temperature_topic = fmt::format("nspanel/{}/temperature", this->_mac);

    this->_mqtt_topic_home_page_status = fmt::format("nspanel/{}/home_page", this->_mac);
    this->_mqtt_topic_home_page_all_rooms_status = fmt::format("nspanel/{}/home_page_all", this->_mac);
    this->_mqtt_topic_room_entities_page_status = fmt::format("nspanel/{}/entities_page", this->_mac);
  }

  if (this->_has_registered_to_manager && !this->_is_register_denied && this->_is_register_accepted) {
    // If this NSPanel is registered to manager, listen to state topics.
    SPDLOG_INFO("Subscribing to NSPanel MQTT topics.");
    MQTT_Manager::subscribe(this->_mqtt_relay1_state_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_relay2_state_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_log_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2)); // TODO: Remove me and use only topic based on MAC-address instead
    MQTT_Manager::subscribe(fmt::format("nspanel/{}/log", this->_mac), boost::bind(&NSPanel::mqtt_log_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_status_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MQTT_Manager::subscribe(this->_mqtt_status_report_topic, boost::bind(&NSPanel::mqtt_callback, this, _1, _2));
    MqttManagerConfig::attach_config_loaded_listener(boost::bind(&NSPanel::send_config, this));
    this->register_to_home_assistant();
    this->send_config();
  }

  this->_go_to_default_room();

  if(this->_selected_room != nullptr) {
      this->_selected_room->attach_room_changed_callback(boost::bind(&NSPanel::_room_change_callback, this, _1));
      // Manually trigger entity subscription as room is already fully loaded.
      this->_room_change_callback(this->_selected_room.get());

      // Manually trigger update of MQTT states
      {
        std::lock_guard<std::mutex> lock_guard(this->_last_state_update_mutex);
        this->_last_state_update = std::chrono::system_clock::now();
        std::thread(&NSPanel::_check_and_send_new_status_update, this).detach();
      }
  } else {
      SPDLOG_ERROR("NSPanel {}::{} failed to find default room, not attaching room change callback.", this->_id, this->_name);
  }

  // Config changed, send "reload" command to web interface
  this->send_websocket_update();
}

void NSPanel::send_config() {
  auto settings_it = std::find_if(MqttManagerConfig::nspanel_configs.begin(), MqttManagerConfig::nspanel_configs.end(), [this](NSPanelSettings s) {
    return s.id() == this->_id;
  });
  if (settings_it != MqttManagerConfig::nspanel_configs.end()) {
    SPDLOG_INFO("Sending config over MQTT for panel {}::{}", this->_id, this->_name);
    NSPanelSettings settings = (*settings_it);
    NSPanelConfig config;
    config.set_nspanel_id(this->_id);
    config.set_name(this->_name);
    config.set_default_room(settings.default_room());
    config.set_default_page(settings.default_page());
    config.set_min_button_push_time(settings.min_button_push_time());
    config.set_button_long_press_time(settings.button_long_press_time());
    config.set_special_mode_trigger_time(settings.special_mode_trigger_time());
    config.set_special_mode_release_time(settings.special_mode_release_time());
    config.set_screen_dim_level(settings.screen_dim_level());
    config.set_screensaver_dim_level(settings.screensaver_dim_level());
    config.set_screensaver_mode(static_cast<NSPanelConfig_NSPanelScreensaverMode>(settings.screensaver_mode()));
    config.set_screensaver_activation_timeout(settings.screensaver_activation_timeout());
    config.set_clock_us_style(settings.clock_format() == time_format::AM_PM);
    config.set_use_fahrenheit(settings.temperature_unit() == temperature_format::FAHRENHEIT);
    config.set_is_us_panel(settings.is_us_panel());
    config.set_reverse_relays(settings.reverse_relays());
    config.set_relay1_default_mode(settings.relay1_default_mode());
    config.set_relay2_default_mode(settings.relay2_default_mode());
    config.set_temperature_calibration(settings.temperature_calibration());
    config.set_button1_mode(settings.button1_mode());
    config.set_button2_mode(settings.button2_mode());
    config.set_button1_mqtt_topic(settings.button1_mqtt_topic());
    config.set_button2_mqtt_topic(settings.button2_mqtt_topic());
    config.set_button1_mqtt_payload(settings.button1_mqtt_payload());
    config.set_button2_mqtt_payload(settings.button2_mqtt_payload());
    config.set_button1_detached_light_id(settings.button1_detached_light_id());
    config.set_button2_detached_light_id(settings.button2_detached_light_id());
    config.set_optimistic_mode(MqttManagerConfig::get_settings().optimistic_mode());
    config.set_raise_light_level_to_100_above(settings.raise_to_100_light_level());

    // Load rooms
    for (int i = 0; i < settings.rooms().size(); i++) {
      config.add_room_ids(settings.rooms().Get(i));
    }

    // TODO: Set scenes
    config.clear_global_scenes();

    std::string config_str = config.SerializeAsString();
    MQTT_Manager::publish(this->_mqtt_config_topic, config_str, true);

    SPDLOG_DEBUG("Sent updated NSPanelConfig to panel {}::{} over MQTT.", this->_id, this->_name);
  } else {
    SPDLOG_WARN("Requested sending config for panel {}::{} over MQTT but no such panel exists in MqttManagerConfig.", this->_id, this->_name);
  }
}

NSPanel::~NSPanel() {
  SPDLOG_INFO("Destroying NSPanel {}::{}", this->_id, this->_name);
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
  MQTT_Manager::clear_retain(this->_mqtt_config_topic);

  this->reset_ha_mqtt_topics();
}

void NSPanel::reset_ha_mqtt_topics() {
  MQTT_Manager::clear_retain(this->_mqtt_light_relay1_topic);
  MQTT_Manager::clear_retain(this->_mqtt_light_relay2_topic);
  MQTT_Manager::clear_retain(this->_mqtt_switch_relay1_topic);
  MQTT_Manager::clear_retain(this->_mqtt_switch_relay2_topic);
  MQTT_Manager::clear_retain(this->_mqtt_switch_screen_topic);
  MQTT_Manager::clear_retain(this->_mqtt_sensor_temperature_topic);
  MQTT_Manager::clear_retain(this->_mqtt_number_screen_brightness_topic);
  MQTT_Manager::clear_retain(this->_mqtt_number_screensaver_brightness_topic);
  MQTT_Manager::clear_retain(this->_mqtt_select_screensaver_topic);
}

void NSPanel::erase() {
  this->reboot();
  this->reset_mqtt_topics();
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
  bool parse_fail = false;
  try {
    if (!payload.empty() && topic.compare(this->_mqtt_log_topic) == 0) {
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
        if (MqttManagerConfig::get_settings().clock_format() == time_format::FULL) {
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
        log_data["mac_address"] = message_parts[0];
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
        while (this->_log_messages.size() > MqttManagerConfig::get_settings().max_log_buffer_size()) {
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
        if (data["temperature"].is_number_float()) {
          this->_temperature = data["temperature"];
        } else if (data["temperature"].is_string()) {
          this->_temperature = atof(std::string(data["temperature"]).c_str());
        } else {
          SPDLOG_ERROR("Incorrect format of temperature data. Expected float.");
        }
        this->_ip_address = data["ip"];
        this->_nspanel_warnings.clear();
        if (data.at("warnings").is_string()) {
          // Loaded from old firmware, split string and assume level warning
          std::vector<std::string> message_lines;
          boost::split(message_lines, std::string(data.at("warnings")), boost::is_any_of("\n"));
          for (std::string line : message_lines) {
            NSPanelWarningWebsocketRepresentation warning_obj = {
                .level = "warning",
                .text = line};
            this->_nspanel_warnings.push_back(warning_obj);
          }
        } else if (data.at("warnings").is_array()) {
          for (nlohmann::json warning : data.at("warnings")) {
            if (warning.contains("level") && warning.contains("text")) {
              NSPanelWarningWebsocketRepresentation warning_obj = {
                  .level = warning.at("level"),
                  .text = warning.at("text")};
              this->_nspanel_warnings.push_back(warning_obj);
            } else {
              SPDLOG_WARN("Failed to load warning from NSPanel {}::{}. Missing level or text attribute.", this->_id, this->_name);
            }
          }
        }

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

        this->send_websocket_status_update();
      }
    } else if (topic.compare(this->_mqtt_relay1_state_topic) == 0) {
      this->_relay1_state = (payload.compare("1") == 0);
      std::vector<std::shared_ptr<NSPanelRelayGroup>> relay_groups = EntityManager::get_all_relay_groups();
      for (auto relay_group : relay_groups) {
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
      std::vector<std::shared_ptr<NSPanelRelayGroup>> relay_groups = EntityManager::get_all_relay_groups();
      for (auto relay_group : relay_groups) {
        if (relay_group->contains(this->_id, 2)) {
          if (payload.compare("1") == 0) {
            relay_group->turn_on();
          } else {
            relay_group->turn_off();
          }
        }
      }
    }
  } catch (std::exception &e) {
    parse_fail = true;
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::diagnostic_information(e, true));
  }

  if (parse_fail) {
    SPDLOG_INFO("Failed to parse data. Will try as protobuf.");

    if (boost::ends_with(topic, "/status_report")) {
      SPDLOG_TRACE("Trying to parse status_report as protobuf.");

      NSPanelStatusReport report;
      if (!report.ParseFromString(payload)) {
        SPDLOG_ERROR("Failed to parse NSPanelStatusReport from string as protobuf.");
        SPDLOG_TRACE("Payload: {}", payload);
        return;
      }

      this->_ip_address = report.ip_address();
      this->_rssi = report.rssi();
      this->_heap_used_pct = report.heap_used_pct();
      this->_temperature = report.temperature();
      switch (report.nspanel_state()) {
      case NSPanelStatusReport_state::NSPanelStatusReport_state_ONLINE:
        this->_update_progress = 0;
        this->_state = MQTT_MANAGER_NSPANEL_STATE::ONLINE;
        break;
      case NSPanelStatusReport_state_OFFLINE:
        this->_state = MQTT_MANAGER_NSPANEL_STATE::OFFLINE; // This should never happen, offline state is handled in "/state" and not "/status_report"
        break;
      case NSPanelStatusReport_state_UPDATING_TFT:
        this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT;
        break;
      case NSPanelStatusReport_state_UPDATING_FIRMWARE:
        this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE;
        break;
      case NSPanelStatusReport_state_UPDATING_LITTLEFS:
        this->_state = MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA;
        break;
      case NSPanelStatusReport_state_NSPanelStatusReport_state_INT_MIN_SENTINEL_DO_NOT_USE_:
      case NSPanelStatusReport_state_NSPanelStatusReport_state_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
      }

      this->_update_progress = report.update_progress();
      this->_nspanel_warnings.clear();
      for (const NSPanelWarning &warning : report.warnings()) {
        NSPanelWarningWebsocketRepresentation ws_warn;
        ws_warn.text = warning.text();
        switch (warning.level()) {
        case CRITICAL:
          ws_warn.level = "CRITICAL";
          break;
        case ERROR:
          ws_warn.level = "ERROR";
          break;
        case WARNING:
          ws_warn.level = "WARNING";
          break;
        case INFO:
          ws_warn.level = "INFO";
          break;
        case DEBUG:
          ws_warn.level = "DEBUG";
          break;
        case TRACE:
          ws_warn.level = "TRACE";
          break;
        case NSPanelWarningLevel_INT_MIN_SENTINEL_DO_NOT_USE_:
        case NSPanelWarningLevel_INT_MAX_SENTINEL_DO_NOT_USE_:
          break;
        }
        this->_nspanel_warnings.push_back(ws_warn);
      }

      // Received new temperature from status report, send out on temperature topic:
      MQTT_Manager::publish(this->_mqtt_temperature_topic, std::to_string(std::round(this->_temperature*100.0f)/100.0f));

      // SPDLOG_TRACE("MAC: {}", report.mac_address());
      // SPDLOG_TRACE("IP : {}", report.ip_address());
      // SPDLOG_TRACE("TMP: {}", report.temperature());
      // SPDLOG_TRACE("HEP: {}", report.heap_used_pct());
      // SPDLOG_TRACE("RSI: {}", report.rssi());
      this->send_websocket_update();
    }
  }
}

void NSPanel::mqtt_log_callback(std::string topic, std::string payload) {
    size_t trim_start_pos = payload.find_first_not_of(" \n\r\t");
    if(trim_start_pos == std::string::npos) {
        return; // Message contains no valid chars, only spaces
    }

    size_t trim_end_pos = payload.find_last_not_of(" \n\r\t");
    if(trim_start_pos == std::string::npos) {
        return; // Message contains no valid chars, only spaces
    }
    payload = payload.substr(trim_start_pos, trim_end_pos+1 - 4); // Trim spaces and such but also the first 7 chars that is the color coding for the message
    if(payload[0] == 0x1B) { // Message formated with color. Remove color
        payload = payload.substr(7);
    }

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm = *std::localtime(&now);
    std::stringstream buffer;
    if (MqttManagerConfig::get_settings().clock_format() == time_format::FULL) {
      buffer << std::put_time(&tm, "%H:%M:%S");
    } else {
      buffer << std::put_time(&tm, "%I:%M:%S %p");
    }

    nlohmann::json log_data;
    log_data["type"] = "log";
    log_data["time"] = buffer.str();
    log_data["panel"] = this->_name;
    log_data["mac_address"] = this->_mac;

    if(payload[0] == 'E') {
        log_data["level"] = "ERROR";
    } else if(payload[0] == 'W') {
        log_data["level"] = "WARNING";
    } else if(payload[0] == 'I') {
        log_data["level"] = "INFO";
    } else if(payload[0] == 'D') {
        log_data["level"] = "DEBUG";
    } else {
        SPDLOG_ERROR("Received log message from NSPanel but could not determin level, will not store/forward log message to web interface! Level: {}, Message: {}", payload[0], payload);
        return;
    }

    // Remove first char that indicates log level. This is store separately
    payload = payload.substr(1);
    log_data["message"] = payload; // TODO: Clean up message before sending it out
    WebsocketServer::broadcast_json(log_data);

    // Save log message in backtrace for when (if) the log interface requests it.
    NSPanelLogMessage message;
    message.time = buffer.str();
    message.level = log_data["level"];
    message.message = payload;
    this->_log_messages.push_front(message);
    // Remove older messages from backtrace.
    while (this->_log_messages.size() > MqttManagerConfig::get_settings().max_log_buffer_size()) {
      this->_log_messages.pop_back();
    }
}

void NSPanel::send_websocket_update() {
  if (this->_has_registered_to_manager) {
    SPDLOG_TRACE("Sending websocket update for {}::{}", this->_id, this->_name);
  } else {
    SPDLOG_TRACE("Sending websocket update for new NSPanel ??::{}", this->_name);
  }
  // Send status over to web interface:
  nlohmann::json event_trigger_data;
  event_trigger_data["event_type"] = fmt::format("nspanel-{}-state-change", this->_id);
  WebsocketServer::broadcast_json(event_trigger_data);
}

void NSPanel::send_websocket_status_update() {
  SPDLOG_TRACE("Sending websocket status update for {}::{}", this->_id, this->_name);
  // Send status over to web interface:
  nlohmann::json event_trigger_data;
  event_trigger_data["event_type"] = fmt::format("nspanel-{}-status-change", this->_id);
  WebsocketServer::broadcast_json(event_trigger_data);
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
    data["nspanel_id"] = this->_id;
  }
  data["name"] = this->_name;
  data["rssi"] = this->_rssi;
  data["heap_used_pct"] = this->_heap_used_pct;
  data["mac_address"] = send_mac;
  data["ip_address"] = this->_ip_address;
  data["temperature"] = this->_temperature;
  // std::string warnings_string = this->_nspanel_warnings;
  // if (warnings_string.length() > 0) {
  //   warnings_string.append("\\n");
  // }
  // warnings_string.append(this->_nspanel_warnings_from_manager);
  // data["warnings"] = warnings_string;

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
  if (!this->_mqtt_command_topic.empty()) {
    std::string buffer = command.dump();
    MQTT_Manager::publish(this->_mqtt_command_topic, buffer);
  } else {
    SPDLOG_ERROR("Failed to send message to NSPanel Command topic. No command topic was set!");
  }
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

void NSPanel::accept_register_request() {
  this->_is_register_accepted = true;
  this->_is_register_denied = false;
  this->_state = MQTT_MANAGER_NSPANEL_STATE::WAITING;
}

void NSPanel::deny_register_request() {
  this->_is_register_accepted = false;
  this->_is_register_denied = true;
  this->_state = MQTT_MANAGER_NSPANEL_STATE::DENIED;

  nlohmann::json data;
  data["mac_address"] = this->_mac;
  data["friendly_name"] = this->_name;
  data["denied"] = true;
  bool result = this->register_to_manager(data);
}

bool NSPanel::has_registered_to_manager() {
  return this->_has_registered_to_manager;
}

bool NSPanel::register_to_manager(const nlohmann::json &register_request_payload) {
  try {
    SPDLOG_INFO("Sending registration data to Django for database management.");
    std::string url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/rest/nspanels";
    std::string response_data;
    std::string payload_data = register_request_payload.dump();
    if (WebHelper::perform_post_request(&url, &response_data, nullptr, &payload_data)) {
      SPDLOG_INFO("Panel registration OK. Updating internal data.");
      this->_has_registered_to_manager = true;
      nlohmann::json data = nlohmann::json::parse(response_data);
      if (data.contains("denied") && data.at("denied") == false && data.contains("accepted") && data.at("accepted") == true) {
        this->_is_register_denied = false;
        this->_is_register_accepted = true;
        // this->update_config(data); // Returned data from registration request is the same as data from the global /api/get_mqtt_manager_config
        this->_state = MQTT_MANAGER_NSPANEL_STATE::ONLINE;
        this->_id = data["nspanel_id"];
        if (this->_name.size() == 0) {
          this->_name = register_request_payload["friendly_name"];
          this->_mac = register_request_payload["mac_origin"];
        }
        this->register_to_home_assistant();
        SPDLOG_INFO("Panel registration completed. Sending accept command to panel.");

        // Everything was successfull, send registration accept to panel:
        nlohmann::json response;
        response["command"] = "register_accept";
        response["address"] = MqttManagerConfig::get_settings().manager_address();
        response["port"] = MqttManagerConfig::get_settings().manager_port();
        response["config_topic"] = this->_mqtt_config_topic;
        std::string reply_topic = fmt::format("nspanel/{}/command", std::string(register_request_payload.at("friendly_name")));
        MQTT_Manager::publish(reply_topic, response.dump());

        SPDLOG_TRACE("Sending websocket update for NSPanel {}::{} state change.", this->_id, this->_name);
        this->send_websocket_update();
        return true;
      } else {
        SPDLOG_ERROR("Malformed register request data. Missing 'friendly_name'.");
      }
    } else {
      this->_is_register_denied = true;
      this->_is_register_accepted = false;
      SPDLOG_INFO("NSPanel {}::{} has yet to be accepted. Will not answer request.", this->_id, this->_name);
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
  if (MqttManagerConfig::get_settings().temperature_unit() == temperature_format::FAHRENHEIT) {
    temperature_sensor_data["unit_of_measurement"] = "°F";
  } else {
    temperature_sensor_data["unit_of_measurement"] = "°C";
  }
  temperature_sensor_data["name"] = "Temperature";
  temperature_sensor_data["state_topic"] = fmt::format("nspanel/mqttmanager_{}/nspanel/{}/temperature", MqttManagerConfig::get_settings().manager_address(), this->_name);
  temperature_sensor_data["unique_id"] = fmt::format("{}_temperature", this->_name);
  std::string temperature_sensor_data_str = temperature_sensor_data.dump();
  SPDLOG_DEBUG("Registring temp sensor for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_sensor_temperature_topic, temperature_sensor_data_str, true);

  // Register relay1
  if (this->_relay1_register_type == NSPanelSettings::RelayRegisterType::NSPanelSettings_RelayRegisterType_SWITCH) {
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
    nlohmann::json relay1_data = nlohmann::json(base_json);
    relay1_data["device_class"] = "light";
    relay1_data["name"] = "Relay 1";
    relay1_data["state_topic"] = this->_mqtt_relay1_state_topic;
    relay1_data["command_topic"] = this->_mqtt_relay1_command_topic;
    // relay1_data["state_template"] = "{% if value == 1 %}on{% else %}off{% endif %}";
    relay1_data["payload_on"] = "1";
    relay1_data["payload_off"] = "0";
    relay1_data["unique_id"] = fmt::format("{}_relay1", this->_mqtt_register_mac);
    std::string relay1_data_str = relay1_data.dump();
    MQTT_Manager::clear_retain(this->_mqtt_switch_relay1_topic);
    SPDLOG_DEBUG("Registring relay1 as light for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
    MQTT_Manager::publish(this->_mqtt_light_relay1_topic, relay1_data_str, true);
  }

  // Register relay2
  if (this->_relay2_register_type == NSPanelSettings::RelayRegisterType::NSPanelSettings_RelayRegisterType_SWITCH) {
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
    nlohmann::json relay2_data = nlohmann::json(base_json);
    relay2_data["device_class"] = "light";
    relay2_data["name"] = "Relay 2";
    relay2_data["state_topic"] = this->_mqtt_relay2_state_topic;
    relay2_data["command_topic"] = this->_mqtt_relay2_command_topic;
    // relay2_data["state_template"] = "{% if value == 1 %}on{% else %}off{% endif %}";
    relay2_data["payload_on"] = "1";
    relay2_data["payload_off"] = "0";
    relay2_data["unique_id"] = fmt::format("{}_relay2", this->_mqtt_register_mac);
    std::string relay2_data_str = relay2_data.dump();
    MQTT_Manager::clear_retain(this->_mqtt_switch_relay2_topic);
    SPDLOG_DEBUG("Registring relay2 as light for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
    MQTT_Manager::publish(this->_mqtt_light_relay2_topic, relay2_data_str, true);
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
  screensaver_brightness_data["name"] = "Screensaver brightness";
  screensaver_brightness_data["command_topic"] = fmt::format("nspanel/{}/brightness_screensaver", this->_name);
  screensaver_brightness_data["min"] = "0";
  screensaver_brightness_data["max"] = "100";
  screensaver_brightness_data["unique_id"] = fmt::format("{}_screensaver_brightness", this->_mqtt_register_mac);
  std::string screensaver_brightness_data_str = screensaver_brightness_data.dump();
  SPDLOG_DEBUG("Registring screensaver brightness for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_number_screensaver_brightness_topic, screensaver_brightness_data_str, true);

  // Register screensaver select
  nlohmann::json screensaver_select_data = nlohmann::json(base_json);
  screensaver_select_data["name"] = "Screensaver mode";
  screensaver_select_data["command_topic"] = fmt::format("nspanel/{}/screensaver_mode", this->_name);
  std::list<std::string> options;
  options.push_back("with_background");
  options.push_back("without_background");
  options.push_back("datetime_with_background");
  options.push_back("datetime_without_background");
  options.push_back("no_screensaver");
  screensaver_select_data["options"] = options;
  screensaver_select_data["unique_id"] = fmt::format("{}_screensaver_mode", this->_mqtt_register_mac);
  std::string screensaver_select_data_str = screensaver_select_data.dump();
  SPDLOG_DEBUG("Registring screensaver mode select for NSPanel {}::{} to Home Assistant.", this->_id, this->_name);
  MQTT_Manager::publish(this->_mqtt_select_screensaver_topic, screensaver_select_data_str, true);
}

void NSPanel::set_relay_state(uint8_t relay, bool state) {
  if (relay == 1 && this->_relay1_state != state) {
    MQTT_Manager::publish(this->_mqtt_relay1_command_topic, state ? "1" : "0");
  } else if (relay == 2 && this->_relay2_state != state) {
    MQTT_Manager::publish(this->_mqtt_relay2_command_topic, state ? "1" : "0");
  }
}

void NSPanel::command_callback(NSPanelMQTTManagerCommand &command) {
    if(command.has_next_entities_page()) {
        SPDLOG_DEBUG("NSPanel {}::{} going to next entities page.", this->_id, this->_name);
        if(command.next_entities_page().nspanel_id() != this->_id) {
            return;
        }

        if(this->_selected_entity_page_index + 1 < this->_selected_room->get_number_of_entity_pages()) {
            this->_selected_entity_page_index++;
        } else {
            // Try to find the next room that has available entity page(s). If found, break loop.
            for(int i = 0; i < UINT16_MAX; i++) {
                this->_go_to_next_room();
                if(this->_selected_room != nullptr) {
                    if(this->_selected_room->get_number_of_entity_pages() > 0) {
                        this->_selected_entity_page_index = 0; // Start from beginning of pages as we went to next room and not previous.
                        break;
                    }
                }
            }
        }
        this->_send_entities_page_update();
    } else if (command.has_previous_entities_page()) {
        SPDLOG_DEBUG("NSPanel {}::{} going to previous entities page.", this->_id, this->_name);
        if(command.previous_entities_page().nspanel_id() != this->_id) {
            return;
        }

        if(this->_selected_entity_page_index > 0) {
            this->_selected_entity_page_index--;
        } else {
            // We are at index 0 and want to go to previos, go to previos room and start at the last index.
            for(int i = 0; i < UINT16_MAX; i++) {
                this->_go_to_previous_room();
                if(this->_selected_room != nullptr) {
                    if(this->_selected_room->get_number_of_entity_pages() > 0) {
                        this->_selected_entity_page_index = this->_selected_room->get_number_of_entity_pages() - 1; // Start from beginning of pages as we went to next room and not previous.
                        break;
                    }
                }
            }
        }
        this->_send_entities_page_update();
    }
}

bool NSPanel::handle_ipc_request_status(nlohmann::json request, nlohmann::json *response_buffer) {
  nlohmann::json data = {
      {"ip_address", this->_ip_address},
      {"rssi", this->_rssi},
      {"temperature", this->_temperature},
      {"ram_usage", this->_heap_used_pct},
      {"update_progress", this->_update_progress},
  };
  data["warnings"] = nlohmann::json::array({});
  for (NSPanelWarningWebsocketRepresentation warning : this->_nspanel_warnings) {
    data["warnings"].push_back(nlohmann::json{
        {"level", warning.level},
        {"text", warning.text},
    });
  }

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
  (*response_buffer) = data;
  return true;
}

bool NSPanel::handle_ipc_request_reboot(nlohmann::json request, nlohmann::json *response_buffer) {
  this->reboot();
  this->send_websocket_update();
  nlohmann::json data = {{"status", "ok"}};
  (*response_buffer) = data;
  return true;
}

bool NSPanel::handle_ipc_request_update_firmware(nlohmann::json request, nlohmann::json *response_buffer) {
  this->firmware_update();
  this->send_websocket_update();
  nlohmann::json data = {{"status", "ok"}};
  (*response_buffer) = data;
  return true;
}

bool NSPanel::handle_ipc_request_update_screen(nlohmann::json request, nlohmann::json *response_buffer) {
  this->tft_update();
  this->send_websocket_update();
  nlohmann::json data = {{"status", "ok"}};
  (*response_buffer) = data;
  return true;
}

bool NSPanel::handle_ipc_request_accept_register_request(nlohmann::json request, nlohmann::json *response_buffer) {
  this->accept_register_request();
  nlohmann::json data = {{"status", "ok"}};
  (*response_buffer) = data;
  return true;
}

bool NSPanel::handle_ipc_request_deny_register_request(nlohmann::json request, nlohmann::json *response_buffer) {
  this->deny_register_request();
  nlohmann::json data = {{"status", "ok"}};
  (*response_buffer) = data;
  return true;
}

bool NSPanel::handle_ipc_request_get_logs(nlohmann::json request, nlohmann::json *response_buffer) {
  nlohmann::json data = {{"status", "ok"}};
  data["messages"] = nlohmann::json::array();
  for(auto it = this->_log_messages.cbegin(); it != this->_log_messages.end(); it++) {
      nlohmann::json log = {
          {"time", it->time},
          {"level", it->level},
          {"message", it->message}
      };
      data["messages"].push_back(log);
  }

  (*response_buffer) = data;
  return true;
}

void NSPanel::_send_home_page_update() {
    if(this->_selected_room != nullptr) {
        NSPanelRoomStatus proto;
        // Update default room topic
        if(this->_selected_room->get_protobuf_room_status(&proto)) {
            std::string serialized_result = proto.SerializeAsString();
            if(serialized_result.size() > 0) {
                MQTT_Manager::publish(this->_mqtt_topic_home_page_status, serialized_result, true);
            }
        } else {
            SPDLOG_ERROR("Failed to send out room status update for room {}::{} for panel {}::{}.", this->_selected_room->get_id(), this->_selected_room->get_name(), this->_id, this->_name);
        }
    } else {
        SPDLOG_ERROR("Tried to send out room state update for NSPanel {}::{} but no room has been selected.", this->_id, this->_name);
    }

    // Update all rooms topics:
    NSPanelRoomStatus all_status;
    all_status.set_id(-1);
    all_status.set_name("ALL");

    std::vector<std::shared_ptr<Light>> lights = EntityManager::get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
    // Remove any light not controlled by main page.
    // TODO: Merge this algorithm with the one from room.cpp (get_protobuf_room_status)
    lights.erase(std::remove_if(lights.begin(), lights.end(), [](std::shared_ptr<Light> light) {
        return !light->get_controlled_from_main_page();
    }), lights.end());

    bool any_lights_on = false;
    // Find if any light is on
    for(auto &light : lights) {
        if(light->get_state()) {
            any_lights_on = true;
            break;
        }
    }

    if(any_lights_on) {
        // Remove any light that is off
        lights.erase(std::remove_if(lights.begin(), lights.end(), [](std::shared_ptr<Light> light) {
            return !light->get_state();
        }), lights.end());
    }

    // Calculate average light level
    uint64_t total_light_level_all = 0;
    uint64_t total_light_level_ceiling = 0;
    uint64_t total_light_level_table = 0;
    uint64_t total_kelvin_level_all = 0;
    uint64_t total_kelvin_ceiling = 0;
    uint64_t total_kelvin_table = 0;
    uint16_t num_lights_total = 0;
    uint16_t num_lights_ceiling = 0;
    uint16_t num_lights_ceiling_on = 0;
    uint16_t num_lights_table = 0;
    uint16_t num_lights_table_on = 0;

    for(auto &light : lights) {
        if ((any_lights_on && light->get_state()) || !any_lights_on) {
            total_light_level_all += light->get_brightness();
            total_kelvin_level_all += light->get_color_temperature();
            num_lights_total++;
        }

        if (light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::TABLE) {
            num_lights_table++;
            if (light->get_state()) {
                total_light_level_table += light->get_brightness();
                total_kelvin_table += light->get_color_temperature();
                num_lights_table_on++;
            }
        } else if (light->get_light_type() == MQTT_MANAGER_LIGHT_TYPE::CEILING) {
            num_lights_ceiling++;
            if (light->get_state()) {
                total_light_level_ceiling += light->get_brightness();
                total_kelvin_ceiling += light->get_color_temperature();
                num_lights_ceiling_on++;
            }
        }
    }

    // Update result if a ceiling or table light is found.
    all_status.set_num_table_lights(num_lights_table);
    all_status.set_num_ceiling_lights(num_lights_ceiling);
    all_status.set_num_table_lights_on(num_lights_table_on);
    all_status.set_num_ceiling_lights_on(num_lights_ceiling_on);

    if (num_lights_total > 0) {
        float average_kelvin = (float)total_kelvin_level_all / num_lights_total;
        average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
        uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;
        if(MqttManagerConfig::get_settings().reverse_color_temperature_slider()) {
            kelvin_pct = 100 - kelvin_pct;
        }

        all_status.set_average_dim_level(total_light_level_all / num_lights_total);
        all_status.set_average_color_temperature(kelvin_pct);
    } else {
        all_status.set_average_dim_level(0);
        all_status.set_average_color_temperature(0);
    }

    if (num_lights_table_on > 0) {
        float average_kelvin = (float)total_kelvin_table / num_lights_table_on;
        average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
        uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;
        if(MqttManagerConfig::get_settings().reverse_color_temperature_slider()) {
            kelvin_pct = 100 - kelvin_pct;
        }

        all_status.set_table_lights_dim_level(total_light_level_table / num_lights_table_on);
        all_status.set_table_lights_color_temperature_value(kelvin_pct);
    } else {
        SPDLOG_TRACE("No table lights found, setting value to 0.");
        all_status.set_table_lights_dim_level(0);
        all_status.set_table_lights_color_temperature_value(0);
    }

    if (num_lights_ceiling_on > 0) {
        float average_kelvin = (float)total_kelvin_ceiling / num_lights_ceiling_on;
        average_kelvin -= MqttManagerConfig::get_settings().color_temp_min();
        uint8_t kelvin_pct = (average_kelvin / (MqttManagerConfig::get_settings().color_temp_max() - MqttManagerConfig::get_settings().color_temp_min())) * 100;
        if(MqttManagerConfig::get_settings().reverse_color_temperature_slider()) {
            kelvin_pct = 100 - kelvin_pct;
        }

        all_status.set_ceiling_lights_dim_level(total_light_level_ceiling / num_lights_ceiling_on);
        all_status.set_ceiling_lights_color_temperature_value(kelvin_pct);
    } else {
        SPDLOG_TRACE("No ceiling lights found, setting value to 0.");
        all_status.set_ceiling_lights_dim_level(0);
        all_status.set_ceiling_lights_color_temperature_value(0);
    }

    std::string serialized_result = all_status.SerializeAsString();
    if(serialized_result.size() > 0) {
        MQTT_Manager::publish(this->_mqtt_topic_home_page_all_rooms_status, serialized_result, true);
    } else {
        SPDLOG_ERROR("Tried to send out all_rooms state update to NSPanel {}::{} but failed to serialize the result.");
    }
}

void NSPanel::_send_entities_page_update() {
    if(this->_selected_room != nullptr) {
        SPDLOG_DEBUG("NSPanel {}::{}, sending out new entities page for pagge index: {}", this->_id, this->_name, this->_selected_entity_page_index);
        NSPanelRoomEntitiesPage proto;
        if(this->_selected_room->get_protobuf_room_entity_page(this->_selected_entity_page_index, &proto)) {
            std::string serialized_result = proto.SerializeAsString();
            if(serialized_result.size() > 0) {
                MQTT_Manager::publish(this->_mqtt_topic_room_entities_page_status, serialized_result, true);
            }
        } else {
            SPDLOG_ERROR("Failed to send out room status update for room {}::{} for panel {}::{}.", this->_selected_room->get_id(), this->_selected_room->get_name(), this->_id, this->_name);
        }
    } else {
        SPDLOG_ERROR("Tried to send out room state update for NSPanel {}::{} but no room has been selected.", this->_id, this->_name);
    }
}

void NSPanel::_go_to_next_room() {
    std::vector<std::shared_ptr<Room>> all_rooms = EntityManager::get_all_rooms();
    auto room_it = std::find_if(all_rooms.begin(), all_rooms.end(), [this](std::shared_ptr<Room> room) {
        return this->_selected_room->get_id() == room->get_id();
    });

    if(room_it == all_rooms.end()) {
        // Selected room not found, go to default room.
        this->_go_to_default_room();
    } else [[likely]] {
        room_it++;
        if(room_it == all_rooms.end()) {
            // Reached end of list of rooms, start from beginning.
            this->_selected_room = all_rooms[0];
        } else [[likely]] {
            this->_selected_room = (*room_it);
        }
    }
}

void NSPanel::_go_to_previous_room() {
    std::vector<std::shared_ptr<Room>> all_rooms = EntityManager::get_all_rooms();
    auto room_it = std::find_if(all_rooms.begin(), all_rooms.end(), [this](std::shared_ptr<Room> room) {
        return this->_selected_room->get_id() == room->get_id();
    });

    if(room_it == all_rooms.end()) {
        // Selected room not found, go to default room.
        this->_go_to_default_room();
    } else [[likely]] {
        if(room_it == all_rooms.begin()) {
            // Reached end of list of rooms, start from beginning.
            this->_selected_room = all_rooms[all_rooms.size()-1];
        } else [[likely]] {
            room_it--;
            this->_selected_room = (*room_it);
        }
    }
}

void NSPanel::_go_to_default_room() {
    auto settings_it = std::find_if(MqttManagerConfig::nspanel_configs.begin(), MqttManagerConfig::nspanel_configs.end(), [this](NSPanelSettings s) {
      return s.id() == this->_id;
    });
    if(settings_it != MqttManagerConfig::nspanel_configs.end()) {
        uint32_t default_room_id = settings_it->default_room();
        SPDLOG_DEBUG("NSPanel {}::{} trying to navigate to default room ID {}.", this->_id, this->_name, default_room_id);

        std::vector<std::shared_ptr<Room>> all_rooms = EntityManager::get_all_rooms();
        auto room_it = std::find_if(all_rooms.begin(), all_rooms.end(), [default_room_id](std::shared_ptr<Room> room) {
            return room->get_id() == default_room_id;
        });

        if(room_it == all_rooms.end()) {
            // Selected room not found, go to default room.
            SPDLOG_ERROR("Couldn't find default room with ID {} for NSPanel {}::{}. Will go to first room in list.", default_room_id, this->_id, this->_name);
            if(all_rooms.size() > 0) [[likely]] {
                this->_selected_room = (*room_it);
            } else {
                SPDLOG_ERROR("No rooms loaded! Setting nullptr!");
                this->_selected_room = nullptr;
            }
        } else [[likely]] {
            this->_selected_room = (*room_it);
        }
    } else {
        SPDLOG_ERROR("Couldn't find config for NSPanel {}::{}.", this->_id, this->_name);
    }
}


void NSPanel::_room_change_callback(Room* room) {
    std::vector<std::shared_ptr<MqttManagerEntity>> entities = this->_selected_room->get_all_entities();
    SPDLOG_DEBUG("Attaching entity change callback to all ({}) entities in currently selected room.", entities.size());
    for(auto &entity : entities) {
        SPDLOG_DEBUG("NSPanel {}::{} attaching change callback to entity with ID {}, type: {}.", this->_id, this->_name, entity->get_id(), (int)entity->get_type());
        entity->attach_entity_changed_callback(boost::bind(&NSPanel::_entity_change_callback, this, _1));
    }
}

void NSPanel::_entity_change_callback(MqttManagerEntity *entity) {
    // TODO: Is Mutex needed?
    {
        std::lock_guard<std::mutex> lock_guard(this->_last_state_update_mutex);
        NSPanel::_last_state_update = std::chrono::system_clock::now();
        if(this->_last_state_update_sent) {
            // Thread is not running, start it.
            SPDLOG_DEBUG("NSPanel send state update thread is not running, starting it.");
            this->_last_state_update_sent = false;
            std::thread(&NSPanel::_check_and_send_new_status_update, this).detach();
        }
    }
}

void NSPanel::_check_and_send_new_status_update() {
    while(true) {
        {
            std::lock_guard<std::mutex> lock_guard(this->_last_state_update_mutex);
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            if(now >= this->_last_state_update + std::chrono::milliseconds(100)) {
                SPDLOG_DEBUG("NSPanel {}::{} past update timeout threshold, sending new protobuf state.", this->_id, this->_name);
                this->_send_home_page_update();
                this->_send_entities_page_update();
                this->_last_state_update_sent = true;
                break; // We have sent state update, exit this thread.
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
