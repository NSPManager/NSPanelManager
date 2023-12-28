#include "weather.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

void MQTTManagerWeather::update_config() {
  this->_controller = MqttManagerConfig::weather_controller;
  this->_entity = MqttManagerConfig::weather_entity;

  if (this->_controller.compare("homeassistant") == 0) {
    HomeAssistantManager::attach_event_observer(this);
    OpenhabManager::detach_event_observer(this);
  } else if (this->_controller.compare("openhab") == 0) {
    OpenhabManager::attach_event_observer(this);
    HomeAssistantManager::detach_event_observer(this);
  } else {
    SPDLOG_ERROR("Unsupported weather controller '{}'.", this->_controller);
  }
}

bool MQTTManagerWeather::home_assistant_event_callback(nlohmann::json &event_data) {
  if (std::string(event_data["event"]["data"]["entity_id"]).compare(this->_entity) == 0) {
    nlohmann::json new_state = event_data["event"]["data"]["new_state"];

    this->_forcast_weather_info.clear();
    for (nlohmann::json forcast : new_state["attributes"]["forecast"]) {
      weather_info info;
      SPDLOG_DEBUG("Loading weather forcast for {}", std::string(forcast["datetime"]));
      info.condition = forcast["condition"];
      info.wind_speed = forcast["wind_speed"];
      info.precipitation = forcast["precipitation"];
      info.precipitation_probability = forcast["precipitation_probability"];
      info.temperature_low = forcast["templow"];
      info.temperature_high = forcast["temperature"];

      // Get day of week
      std::string datetime = forcast["datetime"];
      std::vector<std::string> datetime_parts;
      size_t pos = datetime.find("T");
      std::string date = datetime.substr(0, pos);
      std::string year = date.substr(0, date.find("-"));
      date.erase(0, date.find("-") + 1);
      std::string month = date.substr(0, date.find("-"));
      date.erase(0, date.find("-") + 1);
      std::string day_of_month = date;

      std::tm tm = {01, 00, 00, atoi(day_of_month.c_str()), atoi(month.c_str()) - 1, atoi(year.c_str()) - 1900};
      std::time_t utc_time = std::mktime(&tm);
      const std::tm *localtime = std::localtime(&utc_time);

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

      this->_forcast_weather_info.push_back(info);
    }

    SPDLOG_DEBUG("Loaded forcast for {} days.", this->_forcast_weather_info.size());

    if (this->_forcast_weather_info.size() > 0) {
      this->_current_condition = this->_forcast_weather_info[0].condition;
      this->_current_precipitation_probability = this->_forcast_weather_info[0].precipitation_probability;
      this->_current_wind_speed = new_state["attributes"]["wind_speed"];
      this->_current_temperature = new_state["attributes"]["temperature"];
    } else {
      SPDLOG_ERROR("Failed to process forecast information from Home Assistant.");
      return true;
    }

    this->send_state_update();
    return true;
  }
  return false;
}

bool MQTTManagerWeather::openhab_event_callback(nlohmann::json &event_data) {
  return false;
}

std::string MQTTManagerWeather::_get_icon_from_mapping(std::string &condition) {
  if (this->_controller.compare("homeassistant") == 0) {
    for (nlohmann::json mapping : MqttManagerConfig::icon_mapping["home_assistant_weather_mappings"]) {
      if (std::string(mapping["condition"]).compare(condition) == 0) {
        return std::string(mapping["character-mapping"]);
      }
    }
  } else if (this->_controller.compare("openhab") == 0) {
    for (nlohmann::json mapping : MqttManagerConfig::icon_mapping["openweathermap_weather_mappings"]) {
      if (std::string(mapping["id"]).compare(condition) == 0) {
        return std::string(mapping["character-mapping"]);
      }
    }
  } else {
    SPDLOG_ERROR("Unknown controller {}.", this->_controller);
  }

  SPDLOG_ERROR("Couldn't find a mapping for condition {} using controller {}.", condition, this->_controller);
  return "";
}

void MQTTManagerWeather::send_state_update() {
  nlohmann::json weather_info;
  std::list<nlohmann::json> forcast;
  for (struct weather_info info : this->_forcast_weather_info) {
    nlohmann::json forcast_data;
    forcast_data["icon"] = this->_get_icon_from_mapping(info.condition);
    std::string pre = std::to_string((int)(info.precipitation + 0.5));
    pre.append(this->_precipitation_unit);
    forcast_data["pre"] = pre;
    std::string prepro = std::to_string((int)(info.precipitation_probability + 0.5));
    prepro.append("%");
    forcast_data["prepro"] = prepro;

    std::string templow = std::to_string((int)(info.temperature_low + 0.5));
    templow.append("°");
    std::string temphigh = std::to_string((int)(info.temperature_high + 0.5));
    temphigh.append("°");
    std::string temp_display = temphigh;
    temp_display.append("/");
    temp_display.append(templow);
    forcast_data["maxmin"] = temp_display;

    std::string wind = std::to_string((int)(info.wind_speed + 0.5));
    wind.append(this->_windspeed_unit);
    forcast_data["wind"] = wind;
    forcast_data["day"] = info.day;
    forcast.push_back(forcast_data);
  }
  weather_info["forcast"] = forcast;
  weather_info["icon"] = this->_get_icon_from_mapping(this->_current_condition);
  std::string temp = std::to_string((int)(this->_current_temperature + 0.5));
  temp.append("°");
  weather_info["temp"] = temp;
  std::string wind = std::to_string((int)(this->_current_wind_speed + 0.5));
  wind.append(this->_windspeed_unit);
  weather_info["wind"] = wind;

  MQTT_Manager::publish("nspanel/status/weather", weather_info.dump(), true);
}
