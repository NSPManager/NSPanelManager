#ifndef MQTT_MANAGER_HOMEY_LIGHT
#define MQTT_MANAGER_HOMEY_LIGHT

#include "light.hpp"
#include <string>
#include <vector>

class HomeyLight : public Light
{
public:
    HomeyLight(uint32_t light_id);
    ~HomeyLight();
    void send_state_update_to_controller();
    void homey_event_callback(nlohmann::json event_data);

private:
    std::string _homey_device_id;
    std::vector<std::string> _capabilities;

    // Helper methods for capability control
    void _send_capability_update(const std::string &capability, nlohmann::json value);
    bool _has_capability(const std::string &capability);
};

#endif // !MQTT_MANAGER_HOMEY_LIGHT
