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

  /**
   * Will read a template with a specified name and render it given the data and then send it out over the websocket.
   * This function is mostly helpful together with HTMX on the frontend.
   */
  static void render_template_with_args(std::string template_name, nlohmann::json data);

private:
  static inline std::list<ix::WebSocket> _connected_websockets;
  static inline ix::WebSocketServer *_server;
  static inline std::mutex _server_mutex;
  static inline std::list<std::function<bool(std::string &message, std::string *response_buf)>> _callbacks;
  static inline std::list<std::function<bool()>> _client_connect_callbacks;

  static void _websocket_message_callback(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg);
};

#endif // !MQTTMANAGER_WEBSOCKET_SERVER_HPP
