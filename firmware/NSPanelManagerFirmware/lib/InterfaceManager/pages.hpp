/*
 * pages.h
 *
 *  Created on: Mar 6, 2023
 *      Author: Tim Panajott
 */

#ifndef LIB_INTERFACEMANAGER_PAGES_H_
#define LIB_INTERFACEMANAGER_PAGES_H_

#include <Arduino.h>
#include <InterfaceManager.hpp>

enum LIGHT_PAGE_MODE {
  COLOR_TEMP,
  COLOR_RGB
};

class HomePage {
public:
  static void setDimmingValue(uint8_t value);
  static int getDimmingValue();
  static void updateDimmerValueCache();
  static void setColorTempValue(uint8_t value);
  static int getColorTempValue();
  static void updateColorTempValueCache();
  static void setCeilingBrightnessLabelText(uint8_t value);
  static void setTableBrightnessLabelText(uint8_t value);
  static void setCeilingLightsState(bool state);
  static void setTableLightsState(bool state);
  static void setSliderLightLevelColor(uint color);
  static void setSliderColorTempColor(uint color);
  static void setHighlightCeilingVisibility(bool visable);
  static void setHighlightTableVisibility(bool visable);

private:
  static inline uint8_t _dimmerValue;
  static inline uint8_t _colorTemp;
};

class RoomPage {
public:
  static void setLightVisibility(uint8_t position, bool visibility);
  static void setLightName(uint8_t position, const char *name);
  static void setLightState(uint8_t position, bool state);
  static void setCurrentRoomLabel(const char *label);
  static inline lightConfig *_selectedLight;
};

class LightPage {
public:
  static void updateValues();
  static void show();
  static uint8_t getBrightnessValue();
  static uint8_t getKelvinSatValue();
  static uint16_t getHueValue();
  static inline lightConfig *selectedLight;
  static LIGHT_PAGE_MODE getCurrentMode();
  static void switchMode();

private:
  static inline LIGHT_PAGE_MODE _currentMode;
};

class ScenePage {
  static void show();
  static void showScenes(std::list<sceneConfig> &scenes);
};

class NspanelManagerPage {
public:
  static void show();
  static void setText(const char *text);
  static void setText(std::string &text);
};

#endif /* LIB_INTERFACEMANAGER_PAGES_H_ */
