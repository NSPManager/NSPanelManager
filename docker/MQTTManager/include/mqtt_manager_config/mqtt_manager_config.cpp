#include "mqtt_manager_config.hpp"
#include "web_helper/WebHelper.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <database_manager/database_manager.hpp>
#include <exception>
#include <fmt/core.h>
#include <fstream>
#include <mutex>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <nlohmann/detail/exceptions.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/protobuf_mqttmanager.pb.h>
#include <protobuf_general.pb.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

MqttManagerSettingsHolder MqttManagerConfig::get_settings() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_settings_mutex);
  return MqttManagerConfig::_settings;
}

std::string MqttManagerConfig::get_setting_with_default(std::string key, std::string default_value) {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_database_access_mutex);
  try {
    auto result = database_manager::database.get_all<database_manager::SettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::SettingHolder::name) == key));
    if (result.size() > 0) [[likely]] {
      SPDLOG_TRACE("Found setting {} with value {}", key, result[0].value);
      return result[0].value;
    } else {
      SPDLOG_TRACE("Did not find setting {}. Returning default:   {}", key, default_value);
      return default_value;
    }
  } catch (std::exception &ex) {
    SPDLOG_ERROR("Caught exception while trying to access database to retrieve setting {}. Exception: {}", key, boost::diagnostic_information(ex));
  }
  SPDLOG_TRACE("Did not find setting {}. Returning default:   {}", key, default_value);
  return default_value;
}

void MqttManagerConfig::load() {

  SPDLOG_TRACE("Loading timezone from /etc/timezone.");
  // Begin by loading timezone
  std::ifstream f("/etc/timezone", std::ios::in | std::ios::binary);
  const size_t file_size = std::filesystem::file_size("/etc/timezone");
  std::string timezone_str(file_size, '\0');
  f.read(timezone_str.data(), file_size);
  f.close();
  boost::algorithm::trim(timezone_str);
  MqttManagerConfig::timezone = timezone_str;
  SPDLOG_INFO("Read timezone {} from /etc/timezone.", timezone_str);

  {
    SPDLOG_INFO("Loading MQTT Manager settings.");
    std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_settings_mutex);

    MqttManagerConfig::_settings.manager_address = MqttManagerConfig::get_setting_with_default("manager_address", "");
    MqttManagerConfig::_settings.manager_port = std::stoi(MqttManagerConfig::get_setting_with_default("manager_port", "8000"));
    MqttManagerConfig::_settings.color_temp_min = std::stoi(MqttManagerConfig::get_setting_with_default("color_temp_min", "2000"));
    MqttManagerConfig::_settings.color_temp_max = std::stoi(MqttManagerConfig::get_setting_with_default("color_temp_max", "6000"));
    MqttManagerConfig::_settings.reverse_color_temperature_slider = MqttManagerConfig::get_setting_with_default("reverse_color_temp", "False").compare("True") == 0;
    MqttManagerConfig::_settings.date_format = MqttManagerConfig::get_setting_with_default("date_format", "%a %d/%m/ %Y");
    MqttManagerConfig::_settings.clock_24_hour_format = MqttManagerConfig::get_setting_with_default("clock_us_style", "False").compare("False") == 0;
    MqttManagerConfig::_settings.optimistic_mode = MqttManagerConfig::get_setting_with_default("optimistic_mode", "True").compare("True") == 0;
    MqttManagerConfig::_settings.mqtt_wait_time = std::stoi(MqttManagerConfig::get_setting_with_default("mqtt_wait_time", "1000"));

    const char *is_home_assistant_addon = std::getenv("IS_HOME_ASSISTANT_ADDON");
    if (is_home_assistant_addon != nullptr) {
      if (std::string(is_home_assistant_addon).compare("true") == 0) {
        MqttManagerConfig::_settings.is_home_assistant_addon = true;
      } else {
        MqttManagerConfig::_settings.is_home_assistant_addon = false;
      }
    } else {
      MqttManagerConfig::_settings.is_home_assistant_addon = false;
    }

    std::string turn_on_bevaiour = MqttManagerConfig::get_setting_with_default("turn_on_behaviour", "color_temp");
    if (turn_on_bevaiour.compare("color_temp") == 0) {
      MqttManagerConfig::_settings.light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;
    } else if (turn_on_bevaiour.compare("restore") == 0) {
      MqttManagerConfig::_settings.light_turn_on_behaviour = LightTurnOnBehaviour::RESTORE_PREVIOUS;
    } else {
      SPDLOG_WARN("Failed to determine turn on bevaiour for lights, assuming color temp. Value set: {}", turn_on_bevaiour);
      MqttManagerConfig::_settings.light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;
    }
  }

  // Notify all listeners that the config has been loaded
  MqttManagerConfig::_config_loaded_listeners();
}
