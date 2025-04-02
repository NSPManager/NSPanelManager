#ifndef MQTT_MANAGER_HOME_ASSISTANT_SWITCH
#define MQTT_MANAGER_HOME_ASSISTANT_SWITCH

#include "protobuf_general.pb.h"
#include "switch.hpp"
#include <nlohmann/json.hpp>

class HomeAssistantSwitch : public SwitchEntity {
public:
  HomeAssistantSwitch(uint32_t switch_id);
  ~HomeAssistantSwitch();
  void send_state_update_to_controller();
  void home_assistant_event_callback(nlohmann::json event_data);

private:
  std::string _home_assistant_name;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_SWITCH
