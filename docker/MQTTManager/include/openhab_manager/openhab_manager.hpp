#ifndef MQTT_MANAGER_OPENHAB_MANAGER_HPP
#define MQTT_MANAGER_OPENHAB_MANAGER_HPP

#include <atomic>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/frame.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

class OpenhabManager {
public:
  static void connect();

  /*
   * Reload config values from database.
   */
  static void reload_config();

  static void send_json(nlohmann::json &data); // Helper to convert from JSON to std::string and then send

  /**
   * Attach an event listener to handle Openhab events.
   */
  template <typename CALLBACK_BIND>
  static void attach_event_observer(std::string item, CALLBACK_BIND callback) {
    SPDLOG_TRACE("Attaching observer for item '{}'", item);
    OpenhabManager::_openhab_item_observers[item].disconnect(callback); // Disconnect first in case it was already connected, otherwise multiple signals will be sent.
    OpenhabManager::_openhab_item_observers[item].connect(callback);

    std::string data;
    try {
      data = OpenhabManager::_fetch_item_state_via_rest(item);
      if (data.length() > 0) {
        nlohmann::json update_data;
        update_data["type"] = "ItemStateFetched";
        update_data["payload"] = nlohmann::json::parse(data);
        OpenhabManager::_openhab_item_observers[item](update_data);
      } else {
        SPDLOG_WARN("Failed to get current state for item '{}' via OpenHAB REST API. Current state is not available and will only be updated on next openhab state chage event..", item);
      }
    } catch (std::exception &e) {
      SPDLOG_ERROR("Caught exception during processing of openhab event. Diagnostic information: {}", boost::diagnostic_information(e, true));
      SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
    }
  }

  /**
   * Detach an event listener for Openhab events.
   */
  template <typename CALLBACK_BIND>
  static void detach_event_observer(std::string item, CALLBACK_BIND callback) {
    OpenhabManager::_openhab_item_observers[item].disconnect(callback);
  }

private:
  static void _process_openhab_event(nlohmann::json &event_data);
  // Callback registration for items
  static inline boost::ptr_map<std::string, boost::signals2::signal<void(nlohmann::json data)>> _openhab_item_observers;

  /**
   * Fetch an item state from OpenHAB via REST-api
   */
  static std::string _fetch_item_state_via_rest(std::string item);

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

  static inline std::atomic<bool> _authenticated = false;
  static inline std::atomic<bool> _connected = false;

  static inline std::mutex _setting_values_mutex; // Mutex to prevent simultaneous access to settings
  static inline std::string _openhab_address;     // Address of OpenHAB server
  static inline std::string _openhab_token;       // Access token to authenticate to OpenHAB

  static inline std::atomic<bool> _send_keepalive_messages; // Bool to be able to gracefully shutdown keepalive thread.
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_MANAGER_HPP
