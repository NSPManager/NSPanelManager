/*
 * pages.cpp
 *
 *  Created on: Mar 6, 2023
 *      Author: Tim Panajott
 */

#include <MqttLog.hpp>
#include <NSPanel.hpp>
#include <TftDefines.h>
#include <pages.hpp>

int HomePage::getDimmingValue() {
  return HomePage::_dimmerValue;
}

void HomePage::setDimmingValue(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_DIMMER_SLIDER_NAME, value);
  HomePage::_dimmerValue = value;
}

void HomePage::updateDimmerValueCache() {
  int newValue = NSPanel::instance->getComponentIntVal(HOME_DIMMER_SLIDER_NAME);
  if (newValue > InterfaceManager::instance->config.raiseToMaxLightLevelAbove) {
    HomePage::_dimmerValue = 100;
  } else {
    HomePage::_dimmerValue = newValue;
  }
}

int HomePage::getColorTempValue() {
  return HomePage::_colorTemp;
}

void HomePage::setColorTempValue(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_LIGHT_COLOR_SLIDER_NAME, value);
  HomePage::_colorTemp = value;
}

void HomePage::updateColorTempValueCache() {
  HomePage::_colorTemp = NSPanel::instance->getComponentIntVal(HOME_LIGHT_COLOR_SLIDER_NAME);
}

void HomePage::setCeilingBrightnessLabelText(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_LABEL_CEILING_BRIGHTNESS, value);
}

void HomePage::setTableBrightnessLabelText(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_LABEL_TABLE_BRIGHTNESS, value);
}

void HomePage::setCeilingLightsState(bool state) {
  NSPanel::instance->setComponentVal(HOME_BUTTON_CEILING_NAME, state ? 1 : 0);
}

void HomePage::setTableLightsState(bool state) {
  NSPanel::instance->setComponentVal(HOME_BUTTON_TABLE_NAME, state ? 1 : 0);
}

void HomePage::setSliderLightLevelColor(uint color) {
  NSPanel::instance->setComponentForegroundColor(HOME_DIMMER_SLIDER_NAME, color);
}

void HomePage::setSliderColorTempColor(uint color) {
  NSPanel::instance->setComponentForegroundColor(HOME_LIGHT_COLOR_SLIDER_NAME, color);
}

void HomePage::setHighlightCeilingVisibility(bool visibility) {
  NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_CEILING_NAME, visibility);
}

void HomePage::setHighlightTableVisibility(bool visibility) {
  NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_TABLE_NAME, visibility);
}

void RoomPage::setLightVisibility(uint8_t position, bool visibility) {
  switch (position) {
  case 1:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT1_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT1_SW_NAME, visibility);
    break;
  case 2:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT2_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT2_SW_NAME, visibility);
    break;
  case 3:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT3_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT3_SW_NAME, visibility);
    break;
  case 4:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT4_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT4_SW_NAME, visibility);
    break;
  case 5:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT5_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT5_SW_NAME, visibility);
    break;
  case 6:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT6_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT6_SW_NAME, visibility);
    break;
  case 7:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT7_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT7_SW_NAME, visibility);
    break;
  case 8:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT8_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT8_SW_NAME, visibility);
    break;
  case 9:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT9_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT9_SW_NAME, visibility);
    break;
  case 10:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT10_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT10_SW_NAME, visibility);
    break;
  case 11:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT11_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT11_SW_NAME, visibility);
    break;
  case 12:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT12_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT12_SW_NAME, visibility);
    break;

  default:
    LOG_ERROR("Trying to set visibility of room light that doesn't exist");
    break;
  }
}

