#ifndef MQTT_MANAGER_OPENHAB_MANAGER_HPP
#define MQTT_MANAGER_OPENHAB_MANAGER_HPP

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>

class OpenhabEventObserver {
public:
  /**
   * Process event data. If the event was processed by the given instance, return true, else return false.
   * In case a false is returned the loop will continue until all registered entities has been checked.
   */
  virtual bool openhab_event_callback(nlohmann::json &event_data) = 0;
};

class OpenhabManager {
public:
  static void connect();
  static void send_json(nlohmann::json &data); // Helper to convert from JSON to std::string and then send

  /**
   * Attach an event listener to handle Openhab events.
   */
  static void attach_event_observer(OpenhabEventObserver *observer);

  /**
   * Detach an event listener for Openhab events.
   */
  static void detach_event_observer(OpenhabEventObserver *observer);

private:
  static inline std::list<OpenhabEventObserver *> _openhab_event_observers;
  static void _process_openhab_event(nlohmann::json &event_data);

  /**
   * Fetch all current states for registered entities from the OpenHAB REST api.
   */
  static void _fetch_and_update_current_states();

  /**
   * Send keepalive to OpenHAB to keep websocket connection open.
   */
  static void _send_keepalive();
  static inline std::thread _keepalive_thread;

  static inline std::mutex _mutex_websocket_write_access; // Mutex to prevent simultaneous write access to WebSocket
  static void _send_string(std::string &data);            // Lowest level send function. Will handle mutex.

  static inline ix::WebSocket *_websocket = nullptr;
  static void _websocket_message_callback(const ix::WebSocketMessagePtr &msg);
  static void _process_websocket_message(const std::string &data);

  static inline bool _authenticated = false;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP
