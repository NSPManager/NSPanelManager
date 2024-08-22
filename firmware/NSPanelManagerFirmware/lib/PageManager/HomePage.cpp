#include <HomePage.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceManager.hpp>
#include <Light.hpp>
#include <LightManager.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <TftDefines.h>

void HomePage::init() {
  InterfaceConfig::currentEditLightMode = editLightMode::all_lights;
  InterfaceConfig::currentRoomMode = roomMode::room;
  this->_ignoreMqttMessagesUntil = 0;
  this->_ignoreNextTouchRelease = false;
  RoomManager::attachRoomChangeCallback(this);
}

void HomePage::show() {
  NSPanel::instance->goToPage(HOME_PAGE_NAME);
  PageManager::SetCurrentPage(this);
  this->updateDeviceEntitySubscriptions();
  this->update();
}

void HomePage::updateDeviceEntitySubscriptions() {
  for (Light *light : LightManager::getAllLights()) {
    light->attachUpdateCallback(this);
    light->attachDeconstructCallback(this);
  }
}

void HomePage::update() {
  this->updateLightStatus(true, true);
  this->updateRoomInfo();
  this->updateModeText();
}

void HomePage::roomChangedCallback() {
  this->update();
}

void HomePage::unshow() {
}

void HomePage::entityUpdateCallback(DeviceEntity *entity) {
  if (millis() < this->_ignoreMqttMessagesUntil) {
    return;
  }

  HomePage::_lastDeviceEntityUpdate = millis();
  if (HomePage::_taskHandleUpdateDisplay == NULL) {
    xTaskCreatePinnedToCore(_taskUpdateDisplay, "taskUpdateDisplay", 5000, NULL, 1, &HomePage::_taskHandleUpdateDisplay, CONFIG_ARDUINO_RUNNING_CORE);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void HomePage::_taskUpdateDisplay(void *param) {
  while (millis() < HomePage::_lastDeviceEntityUpdate + 250) {
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }

  PageManager::GetHomePage()->updateLightStatus(true, true);
  HomePage::_taskHandleUpdateDisplay = NULL;
  vTaskDelete(NULL);
}

void HomePage::entityDeconstructCallback(DeviceEntity *entity) {
  LOG_DEBUG("Got entity deconstruct callback, detaching deconstruct from entity.");
  entity->detachDeconstructCallback(this);
  LOG_DEBUG("Detaching update callback.");
  entity->detachUpdateCallback(this);
  LOG_DEBUG("Updating display.");
  this->update();
}

void HomePage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  LOG_DEBUG("Got event, component ", page, ".", component, " ", pressed ? "pressed" : "released");

  HomePage::_isFingerOnDisplay = pressed;

  if (!pressed && this->_ignoreNextTouchRelease) {
    this->_ignoreNextTouchRelease = false; // Reset block
    return;
  }

  if (!pressed) {
    if (component == SWITCH_ROOM_BUTTON_ID && InterfaceConfig::currentRoomMode == roomMode::room) {
      RoomManager::goToNextRoom();
      this->_stopSpecialMode();
    } else if (component == SWITCH_MODE_BUTTON_ID) {
      this->goToNextMode();
      this->_stopSpecialMode();
    } else if (component == CEILING_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights || InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
        this->_lastSpecialModeEventMillis = millis();
        this->_lastMasterCeilingLightsButtonRelease = millis();
        this->_ceilingMasterButtonEvent();
      }
    } else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights || InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
        this->_lastSpecialModeEventMillis = millis();
        this->_lastMasterTableLightsButtonRelease = millis();
        this->_tableMasterButtonEvent();
      }
    } else if (component == HOME_LIGHT_LEVEL_SLIDER_ID) {
      // Dimmer slider changed, update cached value
      this->updateDimmerValueCache();
      if (InterfaceConfig::currentRoomMode == roomMode::room && (*RoomManager::currentRoom)->anyLightsOn()) {
        this->_updateLightsThatAreOnWithNewBrightness(this->getDimmingValue());
      } else if (InterfaceConfig::currentRoomMode == roomMode::house && (*RoomManager::currentRoom)->anyLightsOn()) {
        this->_updateLightsThatAreOnWithNewBrightness(this->getDimmingValue());
      } else {
        this->_updateAllLightsWithNewBrightness(this->getDimmingValue());
      }

      this->_lastSpecialModeEventMillis = millis();
    } else if (component == HOME_LIGHT_COLOR_SLIDER_ID) {
      // Color temp slider changed, update cached value
      this->updateColorTempValueCache();
      this->_lastSpecialModeEventMillis = millis();
      this->_updateLightsColorTempAccordingToSlider();
    } else if (component == ROOM_BUTTON_ID && InterfaceConfig::currentRoomMode == roomMode::room) {
      this->_stopSpecialMode();
      PageManager::GetRoomPage()->show();
    } else if (component == SCENES_BUTTON_ID) {
      PageManager::GetScenePage()->show();
    }
  } else if (pressed) {
    if (component == CEILING_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
        this->_lastMasterCeilingLightsButtonTouch = millis();
        this->_startSpecialModeTriggerTask(editLightMode::ceiling_lights);
      } else {
        this->_ignoreNextTouchRelease = true;
        this->_stopSpecialMode();
      }
    } else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
        this->_lastMasterTableLightsButtonTouch = millis();
        this->_startSpecialModeTriggerTask(editLightMode::table_lights);
      } else {
        this->_ignoreNextTouchRelease = true;
        this->_stopSpecialMode();
      }
    }
  }
}

