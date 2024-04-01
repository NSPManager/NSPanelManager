#include "entity/entity.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <string>
#include <vector>
#ifndef MQTT_MANAGER_WEATHER

class MQTTManagerWeather {
public:
  static void init();
  void update_config();
  void home_assistant_event_callback(nlohmann::json event_data);
  void openhab_current_weather_callback(nlohmann::json event_data);
  void openhab_forecast_weather_callback(nlohmann::json event_data);
  void openhab_temp_sensor_callback(nlohmann::json event_data);
  void send_state_update();

private:
  std::string _get_icon_from_mapping(std::string &condition, uint8_t hour);
  static void _update_forecast();
  static void _process_forecast_data();
  static void _update_current_weather();

  static inline std::thread _update_current_weather_thread;
  static inline std::thread _update_forecast_thread;

  struct weather_info {
    std::string condition;
    int condition_id;
    std::string day;
    std::tm time;
    std::tm sunrise;
    std::tm sunset;
    float precipitation;
    float precipitation_probability;
    float temperature_low;
    float temperature_high;
    float wind_speed;
  };

  std::vector<weather_info> _forecast_weather_info;
  std::string _windspeed_unit;
  std::string _precipitation_unit;
  std::string _current_condition;
  std::tm _current_weather_time;
  int _current_condition_id; // Only used for OpenHAB/OpenWeatherMap
  int _next_sunrise_hour;
  int _next_sunset_hour;
  std::string _next_sunrise;
  std::string _next_sunset;
  float _current_temperature;
  float _current_max_temperature;
  float _current_min_temperature;
  float _current_wind_speed;
  float _current_precipitation_probability;
};

#endif // !MQTT_MANAGER_WEATHER
