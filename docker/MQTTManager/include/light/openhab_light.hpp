#ifndef MQTT_MANAGER_OPENHAB_LIGHT
#define MQTT_MANAGER_OPENHAB_LIGHT

#include "light.hpp"
#include "protobuf_general.pb.h"

enum MQTT_MANAGER_OPENHAB_CONTROL_MODE {
  DIMMER,
  SWITCH
};

class OpenhabLight : public Light {
public:
  OpenhabLight(uint32_t light_id);

  /**
   * Go through the requested states and compare them with the current states.
   * If there is any difference, send the updated values to the controller.
   */
  void send_state_update_to_controller();

  /**
   * Process event data.
   */
  void openhab_event_callback(nlohmann::json event_data);

  ~OpenhabLight();

private:
  // Mutex to only allow one thread to access the openhab items at a time
  std::mutex _openhab_items_mutex;
  std::string _openhab_name;
  MQTT_MANAGER_OPENHAB_CONTROL_MODE _openhab_control_mode;
  std::string _openhab_on_off_item;            // Controls the item for on/off functioanlity
  std::string _openhab_item_color_temperature; // Item for chaning color temp
  std::string _openhab_item_rgb;               // Item for chaning color

  std::mutex _openhab_group_brightness_item_state_changed_event_mutex;
  uint64_t _last_group_brightness_item_state_changed_event_ms;           // Last time a Group Item state changed event was received
  nlohmann::json _last_group_brightness_item_state_changed_event_data;   // Last Group Item state changed event data
  std::thread _openhab_group_brightness_item_state_changed_event_thread; // Event to wait for Group Item state changed events to settle before processing event data
  void _openhab_group_brightness_item_state_changed_event_thread_func();
  bool _openhab_group_brightness_item_state_changed_event_thread_running;

  std::mutex _openhab_group_color_temp_item_state_changed_event_mutex;
  uint64_t _last_group_color_temp_item_state_changed_event_ms;           // Last time a Group Item state changed event was received
  nlohmann::json _last_group_color_temp_item_state_changed_event_data;   // Last Group Item state changed event data
  std::thread _openhab_group_color_temp_item_state_changed_event_thread; // Event to wait for Group Item state changed events to settle before processing event data
  void _openhab_group_color_temp_item_state_changed_event_thread_func();
  bool _openhab_group_color_temp_item_state_changed_event_thread_running;

  std::mutex _openhab_group_rgb_item_state_changed_event_mutex;
  uint64_t _last_group_rgb_item_state_changed_event_ms;           // Last time a Group Item state changed event was received
  nlohmann::json _last_group_rgb_item_state_changed_event_data;   // Last Group Item state changed event data
  std::thread _openhab_group_rgb_item_state_changed_event_thread; // Event to wait for Group Item state changed events to settle before processing event data
  void _openhab_group_rgb_item_state_changed_event_thread_func();
  bool _openhab_group_rgb_item_state_changed_event_thread_running;

  uint64_t _last_on_off_change;
  uint64_t _last_brightness_change;
  uint64_t _last_color_temp_change;
  uint64_t _last_rgb_change;
  uint64_t _last_light_mode_change;
};

#endif // !MQTT_MANAGER_OPENHAB_LIGHT
