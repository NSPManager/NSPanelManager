#ifndef MQTTMANAGER_WEBSOCKET_SERVER_HPP
#define MQTTMANAGER_WEBSOCKET_SERVER_HPP

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>

struct StompFrame {
  enum MessageType {
    // Client commands
    CONNECT,
    DISCONNECT,
    SEND,
    SUBSCRIBE,
    UNSUBSCRIBE,
    ACK,
    NACK,

    // Server commands
    CONNECTED,
    MESSAGE,
    RECEIPT,
    ERROR,
  };

  MessageType type;
  std::unordered_map<std::string, std::string> headers;
  std::string body;
};

class StompTopic {
public:
  StompTopic(std::string topic_name, std::string current_value);

  std::string get_name() const;
  void subscribe(ix::WebSocket &websocket, std::string subscription_id);
  void unsubscribe(ix::WebSocket &websocket, std::string subscription_id);
  void update_value(std::string new_value);
  void update_value(nlohmann::json &new_value);
  void set_retained(bool retained);
  int get_subscriber_count() const;

protected:
  boost::uuids::random_generator _uuid_generator = boost::uuids::random_generator(); // Used to generate unique UUIDs for STOMP clients
  bool _retained = false;
  std::string _topic_name;
  std::string _current_value;
  nlohmann::json _current_json_data;
  std::list<std::pair<ix::WebSocket *, std::string>> _subscribers;
};

class WebsocketServer {
public:
  /**
   * Start the websocket server and bind to 0.0.0.0:5002.
   */
  static void start();

  /**
   * Update the value of a STOMP topic and send out the updated value to all connected clients.
   */
  static void update_stomp_topic_value(std::string topic_name, std::string value);

  /**
   * Update the value of a STOMP topic and send out the updated value to all connected clients.
   */
  static void update_stomp_topic_value(std::string topic_name, nlohmann::json &value);

  /**
   * Update a STOMP topic to retain or not to retain the value. Will create topic if it doesn't exist.
   */
  static void set_stomp_topic_retained(std::string topic_name, bool retain);

  /**
   * Decode a STOMP frame from a string.
   */
  static std::optional<StompFrame> decode_stomp_frame(std::string &data);

  /**
   * Send a STOMP frame to a specific websocket client.
   */
  static void send_stomp_frame(StompFrame &frame, ix::WebSocket &websocket);

  /**
   * Attach a callback to be called when a message is received on a specific topic using STOMP.
   */
  template <typename CALLBACK_BIND>
  static void attach_stomp_callback(std::string topic, CALLBACK_BIND callback) {
    std::lock_guard<std::mutex> mutex_guard(WebsocketServer::_on_stomp_send_message_callbacks_mutex);
    WebsocketServer::_on_stomp_send_message_callbacks[topic].disconnect(callback); // First disconnect in case it was already connected to avaid duplicate callbacks
    WebsocketServer::_on_stomp_send_message_callbacks[topic].connect(callback);
  }

  template <typename CALLBACK_BIND>
  static void detach_stomp_callback(std::string topic, CALLBACK_BIND callback) {
    std::lock_guard<std::mutex> mutex_guard(WebsocketServer::_on_stomp_send_message_callbacks_mutex);
    WebsocketServer::_on_stomp_send_message_callbacks[topic].disconnect(callback);

    if (WebsocketServer::_on_stomp_send_message_callbacks[topic].empty()) {
      WebsocketServer::_on_stomp_send_message_callbacks.erase(topic);
    }
  }

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

  static inline ix::WebSocketServer *_server;
  static inline std::mutex _server_mutex;
  static inline std::list<std::function<bool(std::string &message, std::string *response_buf)>> _callbacks;

  static inline boost::uuids::random_generator _uuid_generator = boost::uuids::random_generator(); // Used to generate unique UUIDs for STOMP clients
  static inline std::vector<StompTopic> _stomp_topics;

  static inline std::list<ix::WebSocket *> _connected_websockets_stomps;

  // Callback for when a SEND message is received on a STOMP topic
  static inline std::mutex _on_stomp_send_message_callbacks_mutex;
  static inline boost::ptr_map<std::string, boost::signals2::signal<void(StompFrame)>> _on_stomp_send_message_callbacks;

  static inline std::mutex _active_warnings_mutex;
  static inline std::list<ActiveWarning> _active_warnings;

  static void _websocket_message_callback(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg);

  static void _stomp_dismiss_warning_callback(StompFrame frame);
  static void _send_active_warnings();
};

#endif // !MQTTMANAGER_WEBSOCKET_SERVER_HPP
