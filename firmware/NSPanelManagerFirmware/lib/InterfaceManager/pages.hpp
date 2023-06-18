#ifndef LIB_INTERFACEMANAGER_PAGES_H_
#define LIB_INTERFACEMANAGER_PAGES_H_

#include <Arduino.h>
#include <vector>
class Light;
class Scene;

class RoomPage {
public:
  static void setLightVisibility(uint8_t position, bool visibility);
  static void setLightName(uint8_t position, const char *name);
  static void setLightState(uint8_t position, bool state);
  static void setCurrentRoomLabel(const char *label);
  static inline Light *_selectedLight;
};

class ScenePage {
public:
  static void show();
  static void setRoomLabelText(const char *text);
  static void showScenes(std::vector<Scene *> &scenes);
  static void processTouchEvent(uint8_t page, uint8_t component, bool pressed);
};

#endif /* LIB_INTERFACEMANAGER_PAGES_H_ */
