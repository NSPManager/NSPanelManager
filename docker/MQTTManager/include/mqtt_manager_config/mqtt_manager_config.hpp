#ifndef MQTTMANAGER_CONFIG_HPP
#define MQTTMANAGER_CONFIG_HPP
#include "light/light.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/signals2.hpp>
#include <database_manager/database_manager.hpp>
#include <expected>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/protobuf_mqttmanager.pb.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

#define MANAGER_ADDRESS "127.0.0.1"
#define MANAGER_PORT "8000"

enum LightTurnOnBehaviour {
  COLOR_TEMPERATURE,
  RESTORE_PREVIOUS,
};

class MqttManagerConfig {
public:
  static void load();
  static inline std::string timezone;

  template <typename T>
  static T get_setting_with_default(std::string key);

  /**
   * Set the value of a setting key in the DB and update the in-memory cache.
   */
  static void set_setting_value(std::string key, std::string value);

  /**
   * Set a value for a NSPanel setting.
   */
  static void set_nspanel_setting_value(int32_t nspanel_id, std::string key, std::string default_value);

  /**
   * Check if we are running in Home Assistant addon mode. If so, return true.
   */
  static bool is_home_assistant_addon();

  /*
   * Get the light turn on behaviour.
   */
  static LightTurnOnBehaviour get_light_turn_on_behaviour();

  static void update_firmware_checksum();
  static void update_tft_checksums();

  static std::expected<std::string, bool> get_firmware_checksum();
  static std::expected<std::string, bool> get_littlefs_checksum();
  static std::expected<std::string, bool> get_eu_tft1_checksum();
  static std::expected<std::string, bool> get_eu_tft2_checksum();
  static std::expected<std::string, bool> get_eu_tft3_checksum();
  static std::expected<std::string, bool> get_eu_tft4_checksum();
  static std::expected<std::string, bool> get_us_tft1_checksum();
  static std::expected<std::string, bool> get_us_tft2_checksum();
  static std::expected<std::string, bool> get_us_tft3_checksum();
  static std::expected<std::string, bool> get_us_tft4_checksum();
  static std::expected<std::string, bool> get_us_horizontal_mirrored_tft1_checksum();
  static std::expected<std::string, bool> get_us_horizontal_mirrored_tft2_checksum();
  static std::expected<std::string, bool> get_us_horizontal_mirrored_tft3_checksum();
  static std::expected<std::string, bool> get_us_horizontal_mirrored_tft4_checksum();

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

  static inline std::mutex _database_access_mutex; // Mutex to only allow one thread to access the "get_setting_with_default" database function as the same time.

  static inline boost::signals2::signal<void()> _config_loaded_listeners;

  static inline std::mutex _settings_mutex;

  // True if we are currently running as a Home Assistant addon
  static inline bool _is_home_assistant_addon = false;

  static inline LightTurnOnBehaviour _light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;

  static std::optional<std::string> _get_file_md5_checksum(std::string file_path);
  static inline std::mutex _md5_checksum_files_mutex;
  static inline std::string _md5_checksum_firmware;
  static inline std::string _md5_checksum_littlefs;
  static inline std::string _md5_checksum_eu_tft1;
  static inline std::string _md5_checksum_eu_tft2;
  static inline std::string _md5_checksum_eu_tft3;
  static inline std::string _md5_checksum_eu_tft4;
  static inline std::string _md5_checksum_us_tft1;
  static inline std::string _md5_checksum_us_tft2;
  static inline std::string _md5_checksum_us_tft3;
  static inline std::string _md5_checksum_us_tft4;
  static inline std::string _md5_checksum_us_horizontal_mirrored_tft1;
  static inline std::string _md5_checksum_us_horizontal_mirrored_tft2;
  static inline std::string _md5_checksum_us_horizontal_mirrored_tft3;
  static inline std::string _md5_checksum_us_horizontal_mirrored_tft4;

