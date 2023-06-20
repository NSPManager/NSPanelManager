#include <ArduinoJson.h>
#include <ButtonManager.hpp>
#include <HomePage.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceManager.hpp>
#include <Light.hpp>
#include <LightManager.hpp>
#include <LightPage.hpp>
#include <MqttLog.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <Scene.hpp>
#include <TftDefines.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <pages.hpp>

void InterfaceManager::init() {
  this->instance = this;
  this->_currentEditMode = editLightMode::all_lights;
  this->_ignoreNextTouchRelease = false;
  this->_lastMasterCeilingLightsButtonTouch = 0;
  this->_lastMasterCeilingLightsButtonRelease = 0;
  this->_lastMasterTableLightsButtonTouch = 0;
  this->_lastMasterTableLightsButtonRelease = 0;
  this->_isFingerOnDisplay = false;
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1");
  xTaskCreatePinnedToCore(_taskLoadConfigAndInit, "taskLoadConfigAndInit", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::stop() {
  LOG_INFO("Stopping interface manager.");
  try {
    if (InterfaceManager::_taskHandleProcessMqttMessages != NULL) {
      vTaskDelete(InterfaceManager::_taskHandleProcessMqttMessages);
    }
    if (InterfaceManager::_taskHandleSpecialModeTimer != NULL) {
      vTaskDelete(InterfaceManager::_taskHandleSpecialModeTimer);
    }

    for (Room *room : RoomManager::rooms) {
      for (auto lightPair : room->ceilingLights) {
        delete lightPair.second;
      }
      room->ceilingLights.clear();
      for (auto lightPair : room->tableLights) {
        delete lightPair.second;
      }
      room->tableLights.clear();
    }

    RoomManager::rooms.clear();
  } catch (const std::exception &e) {
    LOG_ERROR("Error while stopping Interface Manager: ", e.what());
  }
}

void InterfaceManager::_taskLoadConfigAndInit(void *param) {
  unsigned long start = millis();
  PageManager::GetHomePage()->init();
  PageManager::GetNSPanelManagerPage()->show();
  NSPanel::instance->setDimLevel(InterfaceConfig::screen_dim_level);
  while (!WiFi.isConnected() || !MqttManager::connected() && !InterfaceManager::hasRegisteredToManager) {
    if (!WiFi.isConnected()) {
      if (NSPMConfig::instance->NSPMConfig::instance->wifi_ssid.empty()) {
        PageManager::GetNSPanelManagerPage()->setText("Connect to AP NSPMPanel");
      } else {
        PageManager::GetNSPanelManagerPage()->setText("Connecting to WiFi...");
      }
    } else if (!InterfaceManager::hasRegisteredToManager) {
      PageManager::GetNSPanelManagerPage()->setText("Registring to manager...");
    } else if (!MqttManager::connected()) {
      PageManager::GetNSPanelManagerPage()->setText("Connecting to MQTT...");
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  // Wait for panel to become ready
  if (millis() - start < 4000) {
    vTaskDelay((millis() - start) / portTICK_PERIOD_MS);
  }

  PageManager::GetNSPanelManagerPage()->setText("Loading config...");
  RoomManager::loadAllRooms();

  // Set some default values before showing page
  InterfaceManager::instance->_changeMode(roomMode::room);

  // Update Home page cache
  PageManager::GetHomePage()->updateDimmerValueCache();
  PageManager::GetHomePage()->updateColorTempValueCache();

  // Start task for MQTT processing
  xTaskCreatePinnedToCore(_taskProcessMqttMessages, "taskProcessMqttMessages", 5000, NULL, 1, &InterfaceManager::_taskHandleProcessMqttMessages, CONFIG_ARDUINO_RUNNING_CORE);

  // As there may be may be MANY topics to subscribe to, do it in checks of 5 with delays
  // between them to allow for processing all the incoming data.
  PageManager::GetNSPanelManagerPage()->setText("Subscribing...");
  InterfaceManager::subscribeToMqttTopics();
  LightManager::subscribeToMqttLightUpdates();

  // Loading is done, show Home page
  NSPanel::instance->setDimLevel(InterfaceConfig::screen_dim_level);
  PageManager::GetHomePage()->show();
  PageManager::GetHomePage()->setScreensaverTimeout(InterfaceConfig::screensaver_activation_timeout);

  // NSPanel::attachTouchEventCallback(InterfaceManager::processTouchEvent);
  NSPanel::attachSleepCallback(InterfaceManager::processSleepEvent);
  NSPanel::attachWakeCallback(InterfaceManager::processWakeEvent);

  LOG_INFO("Config initialized. Closing taskLoadConfigAndInit");
  vTaskDelete(NULL); // Delete task, we are done
}

void InterfaceManager::subscribeToMqttTopics() {
  // Subscribe to command to wake/put to sleep the display
  vTaskDelay(100 / portTICK_PERIOD_MS);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_screen_cmd_topic.c_str(), &InterfaceManager::mqttCallback);
  if (InterfaceConfig::show_screensaver_clock) {
    PageManager::GetScreensaverPage()->attachMqttTimeCallback();
  } else {
    LOG_DEBUG("Not attaching MQTT clock callback is panel is confiugred to now show clock on screensaver.");
  }

  // Every light in every room
  // for (Light *light : LightManager::getAllLights()) {
  //   InterfaceManager::instance->_subscribeToLightTopics(light);
  // }
}

void InterfaceManager::_subscribeToLightTopics(Light *light) {
  MqttManager::subscribeToTopic(light->getLevelStateTopic().c_str(), &InterfaceManager::mqttCallback);

  if (light->canTemperature()) {
    MqttManager::subscribeToTopic(light->getColorTemperatureStateTopic().c_str(), &InterfaceManager::mqttCallback);
  }

  if (light->canRgb()) {
    MqttManager::subscribeToTopic(light->getSaturationStateTopic().c_str(), &InterfaceManager::mqttCallback);
    MqttManager::subscribeToTopic(light->getHueStateTopic().c_str(), &InterfaceManager::mqttCallback);
  }
}

void InterfaceManager::processWakeEvent() {
  LOG_DEBUG("Got wake event from panel, activating home page.");
  PageManager::GetHomePage()->show();
}

void InterfaceManager::processSleepEvent() {
  // Display went to sleep, reset everything
  LOG_DEBUG("Got sleep event from panel, activating screensaver.");
  InterfaceManager::instance->_isFingerOnDisplay = false; // Reset in case it got stuck
  RoomManager::goToRoomId(InterfaceConfig::homeScreen);
  InterfaceManager::instance->_changeMode(roomMode::room);
  InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
  InterfaceManager::instance->_updatePanelLightStatus();
  // Display auto-magically goes to screensaver page. Only ajust dim-level
  PageManager::GetScreensaverPage()->show();
}

void InterfaceManager::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  // TODO: Split processing of touch events to a function in each page and make this the router to the correct page
  InterfaceManager::instance->_isFingerOnDisplay = pressed;

  if (!pressed && InterfaceManager::instance->_ignoreNextTouchRelease) {
    InterfaceManager::instance->_ignoreNextTouchRelease = false; // Reset block
    return;
  }

  if (page == HOME_PAGE_ID && !pressed) {
    if (component == SWITCH_ROOM_BUTTON_ID && InterfaceManager::instance->_currentRoomMode == roomMode::room) {
      RoomManager::goToNextRoom();
      InterfaceManager::instance->_updatePanelWithNewRoomInfo();
      InterfaceManager::instance->_stopSpecialMode();
    } else if (component == SWITCH_MODE_BUTTON_ID) {
      InterfaceManager::instance->_goToNextMode();
      InterfaceManager::instance->_stopSpecialMode();
    } else if (component == CEILING_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceManager::instance->_currentEditMode == editLightMode::ceiling_lights || InterfaceManager::instance->_currentEditMode == editLightMode::all_lights) {
        InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
        InterfaceManager::instance->_lastMasterCeilingLightsButtonRelease = millis();
        InterfaceManager::instance->_ceilingMasterButtonEvent();
      }
    } else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceManager::instance->_currentEditMode == editLightMode::table_lights || InterfaceManager::instance->_currentEditMode == editLightMode::all_lights) {
        InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
        InterfaceManager::instance->_lastMasterTableLightsButtonRelease = millis();
        InterfaceManager::instance->_tableMasterButtonEvent();
      }
    } else if (component == HOME_LIGHT_LEVEL_SLIDER_ID) {
      // Dimmer slider changed, update cached value
      PageManager::GetHomePage()->updateDimmerValueCache();
      if (InterfaceManager::instance->_currentRoomMode == roomMode::room && (*RoomManager::currentRoom)->anyLightsOn()) {
        InterfaceManager::instance->_updateLightsThatAreOn();
      } else if (InterfaceManager::instance->_currentRoomMode == roomMode::house && (*RoomManager::currentRoom)->anyLightsOn()) {
        InterfaceManager::instance->_updateLightsThatAreOn();
      } else {
        InterfaceManager::instance->_updateAllLights();
      }

      InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
    } else if (component == HOME_LIGHT_COLOR_SLIDER_ID) {
      // Color temp slider changed, update cached value
      PageManager::GetHomePage()->updateColorTempValueCache();
      InterfaceManager::instance->_updateLightsColorTemp();
      InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
    } else if (component == ROOM_BUTTON_ID && InterfaceManager::instance->_currentRoomMode == roomMode::room) {
      // Show page with all lights
      InterfaceManager::instance->_stopSpecialMode();
      NSPanel::instance->goToPage(ROOM_PAGE_NAME);
      InterfaceManager::instance->_populateRoomPage();
    } else if (component == SCENES_BUTTON_ID) {
      ScenePage::show();
      ScenePage::showScenes((*RoomManager::currentRoom)->scenes);
      ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
  } else if (page == HOME_PAGE_ID && pressed) {
    if (component == CEILING_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceManager::instance->_currentEditMode == editLightMode::all_lights) {
        InterfaceManager::instance->_lastMasterCeilingLightsButtonTouch = millis();
        InterfaceManager::instance->_startSpecialModeTriggerTask(editLightMode::ceiling_lights);
      } else {
        InterfaceManager::instance->_ignoreNextTouchRelease = true;
        InterfaceManager::instance->_stopSpecialMode();
      }
    } else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID) {
      if (InterfaceManager::instance->_currentEditMode == editLightMode::all_lights) {
        InterfaceManager::instance->_lastMasterTableLightsButtonTouch = millis();
        InterfaceManager::instance->_startSpecialModeTriggerTask(editLightMode::table_lights);
      } else {
        InterfaceManager::instance->_ignoreNextTouchRelease = true;
        InterfaceManager::instance->_stopSpecialMode();
      }
    }
    LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
  } else if (page == ROOM_PAGE_ID && !pressed) {
    InterfaceManager::instance->_handleRoomPageComponentTouch(component);
    LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
  } else {
    LOG_DEBUG("Unhandled touch event: Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
  }
}

