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

#if defined(TEST_MODE) && TEST_MODE == 1
#include <gtest/gtest.h>
#endif

enum LightTurnOnBehaviour {
  COLOR_TEMPERATURE,
  RESTORE_PREVIOUS,
};

enum MQTT_MANAGER_SETTING
{
  BUTTON_LONG_PRESS_TIME,
  CLOCK_US_STYLE,
  COLOR_TEMP_MAX,
  COLOR_TEMP_MIN,
  THEME,
  DATE_FORMAT,
  IS_HOME_ASSISTANT_ADDON,
  HOME_ASSISTANT_ADDRESS,
  HOME_ASSISTANT_TOKEN,
  HOMEY_ADDRESS,
  HOMEY_TOKEN,
  LOCATION_LATITUDE,
  LOCATION_LONGITUDE,
  MANAGER_ADDRESS,
  MANAGER_PORT,
  MAX_LIVE_LOG_MESSAGES,
  MAX_LOG_BUFFER_SIZE,
  MIN_BUTTON_PUSH_TIME,
  MQTTMANAGER_LOG_LEVEL,
  MQTT_PASSWORD,
  MQTT_PORT,
  MQTT_SERVER,
  MQTT_USERNAME,
  OPENHAB_ADDRESS,
  OPENHAB_TOKEN,
  OPENHAB_BRIGHTNESS_CHANNEL_MAX,
  OPENHAB_BRIGHTNESS_CHANNEL_MIN,
  OPENHAB_BRIGHTNESS_CHANNEL_NAME,
  OPENHAB_COLOR_TEMP_CHANNEL_NAME,
  OPENHAB_RGB_CHANNEL_NAME,
  OUTSIDE_TEMP_SENSOR_ENTITY_ID,
  OUTSIDE_TEMP_SENSOR_PROVIDER,
  WEATHER_PRECIPITATION_FORMAT,
  RAISE_TO_100_LIGHT_LEVEL,
  ALL_ROOMS_STATUS_BACKOFF_TIME,
  REVERSE_COLOR_TEMP,
  SCREEN_DIM_LEVEL,
  SCREENSAVER_ACTIVATION_TIMEOUT,
  SCREENSAVER_DIM_LEVEL,
  SCREENSAVER_MODE,
  SHOW_SCREENSAVER_INSIDE_TEMPERATURE,
  SHOW_SCREENSAVER_OUTSIDE_TEMPERATURE,
  SPECIAL_MODE_RELEASE_TIME,
  SPECIAL_MODE_TRIGGER_TIME,
  TURN_ON_BEHAVIOR,
  USE_FAHRENHEIT,
  WEATHER_UPDATE_INTERVAL,
  WEATHER_WIND_SPEED_FORMAT,
  MQTT_WAIT_TIME,
  OPTIMISTIC_MODE,
  LIGHT_TURN_ON_BRIGHTNESS,
  DEFAULT_NSPANEL_TYPE,
  LAST, // Keep last as to be able to reference the last element in the list.
};

class MqttManagerConfig {
public:
  // Initialize this static library with settings and configuration options
  static void load();

  // Populate the database with default values in case they are not already set
  // and clean up any invalid or outdated settings.
  static void populate_default_and_clean();

  static inline std::string timezone;

  template <typename T>
  static T get_setting_with_default(MQTT_MANAGER_SETTING key);

  /**
   * Set the value of a setting key in the DB and update the in-memory cache.
   */
  static void set_setting_value(MQTT_MANAGER_SETTING key, std::string value);

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

  static inline std::unordered_map<MQTT_MANAGER_SETTING, std::string> _settings_values_cache;