  static inline std::unordered_map<std::string, std::string> _settings_values_cache;
  static inline std::unordered_map<std::string, std::string> _default_settings_values = {
      {"is_home_assistant_addon", "false"},
      {"optimistic_mode", "false"},
      {"mqtt_wait_time", "0"},
      {"light_turn_on_behaviour", "restore"},
      {"button_long_press_time", "5000"},
      {"clock_us_style", "False"},
      {"color_temp_max", "6000"},
      {"color_temp_min", "2000"},
      {"theme", "default"},
      {"date_format", "%a %d/%m %Y"},
      {"home_assistant_address", ""},
      {"home_assistant_token", ""},
      {"location_latitude", ""},
      {"location_longitude", ""},
      {"manager_address", ""},
      {"manager_port", ""},
      {"max_live_log_messages", "10"},
      {"max_log_buffer_size", "10"},
      {"min_button_push_time", "50"},
      {"mqttmanager_log_level", "debug"},
      {"mqtt_password", ""},
      {"mqtt_port", "1883"},
      {"mqtt_server", ""},
      {"mqtt_username", ""},
      {"openhab_address", ""},
      {"openhab_token", ""},
      {"openhab_brightness_channel_max", "255"},
      {"openhab_brightness_channel_min", "0"},
      {"openhab_brightness_channel_name", ""},
      {"openhab_color_temp_channel_name", ""},
      {"openhab_rgb_channel_name", ""},
      {"outside_temp_sensor_entity_id", ""},
      {"outside_temp_sensor_provider", ""},
      {"weather_precipitation_format", "mm"},
      {"raise_to_100_light_level", "95"},
      {"all_rooms_status_backoff_time", "250"},
      {"reverse_color_temp", "False"},
      {"screen_dim_level", "100"},
      {"screensaver_activation_timeout", "30000"},
      {"screensaver_dim_level", "1"},
      {"screensaver_mode", "with_background"},
      {"show_screensaver_inside_temperature", "True"},
      {"show_screensaver_outside_temperature", "True"},
      {"special_mode_release_time", "5000"},
      {"special_mode_trigger_time", "300"},
      {"turn_on_behavior", "color_temp"},
      {"use_fahrenheit", "False"},
      {"weather_update_interval", "10"},
      {"weather_wind_speed_format", "kmh"},
      {"mqtt_wait_time", "1000"},
      {"optimistic_mode", "True"},
      {"light_turn_on_brightness", "50"},
      {"default_nspanel_type", "eu"}

  };
};

template <>
inline std::string MqttManagerConfig::get_setting_with_default(std::string key) {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_database_access_mutex);
  if (MqttManagerConfig::_settings_values_cache.contains(key)) {
    return MqttManagerConfig::_settings_values_cache[key];
  }

  try {
    auto result = database_manager::database.get_all<database_manager::SettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::SettingHolder::name) == key));
    if (result.size() > 0) [[likely]] {
      SPDLOG_TRACE("Found setting {} with value {}", key, result[0].value);
      MqttManagerConfig::_settings_values_cache[key] = result[0].value;
      return result[0].value;
    } else {
      SPDLOG_TRACE("Did not find setting {} in database. Looking for default value.", key);

      if (MqttManagerConfig::_default_settings_values.contains(key)) {
        SPDLOG_TRACE("Returning default value '{}' for setting '{}'", key, MqttManagerConfig::_default_settings_values[key]);
        return MqttManagerConfig::_default_settings_values[key];
      } else {
        SPDLOG_ERROR("Did not find default setting value for key '{}'. Returning empty string.", key);
        return "";
      }
    }
  } catch (std::exception &ex) {
    SPDLOG_ERROR("Caught exception while trying to access database to retrieve setting {}. Exception: {}", key, boost::diagnostic_information(ex));
  }
  SPDLOG_TRACE("Did not find setting {}. Returning empty string.", key);
  return "";
}

template <>
inline bool MqttManagerConfig::get_setting_with_default(std::string key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  boost::to_lower(value);
  return value.compare("true") == 0;
}

template <>
inline int32_t MqttManagerConfig::get_setting_with_default(std::string key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  try {
    return std::stoi(value);
  } catch (std::invalid_argument &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to integer. Returning 0. Exception: {}", boost::diagnostic_information(ex));
    return 0;
  } catch (std::out_of_range &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to integer. Returning 0. Exception: {}", boost::diagnostic_information(ex));
    return 0;
  }
}

template <>
inline uint32_t MqttManagerConfig::get_setting_with_default(std::string key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  try {
    return std::stoul(value);
  } catch (std::invalid_argument &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to unsigned integer. Returning 0. Exception: {}", boost::diagnostic_information(ex));
    return 0;
  } catch (std::out_of_range &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to unsigned integer. Returning 0. Exception: {}", boost::diagnostic_information(ex));
    return 0;
  }
}

template <>
inline float MqttManagerConfig::get_setting_with_default(std::string key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  try {
    return std::stof(value);
  } catch (std::invalid_argument &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to float. Returning 0.0f. Exception: {}", boost::diagnostic_information(ex));
    return 0.0f;
  } catch (std::out_of_range &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to float. Returning 0.0f. Exception: {}", boost::diagnostic_information(ex));
    return 0.0f;
  }
}

#endif // !MQTTMANAGER_CONFIG_HPP
