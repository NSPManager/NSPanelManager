#include "entity/entity.hpp"
#include "home_assistant_manager/home_assistant_manager.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include <string>
#include <vector>
#ifndef MQTT_MANAGER_WEATHER

class MQTTManagerWeather : public HomeAssistantEventObserver, public OpenhabEventObserver {
public:
  void update_config();
  bool home_assistant_event_callback(nlohmann::json &event_data);
  bool openhab_event_callback(nlohmann::json &event_data);
  void send_state_update();

private:
  std::string _get_icon_from_mapping(std::string &condition);

  struct weather_info {
    std::string condition;
    std::string day;
    float precipitation;
    float precipitation_probability;
    float temperature_low;
    float temperature_high;
    float wind_speed;
  };

  std::vector<weather_info> _forcast_weather_info;
  std::string _windspeed_unit;
  std::string _precipitation_unit;
  std::string _current_condition;
  float _current_temperature;
  float _current_wind_speed;
  float _current_precipitation_probability;
};

#endif // !MQTT_MANAGER_WEATHER
