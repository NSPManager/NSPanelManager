#ifndef INTERFACE_LIGHT_PAGE_HPP
#define INTERFACE_LIGHT_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
class Light;
#include <PageBase.hpp>

enum LIGHT_PAGE_MODE {
  COLOR_TEMP,
  COLOR_RGB
};

class LightPage : public PageBase, DeviceEntityObserver {
public:
  void update();
  void updateValues();
  void show();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);
  uint8_t getBrightnessValue();
  uint8_t getKelvinSatValue();
  uint16_t getHueValue();
  Light *selectedLight;
  LIGHT_PAGE_MODE getCurrentMode();
  void switchMode();
  void lightDeconstructCallback(Light *);

  void entityDeconstructCallback(DeviceEntity *);
  void entityUpdateCallback(DeviceEntity *);

private:
  LIGHT_PAGE_MODE _currentMode;
};

#endif
