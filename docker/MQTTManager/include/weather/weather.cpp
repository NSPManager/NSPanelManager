#include "weather.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <bits/types/time_t.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <fmt/core.h>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

void MQTTManagerWeather::init() {
  MQTTManagerWeather::_update_forecast_thread = std::thread(MQTTManagerWeather::_update_forecast);

  MQTTManagerWeather::_update_forecast_thread.join();
}

void MQTTManagerWeather::update_config() {
  if (MqttManagerConfig::outside_temp_sensor_provider.compare("home_assistant") == 0) {
    SPDLOG_INFO("Will load outside temperature from Home Assistant sensor {}", MqttManagerConfig::outside_temp_sensor_entity_id);
    HomeAssistantManager::attach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, boost::bind(&MQTTManagerWeather::home_assistant_event_callback, this, _1));
    OpenhabManager::detach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, boost::bind(&MQTTManagerWeather::openhab_temp_sensor_callback, this, _1));
  } else if (MqttManagerConfig::outside_temp_sensor_provider.compare("openhab") == 0) {
    SPDLOG_INFO("Will load outside temperature from OpenHAB sensor {}", MqttManagerConfig::outside_temp_sensor_entity_id);
    OpenhabManager::attach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, boost::bind(&MQTTManagerWeather::openhab_temp_sensor_callback, this, _1));
    if (MqttManagerConfig::weather_controller.compare("home_assistant") != 0) {
      HomeAssistantManager::detach_event_observer(MqttManagerConfig::outside_temp_sensor_entity_id, boost::bind(&MQTTManagerWeather::home_assistant_event_callback, this, _1));
    }
  }
}

void MQTTManagerWeather::home_assistant_event_callback(nlohmann::json event_data) {
  if (MqttManagerConfig::outside_temp_sensor_provider.compare("home_assistant") == 0 && std::string(event_data["event"]["data"]["entity_id"]).compare(MqttManagerConfig::outside_temp_sensor_entity_id) == 0) {
    nlohmann::json new_state = event_data["event"]["data"]["new_state"];
    this->_current_temperature = atof(std::string(new_state["state"]).c_str());
    this->send_state_update();
  }
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void MQTTManagerWeather::_update_forecast() {
  SPDLOG_INFO("Starting 'Update Forecast' thread.");
  while (true) {
    SPDLOG_DEBUG("Fetching new weather forecast.");
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
      std::string response_data;
      curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/forecast?lat=44.34&lon=10.99&appid=4ff881e8b20a81abe808164d88816a6a");
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);

      /* Perform the request, res will get the return code */
      SPDLOG_DEBUG("Requesting weather.");
      res = curl_easy_perform(curl);
      long http_code;
      SPDLOG_DEBUG("Gathering response code.");
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      /* Check for errors */
      if (res == CURLE_OK && http_code == 200) {
        SPDLOG_DEBUG("Got response: {}", response_data);
      } else {
        SPDLOG_ERROR("Failed to fetch weather forecast. CURL error: {}", curl_easy_strerror(res));
      }

      /* always cleanup */
      SPDLOG_DEBUG("Cleanup.");
      curl_easy_cleanup(curl);
    } else {
      SPDLOG_ERROR("Failed to create curl object.");
    }
    SPDLOG_DEBUG("Weather fetched, will try again in 1 minute.");
    std::this_thread::sleep_for(std::chrono::milliseconds(60000)); // Fetch new forecast every minute TODO: Change this to something more reasonable
  }
}

