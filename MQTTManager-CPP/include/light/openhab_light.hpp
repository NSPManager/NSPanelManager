#ifndef MQTT_MANAGER_OPENHAB_LIGHT
#define MQTT_MANAGER_OPENHAB_LIGHT

#include "light.hpp"

enum MQTT_MANAGER_OPENHAB_CONTROL_MODE {
  DIMMER,
  SWITCH
};

class OpenHabLight : public Light {
  OpenHabLight(nlohmann::json &init_data);

  /**
   * Turn on the light
   */
  void turn_on();

  /**
   * Turn off the light
   */
  void turn_off();

  /**
   * Set the dim level of a light. If the light is in RGB mode it will retain that mode
   * and set_hsb will be called with existing value for hue and saturation instead.
   * @param brightness: Hue level, 0 to 255
   */
  void set_brightness(uint8_t brightness);

  /**
   * Set the color temperature of a light. If the light is in RGB mode it will transission to normal mode.
   * @param color_tmperature: Color temperature in kelvin
   */
  void set_color_temperature(uint color_temperature);

  /**
   * Set the Hue, Saturation and Brightness value of the light.
   * @param hue: Hue level, 0 to 255
   * @param saturation: Hue level, 0 to 255
   * @param brightness: Hue level, 0 to 255
   */
  void set_hsb(uint8_t hue, uint8_t saturation, uint8_t brightness);

private:
  std::string _openhab_name;
  MQTT_MANAGER_OPENHAB_CONTROL_MODE _openhab_control_mode;
  std::string _openhab_on_off_item;            // Controls the item for on/off functioanlity
  std::string _openhab_item_color_temperature; // Item for chaning color temp
  std::string _openhab_item_rgb;               // Item for chaning color
};

#endif // !MQTT_MANAGER_OPENHAB_LIGHT
