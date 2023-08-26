#ifndef MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>

class HomeAssistantEventObserver {
public:
  /**
   * Process event data. If the event was processed by the given instance, return true, else return false.
   * In case a false is returned the loop will continue until all registered entities has been checked.
   */
  virtual bool home_assistant_event_callback(nlohmann::json &event_data) = 0;
};

class HomeAssistantManager {
public:
  static void connect();
  static void send_json(nlohmann::json &data); // Helper to convert from JSON to std::string and then send

  /**
   * Attach an event listener to handle Home Assistant events.
   */
  static void attach_event_observer(HomeAssistantEventObserver *observer);

  /**
   * Detach an event listener for Home Assistant events.
   */
  static void detach_event_observer(HomeAssistantEventObserver *observer);

private:
  static inline std::list<HomeAssistantEventObserver *> _home_assistant_event_observers;
  static void _process_home_assistant_event(nlohmann::json &event_data);

  static inline std::mutex _mutex_websocket_write_access; // Mutex to prevent simultaneous write access to WebSocket
  static void _send_auth();                               // Send login information when authentication is required.
  static void _send_string(std::string &data);            // Lowest level send function. Will handle mutex.

  static inline ix::WebSocket *_websocket = nullptr;
  static void _websocket_message_callback(const ix::WebSocketMessagePtr &msg);
  static void _process_websocket_message(const std::string &data);

  static inline bool _authenticated = false;
  static inline long _next_message_id = 1;
  static inline long _all_statues_request_message_id;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP
