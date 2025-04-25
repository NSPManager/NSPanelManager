#include <fmt/core.h>
#include <fstream>
#include <iterator>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>
#include <websocket_server/websocket_server.hpp>

void WebsocketServer::start() {
  if (WebsocketServer::_server == nullptr) {
    SPDLOG_DEBUG("Creating new ix::WebSocketServer.");
    WebsocketServer::_server = new ix::WebSocketServer(8002, "0.0.0.0");
  }

  WebsocketServer::_server->setOnClientMessageCallback(WebsocketServer::_websocket_message_callback);

  SPDLOG_DEBUG("Start listening for websocket connection on 0.0.0.0:8002.");
  while (true) {
    auto result = WebsocketServer::_server->listen();
    if (result.first) {
      SPDLOG_DEBUG("Successfully bound to 0.0.0.0:8002");
      break;
    } else {
      SPDLOG_ERROR("Failed to open socket on port 8002. Will try again in 1 second. Message: {}", result.second);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }

  WebsocketServer::_server->disablePerMessageDeflate();
  WebsocketServer::_server->start();
  WebsocketServer::_server->wait();
}

void WebsocketServer::_websocket_message_callback(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) {
  try {
    if (msg->type == ix::WebSocketMessageType::Open) {
      // Client just connected, send updated warnings
      std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
      nlohmann::json base;
      base["event_data"] = nlohmann::json::object();
      base["event_data"]["warnings"] = nlohmann::json::array();
      base["event_type"] = "mqttmanager_active_warnings";
      for (auto &warning : WebsocketServer::_active_warnings) {
        base["event_data"]["warnings"].push_back({{"level", warning.level}, {"text", warning.warning_text}});
      }
      std::string json = nlohmann::to_string(base);
      webSocket.sendText(json);
    } else if (msg->type == ix::WebSocketMessageType::Message) {
      try {
        nlohmann::json json = nlohmann::json::parse(msg->str);
        if (std::string(json["type"]).compare("broadcast") == 0) {
          for (auto client : WebsocketServer::_server->getClients()) {
            if (json["data"].is_string()) {
              client->sendText(json["data"]);
            } else {
              client->sendText(nlohmann::to_string(json["data"]));
            }
          }
          return;
        }
      } catch (...) {
      }

      SPDLOG_TRACE("Got message: {}", msg->str);
      std::string message = msg->str;
      std::string response_buffer;
      for (auto callback : WebsocketServer::_callbacks) {
        if (callback(message, &response_buffer)) {
          webSocket.sendText(response_buffer);
          break;
        }
      }
    }
  } catch (std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing websocket event. Exception: {}", ex.what());
  } catch (...) {
    SPDLOG_ERROR("Caught exception while processing WebSocket event.");
  }
}

void WebsocketServer::broadcast_json(nlohmann::json &json) {
  std::string data = json.dump();
  WebsocketServer::broadcast_string(data);
}

void WebsocketServer::broadcast_string(std::string &data) {
  if (WebsocketServer::_server != nullptr) {
    std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
    for (auto websocket : WebsocketServer::_server->getClients()) {
      websocket->send(data);
    }
  } else {
    SPDLOG_ERROR("Trying to send data over WebSocket before websocket exists.");
  }
}

void WebsocketServer::attach_message_callback(std::function<bool(std::string &message, std::string *response_buf)> callback) {
  WebsocketServer::_callbacks.push_back(callback);
}

void WebsocketServer::register_warning(ActiveWarningLevel level, std::string warning_text) {
  bool send_update = false;
  {
    bool found = false;
    std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
    for (auto &warning : WebsocketServer::_active_warnings) {
      if (warning.warning_text.compare(warning_text) == 0) {
        if (warning.level != level) {
          warning.level = level;
          send_update = true;
        }
        send_update = true;
        found = true;
        break;
      }
    }

    if (!found) {
      SPDLOG_DEBUG("Adding warning {}", warning_text);
      WebsocketServer::_active_warnings.push_back({level, warning_text});
      send_update = true;
    }
  }

  if (send_update) {
    WebsocketServer::_send_active_warnings();
  }
}

void WebsocketServer::remove_warning(std::string warning_text) {
  bool send_update = false;
  {
    std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
    auto it = WebsocketServer::_active_warnings.begin();
    while (it != WebsocketServer::_active_warnings.end()) {
      if (it->warning_text.compare(warning_text) == 0) {
        SPDLOG_DEBUG("Removing warning {}", warning_text);
        it = WebsocketServer::_active_warnings.erase(it);

        send_update = true;
        break;
      } else {
        ++it;
      }
    }
  }

  if (send_update) {
    WebsocketServer::_send_active_warnings();
  }
}

void WebsocketServer::_send_active_warnings() {
  std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
  SPDLOG_DEBUG("Broadcasting active warnings to all connected weboscket clients.");
  nlohmann::json base;
  base["event_data"] = nlohmann::json::object();
  base["event_data"]["warnings"] = nlohmann::json::array();
  base["event_type"] = "mqttmanager_active_warnings";
  for (auto &warning : WebsocketServer::_active_warnings) {
    SPDLOG_DEBUG("Sending warnings, found warning: {}", warning.warning_text);
    base["event_data"]["warnings"].push_back({{"level", warning.level}, {"text", warning.warning_text}});
  }

  WebsocketServer::broadcast_json(base);
}
