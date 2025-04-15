#include "home_assistant_manager.hpp"
#include "light/home_assistant_light.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/frame.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <exception>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXSocketTLSOptions.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <mutex>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

void HomeAssistantManager::connect() {
  SPDLOG_DEBUG("Initializing Home Assistant Manager component.");
  ix::initNetSystem();

  HomeAssistantManager::reload_config();

  bool init_success = false;
  while (!init_success) {
    try {
      std::lock_guard<std::mutex> lock_guard(HomeAssistantManager::_settings_mutex);
      if (HomeAssistantManager::_websocket == nullptr) {
        HomeAssistantManager::_websocket = new ix::WebSocket();
        HomeAssistantManager::_websocket->setPingInterval(30);
      }

      std::string home_assistant_websocket_url = HomeAssistantManager::_home_assistant_address;
      if (home_assistant_websocket_url.empty()) {
        SPDLOG_ERROR("No Home Assistant address configured. Will not continue to load Home Assistant component.");
        return;
      }

      if (MqttManagerConfig::get_settings().is_home_assistant_addon) {
        home_assistant_websocket_url.append("/core/websocket");
      } else {
        home_assistant_websocket_url.append("/api/websocket");
      }
      boost::algorithm::replace_first(home_assistant_websocket_url, "https://", "wss://");
      boost::algorithm::replace_first(home_assistant_websocket_url, "http://", "ws://");
      if (boost::algorithm::starts_with(home_assistant_websocket_url, "wss://")) {
        SPDLOG_DEBUG("Setting TLS options");
        ix::SocketTLSOptions tls_options;
        tls_options.tls = true;
        tls_options.caFile = "NONE";
        HomeAssistantManager::_websocket->setTLSOptions(tls_options);
      }

      SPDLOG_INFO("Will connect to Home Assistant websocket at {}", home_assistant_websocket_url);
      HomeAssistantManager::_websocket->setUrl(home_assistant_websocket_url);
      HomeAssistantManager::_websocket->setOnMessageCallback(&HomeAssistantManager::_websocket_message_callback);
      HomeAssistantManager::_websocket->start();

      init_success = true; // We successfully reached end of section without exception, consider it a success and exit loop.
    } catch (std::exception &e) {
      SPDLOG_ERROR("Caught exception: {}", e.what());
      SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
    }
  }
}

void HomeAssistantManager::reload_config() {
  bool reconnect = false;
  {
    std::lock_guard<std::mutex> lock_guard(HomeAssistantManager::_settings_mutex);
    std::string address = MqttManagerConfig::get_setting_with_default("home_assistant_address", "");
    std::string token = MqttManagerConfig::get_setting_with_default("home_assistant_token", "");

    if (HomeAssistantManager::_home_assistant_address.compare(address) != 0 || HomeAssistantManager::_home_assistant_token.compare(token) != 0) {
      HomeAssistantManager::_home_assistant_address = address;
      HomeAssistantManager::_home_assistant_token = token;
      reconnect = true;
    }
  }

  if (reconnect) {
    SPDLOG_INFO("Will connect to Home Assistant with new settings. Server address: {}", HomeAssistantManager::_home_assistant_address);
    if (HomeAssistantManager::_websocket != nullptr) {
      HomeAssistantManager::_websocket->close();
      HomeAssistantManager::connect();
    }
  }
}

void HomeAssistantManager::_websocket_message_callback(const ix::WebSocketMessagePtr &msg) {
  if (msg->type == ix::WebSocketMessageType::Message) {
    HomeAssistantManager::_process_websocket_message(msg->str);
  } else if (msg->type == ix::WebSocketMessageType::Open) {
    SPDLOG_INFO("Connected to Home Assistant websocket.");
    HomeAssistantManager::_connected = true;
  } else if (msg->type == ix::WebSocketMessageType::Close) {
    SPDLOG_WARN("Disconnected from Home Assistant websocket.");
    HomeAssistantManager::_authenticated = false;
    HomeAssistantManager::_connected = false;
  } else if (msg->type == ix::WebSocketMessageType::Error) {
    SPDLOG_ERROR("Failed to connect to Home Assistant websocket. Reason: {}", msg->errorInfo.reason);
    HomeAssistantManager::_authenticated = false;
    HomeAssistantManager::_connected = false;
  }
}

