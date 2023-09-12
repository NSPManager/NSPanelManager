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
  SPDLOG_DEBUG("New websocket client at IP: {}", connectionState->getRemoteIp());
  if (msg->type == ix::WebSocketMessageType::Open) {

    // A connection state object is available, and has a default id
    // You can subclass ConnectionState and pass an alternate factory
    // to override it. It is useful if you want to store custom
    // attributes per connection (authenticated bool flag, attributes, etc...)
    SPDLOG_DEBUG("Client ID: {}", connectionState->getId());
  } else if (msg->type == ix::WebSocketMessageType::Message) {
    // For an echo server, we just send back to the client whatever was received by the server
    // All connected clients are available in an std::set. See the broadcast cpp example.
    // Second parameter tells whether we are sending the message in binary or text mode.
    // Here we send it in the same mode as it was received.
    SPDLOG_DEBUG("Received: {}", msg->str);

    webSocket.send(msg->str, msg->binary);
  }
}
