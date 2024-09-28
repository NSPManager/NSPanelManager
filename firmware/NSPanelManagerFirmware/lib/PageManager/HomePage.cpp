#include <HomePage.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceManager.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
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
  // TODO: Attach callback for room state update
  // for (Light *light : LightManager::getAllLights()) {
  //   light->attachUpdateCallback(this);
  //   light->attachDeconstructCallback(this);
  // }
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
      this->_updateLightsWithNewBrightness(this->getDimmingValue());
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
  if (value != this->_ceiling_lights_display_value) {
    NSPanel::instance->setComponentVal(HOME_LABEL_CEILING_BRIGHTNESS, value);
    this->_ceiling_lights_display_value = value;
  }
}

void HomePage::setTableBrightnessLabelText(uint8_t value) {
  if (value != this->_table_lights_display_value) {
    NSPanel::instance->setComponentVal(HOME_LABEL_TABLE_BRIGHTNESS, value);
    this->_table_lights_display_value = value;
  }
}

void HomePage::setCeilingLightsState(bool state) {
  if (state != this->_ceiling_lights_state) {
    NSPanel::instance->setComponentVal(HOME_BUTTON_CEILING_NAME, state ? 1 : 0);
    this->_ceiling_lights_state = state;
  }
}

void HomePage::setTableLightsState(bool state) {
  if (state != this->_table_lights_state) {
    NSPanel::instance->setComponentVal(HOME_BUTTON_TABLE_NAME, state ? 1 : 0);
    this->_table_lights_state = state;
  }
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
  if (this->_room_name_value.compare(text) != 0) {
    NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, text);
    this->_room_name_value = text;
  }
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

void HomePage::_updateLightsWithNewBrightness(uint8_t brightness) {
  // TODO: Handle global
  if (RoomManager::hasValidCurrentRoom()) {
    if (InterfaceConfig::optimistic_mode) {
      bool update_both = (RoomManager::currentRoom->table_lights_dim_level == 0 && RoomManager::currentRoom->ceiling_lights_dim_level == 0) || (RoomManager::currentRoom->table_lights_dim_level > 0 && RoomManager::currentRoom->ceiling_lights_dim_level > 0);
      if (!update_both && RoomManager::currentRoom->table_lights_dim_level > 0) {
        RoomManager::currentRoom->table_lights_dim_level = brightness;
        if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
          RoomManager::currentRoom->average_dim_level = brightness;
        }
      } else if (!update_both && RoomManager::currentRoom->ceiling_lights_dim_level > 0) {
        RoomManager::currentRoom->ceiling_lights_dim_level = brightness;
        if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
          RoomManager::currentRoom->average_dim_level = brightness;
        }
      } else if (update_both) {
        if (RoomManager::currentRoom->has_ceiling_lights) {
          RoomManager::currentRoom->ceiling_lights_dim_level = brightness;
        }
        if (RoomManager::currentRoom->has_table_lights) {
          RoomManager::currentRoom->table_lights_dim_level = brightness;
        }
        if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
          RoomManager::currentRoom->average_dim_level = brightness;
        }
      }
      this->updateLightStatus(true, false);
    }

    NSPanelMQTTManagerCommand__FirstPageTurnLightOn light_command = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
    light_command.brightness_slider_value = brightness;
    light_command.kelvin_slider_value = 0;
    light_command.selected_room = (*RoomManager::currentRoom).id;
    light_command.global = InterfaceConfig::currentRoomMode == roomMode::house;
    light_command.has_brightness_value = true;
    light_command.has_kelvin_value = false;
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__ALL;
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__CEILING_LIGHTS;
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__TABLE_LIGHTS;
    }

    NSPanelMQTTManagerCommand command;
    nspanel_mqttmanager_command__init(&command);
    command.command_data_case = NSPanelMQTTManagerCommand__CommandDataCase::NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
    command.first_page_turn_on = &light_command;

    size_t pack_length = nspanel_mqttmanager_command__get_packed_size(&command);
    uint8_t buffer[pack_length];
    size_t pack_size = nspanel_mqttmanager_command__pack(&command, buffer);

    std::string full_buffer = std::string(buffer, buffer + pack_size);
    MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, full_buffer, false);
  }
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
  // TODO: Handle case when a light is on/off
  // TODO: Handle global
  if (RoomManager::hasValidCurrentRoom() && RoomManager::currentRoom->has_ceiling_lights) {
    NSPanelMQTTManagerCommand__FirstPageTurnLightOn light_command = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
    light_command.brightness_slider_value = this->getDimmingValue();
    uint16_t sendKelvin = this->getColorTempValue() * ((InterfaceConfig::colorTempMax - InterfaceConfig::colorTempMin) / 100);
    // TODO: Move "reverse color temp slider" behavior and option to be managed in MQTTManager.
    if (InterfaceConfig::reverseColorTempSlider) {
      light_command.kelvin_slider_value = InterfaceConfig::colorTempMax - sendKelvin;
    } else {
      light_command.kelvin_slider_value = InterfaceConfig::colorTempMin + sendKelvin;
    }
    light_command.selected_room = (*RoomManager::currentRoom).id;
    light_command.global = InterfaceConfig::currentRoomMode == roomMode::house;
    light_command.has_brightness_value = true;
    light_command.has_kelvin_value = true;
    light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__CEILING_LIGHTS;

    NSPanelMQTTManagerCommand command;
    nspanel_mqttmanager_command__init(&command);
    command.command_data_case = NSPanelMQTTManagerCommand__CommandDataCase::NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
    command.first_page_turn_on = &light_command;

    size_t pack_length = nspanel_mqttmanager_command__get_packed_size(&command);
    uint8_t buffer[pack_length];
    size_t pack_size = nspanel_mqttmanager_command__pack(&command, buffer);

    std::string full_buffer = std::string(buffer, buffer + pack_size);
    MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, full_buffer, false);
  }
}