  // Map of SETTING key that points to a pair of two strings. Pair.first is database key as string, Pair.second is default value.
  static inline std::unordered_map<MQTT_MANAGER_SETTING, std::pair<std::string, std::string>> _setting_key_map = {
      {MQTT_MANAGER_SETTING::IS_HOME_ASSISTANT_ADDON, {"is_home_assistant_addon", "false"}},
      {MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, {"optimistic_mode", "false"}},
      {MQTT_MANAGER_SETTING::MQTT_WAIT_TIME, {"mqtt_wait_time", "0"}},
      {MQTT_MANAGER_SETTING::BUTTON_LONG_PRESS_TIME, {"button_long_press_time", "5000"}},
      {MQTT_MANAGER_SETTING::CLOCK_US_STYLE, {"clock_us_style", "False"}},
      {MQTT_MANAGER_SETTING::COLOR_TEMP_MAX, {"color_temp_max", "6000"}},
      {MQTT_MANAGER_SETTING::COLOR_TEMP_MIN, {"color_temp_min", "2000"}},
      {MQTT_MANAGER_SETTING::THEME, {"theme", "default"}},
      {MQTT_MANAGER_SETTING::DATE_FORMAT, {"date_format", "%a %d/%m %Y"}},
      {MQTT_MANAGER_SETTING::HOME_ASSISTANT_ADDRESS, {"home_assistant_address", ""}},
      {MQTT_MANAGER_SETTING::HOME_ASSISTANT_TOKEN, {"home_assistant_token", ""}},
      {MQTT_MANAGER_SETTING::HOMEY_ADDRESS, {"homey_address", ""}},
      {MQTT_MANAGER_SETTING::HOMEY_TOKEN, {"homey_token", ""}},
      {MQTT_MANAGER_SETTING::LOCATION_LATITUDE, {"location_latitude", ""}},
      {MQTT_MANAGER_SETTING::LOCATION_LONGITUDE, {"location_longitude", ""}},
      {MQTT_MANAGER_SETTING::MANAGER_ADDRESS, {"manager_address", ""}},
      {MQTT_MANAGER_SETTING::MANAGER_PORT, {"manager_port", ""}},
      {MQTT_MANAGER_SETTING::MAX_LIVE_LOG_MESSAGES, {"max_live_log_messages", "10"}},
      {MQTT_MANAGER_SETTING::MAX_LOG_BUFFER_SIZE, {"max_log_buffer_size", "10"}},
      {MQTT_MANAGER_SETTING::MIN_BUTTON_PUSH_TIME, {"min_button_push_time", "50"}},
      {MQTT_MANAGER_SETTING::MQTTMANAGER_LOG_LEVEL, {"mqttmanager_log_level", "debug"}},
      {MQTT_MANAGER_SETTING::MQTT_PASSWORD, {"mqtt_password", ""}},
      {MQTT_MANAGER_SETTING::MQTT_PORT, {"mqtt_port", "1883"}},
      {MQTT_MANAGER_SETTING::MQTT_SERVER, {"mqtt_server", ""}},
      {MQTT_MANAGER_SETTING::MQTT_USERNAME, {"mqtt_username", ""}},
      {MQTT_MANAGER_SETTING::OPENHAB_ADDRESS, {"openhab_address", ""}},
      {MQTT_MANAGER_SETTING::OPENHAB_TOKEN, {"openhab_token", ""}},
      {MQTT_MANAGER_SETTING::OPENHAB_BRIGHTNESS_CHANNEL_MAX, {"openhab_brightness_channel_max", "255"}},
      {MQTT_MANAGER_SETTING::OPENHAB_BRIGHTNESS_CHANNEL_MIN, {"openhab_brightness_channel_min", "0"}},
      {MQTT_MANAGER_SETTING::OUTSIDE_TEMP_SENSOR_ENTITY_ID, {"outside_temp_sensor_entity_id", ""}},
      {MQTT_MANAGER_SETTING::OUTSIDE_TEMP_SENSOR_PROVIDER, {"outside_temp_sensor_provider", ""}},
      {MQTT_MANAGER_SETTING::WEATHER_PRECIPITATION_FORMAT, {"weather_precipitation_format", "mm"}},
      {MQTT_MANAGER_SETTING::RAISE_TO_100_LIGHT_LEVEL, {"raise_to_100_light_level", "95"}},
      {MQTT_MANAGER_SETTING::ALL_ROOMS_STATUS_BACKOFF_TIME, {"all_rooms_status_backoff_time", "250"}},
      {MQTT_MANAGER_SETTING::REVERSE_COLOR_TEMP, {"reverse_color_temp", "False"}},
      {MQTT_MANAGER_SETTING::SCREEN_DIM_LEVEL, {"screen_dim_level", "100"}},
      {MQTT_MANAGER_SETTING::SCREENSAVER_ACTIVATION_TIMEOUT, {"screensaver_activation_timeout", "30000"}},
      {MQTT_MANAGER_SETTING::SCREENSAVER_DIM_LEVEL, {"screensaver_dim_level", "1"}},
      {MQTT_MANAGER_SETTING::SCREENSAVER_MODE, {"screensaver_mode", "with_background"}},
      {MQTT_MANAGER_SETTING::SHOW_SCREENSAVER_INSIDE_TEMPERATURE, {"show_screensaver_inside_temperature", "True"}},
      {MQTT_MANAGER_SETTING::SHOW_SCREENSAVER_OUTSIDE_TEMPERATURE, {"show_screensaver_outside_temperature", "True"}},
      {MQTT_MANAGER_SETTING::SPECIAL_MODE_RELEASE_TIME, {"special_mode_release_time", "5000"}},
      {MQTT_MANAGER_SETTING::SPECIAL_MODE_TRIGGER_TIME, {"special_mode_trigger_time", "300"}},
      {MQTT_MANAGER_SETTING::TURN_ON_BEHAVIOR, {"turn_on_behavior", "color_temp"}},
      {MQTT_MANAGER_SETTING::USE_FAHRENHEIT, {"use_fahrenheit", "False"}},
      {MQTT_MANAGER_SETTING::WEATHER_UPDATE_INTERVAL, {"weather_update_interval", "10"}},
      {MQTT_MANAGER_SETTING::WEATHER_WIND_SPEED_FORMAT, {"weather_wind_speed_format", "kmh"}},
      {MQTT_MANAGER_SETTING::MQTT_WAIT_TIME, {"mqtt_wait_time", "1000"}},
      {MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, {"optimistic_mode", "True"}},
      {MQTT_MANAGER_SETTING::LIGHT_TURN_ON_BRIGHTNESS, {"light_turn_on_brightness", "50"}},
      {MQTT_MANAGER_SETTING::DEFAULT_NSPANEL_TYPE, {"default_nspanel_type", "eu"}},
#if defined(TEST_MODE) && TEST_MODE == 1
      {MQTT_MANAGER_SETTING::LAST, {"last", "last_value"}},
#endif
  };

#if defined(TEST_MODE) && TEST_MODE == 1
  FRIEND_TEST(MqttManagerConfigTest, verify_all_settings_exists_and_have_db_key);
  FRIEND_TEST(MqttManagerConfigTest, verify_settings_cache_is_used);
#endif
};

