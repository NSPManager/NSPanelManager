#ifndef MQTT_MANAGER_HOME_ASSISTANT_LIGHT
#define MQTT_MANAGER_HOME_ASSISTANT_LIGHT

#include "home_assistant_manager/home_assistant_manager.hpp"
#include "light.hpp"
#include <cstdint>

enum MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE {
  TYPE_LIGHT,
  TYPE_SWITCH
};

class HomeAssistantLight : public Light, public HomeAssistantEventObserver {
public:
  HomeAssistantLight(nlohmann::json &init_data);
  void send_state_update_to_controller();
  bool home_assistant_event_callback(nlohmann::json &event_data);
  ~HomeAssistantLight();

private:
  std::string _home_assistant_name;
  MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE _home_assistant_light_type;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_LIGHT
