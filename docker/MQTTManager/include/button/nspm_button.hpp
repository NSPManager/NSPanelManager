#ifndef MQTT_MANAGER_NSPM_BUTTON
#define MQTT_MANAGER_NSPM_BUTTON

#include "button.hpp"

class NSPMButton : public ButtonEntity {
public:
  NSPMButton(uint32_t button_id);
  ~NSPMButton();
  void send_state_update_to_controller();

private:
  std::string _home_assistant_name;
};

#endif // !MQTT_MANAGER_NSPM_BUTTON
