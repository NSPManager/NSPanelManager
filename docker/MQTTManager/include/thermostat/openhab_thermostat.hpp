#ifndef MQTT_MANAGER_OPENHAB_THERMOSTAT_HPP
#define MQTT_MANAGER_OPENHAB_THERMOSTAT_HPP

#include "thermostat.hpp"

class OpenhabThermostat : public ThermostatEntity {
public:
  OpenhabThermostat(uint32_t thermostat_id);
  ~OpenhabThermostat();
  void reload_config();
  void send_state_update_to_controller();
  void openhab_target_temperature_event_callback(nlohmann::json event_data); // When target temperature changes
  void openhab_fan_mode_event_callback(nlohmann::json event_data);           // When fan mode changes
  void openhab_mode_event_callback(nlohmann::json event_data);               // When mode changes
  void openhab_preset_event_callback(nlohmann::json event_data);             // When preset changes
  void openhab_swing_event_callback(nlohmann::json event_data);              // When swing changes
  void openhab_swingh_event_callback(nlohmann::json event_data);             // When horizontal swing changes
  void command_callback(NSPanelMQTTManagerCommand &command);

private:
  std::string _openhab_target_temperature_item;
  std::string _openhab_fan_mode_item;
  std::string _openhab_mode_item;
  std::string _openhab_preset_item;
  std::string _openhab_swing_item;
  std::string _openhab_swingh_item;

  uint64_t _last_target_temperature_change;
  uint64_t _last_fan_mode_change;
  uint64_t _last_mode_change;
  uint64_t _last_preset_change;
  uint64_t _last_swing_change;
  uint64_t _last_swingh_change;
};

#endif
