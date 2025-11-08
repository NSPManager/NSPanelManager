#ifndef MQTT_MANAGER_OPENHAB_SWITCH
#define MQTT_MANAGER_OPENHAB_SWITCH

#include "protobuf_general.pb.h"
#include "switch.hpp"

class OpenhabSwitch : public SwitchEntity {
public:
  OpenhabSwitch(uint32_t switch_id);

  /**
   * Go through the requested states and compare them with the current states.
   * If there is any difference, send the updated values to the controller.
   */
  void send_state_update_to_controller();

  /**
   * Process event data.
   */
  void openhab_event_callback(nlohmann::json event_data);

  ~OpenhabSwitch();

private:
  // Mutex to only allow one thread to access the openhab items at a time
  std::mutex _openhab_items_mutex;
  std::string _openhab_name;
  std::string _openhab_on_off_item; // Controls the item for on/off functioanlity

  uint64_t _last_on_off_change;
};

#endif // !MQTT_MANAGER_OPENHAB_SWITCH
