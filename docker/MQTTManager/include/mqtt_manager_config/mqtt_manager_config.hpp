#ifndef MQTTMANAGER_CONFIG_HPP
#define MQTTMANAGER_CONFIG_HPP
#include "protobuf_general.pb.h"
#include <boost/signals2.hpp>
#include <list>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/protobuf_mqttmanager.pb.h>
#include <string>
#include <vector>

#define MANAGER_ADDRESS "127.0.0.1"
#define MANAGER_PORT "8000"

enum LightTurnOnBehaviour {
  COLOR_TEMPERATURE,
  RESTORE_PREVIOUS,
};

struct MqttManagerSettingsHolder {
  std::string mqtt_server = "";
  uint16_t mqtt_server_port = 1883;

  uint32_t color_temp_min = 0;
  uint32_t color_temp_max = 0;
  bool reverse_color_temperature_slider = false;
  std::string date_format = "";
  bool clock_24_hour_format = true;
  uint16_t max_log_buffer_size = 100;
  uint16_t manager_port = 8000;
  std::string manager_address = "";
  bool is_home_assistant_addon = false; // Are we running as standalone docker or as an Home Assistant addon?

  bool optimistic_mode;    // Should we assume the values we set on lights are correct or do we wait for confirmation of change.
  uint16_t mqtt_wait_time; // For how long should panels ignore MQTT feedback after change. TODO: Is this used?

  LightTurnOnBehaviour light_turn_on_behaviour; // When turning a light on, should we restore previous setting or assume the user wants color temperature?
};

class MqttManagerConfig {
public:
  static void load();
  static MqttManagerSettingsHolder get_settings();
  static inline std::string timezone;

  static inline nlohmann::json icon_mapping;

  static std::string get_setting_with_default(std::string key, std::string default_value);

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
  // Mutex to make sure only one thread can load settings at the time
  static inline std::mutex _config_load_mutex;

  static inline boost::signals2::signal<void()> _config_loaded_listeners;

  static inline std::mutex _settings_mutex;
  static inline MqttManagerSettingsHolder _settings;
};

#endif // !MQTTMANAGER_CONFIG_HPP
