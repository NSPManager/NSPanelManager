#ifndef MQTTMANAGER_WEBSOCKET_SERVER_HPP
#define MQTTMANAGER_WEBSOCKET_SERVER_HPP

#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>

class WebsocketServer {
public:
  static void start();

private:
  static inline std::list<ix::WebSocket> _connected_websockets;
  static inline ix::WebSocketServer *_server;

  static void _websocket_message_callback(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg);
};

#endif // !MQTTMANAGER_WEBSOCKET_SERVER_HPP
