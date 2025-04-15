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
#include <sys/socket.h>
#include <thread>

void OpenhabManager::connect() {
  SPDLOG_DEBUG("Initializing Openhab Manager component.");
  ix::initNetSystem();
  OpenhabManager::reload_config();

  if (OpenhabManager::_websocket == nullptr) {
    OpenhabManager::_websocket = new ix::WebSocket();
  }

  std::lock_guard<std::mutex> lock_guard(OpenhabManager::_setting_values_mutex);
  std::string openhab_websocket_url = OpenhabManager::_openhab_address;
  if (openhab_websocket_url.empty()) {
    SPDLOG_ERROR("No OpenHAB address configured, will not continue to load OpenHAB component.");
    return;
  }

  openhab_websocket_url.append("/ws");

  boost::algorithm::replace_first(openhab_websocket_url, "https://", "wss://");
  boost::algorithm::replace_first(openhab_websocket_url, "http://", "ws://");
  if (boost::algorithm::starts_with(openhab_websocket_url, "wss://")) {
    SPDLOG_DEBUG("Settings TLS options");
    ix::SocketTLSOptions tls_options;
    tls_options.tls = true;
    tls_options.caFile = "NONE";
    OpenhabManager::_websocket->setTLSOptions(tls_options);
  }

  SPDLOG_INFO("Will connect to Openhab websocket at {}", openhab_websocket_url);
  SPDLOG_DEBUG("Appending Openhab access token to url.");
  openhab_websocket_url.append("?accessToken=");
  openhab_websocket_url.append(OpenhabManager::_openhab_token);

  OpenhabManager::_websocket->setUrl(openhab_websocket_url);
  OpenhabManager::_websocket->setOnMessageCallback(&OpenhabManager::_websocket_message_callback);
  OpenhabManager::_websocket->setPingInterval(10);
  OpenhabManager::_websocket->start();

  OpenhabManager::_send_keepalive_messages = true; // Keep thread alive.
  OpenhabManager::_keepalive_thread = std::thread(OpenhabManager::_send_keepalive);
  OpenhabManager::_keepalive_thread.detach();

  // Set openhab event filter
  nlohmann::json filter;
  filter["type"] = "WebSocketEvent";
  filter["topic"] = "openhab/websocket/filter/type";
  filter["payload"] = "[\"ItemStateEvent\", \"ItemStateChangedEvent\", \"ItemStateUpdatedEvent\"]";
  OpenhabManager::send_json(filter);

  SPDLOG_DEBUG("OpenHAB connect finished.");
}