void RoomPage::setLightName(uint8_t position, const char *name) {
  switch (position) {
  case 1:
    NSPanel::instance->setComponentText(ROOM_LIGHT1_LABEL_NAME, name);
    break;
  case 2:
    NSPanel::instance->setComponentText(ROOM_LIGHT2_LABEL_NAME, name);
    break;
  case 3:
    NSPanel::instance->setComponentText(ROOM_LIGHT3_LABEL_NAME, name);
    break;
  case 4:
    NSPanel::instance->setComponentText(ROOM_LIGHT4_LABEL_NAME, name);
    break;
  case 5:
    NSPanel::instance->setComponentText(ROOM_LIGHT5_LABEL_NAME, name);
    break;
  case 6:
    NSPanel::instance->setComponentText(ROOM_LIGHT6_LABEL_NAME, name);
    break;
  case 7:
    NSPanel::instance->setComponentText(ROOM_LIGHT7_LABEL_NAME, name);
    break;
  case 8:
    NSPanel::instance->setComponentText(ROOM_LIGHT8_LABEL_NAME, name);
    break;
  case 9:
    NSPanel::instance->setComponentText(ROOM_LIGHT9_LABEL_NAME, name);
    break;
  case 10:
    NSPanel::instance->setComponentText(ROOM_LIGHT10_LABEL_NAME, name);
    break;
  case 11:
    NSPanel::instance->setComponentText(ROOM_LIGHT11_LABEL_NAME, name);
    break;
  case 12:
    NSPanel::instance->setComponentText(ROOM_LIGHT12_LABEL_NAME, name);
    break;

  default:
    LOG_ERROR("Trying to set name of room light that doesn't exist");
    break;
  }
}

void RoomPage::setLightState(uint8_t position, bool state) {
  switch (position) {
  case 1:
    NSPanel::instance->setComponentVal(ROOM_LIGHT1_SW_NAME, state ? 1 : 0);
    break;
  case 2:
    NSPanel::instance->setComponentVal(ROOM_LIGHT2_SW_NAME, state ? 1 : 0);
    break;
  case 3:
    NSPanel::instance->setComponentVal(ROOM_LIGHT3_SW_NAME, state ? 1 : 0);
    break;
  case 4:
    NSPanel::instance->setComponentVal(ROOM_LIGHT4_SW_NAME, state ? 1 : 0);
    break;
  case 5:
    NSPanel::instance->setComponentVal(ROOM_LIGHT5_SW_NAME, state ? 1 : 0);
    break;
  case 6:
    NSPanel::instance->setComponentVal(ROOM_LIGHT6_SW_NAME, state ? 1 : 0);
    break;
  case 7:
    NSPanel::instance->setComponentVal(ROOM_LIGHT7_SW_NAME, state ? 1 : 0);
    break;
  case 8:
    NSPanel::instance->setComponentVal(ROOM_LIGHT8_SW_NAME, state ? 1 : 0);
    break;
  case 9:
    NSPanel::instance->setComponentVal(ROOM_LIGHT9_SW_NAME, state ? 1 : 0);
    break;
  case 10:
    NSPanel::instance->setComponentVal(ROOM_LIGHT10_SW_NAME, state ? 1 : 0);
    break;
  case 11:
    NSPanel::instance->setComponentVal(ROOM_LIGHT11_SW_NAME, state ? 1 : 0);
    break;
  case 12:
    NSPanel::instance->setComponentVal(ROOM_LIGHT12_SW_NAME, state ? 1 : 0);
    break;

  default:
    LOG_ERROR("Trying to set state of room light that doesn't exist");
    break;
  }
}

void RoomPage::setCurrentRoomLabel(const char *label) {
  NSPanel::instance->setComponentText(ROOM_PAGE_CURRENT_ROOM_LABEL_NAME, label);
}

void LightPage::show() {
  LightPage::_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
  NSPanel::instance->goToPage(LIGHT_PAGE_NAME);

  if (LightPage::selectedLight != nullptr) {
    if (LightPage::selectedLight->canTemperature) {
      LightPage::_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
    } else if (LightPage::selectedLight->canRgb) {
      LightPage::_currentMode = LIGHT_PAGE_MODE::COLOR_RGB;
    } else {
      LightPage::_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP; // Default to color temp although this wont be shown.
    }
  }
}

