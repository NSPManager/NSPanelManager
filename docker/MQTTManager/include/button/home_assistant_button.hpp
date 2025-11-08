#ifndef MQTT_MANAGER_HOME_ASSISTANT_BUTTON
#define MQTT_MANAGER_HOME_ASSISTANT_BUTTON

#include "button.hpp"

class HomeAssistantButton : public ButtonEntity {
public:
  HomeAssistantButton(uint32_t button_id);
  ~HomeAssistantButton();
  void send_state_update_to_controller();

private:
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_BUTTON
