#ifndef INTERFACECONFIG_HPP
#define INTERFACECONFIG_HPP

#include <Arduino.h>
#include <InterfaceConfigEnums.hpp>
#include <vector>

enum DEFAULT_PAGE {
  MAIN_PAGE,
  SCENES_PAGE,
  ROOM_PAGE
};

class Scene;

class InterfaceConfig {
public:
  static inline uint16_t homeScreen = 0;
  static inline DEFAULT_PAGE default_page;
  static inline bool lock_to_default_room = false;
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
  /// @brief Screen dim level.
  static inline uint8_t screen_dim_level = 100;
  /// @brief Screensaver dim level.
  static inline uint8_t screensaver_dim_level = 0;
  /// @brief Time before screensaver is activated. Time in milliseconds.
  static inline unsigned long screensaver_activation_timeout = 30000;
  /// @brief Wether or not to show clock on screensaver.
  static inline bool show_screensaver_clock = false;
  /// @brief Wether or not to a background on screensaver.
  static inline bool show_screensaver_background = false;
  /// @brief Show clock in US style. AM/PM?
  static inline bool clock_us_style = false;
  /// @brief All the global scenes.
  static inline std::vector<Scene *> global_scenes;
  static Scene *getSceneById(uint16_t id);
  static inline roomMode currentRoomMode;
  static inline editLightMode currentEditLightMode;
  static inline editLightMode _triggerSpecialLightMode;
};

#endif
