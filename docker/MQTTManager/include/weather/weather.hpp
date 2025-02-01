#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>
#ifndef MQTT_MANAGER_WEATHER

class MQTTManagerWeather {
public:
  /**
   * Will initialize the weather manager and start the event loop.
   */
  static void start();

  /**
   * Load new values from config.
   */
  static void reload_config();

  /**
   * Trigger for events from Home Assistant.
   */
  static void home_assistant_event_callback(nlohmann::json event_data);

  /**
   * Trigger for events from OpenHAB.
   */
  static void openhab_temp_sensor_callback(nlohmann::json event_data);

  /**
   * Actually send the weather out over MQTT to all panels.
   */
  static void send_state_update();

private:
  static inline std::thread _instance;

  static void _run_weather_thread();

  /**
   * Will pull new data from Open Meteo and if successfull it will automatically
   * process the data and send it out to the panels.
   */
  static void _pull_new_weather_data();

  /**
   * Process new weather data and push out to the panels.
   */
  static void _process_weather_data(std::string &data);

  static std::string _get_icon_from_mapping(std::string &condition, uint8_t hour, bool allow_night_icon);

  struct weather_info {
    std::string condition;
    int condition_id; // Only used for OpenHAB/OpenWeatherMap
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

  /**
   * Used to make sure that updates from different threads (mostly MQTT thread and the weather update thread)
   * don't try to access values at the same time.
   */
  static inline std::mutex _weater_data_mutex;

  static inline std::vector<weather_info> _forecast_weather_info;
  static inline std::string _windspeed_unit;
  static inline std::string _current_condition;
  static inline std::tm _current_weather_time;
  static inline int _next_sunrise_hour;
  static inline int _next_sunset_hour;
  static inline std::string _next_sunrise;
  static inline std::string _next_sunset;
  static inline float _current_temperature;
  static inline float _current_max_temperature;
  static inline float _current_min_temperature;
  static inline float _current_wind_speed;
  static inline float _current_precipitation_probability;

  static inline std::string _outside_temperature_sensor_provider;
  static inline std::string _outside_temperature_sensor_entity_id;
  static inline std::string _location_latitude;
  static inline std::string _location_longitude;
  static inline std::string _wind_speed_format;
  static inline std::string _precipitation_unit;
  static inline uint32_t _update_interval_minutes;
  static inline std::string _clock_format;
  static inline std::string _temperature_unit;
};

#endif // !MQTT_MANAGER_WEATHER
