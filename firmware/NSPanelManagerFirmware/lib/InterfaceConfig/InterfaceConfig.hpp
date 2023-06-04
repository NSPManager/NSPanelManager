#ifndef INTERFACECONFIG_HPP
#define INTERFACECONFIG_HPP

#include <Arduino.h>

class InterfaceConfig {
public:
  static inline uint8_t homeScreen = 0;
  static inline uint16_t colorTempMin = 2000;
  static inline uint16_t colorTempMax = 6000;
  static inline uint8_t raiseToMaxLightLevelAbove = 95;
  static inline bool reverseColorTempSlider = false;
  /// @brief Mimimum time for a button to be pressed to register as a press
  static inline uint16_t button_min_press_time;
  /// @brief Mimimum time for a button to be held to register as a long press
  static inline uint16_t button_long_press_time;
  /// @brief Mimimum time to hold a button to start special mode
  static inline uint16_t special_mode_trigger_time;
  /// @brief Time to exit special mode after no actions has been taken
  static inline uint16_t special_mode_release_time;
  /// @brief Time to ignore incomming MQTT messages after a command is sent out
  static inline uint16_t mqtt_ignore_time;
  /// @brief Ignore any MQTT state updates until this time is reached (millis() time)
  static inline unsigned long ignore_mqtt_state_events_until = 0;
};

#endif
