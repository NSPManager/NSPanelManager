#ifndef MQTT_MANAGER_SWITCH
#define MQTT_MANAGER_SWITCH

#include "protobuf_nspanel.pb.h"
#include <boost/signals2.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

class SwitchEntity : public MqttManagerEntity {
public:
  SwitchEntity(uint32_t switch_id);

  /**
   * Update the config of the switch from DB.
   */
  void reload_config();

  /**
   * Get the room ID of the switch.
   */
  uint16_t get_room_id();

  /**
   * Turn on the switch
   */
  void turn_on(bool send_update);

  /**
   * Turn off the switch
   */
  void turn_off(bool send_update);

  /**
   * Get the on/off state of the switch.
   */
  bool get_state();

  /**
   * Get the ID of the switch.
   */
  uint16_t get_id();

  /**
   * Get the friendly name for the switch.
   */
  std::string get_name();

  /**
   * Get the ID of the entity page this entity is placed on.
   */
  uint32_t get_entity_page_id();

  /**
   * Get the slot in which this entity is placed on the given entity page.
   */
  uint8_t get_entity_page_slot();

  /**
   * Go through the requested states and compare them with the current states.
   * If there is any difference, send the updated values to the controller.
   */
  virtual void send_state_update_to_controller() = 0;

  /**
   * Attach a callback for when the switch is destroyed
   */
  void attach_delete_callback(void (*callback)(SwitchEntity *switch_entity));

  /**
   * Detach a callback for when the switch is destroyed
   */
  void detach_delete_callback(void (*callback)(SwitchEntity *switch_entity));

  /**
   * Set all requested values equal to current values.
   */
  void reset_requests();

  /**
   * Callback for NSPanelMQTTManagerCommand protobuf received from MQTT
   */
  void command_callback(NSPanelMQTTManagerCommand &command);

  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();
  bool can_toggle();
  void toggle();
  std::string_view get_icon();
  uint16_t get_icon_color();
  uint16_t get_icon_active_color();
  std::string get_mqtt_state_topic();

  ~SwitchEntity();

protected:
  uint _id;
  std::string _name;
  uint16_t _room_id;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;
  uint32_t _entity_page_id;
  uint8_t _entity_page_slot;

  bool _current_state;
  bool _requested_state;

  boost::signals2::signal<void(SwitchEntity *)> _switch_destroyed_callbacks;
};

#endif // !MQTT_MANAGER_SWITCH
