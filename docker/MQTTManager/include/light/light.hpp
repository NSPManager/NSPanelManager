#ifndef MQTT_MANAGER_LIGHT
#define MQTT_MANAGER_LIGHT

#include "protobuf_general.pb.h"
#include "room/room.hpp"
#include <boost/signals2.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <nlohmann/json.hpp>

enum MQTT_MANAGER_LIGHT_MODE {
  DEFAULT, // Normal, no special case.
  RGB      // Send updated values as HSV and not brightness and/or color_temperature.
};

class Light : public MqttManagerEntity {
public:
  Light(LightSettings &config);

  /**
   * Update the config of the light to reflect the new settings
   */
  void update_config(LightSettings &config);

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
   * Go through the requested states and compare them with the current states.
   * If there is any difference, send the updated values to the controller.
   */
  virtual void send_state_update_to_controller() = 0;

  /**
   * Get the current control mode of the light.
   */
  MQTT_MANAGER_LIGHT_MODE get_mode();

  /**
   * Post init, attach to room.
   */
  void post_init();

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

  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();

  ~Light();

protected:
  uint _id;
  std::string _name;
  uint16_t _room_id;
  Room *_room;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;

  bool _can_dim;
  bool _can_color_temperature;
  bool _can_rgb;

  bool _current_state;
  uint8_t _current_brightness;
  uint16_t _current_color_temperature;
  uint16_t _current_hue;
  uint8_t _current_saturation;
  MQTT_MANAGER_LIGHT_MODE _current_mode;

  bool _requested_state;
  uint8_t _requested_brightness;
  uint16_t _requested_color_temperature;
  uint16_t _requested_hue;
  uint8_t _requested_saturation;
  MQTT_MANAGER_LIGHT_MODE _requested_mode;

  std::string _mqtt_brightness_topic;
  std::string _mqtt_kelvin_topic;
  std::string _mqtt_hue_topic;
  std::string _mqtt_saturation_topic;

  boost::signals2::signal<void(Light *)> _light_destroyed_callbacks;
};

#endif // !MQTT_MANAGER_LIGHT