template <>
inline std::string MqttManagerConfig::get_setting_with_default(MQTT_MANAGER_SETTING key) {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_database_access_mutex);
  if (MqttManagerConfig::_settings_values_cache.contains(key)) {
    return MqttManagerConfig::_settings_values_cache[key];
  }

  std::string setting_db_key = MqttManagerConfig::_setting_key_map[key].first;
  try {
    auto result = database_manager::database.get_all<database_manager::SettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::SettingHolder::name) == setting_db_key));
    if (result.size() > 0) [[likely]] {
      SPDLOG_TRACE("Found setting {} with value {}", setting_db_key, result[0].value);
      MqttManagerConfig::_settings_values_cache[key] = result[0].value;
      return result[0].value;
    } else {
      SPDLOG_TRACE("Did not find setting {} in database. Looking for default value.", setting_db_key);

      if (MqttManagerConfig::_setting_key_map.contains(key)) {
        std::string value = MqttManagerConfig::_setting_key_map[key].second;
        SPDLOG_TRACE("Returning default value '{}' for setting '{}'", value, setting_db_key);
        MqttManagerConfig::_settings_values_cache[key] = value;
        return value;
      } else {
        SPDLOG_ERROR("Did not find default setting value for key '{}'. Returning empty string.", setting_db_key);
        return "";
      }
    }
  } catch (std::exception &ex) {
    SPDLOG_ERROR("Caught exception while trying to access database to retrieve setting {}. Exception: {}", setting_db_key, boost::diagnostic_information(ex));
  }
  SPDLOG_TRACE("Did not find setting {}. Returning empty string.", setting_db_key);
  return "";
}

template <>
inline bool MqttManagerConfig::get_setting_with_default(MQTT_MANAGER_SETTING key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  boost::to_lower(value);
  return value.compare("true") == 0;
}

template <>
inline int32_t MqttManagerConfig::get_setting_with_default(MQTT_MANAGER_SETTING key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  try {
    return std::stoi(value);
  } catch (std::invalid_argument &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to integer. Returning 0. Key: {}, Exception: {}", MqttManagerConfig::_setting_key_map[key].first, boost::diagnostic_information(ex));
    return 0;
  } catch (std::out_of_range &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to integer. Returning 0. Key: {}, Exception: {}", MqttManagerConfig::_setting_key_map[key].first, boost::diagnostic_information(ex));
    return 0;
  }
}

template <>
inline uint32_t MqttManagerConfig::get_setting_with_default(MQTT_MANAGER_SETTING key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  try {
    return std::stoul(value);
  } catch (std::invalid_argument &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to unsigned integer. Key: {}, Returning 0. Exception: {}", MqttManagerConfig::_setting_key_map[key].first, boost::diagnostic_information(ex));
    return 0;
  } catch (std::out_of_range &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to unsigned integer. Key: {}, Returning 0. Exception: {}", MqttManagerConfig::_setting_key_map[key].first, boost::diagnostic_information(ex));
    return 0;
  }
}

template <>
inline float MqttManagerConfig::get_setting_with_default(MQTT_MANAGER_SETTING key) {
  std::string value = MqttManagerConfig::get_setting_with_default<std::string>(key);
  try {
    return std::stof(value);
  } catch (std::invalid_argument &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to float. Returning 0.0f. Key: {}, Exception: {}", MqttManagerConfig::_setting_key_map[key].first, boost::diagnostic_information(ex));
    return 0.0f;
  } catch (std::out_of_range &ex) {
    SPDLOG_ERROR("Caught exception while trying to convert string to float. Returning 0.0f. Key: {}, Exception: {}", MqttManagerConfig::_setting_key_map[key].first, boost::diagnostic_information(ex));
    return 0.0f;
  }
}

#endif // !MQTTMANAGER_CONFIG_HPP