void OpenhabManager::reload_config() {
  bool reconnect = false;
  {
    std::lock_guard<std::mutex> lock_guard(OpenhabManager::_setting_values_mutex);
    std::string address = MqttManagerConfig::get_setting_with_default("openhab_address", "");
    std::string token = MqttManagerConfig::get_setting_with_default("openhab_token", "");

    if (OpenhabManager::_openhab_address.compare(address) != 0 || OpenhabManager::_openhab_token.compare(token) != 0) {
      OpenhabManager::_openhab_address = address;
      OpenhabManager::_openhab_token = token;
      reconnect = true;
    }
  }

  if (reconnect) {
    SPDLOG_INFO("Will connect to OpenHAB with new settings. Server address: {}", OpenhabManager::_openhab_address);
    OpenhabManager::_send_keepalive_messages = false;
    while (OpenhabManager::_keepalive_thread.joinable()) {
      SPDLOG_DEBUG("Waiting 500ms and checking if keepalive thread is still alive.");
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    OpenhabManager::_send_keepalive_messages = true; // Re-enable function once thread has exited.
    SPDLOG_DEBUG("Keepalive thread exited, start new connection.");
    // Thread has exited, start new connection.
    if (OpenhabManager::_websocket != nullptr) {
      OpenhabManager::_websocket->close();
      OpenhabManager::connect();
    }
  }
}

void OpenhabManager::_websocket_message_callback(const ix::WebSocketMessagePtr &msg) {
  if (msg->type == ix::WebSocketMessageType::Message) {
    OpenhabManager::_process_websocket_message(msg->str);
  } else if (msg->type == ix::WebSocketMessageType::Open) {
    SPDLOG_INFO("Connected to Openhab websocket.");
    OpenhabManager::_authenticated = true;
    OpenhabManager::_connected = true;

    for (auto item_observer_pairs : OpenhabManager::_openhab_item_observers) {
      if (item_observer_pairs.second->num_slots() > 0) {
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
    }
  } else if (msg->type == ix::WebSocketMessageType::Close) {
    SPDLOG_WARN("Disconnected from Openhab websocket.");
    OpenhabManager::_authenticated = false;
    OpenhabManager::_connected = false;
  } else if (msg->type == ix::WebSocketMessageType::Error) {
    SPDLOG_ERROR("Failed to connect to Openhab websocket. Reason: {}", msg->errorInfo.reason);
    OpenhabManager::_authenticated = false;
    OpenhabManager::_connected = false;
  } else if (msg->type == ix::WebSocketMessageType::Fragment) {
    SPDLOG_ERROR("Received Openhab websocket fragment which is not supported.");
  }
}

void OpenhabManager::_process_websocket_message(const std::string &message) {
  try {
    // SPDLOG_TRACE("Got Openhab message: {}", message);

    nlohmann::json data = nlohmann::json::parse(message);
    std::string type = data["type"];

    if (type.compare("PONG") == 0) {
      SPDLOG_TRACE("Received PONG.");
    } else if (type.compare("WebSocketEvent") == 0 && std::string(data["payload"]).compare("PONG") == 0) {
      SPDLOG_TRACE("Received PONG.");
    } else if (type.compare("ItemStateChangedEvent") == 0 || type.compare("ItemStateUpdatedEvent") == 0) {
      OpenhabManager::_process_openhab_event(data);
    } else {
      // SPDLOG_TRACE("Got unhandled message: {}", message);
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
  SPDLOG_TRACE("Trying to fetch item state from OpenHAB for item '{}'.", item);
  CURL *curl = curl_easy_init();
  CURLcode res;

  if (!curl) {
    SPDLOG_ERROR("Failed to create curl object!");
    return "";
  }

  std::string request_url;
  std::string bearer_token = "Authorization: Bearer ";
  std::string response_data;
  {
    std::lock_guard<std::mutex> lock_guard(OpenhabManager::_setting_values_mutex);
    request_url = OpenhabManager::_openhab_address;
    request_url.append("/rest/items/");
    request_url.append(item);

    bearer_token.append(OpenhabManager::_openhab_token);
  }

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, bearer_token.c_str());
  if (headers == NULL) {
    SPDLOG_ERROR("Failed to set bearer token header for OpenHAB light rest request.");
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    return response_data;
  }
  headers = curl_slist_append(headers, "Content-type: application/json");
  if (headers == NULL) {
    SPDLOG_ERROR("Failed to set content-type header for OpenHAB light rest request.");
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    return response_data;
  }

  curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2); // Wait max 2 seconds for an answer
  curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 10000000);

  /* Perform the request, res will get the return code */
  res = curl_easy_perform(curl);
  long http_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  /* Check for errors */
  if (res != CURLE_OK || http_code != 200) {
    SPDLOG_ERROR("Failed to get data from OpenHAB api at '{}'. Got response: {}", request_url, response_data);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    return response_data;
  }

  /* always cleanup */
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  SPDLOG_TRACE("Got data for OpenHAB item {}.", item);
  return response_data;
}

void OpenhabManager::send_json(nlohmann::json &data) {
  data["source"] = "NSPanelManager::MqttManager";
  std::string buffer = data.dump();
  OpenhabManager::_send_string(buffer);
}

void OpenhabManager::_send_string(std::string &data) {
  if (OpenhabManager::_websocket != nullptr && OpenhabManager::_connected) {
    std::lock_guard<std::mutex> mtex_lock(OpenhabManager::_mutex_websocket_write_access);
    SPDLOG_TRACE("[OH WS] Sending data: {}", data);
    OpenhabManager::_websocket->send(data);
  }
}

void OpenhabManager::_process_openhab_event(nlohmann::json &event_data) {
  SPDLOG_TRACE("Processing Openhab event: {}", event_data.dump());
  if (std::string(event_data["type"]).compare("ItemStateChangedEvent") == 0) {
    // Extract topic into multiple parts
    std::string topic = event_data["topic"];
    std::vector<std::string> topic_parts;
    boost::split(topic_parts, topic, boost::is_any_of("/"));
    if (topic_parts.size() >= 3) {
      std::string topic_item = topic_parts[2];
      if (OpenhabManager::_openhab_item_observers.find(topic_item) != OpenhabManager::_openhab_item_observers.end()) {
        OpenhabManager::_openhab_item_observers.at(topic_item)(event_data);
      }
    }
  }
}

void OpenhabManager::_send_keepalive() {
  SPDLOG_INFO("Started Openhab keepalive thread.");
  while (true) {
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

  SPDLOG_WARN("OpenHAB keepalive thread exited!");
}