int HomePage::getDimmingValue() {
  return this->_dimmerValue;
}

void HomePage::setDimmingValue(uint8_t value) {
  this->updateDimmerValueCache();
  if (value != this->getDimmingValue()) {
    NSPanel::instance->setComponentVal(HOME_DIMMER_SLIDER_NAME, value);
    this->_dimmerValue = value;
  }
}

void HomePage::updateDimmerValueCache() {
  int newValue = NSPanel::instance->getComponentIntVal(HOME_DIMMER_SLIDER_NAME);
  if (newValue > InterfaceConfig::raiseToMaxLightLevelAbove) {
    this->_dimmerValue = 100;
  } else {
    this->_dimmerValue = newValue;
  }
}

int HomePage::getColorTempValue() {
  return this->_colorTemp;
}

void HomePage::setColorTempValue(uint8_t value) {
  this->updateColorTempValueCache();
  if (value != this->getColorTempValue()) {
    NSPanel::instance->setComponentVal(HOME_LIGHT_COLOR_SLIDER_NAME, value);
    this->_colorTemp = value;
  }
}

void HomePage::updateColorTempValueCache() {
  this->_colorTemp = NSPanel::instance->getComponentIntVal(HOME_LIGHT_COLOR_SLIDER_NAME);
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

void HomePage::setScreensaverTimeout(uint16_t timeout) {
  NSPanel::instance->setTimerTimeout(HOME_PAGE_SCREENSAVER_TIMER_NAME, timeout);
}

void HomePage::setRoomText(const char *text) {
  NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, text);
}

void HomePage::setModeText(const char *text) {
  NSPanel::instance->setComponentText(HOME_PAGE_MODE_LABEL_NAME, text);
}

void HomePage::setEditLightMode(editLightMode new_mode) {
  InterfaceConfig::currentEditLightMode = new_mode; // Set current mode
  if (new_mode == editLightMode::all_lights) {
    PageManager::GetHomePage()->setSliderLightLevelColor(HOME_PAGE_SLIDER_NORMAL_COLOR); // Reset to normal color
    PageManager::GetHomePage()->setSliderColorTempColor(HOME_PAGE_SLIDER_NORMAL_COLOR);  // Reset to normal color
    PageManager::GetHomePage()->setHighlightCeilingVisibility(false);
    PageManager::GetHomePage()->setHighlightTableVisibility(false);
  } else {
    if (new_mode == editLightMode::ceiling_lights) {
      PageManager::GetHomePage()->setHighlightCeilingVisibility(true);
      PageManager::GetHomePage()->setHighlightTableVisibility(false);
    } else if (new_mode == editLightMode::table_lights) {
      PageManager::GetHomePage()->setHighlightTableVisibility(true);
      PageManager::GetHomePage()->setHighlightCeilingVisibility(false);
    }

    PageManager::GetHomePage()->setSliderLightLevelColor(HOME_PAGE_SLIDER_LOCK_COLOR); // Change slider color to indicate special mode
    PageManager::GetHomePage()->setSliderColorTempColor(HOME_PAGE_SLIDER_LOCK_COLOR);  // Change slider color to indicate special mode
  }
  this->updateLightStatus(true, true);
  this->_startSpecialModeTimerTask();
}

