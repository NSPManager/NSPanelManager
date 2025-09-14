#ifndef MQTT_MANAGER_HOME_ASSISTANT_THERMOSTAT_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_THERMOSTAT_HPP

#include "thermostat.hpp"

class HomeAssistantThermostat : public ThermostatEntity {
public:
  HomeAssistantThermostat(uint32_t thermostat_id);
  ~HomeAssistantThermostat();
  void send_state_update_to_controller();
  void home_assistant_event_callback(nlohmann::json event_data);
  void command_callback(NSPanelMQTTManagerCommand &command);

private:
  std::string _home_assistant_name;
};
#endif