void InterfaceManager::_handleRoomPageComponentTouch(uint8_t component_id) {
  switch (component_id) {
  case ROOM_PAGE_BACK_BUTTON_ID:
    // NSPanel::instance->goToPage(HOME_PAGE_NAME);
    PageManager::GoBack();
    break;
  case ROOM_PAGE_PREVIOUS_ROOM_BUTTON_ID:
    RoomManager::goToPreviousRoom();
    this->_updateRoomPageStates();
    this->_populateRoomPage();
    break;
  case ROOM_PAGE_NEXT_ROOM_BUTTON_ID:
    RoomManager::goToNextRoom();
    this->_updateRoomPageStates();
    this->_populateRoomPage();
    break;
  case ROOM_LIGHT1_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(1);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(1, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT2_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(2);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(2, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT3_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(3);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(3, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT4_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(4);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(4, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT5_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(5);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(4, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT6_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(6);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(6, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT7_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(7);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(7, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT8_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(8);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(8, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT9_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(9);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(9, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT10_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(10);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(10, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT11_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(11);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(11, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT12_SW_CAP_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(12);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(12, light->getLightLevel() > 0);
    }
    break;
  }
  case ROOM_LIGHT1_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(1);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT2_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(2);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT3_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(3);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT4_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(4);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT5_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(5);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT6_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(6);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT7_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(7);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT8_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(8);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT9_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(9);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT10_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(10);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT11_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(11);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }
  case ROOM_LIGHT12_LABEL_ID: {
    Light *light = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(12);
    if (light != nullptr) {
      PageManager::GetLightPage()->selectedLight = light;
      PageManager::GetLightPage()->show();
      PageManager::GetLightPage()->updateValues();
    }
    break;
  }

  default:
    LOG_ERROR("Unknown component touched on room view: ", component_id);
    break;
  }
}

void InterfaceManager::_ceilingMasterButtonEvent() {
  if (this->_currentRoomMode == roomMode::room) {
    std::list<Light *> onLights = (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = (*RoomManager::currentRoom)->getAllCeilingLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    std::list<Light *> onLights = LightManager::getCeilingLightsThatAreOn();
    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = LightManager::getAllCeilingLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  }

  this->_updatePanelLightStatus();
}

void InterfaceManager::_tableMasterButtonEvent() {
  if (this->_currentRoomMode == roomMode::room) {
    std::list<Light *> onLights = (*RoomManager::currentRoom)->getTableLightsThatAreOn();

    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = (*RoomManager::currentRoom)->getAllTableLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
    this->_updatePanelLightStatus();
  } else if (this->_currentRoomMode == roomMode::house) {
    std::list<Light *> onLights = LightManager::getTableLightsThatAreOn();
    if (onLights.size() > 0) {
      LightManager::ChangeLightsToLevel(&onLights, 0);
    } else {
      std::list<Light *> lightList = LightManager::getAllTableLights();
      LightManager::ChangeLightsToLevel(&lightList, PageManager::GetHomePage()->getDimmingValue());
    }
  }

  this->_updatePanelLightStatus();
}

void InterfaceManager::_updateLightsThatAreOn() {
  std::list<Light *> lights;
  if (this->_currentRoomMode == roomMode::room) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = (*RoomManager::currentRoom)->getAllLightsThatAreOn();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if ((*RoomManager::currentRoom)->anyCeilingLightsOn()) {
        lights = (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if ((*RoomManager::currentRoom)->anyTableLightsOn()) {
        lights = (*RoomManager::currentRoom)->getTableLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllTableLights();
      }
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = LightManager::getAllLightsThatAreOn();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if (LightManager::anyCeilingLightsOn()) {
        lights = LightManager::getCeilingLightsThatAreOn();
      } else {
        lights = LightManager::getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if (LightManager::anyTableLightsOn()) {
        lights = LightManager::getTableLightsThatAreOn();
      } else {
        lights = LightManager::getAllTableLights();
      }
    }
  }

  uint8_t newLevel = PageManager::GetHomePage()->getDimmingValue();
  LightManager::ChangeLightsToLevel(&lights, newLevel);
  this->_updatePanelLightStatus();
}

void InterfaceManager::_updateAllLights() {
  std::list<Light *> lights;
  if (this->_currentRoomMode == roomMode::room) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      if ((*RoomManager::currentRoom)->anyLightsOn()) {
        lights = (*RoomManager::currentRoom)->getAllLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllLights();
      }
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if ((*RoomManager::currentRoom)->anyCeilingLightsOn()) {
        lights = (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if ((*RoomManager::currentRoom)->anyTableLightsOn()) {
        lights = (*RoomManager::currentRoom)->getTableLightsThatAreOn();
      } else {
        lights = (*RoomManager::currentRoom)->getAllTableLights();
      }
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      if (LightManager::anyLightsOn()) {
        lights = LightManager::getAllLightsThatAreOn();
      } else {
        lights = LightManager::getAllLights();
      }
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if (LightManager::anyCeilingLightsOn()) {
        lights = LightManager::getCeilingLightsThatAreOn();
      } else {
        lights = LightManager::getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if (LightManager::anyTableLightsOn()) {
        lights = LightManager::getTableLightsThatAreOn();
      } else {
        lights = LightManager::getAllTableLights();
      }
    }
  }

  uint8_t newLevel = PageManager::GetHomePage()->getDimmingValue();
  LightManager::ChangeLightsToLevel(&lights, newLevel);
  this->_updatePanelLightStatus();
}

void InterfaceManager::_updateLightsColorTemp() {
  std::list<Light *> lights;
  if (this->_currentRoomMode == roomMode::room) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = (*RoomManager::currentRoom)->getAllLights();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      lights = (*RoomManager::currentRoom)->getAllCeilingLights();
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      lights = (*RoomManager::currentRoom)->getAllTableLights();
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = LightManager::getAllLights();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      lights = LightManager::getAllCeilingLights();
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      lights = LightManager::getAllTableLights();
    }
  }

  uint16_t newKelvin = PageManager::GetHomePage()->getColorTempValue();
  LightManager::ChangeLightToColorTemperature(&lights, newKelvin);
  this->_updatePanelLightStatus();
}

void InterfaceManager::_populateRoomPage() {
  RoomPage::setCurrentRoomLabel((*RoomManager::currentRoom)->name.c_str());
  for (int i = 0; i < 12; i++) {
    Light *displayLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(i + 1);

    if (displayLight != nullptr) {
      // Add two spaces to the left of the name before sending name to panel
      // See issue #22
      std::string display_name = "  ";
      display_name.append(displayLight->getName());

      RoomPage::setLightName(displayLight->getRoomViewPosition(), display_name.c_str());
      RoomPage::setLightState(displayLight->getRoomViewPosition(), displayLight->getLightLevel() > 0);
      RoomPage::setLightVisibility(displayLight->getRoomViewPosition(), true);
    } else {
      RoomPage::setLightVisibility(i + 1, false); // If no light was found, hide the position
    }
  }
}

void InterfaceManager::_updateRoomPageStates() {
  for (Light *light : (*RoomManager::currentRoom)->getAllRoomViewLights()) {
    RoomPage::setLightState(light->getRoomViewPosition(), light->getLightLevel() > 0);
  }
}

void InterfaceManager::_setEditLightMode(editLightMode mode) {
  InterfaceManager::instance->_currentEditMode = mode; // Set current mode
  if (mode == editLightMode::all_lights) {
    PageManager::GetHomePage()->setSliderLightLevelColor(23243); // Reset to normal color
    PageManager::GetHomePage()->setSliderColorTempColor(23243);  // Reset to normal color
    PageManager::GetHomePage()->setHighlightCeilingVisibility(false);
    PageManager::GetHomePage()->setHighlightTableVisibility(false);
  } else {
    if (mode == editLightMode::ceiling_lights) {
      PageManager::GetHomePage()->setHighlightCeilingVisibility(true);
      PageManager::GetHomePage()->setHighlightTableVisibility(false);
    } else if (mode == editLightMode::table_lights) {
      PageManager::GetHomePage()->setHighlightTableVisibility(true);
      PageManager::GetHomePage()->setHighlightCeilingVisibility(false);
    }

    PageManager::GetHomePage()->setSliderLightLevelColor(65024); // Change slider color to indicate special mode
    PageManager::GetHomePage()->setSliderColorTempColor(65024);  // Change slider color to indicate special mode
  }
  this->_updatePanelLightStatus();
  InterfaceManager::instance->_startSpecialModeTimer();
}

void InterfaceManager::_startSpecialModeTimer() {
  this->_lastSpecialModeEventMillis = millis();
  if (this->_taskHandleSpecialModeTimer == NULL) {
    xTaskCreatePinnedToCore(_taskSpecialModeTimer, "taskSpecialModeTimer", 5000, NULL, 1, &InterfaceManager::_taskHandleSpecialModeTimer, CONFIG_ARDUINO_RUNNING_CORE);
  }
}

void InterfaceManager::_taskSpecialModeTimer(void *param) {
  // Wait until no event has occured for 5 seconds before returning to normal mode
  while (true) {
    if (!InterfaceManager::instance->_isFingerOnDisplay && millis() > InterfaceManager::instance->_lastSpecialModeEventMillis + InterfaceConfig::special_mode_release_time) {
      break;
    } else if (!InterfaceManager::instance->_isFingerOnDisplay && millis() > InterfaceManager::instance->_lastSpecialModeEventMillis + InterfaceConfig::special_mode_release_time) {
      InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
    }
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }

  InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
  InterfaceManager::_taskHandleSpecialModeTimer = NULL;
  vTaskDelete(NULL);
}

void InterfaceManager::_startSpecialModeTriggerTask(editLightMode triggerMode) {
  this->_triggerSpecialEditLightMode = triggerMode;
  xTaskCreatePinnedToCore(_taskSpecialModeTriggerTask, "taskSpecialModeTriggerTask", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::_stopSpecialMode() {
  if (InterfaceManager::_taskHandleSpecialModeTimer != NULL) {
    vTaskDelete(InterfaceManager::_taskHandleSpecialModeTimer);
    InterfaceManager::_taskHandleSpecialModeTimer = NULL;
  }
  InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
}

void InterfaceManager::_taskSpecialModeTriggerTask(void *param) {
  unsigned long start = millis();

  while (millis() < start + InterfaceConfig::special_mode_trigger_time) {
    if (!InterfaceManager::instance->_isFingerOnDisplay) {
      // User did not hold finger entire period, do not trigger special mode
      vTaskDelete(NULL);
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }

  if (InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::ceiling_lights) {
    InterfaceManager::instance->_ignoreNextTouchRelease = true;
    InterfaceManager::instance->_setEditLightMode(editLightMode::ceiling_lights);
  } else if (InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::table_lights) {
    InterfaceManager::instance->_ignoreNextTouchRelease = true;
    InterfaceManager::instance->_setEditLightMode(editLightMode::table_lights);
  } else if (InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::EXIT_SPECIAL_MODE) {
    InterfaceManager::instance->_ignoreNextTouchRelease = true;
    InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
  }

  vTaskDelete(NULL); // Task is complete, stop task
}

void InterfaceManager::_updatePanelWithNewRoomInfo() {
  if (this->_currentRoomMode == roomMode::room) {
    NSPanel::instance->setComponentText("home.room", (*RoomManager::currentRoom)->name.c_str());
  } else if (this->_currentRoomMode == roomMode::house) {
    NSPanel::instance->setComponentText("home.room", "<--ALL-->");
  }
  this->_updatePanelLightStatus();
}

void InterfaceManager::_goToNextMode() {
  roomMode newMode = static_cast<roomMode>(static_cast<int>(this->_currentRoomMode) + 1);
  if (newMode == roomMode::END) {
    newMode = roomMode::room;
  }
  this->_changeMode(newMode);
}

void InterfaceManager::_changeMode(roomMode mode) {
  this->_currentRoomMode = mode;
  if (this->_currentRoomMode == roomMode::room) {
    NSPanel::instance->setComponentText("home.mode", "Room lights");
    NSPanel::instance->setComponentPic(HOME_BUTTON_SCENES_NAME, HOME_BUTTON_SCENES_ROOM_MODE_PIC);
  } else if (this->_currentRoomMode == roomMode::house) {
    NSPanel::instance->setComponentText("home.mode", "All lights");
    NSPanel::instance->setComponentPic(HOME_BUTTON_SCENES_NAME, HOME_BUTTON_SCENES_ALL_MODE_PIC);
  } else {
    NSPanel::instance->setComponentText("home.mode", "UNKNOWN");
  }
  this->_updatePanelWithNewRoomInfo();
  this->_updatePanelLightStatus();
}

void InterfaceManager::mqttCallback(char *topic, byte *payload, unsigned int length) {
  // If an event has just occured, ignore any new events for
  // a while as it makes the display flicker
  if (millis() < InterfaceManager::instance->_ignoreMqttStatusUpdatesUntil) {
    return;
  }

  // if (InterfaceManager::_mqttMessages.size() < 10) {
  mqttMessage msg;
  msg.topic = topic;
  msg.payload = std::string((char *)payload, length);
  InterfaceManager::_mqttMessages.push_back(msg);
  if (InterfaceManager::_taskHandleProcessMqttMessages) {
    // Notify task that a new message needs processing
    vTaskNotifyGiveFromISR(InterfaceManager::_taskHandleProcessMqttMessages, NULL);
    portYIELD_FROM_ISR();
  }
}

void InterfaceManager::_taskProcessMqttMessages(void *param) {
  LOG_INFO("Started _taskProcessMqttMessages.");
  vTaskDelay(100 / portTICK_PERIOD_MS);
  for (;;) {
    // Wait for notification that we need to process messages.
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
      // Process all the messages
      while (InterfaceManager::_mqttMessages.size() > 0) {
        mqttMessage msg = InterfaceManager::_mqttMessages.front();
        try {
          if (msg.topic.compare(NSPMConfig::instance->mqtt_screen_cmd_topic) == 0) {
            if (msg.payload.compare("1") == 0) {
              PageManager::GetHomePage()->show();
              MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1"); // Send out state information that panel woke from sleep
            } else if (msg.payload.compare("0") == 0) {
              PageManager::GetScreensaverPage()->show();
            } else {
              LOG_ERROR("Invalid payload for screen cmd. Valid payload: 1 or 0");
            }
          } else if (msg.topic.find("nspanel/entities/") == 0) { // If topic begins with nspanel/entities/
            std::string domain = msg.topic;
            domain = domain.erase(0, strlen("nspanel/entities/"));
            domain = domain.substr(0, domain.find('/'));

            std::string entity = msg.topic;
            entity = entity.erase(0, strlen("nspanel/entities/"));
            entity = entity.erase(0, entity.find('/') + 1);
            entity = entity.substr(0, entity.find('/'));

            std::string attribute = msg.topic;
            attribute = attribute.erase(0, attribute.find_last_of('/') + 1);

            if (domain.compare("light") == 0 && attribute.compare("state_brightness_pct") == 0) {
              InterfaceManager::instance->_setLightLevel(atoi(entity.c_str()), atoi(msg.payload.c_str()));
            } else if (domain.compare("light") == 0 && attribute.compare("state_kelvin") == 0) {
              uint16_t colorTemp = atoi(msg.payload.c_str());
              if (colorTemp > InterfaceConfig::colorTempMax) {
                colorTemp = InterfaceConfig::colorTempMax;
              } else if (colorTemp < InterfaceConfig::colorTempMin) {
                colorTemp = InterfaceConfig::colorTempMin;
              }

              colorTemp = ((colorTemp - InterfaceConfig::colorTempMin) * 100) / (InterfaceConfig::colorTempMax - InterfaceConfig::colorTempMin);

              if (InterfaceConfig::reverseColorTempSlider) {
                colorTemp = 100 - colorTemp;
              }

              InterfaceManager::instance->_setLightColorTemperature(atoi(entity.c_str()), colorTemp);
            }
          }
        } catch (...) {
          LOG_ERROR("Error processing MQTT message on topic ", msg.topic.c_str());
        }
        InterfaceManager::_mqttMessages.pop_front();
        vTaskDelay(10 / portTICK_PERIOD_MS); // Wait 10ms between processing each event to allow for other tasks, ie. more MQTT messages to arrive.
      }
    }
  }
}

void InterfaceManager::_setLightLevel(uint16_t light_id, uint8_t level) {
  // TODO: Only update the displayed light level after all MQTT messages has processed
  Light *light = LightManager::getLightById(light_id);
  if (light != nullptr) {
    light->setLightLevel(level);
    this->_updatePanelLightStatus();
    this->_updateRoomPageStates();
    light->callUpdateCallbacks();
  }
}

void InterfaceManager::_setLightColorTemperature(uint16_t light_id, uint8_t level) {
  // TODO: Only update the displayed light level after all MQTT messages has processed
  // TODO: Only update the displayed light level after all MQTT messages has processed
  Light *light = LightManager::getLightById(light_id);
  if (light != nullptr) {
    light->setColorTemperature(level);
    this->_updatePanelLightStatus();
    this->_updateRoomPageStates();
    light->callUpdateCallbacks();
  }
}

void InterfaceManager::_onOffLight(Light *light) {
  std::list<Light *> lightsToChange;
  lightsToChange.push_back(light);
  if (light->getLightLevel() == 0) {
    LightManager::ChangeLightsToLevel(&lightsToChange, PageManager::GetHomePage()->getDimmingValue());
  } else {
    LightManager::ChangeLightsToLevel(&lightsToChange, 0);
  }
  this->_updatePanelLightStatus();
}

void InterfaceManager::_updatePanelLightStatus() {
  // TODO: Move this function to the "update" function on page HomeScreen
  uint totalBrightness = 0;
  uint totalBrightnessLights = 0;
  uint totalKelvinLightsCeiling = 0;
  uint16_t totalKelvinValueCeilingLights = 0;

  // Calculate average for ceiling lights
  if (this->_currentEditMode == editLightMode::all_lights || this->_currentEditMode == editLightMode::ceiling_lights) {
    if (this->_currentRoomMode == roomMode::room) {
      for (auto lightPair : (*RoomManager::currentRoom)->ceilingLights) {
        if (lightPair.second->getLightLevel() > 0) {
          totalBrightnessLights++;
          totalBrightness += lightPair.second->getLightLevel();
          if (lightPair.second->canTemperature()) {
            totalKelvinLightsCeiling++;
            totalKelvinValueCeilingLights += lightPair.second->getColorTemperature();
          }
        }
      }
    } else if (this->_currentRoomMode == roomMode::house) {
      for (Light *light : LightManager::getAllCeilingLights()) {
        if (light->getLightLevel() > 0) {
          totalBrightnessLights++;
          totalBrightness += light->getLightLevel();
          if (light->canTemperature()) {
            totalKelvinLightsCeiling++;
            totalKelvinValueCeilingLights += light->canTemperature();
          }
        }
      }
    }
  }

  uint8_t averageCeilingBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
  uint8_t averageCeilingKelvin = totalKelvinLightsCeiling == 0 ? 0 : totalKelvinValueCeilingLights / totalKelvinLightsCeiling;
  PageManager::GetHomePage()->setCeilingLightsState(averageCeilingBrightness > 0);
  PageManager::GetHomePage()->setCeilingBrightnessLabelText(averageCeilingBrightness);

  // Calculate average for table lights
  totalBrightness = 0;
  totalBrightnessLights = 0;
  uint8_t totalKelvinLightsTable = 0;
  uint16_t totalKelvinValueTableLights = 0;
  if (this->_currentEditMode == editLightMode::all_lights || this->_currentEditMode == editLightMode::table_lights) {
    if (this->_currentRoomMode == roomMode::room) {
      for (auto lightPair : (*RoomManager::currentRoom)->tableLights) {
        if (lightPair.second->getLightLevel() > 0) {
          totalBrightnessLights++;
          totalBrightness += lightPair.second->getLightLevel();
          if (lightPair.second->canTemperature()) {
            totalKelvinLightsTable++;
            totalKelvinValueTableLights += lightPair.second->getColorTemperature();
          }
        }
      }
    } else if (this->_currentRoomMode == roomMode::house) {
      for (Light *light : LightManager::getAllTableLights()) {
        if (light->getLightLevel() > 0) {
          totalBrightnessLights++;
          totalBrightness += light->getLightLevel();
          if (light->canTemperature()) {
            totalKelvinLightsTable++;
            totalKelvinValueTableLights += light->getColorTemperature();
          }
        }
      }
    }
  }
  uint8_t averageTableBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
  uint8_t averageTableKelvin = totalKelvinLightsTable == 0 ? 0 : totalKelvinValueTableLights / totalKelvinLightsTable;
  PageManager::GetHomePage()->setTableLightsState(averageTableBrightness > 0);
  PageManager::GetHomePage()->setTableBrightnessLabelText(averageTableBrightness);

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
  if (totalAverageBrightness > 0 && totalAverageBrightness != PageManager::GetHomePage()->getDimmingValue()) {
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
  if (totalAverageKelvin > 0 and totalAverageKelvin != PageManager::GetHomePage()->getColorTempValue()) {
    PageManager::GetHomePage()->setColorTempValue(totalAverageKelvin);
  }
}

bool InterfaceManager::_getPanelConfig() {
  WiFiClient client;
  unsigned long contentLength = 0;
  bool isValidContentType = false;
  if (client.connect(NSPMConfig::instance->manager_address.c_str(), NSPMConfig::instance->manager_port)) {
    client.print(String("GET /api/get_nspanel_config?mac=") + WiFi.macAddress() + " HTTP/1.1\r\n" +
                 "Host: " + NSPMConfig::instance->manager_address.c_str() + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        LOG_ERROR("Timeout while downloading firmware!");
        client.stop();
      }
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    while (client.available()) {
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      if (!line.length()) {
        // headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          LOG_ERROR("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        if (contentType == "application/json") {
          isValidContentType = true;
        } else {
          LOG_ERROR("Received invalid content type: ", contentType.c_str());
        }
      }
    }
  } else {
    LOG_ERROR("Failed to connect to manager!");
  }

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    DeserializationError error = deserializeJson((*this->_roomDataJson), client);
    if (!error) {
      return true;
    }
  } else {
    LOG_ERROR("There was no content in the OTA response!");
    client.flush();
  }
  return false;
}

bool InterfaceManager::_getRoomConfig(int room_id, DynamicJsonDocument *buffer) {
  WiFiClient client;
  unsigned long contentLength = 0;
  bool isValidContentType = false;
  if (client.connect(NSPMConfig::instance->manager_address.c_str(), NSPMConfig::instance->manager_port)) {
    client.print(String("GET /api/get_nspanel_config/room/") + String(room_id) + " HTTP/1.1\r\n" +
                 "Host: " + NSPMConfig::instance->manager_address.c_str() + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        LOG_ERROR("Timeout while downloading firmware!");
        client.stop();
      }
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    while (client.available()) {
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      if (!line.length()) {
        // headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          LOG_ERROR("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        if (contentType == "application/json") {
          isValidContentType = true;
        } else {
          LOG_ERROR("Received invalid content type: ", contentType.c_str());
        }
      }
    }
  } else {
    LOG_ERROR("Failed to connect to manager!");
  }

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    DeserializationError error = deserializeJson((*buffer), client);
    if (!error) {
      return true;
    }
  } else {
    LOG_ERROR("There was no content in the OTA response!");
    client.flush();
  }
  return false;
}