void HomePage::_tableMasterButtonEvent() {
  // TODO: Handle case when a light is on/off
  // TODO: Handle global
  if (RoomManager::hasValidCurrentRoom() && RoomManager::currentRoom->has_table_lights) {
    NSPanelMQTTManagerCommand__FirstPageTurnLightOn light_command = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
    light_command.brightness_slider_value = this->getDimmingValue();
    uint16_t sendKelvin = this->getColorTempValue() * ((InterfaceConfig::colorTempMax - InterfaceConfig::colorTempMin) / 100);
    // TODO: Move "reverse color temp slider" behavior and option to be managed in MQTTManager.
    if (InterfaceConfig::reverseColorTempSlider) {
      light_command.kelvin_slider_value = InterfaceConfig::colorTempMax - sendKelvin;
    } else {
      light_command.kelvin_slider_value = InterfaceConfig::colorTempMin + sendKelvin;
    }
    light_command.selected_room = (*RoomManager::currentRoom).id;
    light_command.global = InterfaceConfig::currentRoomMode == roomMode::house;
    light_command.has_brightness_value = true;
    light_command.has_kelvin_value = true;
    light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__TABLE_LIGHTS;

    NSPanelMQTTManagerCommand command;
    nspanel_mqttmanager_command__init(&command);
    command.command_data_case = NSPanelMQTTManagerCommand__CommandDataCase::NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
    command.first_page_turn_on = &light_command;

    size_t pack_length = nspanel_mqttmanager_command__get_packed_size(&command);
    uint8_t buffer[pack_length];
    size_t pack_size = nspanel_mqttmanager_command__pack(&command, buffer);

    std::string full_buffer = std::string(buffer, buffer + pack_size);
    MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, full_buffer, false);
  }
}

