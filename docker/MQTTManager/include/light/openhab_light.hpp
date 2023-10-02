#ifndef MQTT_MANAGER_OPENHAB_LIGHT
#define MQTT_MANAGER_OPENHAB_LIGHT

#include "light.hpp"
#include "openhab_manager/openhab_manager.hpp"

enum MQTT_MANAGER_OPENHAB_CONTROL_MODE {
  DIMMER,
  SWITCH
};

class OpenhabLight : public Light, public OpenhabEventObserver {
public:
  OpenhabLight(nlohmann::json &init_data);

  /**
   * Go through the requested states and compare them with the current states.
   * If there is any difference, send the updated values to the controller.
   */
  void send_state_update_to_controller();

  /**
   * Process event data. If the event was processed by the given instance, return true, else return false.
   * In case a false is returned the loop will continue until all registered entities has been checked.
   */
  bool openhab_event_callback(nlohmann::json &event_data);

private:
  std::string _openhab_name;
  MQTT_MANAGER_OPENHAB_CONTROL_MODE _openhab_control_mode;
  std::string _openhab_on_off_item;            // Controls the item for on/off functioanlity
  std::string _openhab_item_color_temperature; // Item for chaning color temp
  std::string _openhab_item_rgb;               // Item for chaning color

  uint64_t _last_on_off_change;
  uint64_t _last_brightness_change;
  uint64_t _last_color_temp_change;
  uint64_t _last_rgb_change;
  uint64_t _last_light_mode_change;
};

#endif // !MQTT_MANAGER_OPENHAB_LIGHT