void MQTTManagerWeather::openhab_current_weather_callback(nlohmann::json event_data) {
  // Set timezone
  setenv("TZ", MqttManagerConfig::timezone.c_str(), 1);
  tzset();

  std::string weather_state;
  if (std::string(event_data["type"]).compare("ItemStateChangedEvent") == 0) {
    nlohmann::json payload = nlohmann::json::parse(std::string(event_data["payload"]));
    weather_state = payload["value"];
  } else if (std::string(event_data["type"]).compare("ItemStateFetched") == 0) {
    weather_state = event_data["payload"]["state"];
  }

  if (weather_state.size() > 0) {
    nlohmann::json json_decode = nlohmann::json::parse(weather_state);
    nlohmann::json weather_json = json_decode[0];

    this->_current_condition = weather_json["WeatherText"];
    this->_current_condition_id = weather_json["WeatherIcon"];

    if (MqttManagerConfig::use_farenheit) {
      if (MqttManagerConfig::outside_temp_sensor_provider.length() == 0 && MqttManagerConfig::outside_temp_sensor_entity_id.length() == 0) {
        this->_current_temperature = weather_json["Temperature"]["Imperial"]["Value"];
      }
      this->_current_min_temperature = weather_json["TemperatureSummary"]["Past6HourRange"]["Minimum"]["Imperial"]["Value"];
      this->_current_max_temperature = weather_json["TemperatureSummary"]["Past6HourRange"]["Maximum"]["Imperial"]["Value"];
      this->_current_wind_speed = weather_json["Wind"]["Speed"]["Imperial"]["Value"];
      this->_windspeed_unit = weather_json["Wind"]["Speed"]["Imperial"]["Unit"];
      this->_precipitation_unit = weather_json["PrecipitationSummary"]["Precipitation"]["Imperial"]["Unit"];
    } else {
      if (MqttManagerConfig::outside_temp_sensor_provider.length() == 0 && MqttManagerConfig::outside_temp_sensor_entity_id.length() == 0) {
        this->_current_temperature = weather_json["Temperature"]["Metric"]["Value"];
      }
      this->_current_min_temperature = weather_json["TemperatureSummary"]["Past6HourRange"]["Minimum"]["Metric"]["Value"];
      this->_current_max_temperature = weather_json["TemperatureSummary"]["Past6HourRange"]["Maximum"]["Metric"]["Value"];
      this->_current_wind_speed = weather_json["Wind"]["Speed"]["Metric"]["Value"];
      this->_windspeed_unit = weather_json["Wind"]["Speed"]["Metric"]["Unit"];
      this->_precipitation_unit = weather_json["PrecipitationSummary"]["Precipitation"]["Metric"]["Unit"];
    }

    time_t weather_time = uint64_t(weather_json["EpochTime"]);
    this->_current_weather_time = *std::localtime(&weather_time);

    this->send_state_update();
  }
}