void LightPage::updateValues() {
  if (LightPage::selectedLight != nullptr) {
    NSPanel::instance->setComponentText(LIGHT_PAGE_LIGHT_LABEL_NAME, LightPage::selectedLight->name.c_str());
    NSPanel::instance->setComponentVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME, LightPage::selectedLight->level);

    LOG_DEBUG("Selected light can Color Temp? ", LightPage::selectedLight->canTemperature ? "Yes" : "No");
    LOG_DEBUG("Selected light can RGB? ", LightPage::selectedLight->canRgb ? "Yes" : "No");

    if (LightPage::selectedLight->canTemperature && LightPage::_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, LightPage::selectedLight->colorTemperature);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_TEMP_MODE_PIC);
    } else if (LightPage::selectedLight->canRgb && LightPage::_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_HUE_SLIDER_NAME, LightPage::selectedLight->colorHue);
      NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, LightPage::selectedLight->colorSat);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_RGB_MODE_PIC);
    }

    if (LightPage::selectedLight->canTemperature && LightPage::selectedLight->canRgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
    } else if (LightPage::selectedLight->canTemperature && !LightPage::selectedLight->canRgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!LightPage::selectedLight->canTemperature && LightPage::selectedLight->canRgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!LightPage::selectedLight->canTemperature && !LightPage::selectedLight->canRgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    }
  }
}

LIGHT_PAGE_MODE LightPage::getCurrentMode() {
  return LightPage::_currentMode;
}

void LightPage::switchMode() {
  if (LightPage::_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
    LightPage::_currentMode = LIGHT_PAGE_MODE::COLOR_RGB;
  } else if (LightPage::_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
    LightPage::_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
  }
  LightPage::updateValues();
}

uint8_t LightPage::getBrightnessValue() {
  return NSPanel::instance->getComponentIntVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME);
}

uint8_t LightPage::getKelvinSatValue() {
  return NSPanel::instance->getComponentIntVal(LIGHT_PAGE_KELVIN_SLIDER_NAME);
}

uint16_t LightPage::getHueValue() {
  return NSPanel::instance->getComponentIntVal(LIGHT_PAGE_HUE_SLIDER_NAME);
}

void NspanelManagerPage::show() {
  NSPanel::instance->goToPage("bootscreen");
}

void NspanelManagerPage::setText(std::string &text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_NAME, text.c_str());
}

void NspanelManagerPage::setText(const char *text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_NAME, text);
}

void ScenePage::show() {
  // TODO: Enter correct name given in TftDefines
  NSPanel::instance->goToPage(SCENES_PAGE_NAME);
}

void ScenePage::showScenes(std::list<sceneConfig> &scenes) {
  // Hide any elements that wont be used
  // 4 is the number of scenes we can show on the screen.
  for (uint8_t i = scenes.size() - 1; i < 4; i++) {
    switch (i) {
    case 0:
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, false);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, false);
    case 1:
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, false);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, false);
    case 2:
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, false);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, false);
    case 3:
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_LABEL_NAME, false);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME, false);
    default:
      break;
    }
  }

  // TODO: Loop over scenes and populate scenes list in room. Use names from TftDefines
  uint8_t i = 0;
  for (sceneConfig &scene : scenes) {
    switch (i) {
    case 0:
      NSPanel::instance->setComponentText(SCENES_PAGE_SCENE1_LABEL_NAME, scene.name.c_str());
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, true);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, true);
      break;
    case 1:
      NSPanel::instance->setComponentText(SCENES_PAGE_SCENE2_LABEL_NAME, scene.name.c_str());
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, true);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, true);
      break;
    case 2:
      NSPanel::instance->setComponentText(SCENES_PAGE_SCENE3_LABEL_NAME, scene.name.c_str());
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, true);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, true);
      break;
    case 3:
      NSPanel::instance->setComponentText(SCENES_PAGE_SCENE4_LABEL_NAME, scene.name.c_str());
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_LABEL_NAME, true);
      NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME, true);
      break;
    default:
      break;
    }
    i++;
  }
}
