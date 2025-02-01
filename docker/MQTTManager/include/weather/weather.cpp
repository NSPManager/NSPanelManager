#include "weather.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include "web_helper/WebHelper.hpp"
#include <bits/types/time_t.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <exception>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <protobuf_nspanel.pb.h>
#include <spdlog/spdlog.h>
#include <string>

void MQTTManagerWeather::start() {
  MQTTManagerWeather::reload_config();
  if (!MQTTManagerWeather::_instance.joinable()) {
    SPDLOG_INFO("No running instance of weather manager found. Will start new thread.");
    MQTTManagerWeather::_instance = std::thread(MQTTManagerWeather::_run_weather_thread);
  } else {
    SPDLOG_INFO("Weather manager already running.");
  }
  // Clear old topic of old retain data.
  MQTT_Manager::clear_retain("nspanel/status/weather");
}

void MQTTManagerWeather::_run_weather_thread() {
  while (true) {
    MQTTManagerWeather::_pull_new_weather_data();
    std::this_thread::sleep_for(std::chrono::minutes(MQTTManagerWeather::_update_interval_minutes));
  }
}

void MQTTManagerWeather::reload_config() {
  std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
  if (MqttManagerConfig::get_setting_with_default("outside_temp_provider", "").compare("home_assistant") == 0) {
    OpenhabManager::detach_event_observer(MQTTManagerWeather::_outside_temperature_sensor_provider, &MQTTManagerWeather::openhab_temp_sensor_callback);
    MQTTManagerWeather::_outside_temperature_sensor_entity_id = MqttManagerConfig::get_setting_with_default("outside_temp_sensor_entity_id", "");
    HomeAssistantManager::attach_event_observer(MQTTManagerWeather::_outside_temperature_sensor_entity_id, &MQTTManagerWeather::home_assistant_event_callback);
    SPDLOG_INFO("Will load outside temperature from Home Assistant sensor {}", MQTTManagerWeather::_outside_temperature_sensor_entity_id);
  } else if (MqttManagerConfig::get_setting_with_default("outside_temp_provider", "").compare("openhab") == 0) {
    HomeAssistantManager::detach_event_observer(MQTTManagerWeather::_outside_temperature_sensor_entity_id, &MQTTManagerWeather::home_assistant_event_callback);
    MQTTManagerWeather::_outside_temperature_sensor_entity_id = MqttManagerConfig::get_setting_with_default("outside_temp_sensor_entity_id", "");
    OpenhabManager::attach_event_observer(MQTTManagerWeather::_outside_temperature_sensor_entity_id, &MQTTManagerWeather::openhab_temp_sensor_callback);
    SPDLOG_INFO("Will load outside temperature from OpenHAB sensor {}", MQTTManagerWeather::_outside_temperature_sensor_entity_id);
  }

  MQTTManagerWeather::_location_latitude = MqttManagerConfig::get_setting_with_default("location_latitude", "");
  MQTTManagerWeather::_location_longitude = MqttManagerConfig::get_setting_with_default("location_longitude", "");
  MQTTManagerWeather::_update_interval_minutes = std::stoi(MqttManagerConfig::get_setting_with_default("weather_update_internval", "10"));
  MQTTManagerWeather::_wind_speed_format = MqttManagerConfig::get_setting_with_default("wind_speed_format", "");
  MQTTManagerWeather::_precipitation_unit = MqttManagerConfig::get_setting_with_default("precipitation_format", "");
  MQTTManagerWeather::_temperature_unit = MqttManagerConfig::get_setting_with_default("use_fahrenheit", "False").compare("True") == 0 ? "fahrenheit" : "celsius";
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void MQTTManagerWeather::_pull_new_weather_data() {
  std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
  std::string pull_weather_url = fmt::format("https://api.open-meteo.com/v1/forecast?latitude={}&longitude={}&current=temperature_2m,relative_humidity_2m,precipitation,weather_code,wind_speed_10m,wind_gusts_10m&daily=weather_code,temperature_2m_max,temperature_2m_min,sunrise,sunset,precipitation_sum,precipitation_probability_max,wind_speed_10m_max,wind_gusts_10m_max&timeformat=unixtime&wind_speed_unit={}&timezone={}&precipitation_unit={}&temperature_unit={}",
                                             MQTTManagerWeather::_location_latitude,
                                             MQTTManagerWeather::_location_longitude,
                                             MQTTManagerWeather::_windspeed_unit,
                                             MqttManagerConfig::timezone,
                                             MQTTManagerWeather::_precipitation_unit,
                                             MQTTManagerWeather::_temperature_unit);
  std::list<const char *> headers = {"Content-type: application/json"};
  std::string response_data;
  if (WebHelper::perform_get_request(&pull_weather_url, &response_data, &headers)) {
    SPDLOG_DEBUG("Successfully received new weather forecast from Open Meteo. Will process new data.");
    MQTTManagerWeather::_process_weather_data(response_data);
    SPDLOG_DEBUG("Weather data processed.");
  } else {
    SPDLOG_ERROR("Failed to get weather.");
  }
}

void MQTTManagerWeather::_process_weather_data(std::string &weather_string) {
  // Set timezone
  setenv("TZ", MqttManagerConfig::timezone.c_str(), 1);
  tzset();

  // Start processing data
  try {
    nlohmann::json data = nlohmann::json::parse(weather_string);
    MQTTManagerWeather::_windspeed_unit = data["current_units"]["wind_speed_10m"];
    MQTTManagerWeather::_precipitation_unit = data["current_units"]["precipitation"];

    // Load forecast, 5 days:
    MQTTManagerWeather::_forecast_weather_info.clear();
    for (int i = 0; i < 5; i++) {
      weather_info info;
      info.condition = std::to_string(int(data["daily"]["weather_code"][i]));
      info.wind_speed = data["daily"]["wind_speed_10m_max"][i];
      info.precipitation = data["daily"]["precipitation_sum"][i];
      info.precipitation_probability = data["daily"]["precipitation_probability_max"][i];
      info.temperature_low = data["daily"]["temperature_2m_min"][i];
      info.temperature_high = data["daily"]["temperature_2m_max"][i];

      const std::time_t timestamp = data["daily"]["time"][i];
      std::tm *localtime = std::localtime(&timestamp);
      info.time = *localtime;

      const std::time_t sunrise = data["daily"]["sunrise"][i];
      std::tm *sunrise_tm = std::localtime(&sunrise);
      info.sunrise = *sunrise_tm;

      const std::time_t sunset = data["daily"]["sunset"][i];
      std::tm *sunset_tm = std::localtime(&sunset);
      info.sunset = *sunset_tm;

      switch (localtime->tm_wday) {
      case 0:
        info.day = "Sun";
        break;
      case 1:
        info.day = "Mon";
        break;
      case 2:
        info.day = "Tue";
        break;
      case 3:
        info.day = "Wed";
        break;
      case 4:
        info.day = "Thu";
        break;
      case 5:
        info.day = "Fri";
        break;
      case 6:
        info.day = "Sat";
        break;
      default:
        info.day = std::to_string(localtime->tm_wday);
        break;
      }

      MQTTManagerWeather::_forecast_weather_info.push_back(info);
    }

    if (MQTTManagerWeather::_forecast_weather_info.size() > 0) {
      SPDLOG_DEBUG("Loaded forecast for {} days.", MQTTManagerWeather::_forecast_weather_info.size());
      if (MQTTManagerWeather::_outside_temperature_sensor_provider.empty() || MQTTManagerWeather::_outside_temperature_sensor_entity_id.empty()) {
        // Only update the current temperature if no local sensor is configured
        MQTTManagerWeather::_current_temperature = data["current"]["temperature_2m"];
      }
      MQTTManagerWeather::_next_sunrise = fmt::format("{:%H:%M}", MQTTManagerWeather::_forecast_weather_info[0].sunrise);
      MQTTManagerWeather::_next_sunrise_hour = MQTTManagerWeather::_forecast_weather_info[0].sunrise.tm_hour;
      MQTTManagerWeather::_next_sunset = fmt::format("{:%H:%M}", MQTTManagerWeather::_forecast_weather_info[0].sunset);
      MQTTManagerWeather::_next_sunset_hour = MQTTManagerWeather::_forecast_weather_info[0].sunset.tm_hour;
      MQTTManagerWeather::_current_condition = std::to_string(int(data["current"]["weather_code"]));
      MQTTManagerWeather::_current_wind_speed = data["current"]["wind_speed_10m"];
      MQTTManagerWeather::_current_min_temperature = MQTTManagerWeather::_forecast_weather_info[0].temperature_low;
      MQTTManagerWeather::_current_max_temperature = MQTTManagerWeather::_forecast_weather_info[0].temperature_high;

      std::time_t current_weather_time = data["current"]["time"];
      MQTTManagerWeather::_current_weather_time = *std::localtime(&current_weather_time);
      MQTTManagerWeather::send_state_update();
    } else {
      SPDLOG_ERROR("Failed to load forecast!");
    }

  } catch (const std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to decode weather data: {}", boost::diagnostic_information(e, true));
  }
}

void MQTTManagerWeather::home_assistant_event_callback(nlohmann::json event_data) {
  if (MQTTManagerWeather::_outside_temperature_sensor_provider.compare("home_assistant") == 0 && std::string(event_data["event"]["data"]["entity_id"]).compare(MQTTManagerWeather::_outside_temperature_sensor_entity_id) == 0) {
    std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
    SPDLOG_DEBUG("Received current outside temperature from Home Assistant sensor.");
    nlohmann::json new_state = event_data["event"]["data"]["new_state"];
    MQTTManagerWeather::_current_temperature = atof(std::string(new_state["state"]).c_str());
    MQTTManagerWeather::send_state_update();
    SPDLOG_DEBUG("Temperature data processed.");
  }
}

std::string MQTTManagerWeather::_get_icon_from_mapping(std::string &condition, uint8_t hour, bool allow_night_icon) {
  for (nlohmann::json mapping : MqttManagerConfig::icon_mapping["openmeteo_weather_mappings"]) {
    if (std::string(mapping["id"]).compare(condition) == 0) {
      if (mapping.contains("character-mapping-day") && ((hour >= MQTTManagerWeather::_next_sunrise_hour && hour <= MQTTManagerWeather::_next_sunset_hour) || !allow_night_icon)) {
        return mapping["character-mapping-day"];
      } else if (mapping.contains("character-mapping-night") && (hour <= MQTTManagerWeather::_next_sunrise_hour || hour >= MQTTManagerWeather::_next_sunset_hour)) {
        return mapping["character-mapping-night"];
      } else if (mapping.contains("character-mapping")) {
        return std::string(mapping["character-mapping"]);
      } else {
        SPDLOG_ERROR("Found matching condition for {} but no icon-mapping!", condition);
        SPDLOG_ERROR("Matching condition current hour: {}, sunrise: {}, sunset: {}", hour, MQTTManagerWeather::_next_sunrise_hour, MQTTManagerWeather::_next_sunset_hour);
      }
    }
  }

  SPDLOG_ERROR("Couldn't find a mapping for condition {}.", condition);
  return "";
}

void MQTTManagerWeather::openhab_temp_sensor_callback(nlohmann::json event_data) {
  if (MQTTManagerWeather::_outside_temperature_sensor_provider.compare("openhab") == 0) {
    std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
    SPDLOG_DEBUG("Received current outside temperature from OpenHAB sensor.");
    std::string temp_data;
    if (std::string(event_data["type"]).compare("ItemStateChangedEvent") == 0) {
      nlohmann::json payload = nlohmann::json::parse(std::string(event_data["payload"]));
      temp_data = payload["value"];
    } else if (std::string(event_data["type"]).compare("ItemStateFetched") == 0) {
      temp_data = event_data["payload"]["state"];
    }

    if (temp_data.size() > 0) {
      MQTTManagerWeather::_current_temperature = atof(temp_data.c_str());
      MQTTManagerWeather::send_state_update();
    }
    SPDLOG_DEBUG("Temperature data processed.");
  }
}

void MQTTManagerWeather::send_state_update() {
  NSPanelWeatherUpdate weather_protbuf;
  for (struct weather_info &info : MQTTManagerWeather::_forecast_weather_info) {
    NSPanelWeatherUpdate_ForecastItem *forecast_item = weather_protbuf.add_forecast_items();
    forecast_item->set_weather_icon(MQTTManagerWeather::_get_icon_from_mapping(info.condition, info.time.tm_hour, false));
    // TODO: Implement ability for users to select precipitation volume instead of probability.
    forecast_item->set_precipitation_string(fmt::format("{:.0f}%", info.precipitation));
    forecast_item->set_temperature_maxmin_string(fmt::format("{:.0f}°/{:.0f}°", info.temperature_high, info.temperature_low));
    forecast_item->set_wind_string(fmt::format("{:.0f}{}", info.wind_speed, MQTTManagerWeather::_windspeed_unit));
    // TODO: Implement ability for users to display comming 5 hours instead of 5 days.
    forecast_item->set_display_string(info.day);
  }
  weather_protbuf.set_current_weather_icon(MQTTManagerWeather::_get_icon_from_mapping(MQTTManagerWeather::_current_condition, MQTTManagerWeather::_current_weather_time.tm_hour, true));
  weather_protbuf.set_current_temperature_string(fmt::format("{:.0f}°", MQTTManagerWeather::_current_temperature));
  weather_protbuf.set_current_maxmin_temperature(fmt::format("{:.0f}°/{:.0f}°", MQTTManagerWeather::_current_max_temperature, MQTTManagerWeather::_current_min_temperature));
  weather_protbuf.set_current_wind_string(fmt::format("{:.0f}{}", MQTTManagerWeather::_current_wind_speed, MQTTManagerWeather::_windspeed_unit));
  weather_protbuf.set_sunrise_string(MQTTManagerWeather::_next_sunrise);
  weather_protbuf.set_sunset_string(MQTTManagerWeather::_next_sunset);
  // TODO: Implement ability for users to select precipitation volume instead of probability.
  weather_protbuf.set_current_precipitation_string(fmt::format("{:.0f}%", MQTTManagerWeather::_current_precipitation_probability));

  std::string new_weather_data;
  if (weather_protbuf.SerializeToString(&new_weather_data)) {
    std::string weather_update_topic = fmt::format("nspanel/mqttmanager_{}/status/weather", MqttManagerConfig::get_settings().manager_address);
    SPDLOG_DEBUG("Sending new weather data out on topic {}.", weather_update_topic);
    MQTT_Manager::publish(weather_update_topic, new_weather_data, true);
  } else {
    SPDLOG_ERROR("Failed to serialize weather info to string.");
  }
}
