#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#define BUTTON_MANAGER_BUTTON_DEBOUNCE_MS 50
#define BUTTON_MANAGER_BUTTON1_PIN 14
#define BUTTON_MANAGER_BUTTON2_PIN 27
#define BUTTON_MANAGER_RELAY1_PIN 22
#define BUTTON_MANAGER_RELAY2_PIN 19

#include <Arduino.h>
// #include <Light.hpp>
class Light;

class ButtonManager {
public:
  static void init();
  static void mqttCallback(char *topic, byte *payload, unsigned int length);
  static inline Light *button1_detached_mode_light;
  static inline Light *button2_detached_mode_light;
  static void setRelayState(uint8_t relay, bool state);
  static bool getRelayState(uint8_t relay);

private:
  static void _loop(void *param);
  static inline TaskHandle_t _loop_task_handle;
  static void _processButtonStateChange(uint8_t button, bool new_state);

  static void button1_state_change();
  static void button2_state_change();

  // Which button was changed. 1 or 2
  static inline uint8_t _button_state_change;
  static inline bool _lastButton1State;
  static inline bool _newButton1State;
  static inline unsigned long _lastButton1StateChange;
  static inline bool _lastButton2State;
  static inline bool _newButton2State;
  static inline unsigned long _lastButton2StateChange;
};

#endif
