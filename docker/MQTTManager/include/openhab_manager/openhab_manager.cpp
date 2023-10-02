#include "openhab_manager.hpp"
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
#include <thread>

void OpenhabManager::connect() {
  SPDLOG_DEBUG("Initializing Openhab Manager component.");
  ix::initNetSystem();

  if (OpenhabManager::_websocket == nullptr) {
    OpenhabManager::_websocket = new ix::WebSocket();
  }

  std::string openhab_websocket_url = MqttManagerConfig::openhab_address;
  openhab_websocket_url.append("/ws");
  if (openhab_websocket_url.find("https://") != std::string::npos) {
    // Replace https with wss
    openhab_websocket_url = openhab_websocket_url.replace(openhab_websocket_url.find("https://"), sizeof("https://") - 1, "wss://");
    SPDLOG_DEBUG("Settings TLS options");
    ix::SocketTLSOptions tls_options;
    tls_options.tls = true;
    tls_options.caFile = "NONE";
    OpenhabManager::_websocket->setTLSOptions(tls_options);
  } else if (openhab_websocket_url.find("http://") != std::string::npos) {
    // Replace http with ws
    openhab_websocket_url = openhab_websocket_url.replace(openhab_websocket_url.find("http://"), sizeof("http://") - 1, "ws://");
  } else {
    SPDLOG_ERROR("Unknown connection type for Openhab. Will not continue!");
    return;
  }

  SPDLOG_INFO("Will connect to Openhab websocket at {}", openhab_websocket_url);
  SPDLOG_DEBUG("Appending Openhab access token to url.");
  openhab_websocket_url.append("?accessToken=");
  openhab_websocket_url.append(MqttManagerConfig::openhab_access_token);
  OpenhabManager::_websocket->setUrl(openhab_websocket_url);
  OpenhabManager::_websocket->setOnMessageCallback(&OpenhabManager::_websocket_message_callback);
  OpenhabManager::_websocket->setPingInterval(10);
  OpenhabManager::_websocket->start();

  OpenhabManager::_keepalive_thread = std::thread(OpenhabManager::_send_keepalive);
  OpenhabManager::_keepalive_thread.join();

  // Set openhab event filter
  nlohmann::json filter;
  filter["type"] = "WebSocketEvent";
  filter["topic"] = "openhab/websocket/filter/type";
  filter["payload"] = "[\"ItemStateEvent\", \"ItemStateChangedEvent\"]";
  OpenhabManager::send_json(filter);
}

void OpenhabManager::_websocket_message_callback(const ix::WebSocketMessagePtr &msg) {
  if (msg->type == ix::WebSocketMessageType::Message) {
    OpenhabManager::_process_websocket_message(msg->str);
  } else if (msg->type == ix::WebSocketMessageType::Open) {
    SPDLOG_INFO("Connected to Openhab websocket.");
    OpenhabManager::_authenticated = true;
    OpenhabManager::_fetch_and_update_current_states();
  } else if (msg->type == ix::WebSocketMessageType::Close) {
    SPDLOG_WARN("Disconnected from Openhab websocket.");
    OpenhabManager::_authenticated = false;
  } else if (msg->type == ix::WebSocketMessageType::Error) {
    SPDLOG_ERROR("Failed to connect to Openhab websocket. Reason: {}", msg->errorInfo.reason);
    OpenhabManager::_authenticated = false;
  } else if (msg->type == ix::WebSocketMessageType::Fragment) {
    SPDLOG_ERROR("Received Openhab websocket fragment which is not supported.");
  }
}

void OpenhabManager::_process_websocket_message(const std::string &message) {
  try {
    SPDLOG_TRACE("Got Openhab message: {}", message);

    nlohmann::json data = nlohmann::json::parse(message);
    std::string type = data["type"];

    if (type.compare("PONG") == 0) {
      SPDLOG_TRACE("Received PONG from Openhab.");
    } else if (type.compare("ItemStateChangedEvent") == 0) {
      OpenhabManager::_process_openhab_event(data);
    } else {
      SPDLOG_TRACE("Got unhandled message: {}", message);
    }
  } catch (std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing WebSocketMessage. Exception: {}, message: {}", ex.what(), message);
  } catch (...) {
    SPDLOG_ERROR("Caught exception while processing WebSocket message: {}", message);
  }
}

void OpenhabManager::_fetch_and_update_current_states() {
  SPDLOG_INFO("Fetching current states for Openhab entities.");
}

void OpenhabManager::send_json(nlohmann::json &data) {
  data["source"] = "NSPanelManager::MqttManager";
  std::string buffer = data.dump();
  SPDLOG_TRACE("Sending data: {}", buffer);
  OpenhabManager::_send_string(buffer);
}

void OpenhabManager::_send_string(std::string &data) {
  if (OpenhabManager::_websocket != nullptr) {
    std::lock_guard<std::mutex> mtex_lock(OpenhabManager::_mutex_websocket_write_access);
    // spdlog::debug("[HA WS] Sending data: {}", data);
    OpenhabManager::_websocket->send(data);
  }
}

void OpenhabManager::attach_event_observer(OpenhabEventObserver *observer) {
  OpenhabManager::_openhab_event_observers.push_back(observer);
}

void OpenhabManager::detach_event_observer(OpenhabEventObserver *observer) {
  OpenhabManager::_openhab_event_observers.remove(observer);
}

void OpenhabManager::_process_openhab_event(nlohmann::json &event_data) {
  SPDLOG_DEBUG("Processing Openhab event: {}", event_data.dump());
  for (OpenhabEventObserver *observer : OpenhabManager::_openhab_event_observers) {
    if (observer->openhab_event_callback(event_data)) {
      return;
    }
  }
}

void OpenhabManager::_send_keepalive() {
  SPDLOG_INFO("Started Openhab keepalive thread.");
  for (;;) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    SPDLOG_TRACE("Sending openhab keepalive.");
    if (OpenhabManager::_authenticated) {
      std::lock_guard<std::mutex> mtex_lock(OpenhabManager::_mutex_websocket_write_access);
      nlohmann::json keepalive_message;
      keepalive_message["type"] = "WebSocketEvent";
      keepalive_message["topic"] = "openhab/websocket/heartbeat";
      keepalive_message["payload"] = "PING";
      keepalive_message["source"] = "NSPanelManager::MqttManager";
      OpenhabManager::_websocket->send(keepalive_message.dump());
    }
  }
}
