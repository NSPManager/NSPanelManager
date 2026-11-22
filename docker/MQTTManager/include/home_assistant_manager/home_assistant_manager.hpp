#ifndef MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP

#include <atomic>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>

class HomeAssistantManager {
public:
  static void init(); // Start a new thread and connect to HA.

  static void connect();

  /*
   * Reload config from DB and if needed, reconnect.
   */
  static void reload_config();

  static void send_json(nlohmann::json &data); // Helper to convert from JSON to std::string and then send

  /**
   * Attach an event listener to handle Openhab events.
   */
  template <typename CALLBACK_BIND>
  static void attach_event_observer(std::string item, CALLBACK_BIND callback) {
    HomeAssistantManager::_home_assistant_observers[item].disconnect(callback); // Disconnect first in case it was already connected, otherwise multiple signals will be sent.
    HomeAssistantManager::_home_assistant_observers[item].connect(callback);
    HomeAssistantManager::_request_all_states();
  }

  /**
   * Detach an event listener for Openhab events.
   */
  template <typename CALLBACK_BIND>
  static void detach_event_observer(std::string item, CALLBACK_BIND callback) {
    HomeAssistantManager::_home_assistant_observers[item].disconnect(callback);
  }

private:
  // Request all states that Home Assistant has as there is no way to only request state of one object.
  static void _request_all_states();

  static inline boost::ptr_map<std::string, boost::signals2::signal<void(nlohmann::json data)>> _home_assistant_observers;
  static void _process_home_assistant_event(nlohmann::json &event_data);

  static inline std::mutex _mutex_websocket_write_access; // Mutex to prevent simultaneous write access to WebSocket
  static void _send_auth();                               // Send login information when authentication is required.
  static void _send_string(std::string &data);            // Lowest level send function. Will handle mutex.

  static inline ix::WebSocket *_websocket = nullptr;
  static void _websocket_message_callback(const ix::WebSocketMessagePtr &msg);
  static void _process_websocket_message(const std::string &data);

  static inline std::atomic<bool> _authenticated = false;
  static inline std::atomic<bool> _connected = false;
  static inline std::atomic<long> _next_message_id = 1;
  static inline std::atomic<long> _all_statues_request_message_id;

  static inline std::mutex _settings_mutex;
  static inline std::string _home_assistant_address;
  static inline std::string _home_assistant_token;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP
