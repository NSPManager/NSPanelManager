#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#define BUTTON_MANAGER_BUTTON_DEBOUNCE_MS 50
#define BUTTON_MANAGER_BUTTON1_PIN 14
#define BUTTON_MANAGER_BUTTON2_PIN 27
#define BUTTON_MANAGER_RELAY1_PIN 22
#define BUTTON_MANAGER_RELAY2_PIN 19


#include <Arduino.h>

class ButtonManager {
public:
  static void init();
  static void mqttCallback(char *topic, byte *payload, unsigned int length);
private:
  static void _loop(void* param);
  static void _setRelayState(uint8_t relay, bool state);
  
  static inline bool _lastButton1State;
  static inline bool _newButton1State;
  static inline unsigned long _lastButton1StateChange;
  static inline bool _lastButton2State;
  static inline bool _newButton2State;
  static inline unsigned long _lastButton2StateChange;
};

#endif