void MQTTManagerWeather::openhab_forecast_weather_callback(nlohmann::json event_data) {
  // Set timezone
  setenv("TZ", MqttManagerConfig::timezone.c_str(), 1);
  tzset();

  std::string forecast_state;
  if (std::string(event_data["type"]).compare("ItemStateChangedEvent") == 0) {
    nlohmann::json payload = nlohmann::json::parse(std::string(event_data["payload"]));
    forecast_state = payload["value"];
  } else if (std::string(event_data["type"]).compare("ItemStateFetched") == 0) {
    forecast_state = event_data["payload"]["state"];
  }

  if (forecast_state.size() > 0) {
    nlohmann::json forecast_json = nlohmann::json::parse(forecast_state);

    this->_forecast_weather_info.clear();
    for (nlohmann::json &day_info : forecast_json["DailyForecasts"]) {
      struct weather_info day_summary;
      time_t dt = uint64_t(day_info["EpochDate"]);
      day_summary.time = *std::localtime(&dt);

      day_summary.condition = day_info["Day"]["ShortPhrase"];
      day_summary.condition_id = day_info["Day"]["Icon"];
      day_summary.temperature_low = day_info["Temperature"]["Minimum"]["Value"];
      day_summary.temperature_high = day_info["Temperature"]["Maximum"]["Value"];
      day_summary.wind_speed = day_info["Day"]["Wind"]["Speed"]["Value"];
      day_summary.precipitation_probability = day_info["Day"]["PrecipitationProbability"];

      // sunrise
      time_t dt_sunrise = uint64_t(day_info["Sun"]["EpochRise"]);
      day_summary.sunrise = *std::localtime(&dt_sunrise);
      // sunset
      time_t dt_sunset = uint64_t(day_info["Sun"]["EpochSet"]);
      day_summary.sunset = *std::localtime(&dt_sunset);

      switch (day_summary.time.tm_wday) {
      case 0:
        day_summary.day = "Sun";
        break;
      case 1:
        day_summary.day = "Mon";
        break;
      case 2:
        day_summary.day = "Tue";
        break;
      case 3:
        day_summary.day = "Wed";
        break;
      case 4:
        day_summary.day = "Thu";
        break;
      case 5:
        day_summary.day = "Fri";
        break;
      case 6:
        day_summary.day = "Sat";
        break;
      default:
        day_summary.day = std::to_string(day_summary.time.tm_wday);
        break;
      }

      SPDLOG_DEBUG("Adding OpenHAB weather(OpenWeatherMap) for {}-{:0>2}-{:0>2} {:0>2}:{:0>2} to forecast list.", day_summary.time.tm_year + 1900, day_summary.time.tm_mon + 1, day_summary.time.tm_mday, day_summary.time.tm_hour, day_summary.time.tm_min);
      this->_forecast_weather_info.push_back(day_summary);
    }

    if (this->_forecast_weather_info.size() > 0) {
      // Unfortunetly OpenWeatherMap doesn't provide a precipitation_probability for the current time and so the next best thing is the first item in the forecast.
      this->_current_precipitation_probability = this->_forecast_weather_info[0].precipitation_probability;
      this->_next_sunrise_hour = this->_forecast_weather_info[0].sunrise.tm_hour;
      this->_next_sunrise = fmt::format("{:0>2}:{:0>2}", this->_forecast_weather_info[0].sunrise.tm_hour, this->_forecast_weather_info[0].sunrise.tm_min);
      this->_next_sunset_hour = this->_forecast_weather_info[0].sunset.tm_hour;
      this->_next_sunset = fmt::format("{:0>2}:{:0>2}", this->_forecast_weather_info[0].sunset.tm_hour, this->_forecast_weather_info[0].sunset.tm_min);
      this->send_state_update();
    } else {
      SPDLOG_ERROR("Failed to load any forecast from OpenHAB weather.");
    }
  }
}

std::string MQTTManagerWeather::_get_icon_from_mapping(std::string &condition, uint8_t hour) {
  if (MqttManagerConfig::weather_controller.compare("home_assistant") == 0) {
    for (nlohmann::json mapping : MqttManagerConfig::icon_mapping["home_assistant_weather_mappings"]) {
      if (std::string(mapping["condition"]).compare(condition) == 0) {
        return std::string(mapping["character-mapping"]);
      }
    }
  } else if (MqttManagerConfig::weather_controller.compare("openhab") == 0) {
    for (nlohmann::json mapping : MqttManagerConfig::icon_mapping["accu_weather_mappings"]) {
      if (std::string(mapping["id"]).compare(condition) == 0) {
        if (mapping.contains("character-mapping")) {
          return std::string(mapping["character-mapping"]);
        } else if (mapping.contains("character-mapping-day") && hour >= MQTTManagerWeather::_next_sunrise_hour && hour <= MQTTManagerWeather::_next_sunset_hour) {
          return mapping["character-mapping-day"];
        } else if (mapping.contains("character-mapping-night") && (hour <= MQTTManagerWeather::_next_sunrise_hour || hour >= MQTTManagerWeather::_next_sunset_hour)) {
          return mapping["character-mapping-night"];
        } else {
          SPDLOG_ERROR("Found matching condition for {} but no icon-mapping!", condition);
          SPDLOG_ERROR("Matching condition current hour: {}, sunrise: {}, sunset: {}", hour, MQTTManagerWeather::_next_sunrise_hour, MQTTManagerWeather::_next_sunset_hour);
        }
      }
    }
  } else {
    SPDLOG_ERROR("Unknown controller {}.", MqttManagerConfig::weather_controller);
  }

  SPDLOG_ERROR("Couldn't find a mapping for condition {} using controller {}.", condition, MqttManagerConfig::weather_controller);
  return "";
}

