#include <fmt/core.h>
#include <fstream>
#include <inja/inja.hpp>
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
      SPDLOG_DEBUG("New websocket client at IP: {}", connectionState->getRemoteIp());
    } else if (msg->type == ix::WebSocketMessageType::Close) {
      SPDLOG_DEBUG("Websocket closed to client at IP: {}", connectionState->getRemoteIp());
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      SPDLOG_ERROR("Websocket error to client at IP: {}", connectionState->getRemoteIp());
    } else if (msg->type == ix::WebSocketMessageType::Fragment) {
      SPDLOG_ERROR("Received websocket fragment from client at IP: {}. Websocket fragments are not suported.", connectionState->getRemoteIp());
    } else if (msg->type == ix::WebSocketMessageType::Message) {
      std::string message = msg->str;
      SPDLOG_DEBUG("Got message from websocket. Message: {}", message);
      std::string response_buffer;
      for (auto callback : WebsocketServer::_callbacks) {
        if (callback(message, &response_buffer)) {
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

void WebsocketServer::render_template_with_args(std::string template_name, nlohmann::json data) {
  try {
    // Read content from template file
    std::string template_path = fmt::format("../templates/{}", template_name);
    std::ifstream inFile(template_path);
    if (!inFile.good()) {
      SPDLOG_ERROR("Failed to open template '{}'.", template_path);
      return;
    }

    std::string template_data = std::string((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    std::string rendered_template = inja::render(template_data, data);
    WebsocketServer::broadcast_string(rendered_template);
  } catch (std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while rendering inja template 'templates/{}'. Exception: {}", template_name, ex.what());
  } catch (...) {
    SPDLOG_ERROR("Caught exception while rendering inja template 'templates/{}'.", template_name);
  }
}