void HomePage::_updateLightsColorTempAccordingToSlider() {
  if (RoomManager::hasValidCurrentRoom()) {
    NSPanelMQTTManagerCommand__FirstPageTurnLightOn light_command = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
    light_command.brightness_slider_value = 0;
    uint16_t sendKelvin = this->getColorTempValue() * ((InterfaceConfig::colorTempMax - InterfaceConfig::colorTempMin) / 100);
    // TODO: Move "reverse color temp slider" behavior and option to be managed in MQTTManager.
    if (InterfaceConfig::reverseColorTempSlider) {
      light_command.kelvin_slider_value = InterfaceConfig::colorTempMax - sendKelvin;
    } else {
      light_command.kelvin_slider_value = InterfaceConfig::colorTempMin + sendKelvin;
    }
    light_command.selected_room = (*RoomManager::currentRoom).id;
    light_command.global = InterfaceConfig::currentRoomMode == roomMode::house;
    light_command.has_brightness_value = false;
    light_command.has_kelvin_value = true;
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__ALL;
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__CEILING_LIGHTS;
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      light_command.affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__TABLE_LIGHTS;
    }

    NSPanelMQTTManagerCommand command;
    nspanel_mqttmanager_command__init(&command);
    command.command_data_case = NSPanelMQTTManagerCommand__CommandDataCase::NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
    command.first_page_turn_on = &light_command;

    size_t pack_length = nspanel_mqttmanager_command__get_packed_size(&command);
    uint8_t buffer[pack_length];
    size_t pack_size = nspanel_mqttmanager_command__pack(&command, buffer);

    std::string full_buffer = std::string(buffer, buffer + pack_size);
    MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, full_buffer, false);
  }

  // TODO: Implement via protobuf
  // std::list<Light *> lights;
  // if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
  //   if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
  //     lights = (*RoomManager::currentRoom)->getAllLights();
  //   } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
  //     lights = (*RoomManager::currentRoom)->getAllCeilingLights();
  //   } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
  //     lights = (*RoomManager::currentRoom)->getAllTableLights();
  //   }
  // } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
  //   if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
  //     lights = LightManager::getAllLights();
  //   } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
  //     lights = LightManager::getAllCeilingLights();
  //   } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
  //     lights = LightManager::getAllTableLights();
  //   }
  // }

  // LightManager::ChangeLightToColorTemperature(&lights, this->getColorTempValue());
  // this->_ignoreMqttMessagesUntil = millis() + InterfaceConfig::mqtt_ignore_time;
  // this->updateLightStatus(false, true);
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
  if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
    if (InterfaceConfig::currentEditLightMode == editLightMode::all_lights) {
      if (RoomManager::currentRoom->has_ceiling_lights) {
        PageManager::GetHomePage()->setCeilingLightsState((*RoomManager::currentRoom).ceiling_lights_dim_level > 0);
        PageManager::GetHomePage()->setCeilingBrightnessLabelText((*RoomManager::currentRoom).ceiling_lights_dim_level);
      } else {
        PageManager::GetHomePage()->setCeilingLightsState(false);
        PageManager::GetHomePage()->setCeilingBrightnessLabelText(0);
      }
      if (RoomManager::currentRoom->has_table_lights) {
        PageManager::GetHomePage()->setTableLightsState((*RoomManager::currentRoom).table_lights_dim_level > 0);
        PageManager::GetHomePage()->setTableBrightnessLabelText((*RoomManager::currentRoom).table_lights_dim_level);
      } else {
        PageManager::GetHomePage()->setTableLightsState(false);
        PageManager::GetHomePage()->setTableBrightnessLabelText(0);
      }
      // Only update light level slider if the new value is greater than 0.
      if ((*RoomManager::currentRoom).average_dim_level > 0) {
        PageManager::GetHomePage()->setDimmingValue((*RoomManager::currentRoom).average_dim_level);
      }
      PageManager::GetHomePage()->setColorTempValue((*RoomManager::currentRoom).average_color_temperature);
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::ceiling_lights) {
      if (RoomManager::currentRoom->has_ceiling_lights) {
        PageManager::GetHomePage()->setCeilingLightsState((*RoomManager::currentRoom).ceiling_lights_dim_level > 0);
        PageManager::GetHomePage()->setCeilingBrightnessLabelText((*RoomManager::currentRoom).ceiling_lights_dim_level);
      } else {
        PageManager::GetHomePage()->setCeilingLightsState(false);
        PageManager::GetHomePage()->setCeilingBrightnessLabelText(0);
      }
      PageManager::GetHomePage()->setTableLightsState(false);
      PageManager::GetHomePage()->setTableBrightnessLabelText(0);
      // Only update light level slider if the new value is greater than 0.
      if ((*RoomManager::currentRoom).ceiling_lights_dim_level > 0) {
        PageManager::GetHomePage()->setDimmingValue((*RoomManager::currentRoom).ceiling_lights_dim_level);
      }
      PageManager::GetHomePage()->setColorTempValue((*RoomManager::currentRoom).ceiling_lights_color_temperature_value);
    } else if (InterfaceConfig::currentEditLightMode == editLightMode::table_lights) {
      if (RoomManager::currentRoom->has_table_lights) {
        PageManager::GetHomePage()->setTableLightsState((*RoomManager::currentRoom).table_lights_dim_level > 0);
        PageManager::GetHomePage()->setTableBrightnessLabelText((*RoomManager::currentRoom).table_lights_dim_level);
      } else {
        PageManager::GetHomePage()->setTableLightsState(false);
        PageManager::GetHomePage()->setTableBrightnessLabelText(0);
      }
      PageManager::GetHomePage()->setTableLightsState((*RoomManager::currentRoom).table_lights_dim_level > 0);
      PageManager::GetHomePage()->setTableBrightnessLabelText((*RoomManager::currentRoom).table_lights_dim_level);
      // Only update light level slider if the new value is greater than 0.
      if ((*RoomManager::currentRoom).table_lights_dim_level > 0) {
        PageManager::GetHomePage()->setDimmingValue((*RoomManager::currentRoom).table_lights_dim_level);
      }
      PageManager::GetHomePage()->setColorTempValue((*RoomManager::currentRoom).table_lights_color_temperature_value);
    } else {
      LOG_ERROR("Unknown editLightMode!");
    }
  }
}

void HomePage::updateRoomInfo() {
  if (RoomManager::hasValidCurrentRoom()) {
    if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
      NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, (*RoomManager::currentRoom).name);
    } else if (InterfaceConfig::currentRoomMode == roomMode::house) {
      NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, "All");
    }
    this->updateLightStatus(true, true);
  } else {
    LOG_ERROR("No 'current room' is set. Iterator is invalid! Will not change room text.");
  }
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
