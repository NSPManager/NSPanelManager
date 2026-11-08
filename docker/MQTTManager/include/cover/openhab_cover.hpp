#ifndef MQTT_MANAGER_OPENHAB_COVER_HPP
#define MQTT_MANAGER_OPENHAB_COVER_HPP

#include "cover.hpp"
#include <nlohmann/json.hpp>
#include <string>

class OpenhabCover : public CoverEntity {
public:
  OpenhabCover(uint32_t cover_id);
  ~OpenhabCover();
  void reload_config();
  void openhab_event_callback(nlohmann::json event_data);

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
   * Load the OpenHAB item name from entity_data.
   * @return true on success.
   */
  bool _load_openhab_config();

  /**
   * Send an ItemCommandEvent to OpenHAB.
   */
  void _send_command(const std::string &type, const std::string &value);

  /**
   * Update the panel state from a Rollershutter percentage state.
   */
  void _update_from_state(const nlohmann::json &state);

  std::string _openhab_item;
};

#endif // !MQTT_MANAGER_OPENHAB_COVER_HPP
