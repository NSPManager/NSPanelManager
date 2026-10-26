#ifndef MQTT_MANAGER_HOME_ASSISTANT_COVER_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_COVER_HPP

#include "cover.hpp"
#include <nlohmann/json.hpp>
#include <string>

class HomeAssistantCover : public CoverEntity {
public:
  HomeAssistantCover(uint32_t cover_id);
  ~HomeAssistantCover();
  void reload_config();
  void home_assistant_event_callback(nlohmann::json event_data);

  void open();
  void close();
  void stop();
  void set_position(uint8_t position);
  void open_tilt();
  void close_tilt();
  void stop_tilt();
  void set_tilt_position(uint8_t position);

private:
  /**
   * Load the Home Assistant entity id from entity_data.
   * @return true on success.
   */
  bool _load_home_assistant_config();

  /**
   * Call a "cover" domain service in Home Assistant.
   */
  void _call_service(const std::string &service, nlohmann::json service_data = nlohmann::json::object());

  std::string _home_assistant_name;

  // Home Assistant CoverEntityFeature bits.
  static constexpr uint32_t feature_open = 1;
  static constexpr uint32_t feature_close = 2;
  static constexpr uint32_t feature_set_position = 4;
  static constexpr uint32_t feature_stop = 8;
  static constexpr uint32_t feature_open_tilt = 16;
  static constexpr uint32_t feature_close_tilt = 32;
  static constexpr uint32_t feature_stop_tilt = 64;
  static constexpr uint32_t feature_set_tilt_position = 128;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_COVER_HPP