void HomeAssistantManager::_process_websocket_message(const std::string &message) {
  try {
    nlohmann::json data = nlohmann::json::parse(message);
    std::string type = data["type"];

    if (type.compare("auth_required") == 0) {
      HomeAssistantManager::_send_auth();
    } else if (type.compare("auth_ok") == 0) {
      SPDLOG_INFO("Successfully authenticated to Home Assistant websocket API.");
      HomeAssistantManager::_authenticated = true;

      // Subscribe to all state changes.
      nlohmann::json subscribe_command;
      subscribe_command["type"] = "subscribe_events";
      subscribe_command["event_type"] = "state_changed";
      HomeAssistantManager::send_json(subscribe_command);

      HomeAssistantManager::_request_all_states();
    } else if (type.compare("result") == 0) {
      bool success = data["success"];
      if (!success) {
        SPDLOG_ERROR("[HA WS] Command failed. Data: {}", message);
      }

      if ((long)data["id"] == HomeAssistantManager::_all_statues_request_message_id) {
        for (nlohmann::json entity_data : data["result"]) {
          // Modify data to look like a state event for entities to handle.
          // spdlog::debug("Building from JSON: {}", entity_data.dump());
          nlohmann::json modified_data;
          modified_data["event"]["data"]["new_state"] = entity_data;
          modified_data["event"]["data"]["entity_id"] = entity_data["entity_id"];
          modified_data["event"]["event_type"] = "state_changed";
          // spdlog::debug("Building JSON: {}", modified_data.dump());
          HomeAssistantManager::_process_home_assistant_event(modified_data);
        }
      }
    } else if (type.compare("event") == 0) {
      HomeAssistantManager::_process_home_assistant_event(data);
    } else {
      SPDLOG_WARN("Got unhandled message: {}", message);
    }
  } catch (std::exception ex) {
    SPDLOG_ERROR("Stacktrace: {}", boost::diagnostic_information(ex, true));
  } catch (...) {
    SPDLOG_ERROR("Caught exception while processing WebSocket message: {}", message);
  }
}

void HomeAssistantManager::_send_auth() {
  std::lock_guard<std::mutex> lock_guard(HomeAssistantManager::_settings_mutex);
  nlohmann::json auth_data;
  auth_data["type"] = "auth";
  auth_data["access_token"] = HomeAssistantManager::_home_assistant_token;
  std::string buffer = auth_data.dump();

  HomeAssistantManager::_send_string(buffer);
}

void HomeAssistantManager::send_json(nlohmann::json &data) {
  data["id"] = HomeAssistantManager::_next_message_id++;
  std::string buffer = data.dump();
  HomeAssistantManager::_send_string(buffer);
}

void HomeAssistantManager::_send_string(std::string &data) {
  if (HomeAssistantManager::_websocket != nullptr && HomeAssistantManager::_connected) {
    std::lock_guard<std::mutex> mtex_lock(HomeAssistantManager::_mutex_websocket_write_access);
    SPDLOG_TRACE("[HA WS] Sending data: {}", data);
    HomeAssistantManager::_websocket->send(data);
  }
}

void HomeAssistantManager::_request_all_states() {
  // Request all current states in HA
  HomeAssistantManager::_all_statues_request_message_id = HomeAssistantManager::_next_message_id.load();
  nlohmann::json all_states_request_command;
  all_states_request_command["type"] = "get_states";
  HomeAssistantManager::send_json(all_states_request_command);
}

void HomeAssistantManager::_process_home_assistant_event(nlohmann::json &event_data) {
  if (event_data.contains("event") && !event_data["event"].is_null()) {
    if (event_data["event"].contains("event_type") && !event_data["event"]["event_type"].is_null()) {
      if (std::string(event_data["event"]["event_type"]).compare("state_changed") == 0) {
        std::string home_assistant_entity_name = event_data["event"]["data"]["entity_id"];
        if (HomeAssistantManager::_home_assistant_observers.find(home_assistant_entity_name) != HomeAssistantManager::_home_assistant_observers.end()) {
          try {
            HomeAssistantManager::_home_assistant_observers.at(home_assistant_entity_name)(event_data);
          } catch (const std::exception &e) {
            SPDLOG_ERROR("Caught exception during processing of home assistant event. Diagnostic information: {}", boost::diagnostic_information(e, true));
            SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
          }
        }
      }
    }
  }
}
