#ifndef MQTTMANAGER_CONFIG_HPP
#define MQTTMANAGER_CONFIG_HPP
#include <boost/signals2.hpp>
#include <list>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/mqttmanager.pb.h>
#include <string>

#define MANAGER_ADDRESS "127.0.0.1"
#define MANAGER_PORT "8000"

class MqttManagerConfig {
public:
  static void load();
  static MQTTManagerSettings get_settings();
  static MQTTManagerPrivateSettings get_private_settings();
  static inline std::string timezone;

  static inline nlohmann::json icon_mapping;
  static inline std::list<nlohmann::json> light_configs;
  static inline std::list<nlohmann::json> nspanel_configs;
  static inline std::list<nlohmann::json> scenes_configs;
  static inline std::list<nlohmann::json> nspanel_relay_group_configs;
  static inline std::list<nlohmann::json> room_configs;

  /**
   * Attach a callback to be called when a new configuration has been added
   */
  template <typename CALLBACK_BIND>
  static void attach_config_added_listener(CALLBACK_BIND callback) {
    MqttManagerConfig::_config_added_listener.connect(callback);
  }

  /**
   * Detach a callback to be called when a new configuration has been added
   */
  template <typename CALLBACK_BIND>
  static void dettach_config_added_listener(CALLBACK_BIND callback) {
    MqttManagerConfig::_config_added_listener.disconnect(callback);
  }

  /**
   * Attach a callback to be called when existing configuration is being removed
   */
  template <typename CALLBACK_BIND>
  static void attach_config_removed_listener(CALLBACK_BIND callback) {
    MqttManagerConfig::_config_removed_listener.connect(callback);
  }

  /**
   * Detach a callback to be called when existing configuration is being removed
   */
  template <typename CALLBACK_BIND>
  static void dettach_config_removed_listener(CALLBACK_BIND callback) {
    MqttManagerConfig::_config_removed_listener.disconnect(callback);
  }

  /**
   * Attach a callback to be called when config has been fully loaded.
   */
  template <typename CALLBACK_BIND>
  static void attach_config_loaded_listener(CALLBACK_BIND callback) {
    MqttManagerConfig::_config_loaded_listeners.connect(callback);
  }

  /**
   * Detach a callback to be called when config has been fully loaded.
   */
  template <typename CALLBACK_BIND>
  static void dettach_config_loaded_listener(CALLBACK_BIND callback) {
    MqttManagerConfig::_config_loaded_listeners.disconnect(callback);
  }

private:
  static void populate_settings_from_config(nlohmann::json &data);
  // Mutex to make sure only one thread can load settings at the time
  static inline std::mutex _config_load_mutex;

  static inline boost::signals2::signal<void(nlohmann::json *)> _config_added_listener;
  static inline boost::signals2::signal<void(nlohmann::json *)> _config_removed_listener;
  static inline boost::signals2::signal<void()> _config_loaded_listeners;

  static inline MQTTManagerSettings _settings;
  static inline MQTTManagerPrivateSettings _private_settings;
};

#endif // !MQTTMANAGER_CONFIG_HPP