void HomePage::_updateLightsThatAreOnWithNewBrightness(uint8_t brightness) {
  std::list<Light *> lights;
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      lights = (*RoomManager::currentRoom)->getAllLightsThatAreOn();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      if ((*RoomManager::currentRoom)->anyCeilingLightsOn()) {
        lights = (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllCeilingLights();
      }
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      if ((*RoomManager::currentRoom)->anyTableLightsOn()) {
        lights = (*RoomManager::currentRoom)->getTableLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllTableLights();
      }
    }
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      lights = LightManager::getAllLightsThatAreOn();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      if (LightManager::anyCeilingLightsOn()) {
        lights = LightManager::getCeilingLightsThatAreOn();
      } else {
        lights = LightManager::getAllCeilingLights();
      }
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      if (LightManager::anyTableLightsOn()) {
        lights = LightManager::getTableLightsThatAreOn();
      } else {
        lights = LightManager::getAllTableLights();
      }
    }
  }

  LightManager::ChangeLightsToLevel(&lights, brightness);
  this->_ignoreMqttMessagesUntil = millis() + InterfaceConfig::mqtt_ignore_time;
  this->updateLightStatus(true, false);
}

void HomePage::_updateAllLightsWithNewBrightness(uint8_t brightness) {
  std::list<Light *> lights;
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      if ((*RoomManager::currentRoom)->anyLightsOn()) {
        lights = (*RoomManager::currentRoom)->getAllLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllLights();
      }
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      if ((*RoomManager::currentRoom)->anyCeilingLightsOn()) {
        lights = (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllCeilingLights();
      }
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      if ((*RoomManager::currentRoom)->anyTableLightsOn()) {
        lights = (*RoomManager::currentRoom)->getTableLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllTableLights();
      }
    }
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      if (LightManager::anyLightsOn()) {
        lights = LightManager::getAllLightsThatAreOn();
      } else {
        lights = LightManager::getAllLights();
      }
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      if (LightManager::anyCeilingLightsOn()) {
        lights = LightManager::getCeilingLightsThatAreOn();
      } else {
        lights = LightManager::getAllCeilingLights();
      }
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      if (LightManager::anyTableLightsOn()) {
        lights = LightManager::getTableLightsThatAreOn();
      } else {
        lights = LightManager::getAllTableLights();
      }
    }
  }

  uint8_t newLevel = PageManager::GetHomePage()->getDimmingValue();
  LightManager::ChangeLightsToLevel(&lights, newLevel);
  this->_ignoreMqttMessagesUntil = millis() + InterfaceConfig::mqtt_ignore_time;
  this->updateLightStatus(true, false);
}

