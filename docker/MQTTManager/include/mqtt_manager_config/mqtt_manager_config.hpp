#ifndef MQTTMANAGER_CONFIG_HPP
#define MQTTMANAGER_CONFIG_HPP
#include <boost/signals2.hpp>
#include <list>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

#define MANAGER_ADDRESS "127.0.0.1"
#define MANAGER_PORT "8000"

enum LIGHT_TURN_ON_BEHAVIOR {
  RESTORE,
  COLOR_TEMP
};

class MqttManagerConfig {
public:
  static void load();

  static inline std::string manager_address;
  static inline uint16_t manager_port;

  static inline std::string mqtt_server;
  static inline uint mqtt_port = 1883;
  static inline std::string mqtt_username;
  static inline std::string mqtt_password;

  static inline std::string home_assistant_address;
  static inline std::string home_assistant_access_token;

  static inline std::string openhab_address;
  static inline std::string openhab_access_token;

  static inline uint color_temp_min;
  static inline uint color_temp_max;

  static inline bool is_home_assistant_addon;
  static inline std::string date_format;
  static inline std::string weather_controller;
  static inline std::string home_assistant_weather_entity;
  static inline std::string openhab_current_weather_item;
  static inline std::string openhab_forcast_weather_item;
  static inline nlohmann::json icon_mapping;
  static inline bool clock_us_style;
  static inline bool use_farenheit;
  static inline bool turn_on_behavior;
  static inline uint16_t max_log_buffer_size;

  static inline std::list<nlohmann::json> light_configs;
  static inline std::list<nlohmann::json> nspanel_configs;
  static inline std::list<nlohmann::json> scenes_configs;
  static inline std::list<nlohmann::json> nspanel_relay_group_configs;
  static inline std::list<nlohmann::json> room_configs;

  /**
   * Attach a callback to be called when a new configuration has been added
   */
  static void attach_config_added_listener(void (*callback)(nlohmann::json *));

  /**
   * Detach a callback to be called when a new configuration has been added
   */
  static void dettach_config_added_listener(void (*callback)(nlohmann::json *));

  /**
   * Attach a callback to be called when existing configuration is being removed
   */
  static void attach_config_removed_listener(void (*callback)(nlohmann::json *));

  /**
   * Detach a callback to be called when existing configuration is being removed
   */
  static void dettach_config_removed_listener(void (*callback)(nlohmann::json *));

  /**
   * Attach a callback to be called when config has been fully loaded.
   */
  static void attach_config_loaded_listener(void (*callback)());

  /**
   * Detach a callback to be called when config has been fully loaded.
   */
  static void dettach_config_loaded_listener(void (*callback)());

private:
  static void populate_settings_from_config(nlohmann::json &data);

  static inline boost::signals2::signal<void(nlohmann::json *)> _config_added_listener;
  static inline boost::signals2::signal<void(nlohmann::json *)> _config_removed_listener;
  static inline boost::signals2::signal<void()> _config_loaded_listeners;
};

#endif // !MQTTMANAGER_CONFIG_HPP