void MQTTManagerWeather::openhab_temp_sensor_callback(nlohmann::json event_data) {
  std::string temp_data;
  if (std::string(event_data["type"]).compare("ItemStateChangedEvent") == 0) {
    nlohmann::json payload = nlohmann::json::parse(std::string(event_data["payload"]));
    temp_data = payload["value"];
  } else if (std::string(event_data["type"]).compare("ItemStateFetched") == 0) {
    temp_data = event_data["payload"]["state"];
  }

  if (temp_data.size() > 0) {
    this->_current_temperature = atof(temp_data.c_str());
    this->send_state_update();
  }
}

void MQTTManagerWeather::send_state_update() {
  nlohmann::json weather_info;
  std::list<nlohmann::json> forecast;
  for (struct weather_info info : this->_forecast_weather_info) {
    nlohmann::json forecast_data;
    if (MqttManagerConfig::weather_controller.compare("home_assistant") == 0) {
      forecast_data["icon"] = this->_get_icon_from_mapping(info.condition, info.time.tm_hour);
    } else if (MqttManagerConfig::weather_controller.compare("openhab") == 0) {
      std::string condition_id = std::to_string(info.condition_id);
      forecast_data["icon"] = this->_get_icon_from_mapping(condition_id, info.time.tm_hour);
    } else {
      SPDLOG_ERROR("Unknown weather controller {}. Will not set an icon!", MqttManagerConfig::weather_controller);
      weather_info["icon"] = "";
    }
    std::string pre = std::to_string((int)round(info.precipitation));
    pre.append(this->_precipitation_unit);
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
    wind.append(this->_windspeed_unit);
    forecast_data["wind"] = wind;
    forecast_data["day"] = info.day;
    forecast.push_back(forecast_data);
  }
  weather_info["forecast"] = forecast;
  if (MqttManagerConfig::weather_controller.compare("home_assistant") == 0) {
    weather_info["icon"] = this->_get_icon_from_mapping(this->_current_condition, this->_current_weather_time.tm_hour);
  } else if (MqttManagerConfig::weather_controller.compare("openhab") == 0) {
    std::string condition_id = std::to_string(this->_current_condition_id);
    weather_info["icon"] = this->_get_icon_from_mapping(condition_id, this->_current_weather_time.tm_hour);
  } else {
    SPDLOG_ERROR("Unknown weather controller {}. Will not set an icon!", MqttManagerConfig::weather_controller);
    weather_info["icon"] = "";
  }
  std::string temp = std::to_string((int)round(this->_current_temperature));
  temp.append("°");
  weather_info["temp"] = temp;

  std::string templow = std::to_string((int)round(this->_current_min_temperature));
  templow.append("°");
  std::string temphigh = std::to_string((int)round(this->_current_max_temperature));
  temphigh.append("°");
  std::string temp_display = temphigh;
  temp_display.append("/");
  temp_display.append(templow);
  weather_info["maxmin"] = temp_display;

  std::string wind = std::to_string((int)round(this->_current_wind_speed));
  wind.append(this->_windspeed_unit);
  weather_info["wind"] = wind;

  weather_info["sunrise"] = this->_next_sunrise;
  weather_info["sunset"] = this->_next_sunset;
  weather_info["prepro"] = fmt::format("{}%", this->_current_precipitation_probability);

  std::string new_weather_data = weather_info.dump();
  MQTT_Manager::publish("nspanel/status/weather", new_weather_data, true);
}
