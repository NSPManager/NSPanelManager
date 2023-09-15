#include <ixwebsocket/IXWebSocketMessageType.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <spdlog/spdlog.h>
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
      SPDLOG_DEBUG("Websocket error to client at IP: {}", connectionState->getRemoteIp());
    } else if (msg->type == ix::WebSocketMessageType::Fragment) {
      SPDLOG_ERROR("Received websocket fragment from client at IP: {}. Websocket fragments are not suported.", connectionState->getRemoteIp());
    } else if (msg->type == ix::WebSocketMessageType::Message) {
      std::string message = msg->str;
      std::string response_buffer;
      bool found_callback = false;
      for (auto callback : WebsocketServer::_callbacks) {
        if (callback(message, &response_buffer)) {
          found_callback = true;
          break;
        }
      }
      if (found_callback) {
        if (!response_buffer.empty()) {
          webSocket.send(response_buffer);
        }
      } else {
        SPDLOG_ERROR("Received message on websocket that was unhandled. Message: {}", message);
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
  for (auto websocket : WebsocketServer::_server->getClients()) {
    websocket->send(data);
  }
}

void WebsocketServer::attach_message_callback(std::function<bool(std::string &message, std::string *response_buf)> callback) {
  WebsocketServer::_callbacks.push_back(callback);
}
