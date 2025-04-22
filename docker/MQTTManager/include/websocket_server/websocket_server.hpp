#ifndef MQTTMANAGER_WEBSOCKET_SERVER_HPP
#define MQTTMANAGER_WEBSOCKET_SERVER_HPP

#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>

class WebsocketServer {
public:
  /**
   * Start the websocket server and bind to 0.0.0.0:5002.
   */
  static void start();

  /**
   * Convert a JSON payload to a string and broadcast to all connected clients.
   */
  static void broadcast_json(nlohmann::json &json);

  /**
   * Send a message to all connected clients.
   */
  static void broadcast_string(std::string &data);

  /**
   * Attach a message callback. Callback should return true if the message was handled.
   * If any callback returns true, the data in the *reponse_buf will be sent back to the client that make the request.
   */
  static void attach_message_callback(std::function<bool(std::string &message, std::string *response_buf)> callback);

  enum ActiveWarningLevel {
    ERROR,
    WARNING,
    INFO,
    DEBUG
  };

  /*
   * Register a warning from the list of active warnings and send an update to all connected clients.
   */
  static void register_warning(ActiveWarningLevel level, std::string warning_text);

  /*
   * Remove a warning from the list of active warnings and send an update to all connected clients.
   */
  static void remove_warning(std::string warning_text);

private:
  struct ActiveWarning {
    ActiveWarningLevel level;
    std::string warning_text;
  };

  static inline std::list<ix::WebSocket> _connected_websockets;
  static inline ix::WebSocketServer *_server;
  static inline std::mutex _server_mutex;
  static inline std::list<std::function<bool(std::string &message, std::string *response_buf)>> _callbacks;
  static inline std::list<std::function<bool()>> _client_connect_callbacks;

  static inline std::mutex _active_warnings_mutex;
  static inline std::list<ActiveWarning> _active_warnings;

  static void _websocket_message_callback(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg);

  static void _send_active_warnings();
};

#endif // !MQTTMANAGER_WEBSOCKET_SERVER_HPP
