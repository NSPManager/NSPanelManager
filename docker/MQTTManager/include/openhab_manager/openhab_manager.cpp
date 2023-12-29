#include "openhab_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/algorithm/string.hpp>
#include <curl/curl.h>
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
  filter["payload"] = "[\"ItemStateEvent\", \"ItemStateChangedEvent\", \"ItemStateUpdatedEvent\"]";
  OpenhabManager::send_json(filter);
}

void OpenhabManager::_websocket_message_callback(const ix::WebSocketMessagePtr &msg) {
  if (msg->type == ix::WebSocketMessageType::Message) {
    OpenhabManager::_process_websocket_message(msg->str);
  } else if (msg->type == ix::WebSocketMessageType::Open) {
    SPDLOG_INFO("Connected to Openhab websocket.");
    OpenhabManager::_authenticated = true;
    for (auto item_observer_pairs : OpenhabManager::_openhab_item_observers) {
      try {
        std::string data = OpenhabManager::_fetch_item_state_via_rest(item_observer_pairs.first);
        if (data.length() > 0) {
          nlohmann::json update_data;
          update_data["type"] = "ItemStateFetched";
          update_data["payload"] = nlohmann::json::parse(data);
          OpenhabManager::_openhab_item_observers[item_observer_pairs.first](update_data);
        } else {
          SPDLOG_ERROR("Failed to get current state for item '{}' via OpenHAB REST API.", item_observer_pairs.first);
        }
      } catch (std::exception &e) {
        SPDLOG_ERROR("Caught exception: {}", e.what());
        SPDLOG_ERROR("Stacktrace: {}", boost::diagnostic_information(e, true));
      }
    }
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
    } else if (type.compare("ItemStateChangedEvent") == 0 || type.compare("ItemStateUpdatedEvent") == 0) {
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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string OpenhabManager::_fetch_item_state_via_rest(std::string item) {
  CURL *curl = curl_easy_init();
  CURLcode res;

  std::string response_data;
  std::string request_url = MqttManagerConfig::openhab_address;
  request_url.append("/rest/items/");
  request_url.append(item);
  SPDLOG_TRACE("Requesting openhab item state from: {}", request_url);

  std::string bearer_token = "Bearer ";
  bearer_token.append(MqttManagerConfig::openhab_access_token);

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, bearer_token.c_str());
  if (headers == NULL) {
    SPDLOG_ERROR("Failed to set bearer token header for OpenHAB light rest request.");
    return response_data;
  }
  headers = curl_slist_append(headers, "Content-type: application/json");
  if (headers == NULL) {
    SPDLOG_ERROR("Failed to set content-type header for OpenHAB light rest request.");
    return response_data;
  }

  curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  /* Perform the request, res will get the return code */
  res = curl_easy_perform(curl);
  long http_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  /* Check for errors */
  if (res != CURLE_OK || http_code != 200) {
    SPDLOG_ERROR("Failed to get data from OpenHAB api at '{}'. Got response: {}", request_url, response_data);
    return response_data;
  }

  /* always cleanup */
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  return response_data;
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

void OpenhabManager::_process_openhab_event(nlohmann::json &event_data) {
  SPDLOG_TRACE("Processing Openhab event: {}", event_data.dump());
  if (std::string(event_data["type"]).compare("ItemStateChangedEvent") == 0) {
    SPDLOG_DEBUG("Got new state data: {}", event_data.dump());
    // Extract topic into multiple parts
    std::string topic = event_data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    if (topic_parts.size() >= 3) {
      std::string topic_item = topic_parts[2];
      OpenhabManager::_openhab_item_observers[topic_item](event_data);
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