void HomePage::_startSpecialModeTriggerTask(editLightMode triggerMode) {
  InterfaceConfig::_triggerSpecialLightMode = triggerMode;
  xTaskCreatePinnedToCore(_taskTriggerSpecialModeTriggerTask, "taskSpecialModeTriggerTask", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void HomePage::_taskTriggerSpecialModeTriggerTask(void *param) {
  unsigned long start = millis();

  while (millis() < start + InterfaceConfig::special_mode_trigger_time) {
    if (!PageManager::GetHomePage()->_isFingerOnDisplay) {
      // User did not hold finger entire period, do not trigger special mode
      vTaskDelete(NULL);
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }

  if (InterfaceConfig::_triggerSpecialLightMode == editLightMode::ceiling_lights) {
    PageManager::GetHomePage()->_ignoreNextTouchRelease = true;
    PageManager::GetHomePage()->setEditLightMode(editLightMode::ceiling_lights);
  } else if (InterfaceConfig::_triggerSpecialLightMode == editLightMode::table_lights) {
    PageManager::GetHomePage()->_ignoreNextTouchRelease = true;
    PageManager::GetHomePage()->setEditLightMode(editLightMode::table_lights);
  } else if (InterfaceConfig::_triggerSpecialLightMode == editLightMode::EXIT_SPECIAL_MODE) {
    PageManager::GetHomePage()->_ignoreNextTouchRelease = true;
    PageManager::GetHomePage()->setEditLightMode(editLightMode::all_lights);
  }

  vTaskDelete(NULL); // Task is complete, stop task
}

void HomePage::_startSpecialModeTimerTask() {
  this->_lastSpecialModeEventMillis = millis();
  if (this->_specialModeTimerTaskHandle == NULL) {
    xTaskCreatePinnedToCore(_taskSpecialModeTimerTask, "taskSpecialModeTimer", 5000, NULL, 0, &HomePage::_specialModeTimerTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
  }
}

void HomePage::_taskSpecialModeTimerTask(void *param) {
  // Wait until no event has occured for 5 seconds before returning to normal mode
  while (true) {
    if (!PageManager::GetHomePage()->_isFingerOnDisplay && millis() > PageManager::GetHomePage()->_lastSpecialModeEventMillis + InterfaceConfig::special_mode_release_time) {
      break;
    } else if (!PageManager::GetHomePage()->_isFingerOnDisplay && millis() > PageManager::GetHomePage()->_lastSpecialModeEventMillis + InterfaceConfig::special_mode_release_time) {
      PageManager::GetHomePage()->_lastSpecialModeEventMillis = millis();
    }
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }

  PageManager::GetHomePage()->setEditLightMode(editLightMode::all_lights);
  HomePage::_specialModeTimerTaskHandle = NULL;
  vTaskDelete(NULL);
}

void HomePage::_stopSpecialMode() {
  if (this->_specialModeTimerTaskHandle != NULL) {
    vTaskDelete(this->_specialModeTimerTaskHandle);
    this->_specialModeTimerTaskHandle = NULL;
  }
  this->setEditLightMode(editLightMode::all_lights);
}

void HomePage::_ceilingMasterButtonEvent() {
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    std::list<Light *> onLights = (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = (*RoomManager::currentRoom)->getAllCeilingLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    std::list<Light *> onLights = LightManager::getCeilingLightsThatAreOn();
    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = LightManager::getAllCeilingLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  }

  this->_ignoreMqttMessagesUntil = millis() + InterfaceConfig::mqtt_ignore_time;
  this->updateLightStatus(true, false);
}

void HomePage::_tableMasterButtonEvent() {
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    std::list<Light *> onLights = (*RoomManager::currentRoom)->getTableLightsThatAreOn();

    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = (*RoomManager::currentRoom)->getAllTableLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    std::list<Light *> onLights = LightManager::getTableLightsThatAreOn();
    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = LightManager::getAllTableLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  }

  this->_ignoreMqttMessagesUntil = millis() + InterfaceConfig::mqtt_ignore_time;
  this->updateLightStatus(true, false);
}

void HomePage::_updateLightsColorTempAccordingToSlider() {
  std::list<Light *> lights;
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      lights = (*RoomManager::currentRoom)->getAllLights();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      lights = (*RoomManager::currentRoom)->getAllCeilingLights();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      lights = (*RoomManager::currentRoom)->getAllTableLights();
    }
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      lights = LightManager::getAllLights();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      lights = LightManager::getAllCeilingLights();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      lights = LightManager::getAllTableLights();
    }
  }

  LightManager::ChangeLightToColorTemperature(&lights, this->getColorTempValue());
  this->_ignoreMqttMessagesUntil = millis() + InterfaceConfig::mqtt_ignore_time;
  this->updateLightStatus(false, true);
}

void HomePage::goToNextMode() {
  roomMode newMode = static_cast<roomMode>(static_cast<int>(InterfaceConfig::currentRoomMode) + 1);
  if (newMode == roomMode::END) {
    newMode = roomMode::room;
  }
  this->setCurrentMode(newMode);
}

void HomePage::setCurrentMode(roomMode mode) {
  InterfaceConfig::currentRoomMode = mode;
  this->updateModeText();
  this->updateRoomInfo();
}

