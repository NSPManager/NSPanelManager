#ifndef MQTT_MANAGER_HOMEY_SWITCH
#define MQTT_MANAGER_HOMEY_SWITCH

#include "switch.hpp"
#include <string>

class HomeySwitch : public SwitchEntity
{
public:
    HomeySwitch(uint32_t switch_id);
    ~HomeySwitch();
    void send_state_update_to_controller();
    void homey_event_callback(nlohmann::json event_data);

private:
    std::string _homey_device_id;
};

#endif // !MQTT_MANAGER_HOMEY_SWITCH
