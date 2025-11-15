#ifndef MQTT_MANAGER_HOMEY_BUTTON
#define MQTT_MANAGER_HOMEY_BUTTON

#include "button.hpp"
#include <string>

class HomeyButton : public ButtonEntity
{
public:
    HomeyButton(uint32_t button_id);
    ~HomeyButton();
    void send_state_update_to_controller();
    void homey_event_callback(nlohmann::json event_data);

private:
    std::string _homey_device_id;
};

#endif // !MQTT_MANAGER_HOMEY_BUTTON
