#ifndef MQTT_MANAGER_HOMEY_MANAGER_HPP
#define MQTT_MANAGER_HOMEY_MANAGER_HPP

#include <atomic>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>

class HomeyManager
{
public:
    static void init(); // Start a new thread and connect to Homey.

    static void connect();

    /*
     * Reload config from DB and if needed, reconnect.
     */
    static void reload_config();

    static void send_json(nlohmann::json &data); // Helper to convert from JSON to std::string and then send

    /**
     * Attach an event listener to handle Homey device events.
     */
    template <typename CALLBACK_BIND>
    static void attach_event_observer(std::string device_id, CALLBACK_BIND callback)
    {
        HomeyManager::_homey_observers[device_id].disconnect(callback); // Disconnect first in case it was already connected, otherwise multiple signals will be sent.
        HomeyManager::_homey_observers[device_id].connect(callback);
    }

    /**
     * Detach an event listener for Homey device events.
     */
    template <typename CALLBACK_BIND>
    static void detach_event_observer(std::string device_id, CALLBACK_BIND callback)
    {
        HomeyManager::_homey_observers[device_id].disconnect(callback);
    }

private:
    static inline boost::ptr_map<std::string, boost::signals2::signal<void(nlohmann::json data)>> _homey_observers;
    static void _process_homey_event(nlohmann::json &event_data);

    static inline std::mutex _mutex_websocket_write_access; // Mutex to prevent simultaneous write access to WebSocket
    static void _send_string(std::string &data);            // Lowest level send function. Will handle mutex.

    static inline ix::WebSocket *_websocket = nullptr;
    static void _websocket_message_callback(const ix::WebSocketMessagePtr &msg);
    static void _process_websocket_message(const std::string &data);

    static inline std::atomic<bool> _connected = false;

    static inline std::mutex _settings_mutex;
    static inline std::string _homey_address;
    static inline std::string _homey_token;
};

#endif // !MQTT_MANAGER_HOMEY_MANAGER_HPP
