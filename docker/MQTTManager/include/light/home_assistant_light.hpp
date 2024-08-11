#ifndef MQTT_MANAGER_HOME_ASSISTANT_LIGHT
#define MQTT_MANAGER_HOME_ASSISTANT_LIGHT

#include "light.hpp"
#include "protobuf_general.pb.h"

enum MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE {
  TYPE_LIGHT,
  TYPE_SWITCH
};

class HomeAssistantLight : public Light {
public:
  HomeAssistantLight(LightSettings &init_data);
  void send_state_update_to_controller();
  void home_assistant_event_callback(nlohmann::json event_data);
  ~HomeAssistantLight();

private:
  std::string _home_assistant_name;
  MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE _home_assistant_light_type;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_LIGHT