void HomePage::updateLightStatus(bool updateLightLevel, bool updateColorTemperature) {
  uint totalBrightness = 0;
  uint totalBrightnessLights = 0;
  uint totalKelvinLightsCeiling = 0;
  unsigned long totalKelvinValueCeilingLights = 0;
  bool anyLightsOn = false;
  std::list<Light *> ceilingLights;
  std::list<Light *> tableLights;

  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      anyLightsOn = (*RoomManager::currentRoom)->anyLightsOn();

    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      anyLightsOn = (*RoomManager::currentRoom)->anyCeilingLightsOn();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      anyLightsOn = (*RoomManager::currentRoom)->anyTableLightsOn();
    }
    for (auto lightPair : (*RoomManager::currentRoom)->ceilingLights) {
      ceilingLights.push_back(lightPair.second);
    }
    for (auto lightPair : (*RoomManager::currentRoom)->tableLights) {
      tableLights.push_back(lightPair.second);
    }
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      anyLightsOn = LightManager::anyLightsOn();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      anyLightsOn = LightManager::anyCeilingLightsOn();
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      anyLightsOn = LightManager::anyTableLightsOn();
    }
    ceilingLights = LightManager::getAllCeilingLights();
    tableLights = LightManager::getAllTableLights();
  }

  // Calculate average for ceiling lights
  if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights || InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
    for (Light *light : ceilingLights) {
      if (light->getLightLevel() > 0 || !anyLightsOn) {
        totalBrightnessLights++;
        totalBrightness += light->getLightLevel();
        if (light->canTemperature()) {
          totalKelvinLightsCeiling++;
          totalKelvinValueCeilingLights += light->getColorTemperature();
        }
      }
    }
  }

  uint8_t averageCeilingBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
  uint8_t averageCeilingKelvin = totalKelvinLightsCeiling == 0 ? 0 : totalKelvinValueCeilingLights / totalKelvinLightsCeiling;
  if (updateLightLevel) {
    PageManager::GetHomePage()->setCeilingLightsState(averageCeilingBrightness > 0);
    PageManager::GetHomePage()->setCeilingBrightnessLabelText(averageCeilingBrightness);
  }

  // Calculate average for table lights
  totalBrightness = 0;
  totalBrightnessLights = 0;
  uint8_t totalKelvinLightsTable = 0;
  unsigned long totalKelvinValueTableLights = 0;
  if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights || InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
    for (Light *light : tableLights) {
      if (light->getLightLevel() > 0 || !anyLightsOn) {
        totalBrightnessLights++;
        totalBrightness += light->getLightLevel();
        if (light->canTemperature()) {
          totalKelvinLightsTable++;
          totalKelvinValueTableLights += light->getColorTemperature();
        }
      }
    }
  }

  uint8_t averageTableBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
  uint8_t averageTableKelvin = totalKelvinLightsTable == 0 ? 0 : totalKelvinValueTableLights / totalKelvinLightsTable;
  if (updateLightLevel) {
    PageManager::GetHomePage()->setTableLightsState(averageTableBrightness > 0);
    PageManager::GetHomePage()->setTableBrightnessLabelText(averageTableBrightness);
  }

  uint8_t totalAverageBrightness;
  if (averageCeilingBrightness > 0 && averageTableBrightness > 0) {
    totalAverageBrightness = (averageCeilingBrightness + averageTableBrightness) / 2;
  } else if (averageCeilingBrightness > 0) {
    totalAverageBrightness = averageCeilingBrightness;
  } else if (averageTableBrightness > 0) {
    totalAverageBrightness = averageTableBrightness;
  } else {
    totalAverageBrightness = 0;
  }

  // Only set a new value if any lights are on.
  // This value will be used as the next "on" value.
  if (updateLightLevel && totalAverageBrightness > 0 && totalAverageBrightness != PageManager::GetHomePage()->getDimmingValue()) {
    PageManager::GetHomePage()->setDimmingValue(totalAverageBrightness);
  }

  uint8_t totalAverageKelvin;
  if (totalKelvinLightsCeiling > 0 && totalKelvinLightsTable > 0) {
    totalAverageKelvin = (averageCeilingKelvin + averageTableKelvin) / 2;
  } else if (totalKelvinLightsCeiling > 0) {
    totalAverageKelvin = averageCeilingKelvin;
  } else if (totalKelvinLightsTable > 0) {
    totalAverageKelvin = averageTableKelvin;
  } else {
    totalAverageKelvin = 0;
  }

  // Only set a new value if it is not the same as already set and a new value was discovered (ie, > 0).
  if (updateColorTemperature && totalAverageKelvin != PageManager::GetHomePage()->getColorTempValue()) {
    PageManager::GetHomePage()->setColorTempValue(totalAverageKelvin);
  }
}

void HomePage::updateRoomInfo() {
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, (*RoomManager::currentRoom)->name.c_str());
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, "All");
  }
  this->updateLightStatus(true, true);
}

void HomePage::updateModeText() {
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    NSPanel::instance->setComponentText(HOME_PAGE_MODE_LABEL_NAME, "Room lights");
    NSPanel::instance->setComponentPic(HOME_BUTTON_SCENES_NAME, HOME_BUTTON_SCENES_ROOM_MODE_PIC);
  } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
    NSPanel::instance->setComponentText(HOME_PAGE_MODE_LABEL_NAME, "All lights");
    NSPanel::instance->setComponentPic(HOME_BUTTON_SCENES_NAME, HOME_BUTTON_SCENES_ALL_MODE_PIC);
  } else {
    NSPanel::instance->setComponentText(HOME_PAGE_MODE_LABEL_NAME, "UNKNOWN");
  }
}
