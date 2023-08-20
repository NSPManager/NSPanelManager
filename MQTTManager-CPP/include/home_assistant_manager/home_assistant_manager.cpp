#include "home_assistant_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
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

void HomeAssistantManager::connect() {
  spdlog::debug("Initializing Home Assistant Manager component.");
  ix::initNetSystem();

  if (HomeAssistantManager::_websocket == nullptr) {
    HomeAssistantManager::_websocket = new ix::WebSocket();
  }

  std::string home_assistant_websocket_url = MqttManagerConfig::home_assistant_address;
  home_assistant_websocket_url.append("/api/websocket"); // TODO: Implement /core/websocket if running as HA addon
  if (home_assistant_websocket_url.find("https://") != std::string::npos) {
    spdlog::debug("Replacing https with wss");
    // Replace https with wss
    home_assistant_websocket_url = home_assistant_websocket_url.replace(home_assistant_websocket_url.find("https://"), sizeof("https://") - 1, "wss://");
    spdlog::debug("Settings TLS options");
    ix::SocketTLSOptions tls_options;
    tls_options.tls = true;
    tls_options.caFile = "NONE";
    HomeAssistantManager::_websocket->setTLSOptions(tls_options);
  } else if (home_assistant_websocket_url.find("http://") != std::string::npos) {
    // Replace http with ws
    spdlog::debug("Replacing http with ws");
    home_assistant_websocket_url = home_assistant_websocket_url.replace(home_assistant_websocket_url.find("http://"), sizeof("http://") - 1, "ws://");
  } else {
    spdlog::error("Unknown connection type of Home Assistant. Will not continue!");
    return;
  }

  spdlog::info("Will connect to Home Assistant websocket at {}", home_assistant_websocket_url);
  HomeAssistantManager::_websocket->setUrl(home_assistant_websocket_url);
  HomeAssistantManager::_websocket->setOnMessageCallback(&HomeAssistantManager::_websocket_message_callback);
  HomeAssistantManager::_websocket->start();
}

void HomeAssistantManager::_websocket_message_callback(const ix::WebSocketMessagePtr &msg) {
  if (msg->type == ix::WebSocketMessageType::Message) {
    HomeAssistantManager::_process_websocket_message(msg->str);
  } else if (msg->type == ix::WebSocketMessageType::Open) {
    spdlog::info("Connected to Home Assistant websocket.");
  } else if (msg->type == ix::WebSocketMessageType::Close) {
    spdlog::info("Disconnected from Home Assistant websocket.");
    HomeAssistantManager::_authenticated = false;
  } else if (msg->type == ix::WebSocketMessageType::Error) {
    spdlog::error("Failed to connect to Home Assistant websocket. Reason: {}", msg->errorInfo.reason);
    HomeAssistantManager::_authenticated = false;
  }
}

void HomeAssistantManager::_process_websocket_message(const std::string &message) {
  try {
    nlohmann::json data = nlohmann::json::parse(message);
    std::string type = data["type"];

    if (type.compare("auth_required") == 0) {
      HomeAssistantManager::_send_auth();
    } else if (type.compare("auth_ok") == 0) {
      spdlog::info("Successfully authenticated to Home Assistant websocket API.");
      HomeAssistantManager::_authenticated = true;

      // Subscribe to all state changes.
      nlohmann::json subscribe_command;
      subscribe_command["type"] = "subscribe_events";
      subscribe_command["event_type"] = "state_changed";
      HomeAssistantManager::send_json(subscribe_command);

      // Request all current states in HA
      HomeAssistantManager::_all_statues_request_message_id = HomeAssistantManager::_next_message_id;
      nlohmann::json all_states_request_command;
      all_states_request_command["type"] = "get_states";
      HomeAssistantManager::send_json(all_states_request_command);
    } else if (type.compare("result") == 0) {
      bool success = data["success"];
      if (!success) {
        spdlog::error("[HA WS] Command failed. Data: {}", message);
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
      spdlog::info("Got unhandled message: {}", message);
    }
  } catch (std::exception ex) {
    spdlog::error("Caught std::exception while processing WebSocketMessage. Exception: {}, message: {}", ex.what(), message);
  } catch (...) {
    spdlog::error("Caught exception while processing WebSocket message: {}", message);
  }
}

void HomeAssistantManager::_send_auth() {
  nlohmann::json auth_data;
  auth_data["type"] = "auth";
  auth_data["access_token"] = MqttManagerConfig::home_assistant_access_token;
  std::string buffer = auth_data.dump();

  HomeAssistantManager::_send_string(buffer);
}

void HomeAssistantManager::send_json(nlohmann::json &data) {
  data["id"] = HomeAssistantManager::_next_message_id++;
  std::string buffer = data.dump();
  HomeAssistantManager::_send_string(buffer);
}

void HomeAssistantManager::_send_string(std::string &data) {
  if (HomeAssistantManager::_websocket != nullptr) {
    std::lock_guard<std::mutex> mtex_lock(HomeAssistantManager::_mutex_websocket_write_access);
    // spdlog::debug("[HA WS] Sending data: {}", data);
    HomeAssistantManager::_websocket->send(data);
  }
}

void HomeAssistantManager::attach_event_observer(HomeAssistantEventObserver *observer) {
  HomeAssistantManager::_home_assistant_event_observers.push_back(observer);
}

void HomeAssistantManager::detach_event_observer(HomeAssistantEventObserver *observer) {
  HomeAssistantManager::_home_assistant_event_observers.remove(observer);
}

void HomeAssistantManager::_process_home_assistant_event(nlohmann::json &event_data) {
  for (HomeAssistantEventObserver *observer : HomeAssistantManager::_home_assistant_event_observers) {
    if (observer->home_assistant_event_callback(event_data)) {
      return;
    }
  }
}
