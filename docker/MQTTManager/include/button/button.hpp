#ifndef MQTT_MANAGER_BUTTON
#define MQTT_MANAGER_BUTTON

#include "protobuf_nspanel.pb.h"
#include <boost/signals2.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

class ButtonEntity : public MqttManagerEntity {
public:
  ButtonEntity(uint32_t button_id);

  /**
   * Update the config of the button from DB.
   */
  void reload_config();

  /**
   * Get the room ID of the button.
   */
  uint16_t get_room_id();

  /**
   * Turn on the button
   */
  void press(bool send_update);

  /**
   * Get the ID of the button.
   */
  uint16_t get_id();

  /**
   * Get the on/off state of the switch.
   */
  bool get_state();

  /**
   * Get the friendly name for the button.
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
   * Attach a callback for when the button is destroyed
   */
  void attach_delete_callback(void (*callback)(ButtonEntity *button_entity));

  /**
   * Detach a callback for when the button is destroyed
   */
  void detach_delete_callback(void (*callback)(ButtonEntity *button_entity));

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
  bool can_toggle(); // Return true, we use toggle as a "press" event.
  void toggle();
  std::string_view get_icon();
  uint16_t get_icon_color();
  uint16_t get_icon_active_color();
  std::string get_mqtt_state_topic();

  ~ButtonEntity();

protected:
  uint _id;
  std::string _name;
  uint16_t _room_id;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;
  uint32_t _entity_page_id;
  uint8_t _entity_page_slot;
  std::mutex _entity_data_mutex;
  nlohmann::json _entity_data;

  bool _current_state;
  bool _requested_state;

  boost::signals2::signal<void(ButtonEntity *)> _button_destroyed_callbacks;
};

#endif // !MQTT_MANAGER_BUTTON
