#ifndef MQTT_MANAGER_HOMEY_THERMOSTAT_HPP
#define MQTT_MANAGER_HOMEY_THERMOSTAT_HPP

#include "thermostat.hpp"
#include <string>

class HomeyThermostat : public ThermostatEntity
{
public:
    HomeyThermostat(uint32_t thermostat_id);
    ~HomeyThermostat();
    void send_state_update_to_controller();
    void command_callback(NSPanelMQTTManagerCommand &command);

private:
    std::string _homey_device_id;
    void _send_capability_update(const std::string &capability, const nlohmann::json &value);
};

#endif // MQTT_MANAGER_HOMEY_THERMOSTAT_HPP
