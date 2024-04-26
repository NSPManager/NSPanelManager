#include "weather.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
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
#include <spdlog/spdlog.h>
#include <string>

void MQTTManagerWeather::start() {
  if (!MQTTManagerWeather::_instance.joinable()) {
    SPDLOG_INFO("No running instance of weather manager found. Will start new thread.");
    MQTTManagerWeather::_instance = std::thread(MQTTManagerWeather::_run_weather_thread);
  } else {
    SPDLOG_INFO("Weather manager already running.");
  }
}

void MQTTManagerWeather::_run_weather_thread() {
  while (true) {
    MQTTManagerWeather::_pull_new_weather_data();
    std::this_thread::sleep_for(std::chrono::minutes(MqttManagerConfig::weather_update_interval));
  }
}

void MQTTManagerWeather::update_config() {
  std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
  if (MqttManagerConfig::outside_temp_sensor_provider.compare("home_assistant") == 0) {
    SPDLOG_INFO("Will load outside temperature from Home Assistant sensor {}", MqttManagerConfig::outside_temp_sensor_entity_id);
    OpenhabManager::detach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, &MQTTManagerWeather::openhab_temp_sensor_callback);
    HomeAssistantManager::attach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, &MQTTManagerWeather::home_assistant_event_callback);
  } else if (MqttManagerConfig::outside_temp_sensor_provider.compare("openhab") == 0) {
    SPDLOG_INFO("Will load outside temperature from OpenHAB sensor {}", MqttManagerConfig::outside_temp_sensor_entity_id);
    HomeAssistantManager::detach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, &MQTTManagerWeather::home_assistant_event_callback);
    OpenhabManager::attach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, &MQTTManagerWeather::openhab_temp_sensor_callback);
  }

  MQTTManagerWeather::start();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void MQTTManagerWeather::_pull_new_weather_data() {
  std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
  try {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
      std::string response_data; // This will contain any response data from the CURL request.
      std::string bearer_token = "Authorization: Bearer ";
      bearer_token.append(MqttManagerConfig::openhab_access_token);

      struct curl_slist *headers = NULL;
      headers = curl_slist_append(headers, bearer_token.c_str());
      if (headers == NULL) {
        SPDLOG_ERROR("Failed to set bearer token header for OpenHAB light rest request.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
      }
      headers = curl_slist_append(headers, "Content-type: application/json");
      if (headers == NULL) {
        SPDLOG_ERROR("Failed to set content-type header for OpenHAB light rest request.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
      }

      std::string temperature_unit;
      if (MqttManagerConfig::use_fahrenheit) {
        temperature_unit = "fahrenheit";
      } else {
        temperature_unit = "celsius";
      }

      std::string pull_weather_url = fmt::format("https://api.open-meteo.com/v1/forecast?latitude={}&longitude={}&current=temperature_2m,relative_humidity_2m,precipitation,weather_code,wind_speed_10m,wind_gusts_10m&daily=weather_code,temperature_2m_max,temperature_2m_min,sunrise,sunset,precipitation_sum,precipitation_probability_max,wind_speed_10m_max,wind_gusts_10m_max&timeformat=unixtime&wind_speed_unit={}&timezone={}&precipitation_unit={}&temperature_unit={}", MqttManagerConfig::weather_location_latitude, MqttManagerConfig::weather_location_longitude, MqttManagerConfig::weather_wind_speed_format, MqttManagerConfig::timezone, MqttManagerConfig::weather_precipitation_format, temperature_unit);
      curl_easy_setopt(curl, CURLOPT_URL, pull_weather_url.c_str());
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

      SPDLOG_DEBUG("Requesting new weather forecast from Open Meteo.");

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      long http_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      /* Check for errors */
      if (res == CURLE_OK && http_code == 200) {
        SPDLOG_DEBUG("Successfully received new weather forcast from Open Meteo. Will process new data.");
        MQTTManagerWeather::_process_weather_data(response_data);
        SPDLOG_DEBUG("Weather data processed.");
      } else {
        SPDLOG_ERROR("curl_easy_perform() when getting new weather forecast, got code: {}.", curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
    }
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to fetch new weather data: {}", boost::diagnostic_information(e, true));
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
      if (MqttManagerConfig::outside_temp_sensor_provider.length() == 0 && MqttManagerConfig::outside_temp_sensor_entity_id.length() == 0) {
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
  if (MqttManagerConfig::outside_temp_sensor_provider.compare("home_assistant") == 0 && std::string(event_data["event"]["data"]["entity_id"]).compare(MqttManagerConfig::outside_temp_sensor_entity_id) == 0) {
    std::lock_guard<std::mutex> lock_guard(MQTTManagerWeather::_weater_data_mutex);
    SPDLOG_DEBUG("Received current outside temperature from Home Assistant sensor.");
    nlohmann::json new_state = event_data["event"]["data"]["new_state"];
    MQTTManagerWeather::_current_temperature = atof(std::string(new_state["state"]).c_str());
    MQTTManagerWeather::send_state_update();
    SPDLOG_DEBUG("Temperature data processed.");
  }
}

std::string MQTTManagerWeather::_get_icon_from_mapping(std::string &condition, uint8_t hour) {
  for (nlohmann::json mapping : MqttManagerConfig::icon_mapping["openmeteo_weather_mappings"]) {
    if (std::string(mapping["id"]).compare(condition) == 0) {
      if (mapping.contains("character-mapping-day") && hour >= MQTTManagerWeather::_next_sunrise_hour && hour <= MQTTManagerWeather::_next_sunset_hour) {
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
  if (MqttManagerConfig::outside_temp_sensor_provider.compare("openhab") == 0) {
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
  nlohmann::json weather_info;
  std::list<nlohmann::json> forecast;
  for (struct weather_info &info : MQTTManagerWeather::_forecast_weather_info) {
    nlohmann::json forecast_data;
    forecast_data["icon"] = MQTTManagerWeather::_get_icon_from_mapping(info.condition, info.time.tm_hour);
    std::string pre = std::to_string((int)round(info.precipitation));
    pre.append(MQTTManagerWeather::_precipitation_unit);
    forecast_data["pre"] = pre;
    std::string prepro = std::to_string((int)round(info.precipitation_probability));
    prepro.append("%");
    forecast_data["prepro"] = prepro;

    std::string templow = std::to_string((int)round(info.temperature_low));
    templow.append("°");
    std::string temphigh = std::to_string((int)round(info.temperature_high));
    temphigh.append("°");
    std::string temp_display = temphigh;
    temp_display.append("/");
    temp_display.append(templow);
    forecast_data["maxmin"] = temp_display;

    std::string wind = std::to_string((int)round(info.wind_speed));
    wind.append(MQTTManagerWeather::_windspeed_unit);
    forecast_data["wind"] = wind;
    forecast_data["day"] = info.day;
    forecast.push_back(forecast_data);
  }
  weather_info["forecast"] = forecast;
  weather_info["icon"] = MQTTManagerWeather::_get_icon_from_mapping(MQTTManagerWeather::_current_condition, MQTTManagerWeather::_current_weather_time.tm_hour);
  std::string temp = std::to_string((int)round(MQTTManagerWeather::_current_temperature));
  temp.append("°");
  weather_info["temp"] = temp;

  std::string templow = std::to_string((int)round(MQTTManagerWeather::_current_min_temperature));
  templow.append("°");
  std::string temphigh = std::to_string((int)round(MQTTManagerWeather::_current_max_temperature));
  temphigh.append("°");
  std::string temp_display = temphigh;
  temp_display.append("/");
  temp_display.append(templow);
  weather_info["maxmin"] = temp_display;

  std::string wind = std::to_string((int)round(MQTTManagerWeather::_current_wind_speed));
  wind.append(MQTTManagerWeather::_windspeed_unit);
  weather_info["wind"] = wind;

  weather_info["sunrise"] = MQTTManagerWeather::_next_sunrise;
  weather_info["sunset"] = MQTTManagerWeather::_next_sunset;
  weather_info["prepro"] = fmt::format("{}%", MQTTManagerWeather::_current_precipitation_probability);

  std::string new_weather_data = weather_info.dump();
  MQTT_Manager::publish("nspanel/status/weather", new_weather_data, true);
}
