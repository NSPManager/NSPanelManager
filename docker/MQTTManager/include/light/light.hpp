#ifndef MQTT_MANAGER_LIGHT
#define MQTT_MANAGER_LIGHT

#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include <boost/signals2.hpp>
#include <chrono>
#include <cstdint>
#include <entity/entity.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

enum MQTT_MANAGER_LIGHT_MODE {
  DEFAULT, // Normal, no special case.
  RGB      // Send updated values as HSV and not brightness and/or color_temperature.
};

enum MQTT_MANAGER_LIGHT_TYPE {
  CEILING,
  TABLE
};

class Light : public MqttManagerEntity {
public:
  Light(uint32_t light_id);

  /**
   * Update the config of the light from DB.
   */
  void reload_config();

  /**
   * Get the room ID of the light.
   */
  uint16_t get_room_id();

  /**
   * Get light type (ceiling or table)
   */
  MQTT_MANAGER_LIGHT_TYPE get_light_type();

  /**
   * Turn on the light
   */
  void turn_on(bool send_update);

  /**
   * Turn off the light
   */
  void turn_off(bool send_update);

  /**
   * Get the on/off state of the light.
   */
  bool get_state();

  /**
   * Set the dim level of a light. If the light is in RGB mode it will retain that mode
   * and set_hsb will be called with existing value for hue and saturation instead.
   * @param brightness: Light brightness 0-100
   * @param send_update: Wether to send the update to the controller straight away or not.
   */
  void set_brightness(uint8_t brightness, bool send_update);

  /**
   * Get the current brightness value of the light.
   */
  uint8_t get_brightness();

  /**
   * Set the color temperature of a light. If the light is in RGB mode it will transission to normal mode.
   * @param color_tmperature: Color temperature in kelvin
   * @param send_update: Wether to send the update to the controller straight away or not.
   */
  void set_color_temperature(uint color_temperature, bool send_update);

  /**
   * Get the color temperature of the light.
   */
  uint get_color_temperature();

  /**
   * Set the Hue value of the light.
   * @param hue: Hue level, 0 to 359
   * @param send_update: Wether to send the update to the controller straight away or not.
   */
  void set_hue(uint16_t hue, bool send_update);

  /**
   * Get the Hue value of the light.
   */
  uint16_t get_hue();

  /**
   * Set the Saturation value of the light.
   * @param saturation: Saturation level, 0 to 100
   * @param send_update: Wether to send the update to the controller straight away or not.
   */
  void set_saturation(uint8_t saturation, bool send_update);

  /**
   * Get the Saturation value of the light.
   */
  uint8_t get_saturation();

  /**
   * Set the Hue, Saturation and Brightness value of the light.
   * @param hue: Hue level, 0 to 100
   * @param saturation: Hue level, 0 to 100
   * @param brightness: Hue level, 0 to 100
   * @param send_update: Wether to send the update to the controller straight away or not.
   */
  void set_hsb(uint16_t hue, uint8_t saturation, uint8_t brightness, bool send_update);

  /**
   * Get the ID of the light.
   */
  uint16_t get_id();

  /**
   * Get the friendly name for the light.
   */
  std::string get_name();

  /**
   * Is this light controlled from the NSPanel main page?
   */
  bool get_controlled_from_main_page();

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
   * Get the current control mode of the light.
   */
  MQTT_MANAGER_LIGHT_MODE get_mode();

  /**
   * Is the light dimmable?
   */
  bool can_dim();

  /**
   * Can the light change color temperature?
   */
  bool can_color_temperature();

  /**
   * Can the light do RGB colors?
   */
  bool can_rgb();

  /**
   * Attach a callback for when the light is destroyed
   */
  void attach_delete_callback(void (*callback)(Light *light));

  /**
   * Detach a callback for when the light is destroyed
   */
  void detach_delete_callback(void (*callback)(Light *light));

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

  ~Light();

protected:
  uint _id;
  std::string _name;
  uint16_t _room_id;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;
  bool _controlled_from_main_page;
  uint32_t _entity_page_id;
  uint8_t _entity_page_slot;

  bool _can_dim;
  bool _can_color_temperature;
  bool _can_rgb;

  bool _current_state;
  uint8_t _current_brightness;
  uint16_t _current_color_temperature;
  uint16_t _current_hue;
  uint8_t _current_saturation;
  MQTT_MANAGER_LIGHT_MODE _current_mode;
  MQTT_MANAGER_LIGHT_TYPE _light_type;

  bool _requested_state;
  uint8_t _requested_brightness;
  uint16_t _requested_color_temperature;
  uint16_t _requested_hue;
  uint8_t _requested_saturation;
  MQTT_MANAGER_LIGHT_MODE _requested_mode;

  boost::signals2::signal<void(Light *)> _light_destroyed_callbacks;
};

#endif // !MQTT_MANAGER_LIGHT
