#ifndef INTERFACE_LIGHT_PAGE_HPP
#define INTERFACE_LIGHT_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
#include <PageBase.hpp>
#include <protobuf_defines.h>

enum LIGHT_PAGE_MODE {
  COLOR_TEMP,
  COLOR_RGB
};

class LightPage : public PageBase {
public:
  void update();
  void updateValues();
  void show();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);
  uint8_t getBrightnessValue();
  uint8_t getKelvinSatValue();
  uint16_t getHueValue();
  PROTOBUF_NSPANEL_LIGHT_STATUS *selectedLight;
  LIGHT_PAGE_MODE getCurrentMode();
  void switchMode();

private:
  LIGHT_PAGE_MODE _currentMode;
  uint8_t _last_brightness = 0;
  uint8_t _last_kelvin_saturation = 0;
  uint16_t _last_hue = 0;
};

#endif
