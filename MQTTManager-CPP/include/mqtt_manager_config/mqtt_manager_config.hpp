#ifndef MQTTMANAGER_CONFIG_HPP
#define MQTTMANAGER_CONFIG_HPP
#include <list>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

#define MANAGER_ADDRESS "10.0.0.10"
#define MANAGER_PORT "8000"

enum LIGHT_TURN_ON_BEHAVIOR {
  RESTORE,
  COLOR_TEMP
};

class MqttManagerConfig {
public:
  static void load();

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

  static inline bool clock_us_style;
  static inline bool use_farenheit;
  static inline bool turn_on_behavior;

  static inline std::list<nlohmann::json> light_configs;
  static inline std::list<nlohmann::json> nspanel_configs;

private:
  static void populate_settings_from_config(nlohmann::json &data);
};

#endif // !MQTTMANAGER_CONFIG_HPP
