#include <ButtonManager.hpp>
#include <InterfaceManager.hpp>
#include <MqttLog.hpp>
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
  NSPanel::attachTouchEventCallback(InterfaceManager::processTouchEvent);
  NSPanel::attachSleepCallback(InterfaceManager::processSleepEvent);
  NSPanel::attachWakeCallback(InterfaceManager::processWakeEvent);
  InterfaceManager::processWakeEvent(); // Send state update to MQTT that the screen turned on
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

    for (roomConfig &room : InterfaceManager::instance->config.rooms) {
      room.ceilingLights.clear();
      room.tableLights.clear();
    }

    InterfaceManager::instance->config.rooms.clear();
  } catch (const std::exception &e) {
    LOG_ERROR("Error while stopping Interface Manager: ", e.what());
  }
}

void InterfaceManager::_taskLoadConfigAndInit(void *param) {
  unsigned long start = millis();
  NspanelManagerPage::show();
  while (!WiFi.isConnected() || !MqttManager::connected() && !InterfaceManager::hasRegisteredToManager) {
    if (!WiFi.isConnected()) {
      if (NSPMConfig::instance->NSPMConfig::instance->wifi_ssid.empty()) {
        NspanelManagerPage::setText("Connect to AP NSPMPanel");
      } else {
        NspanelManagerPage::setText("Connecting to WiFi...");
      }
    } else if (!InterfaceManager::hasRegisteredToManager) {
      NspanelManagerPage::setText("Registring to manager...");
    } else if (!MqttManager::connected()) {
      NspanelManagerPage::setText("Connecting to MQTT...");
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  // Wait for panel to become ready
  if (millis() - start < 4000) {
    vTaskDelay((millis() - start) / portTICK_PERIOD_MS);
  }

  NspanelManagerPage::setText("Loading config...");

  InterfaceManager::instance->_roomDataJson = new DynamicJsonDocument(2048);
  uint8_t tries = 0;
  bool successDownloadingConfig = false;
  do {
    successDownloadingConfig = InterfaceManager::instance->_getPanelConfig();
    if (!successDownloadingConfig) {
      tries++;
      LOG_ERROR("Failed to download config, will try again in 5 seconds.");
      vTaskDelay(5000 / portTICK_PERIOD_MS);

      // 30 failed tries to download config, restart and try again.
      if (tries == 30) {
        LOG_ERROR("Failed to download config, will restart and try again.");
        NspanelManagerPage::setText("Restarting...");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP.restart();
      }
    }
  } while (!successDownloadingConfig);
  // Config downloaded, process the raw data
  InterfaceManager::instance->_processPanelConfig();
  delete InterfaceManager::instance->_roomDataJson; // All JSON-data processed, delete data from memory

  // Set some default values before showing page
  InterfaceManager::instance->_changeMode(roomMode::room);

  // Update Home page cache
  HomePage::updateDimmerValueCache();
  HomePage::updateColorTempValueCache();

  // Start task for MQTT processing
  xTaskCreatePinnedToCore(_taskProcessMqttMessages, "taskProcessMqttMessages", 5000, NULL, 1, &InterfaceManager::_taskHandleProcessMqttMessages, CONFIG_ARDUINO_RUNNING_CORE);

  // As there may be may be MANY topics to subscribe to, do it in checks of 5 with delays
  // between them to allow for processing all the incoming data.
  NspanelManagerPage::setText("Subscribing...");
  InterfaceManager::subscribeToMqttTopics();

  vTaskDelete(NULL); // Delete task, we are done
}

void InterfaceManager::subscribeToMqttTopics() {
  // Subscribe to command to wake/put to sleep the display
  vTaskDelay(100 / portTICK_PERIOD_MS);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_screen_cmd_topic.c_str(), &InterfaceManager::mqttCallback);

  // Every light in every room
  for (roomConfig &roomCfg : InterfaceManager::instance->config.rooms) {
    for (lightConfig &lightCfg : roomCfg.ceilingLights) {
      InterfaceManager::instance->_subscribeToLightTopics(&lightCfg);
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    for (lightConfig &lightCfg : roomCfg.tableLights) {
      InterfaceManager::instance->_subscribeToLightTopics(&lightCfg);
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
}

void InterfaceManager::_subscribeToLightTopics(lightConfig *cfg) {
  // Build topic from name
  std::string levelStatusTopic = "nspanel/entities/light/";
  levelStatusTopic.append(std::to_string(cfg->id));
  levelStatusTopic.append("/state_brightness_pct");
  MqttManager::subscribeToTopic(levelStatusTopic.c_str(), &InterfaceManager::mqttCallback);

  if (cfg->canTemperature) {
    std::string colorTempStateTopic = "nspanel/entities/light/";
    colorTempStateTopic.append(std::to_string(cfg->id));
    colorTempStateTopic.append("/state_kelvin");
    MqttManager::subscribeToTopic(colorTempStateTopic.c_str(), &InterfaceManager::mqttCallback);
  }

  if (cfg->canRgb) {
    std::string colorSaturationTopic = "nspanel/entities/light/";
    colorSaturationTopic.append(std::to_string(cfg->id));
    colorSaturationTopic.append("/state_sat");
    MqttManager::subscribeToTopic(colorSaturationTopic.c_str(), &InterfaceManager::mqttCallback);

    std::string colorHueTopic = "nspanel/entities/light/";
    colorHueTopic.append(std::to_string(cfg->id));
    colorHueTopic.append("/state_hue");
    MqttManager::subscribeToTopic(colorHueTopic.c_str(), &InterfaceManager::mqttCallback);
  }
}

void InterfaceManager::processWakeEvent() {
  // Send screen state
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1");
}

void InterfaceManager::processSleepEvent() {
  LOG_DEBUG("Display went to sleep, resetting display to default.");
  // Display went to sleep, reset everything
  InterfaceManager::instance->_isFingerOnDisplay = false; // Reset in case it got stuck
  InterfaceManager::instance->_changeRoom(InterfaceManager::instance->config.homeScreen);
  InterfaceManager::instance->_changeMode(roomMode::room);
  InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
  InterfaceManager::instance->_updatePanelLightStatus();

  // Send screen state
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "0");
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
      InterfaceManager::instance->_goToNextRoom();
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
      HomePage::updateDimmerValueCache();
      if (InterfaceManager::instance->_currentRoomMode == roomMode::room && InterfaceManager::instance->config.currentRoom->anyLightsOn()) {
        InterfaceManager::instance->_updateLightsThatAreOn();
      } else if (InterfaceManager::instance->_currentRoomMode == roomMode::house && InterfaceManager::instance->config.anyLightsOn()) {
        InterfaceManager::instance->_updateLightsThatAreOn();
      } else {
        InterfaceManager::instance->_updateAllLights();
      }

      InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
    } else if (component == HOME_LIGHT_COLOR_SLIDER_ID) {
      // Color temp slider changed, update cached value
      HomePage::updateColorTempValueCache();
      InterfaceManager::instance->_updateLightsColorTemp();
      InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
    } else if (component == ROOM_BUTTON_ID && InterfaceManager::instance->_currentRoomMode == roomMode::room) {
      // Show page with all lights
      InterfaceManager::instance->_stopSpecialMode();
      NSPanel::instance->goToPage(ROOM_PAGE_NAME);
      InterfaceManager::instance->_populateRoomPage();
    } else if (component == SCENES_BUTTON_ID) {
      ScenePage::show();
      ScenePage::showScenes(InterfaceManager::instance->config.currentRoom->scenes);
    }
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
  } else if (page == LIGHT_PAGE_ID && !pressed) {
    InterfaceManager::instance->_handleLightPageComponentTouch(component);
    LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
  } else if (page == SCENES_PAGE_ID) {
    ScenePage::processTouchEvent(page, component, pressed);
  } else {
    LOG_DEBUG("Unhandled touch event: Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
  }
}

void InterfaceManager::_handleRoomPageComponentTouch(uint8_t component_id) {
  switch (component_id) {
  case ROOM_PAGE_BACK_BUTTON_ID:
    NSPanel::instance->goToPage(HOME_PAGE_NAME);
    break;
  case ROOM_PAGE_PREVIOUS_ROOM_BUTTON_ID:
    this->_goToPreviousRoom();
    this->_populateRoomPage();
    break;
  case ROOM_PAGE_NEXT_ROOM_BUTTON_ID:
    this->_goToNextRoom();
    this->_populateRoomPage();
    break;
  case ROOM_LIGHT1_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(1);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(1, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT2_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(2);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(2, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT3_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(3);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(3, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT4_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(4);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(4, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT5_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(5);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(5, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT6_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(6);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(6, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT7_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(7);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(7, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT8_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(8);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(8, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT9_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(9);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(9, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT10_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(10);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(10, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT11_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(11);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(11, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT12_SW_CAP_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(12);
    if (light != nullptr) {
      this->_onOffLight(light);
      RoomPage::setLightState(12, light->level > 0);
    }
    break;
  }
  case ROOM_LIGHT1_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(1);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT2_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(2);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT3_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(3);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT4_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(4);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT5_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(5);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT6_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(6);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT7_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(7);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT8_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(8);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT9_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(9);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT10_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(10);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT11_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(11);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }
  case ROOM_LIGHT12_LABEL_ID: {
    lightConfig *light = this->config.currentRoom->getLightAtRoomViewPosition(12);
    if (light != nullptr) {
      LightPage::selectedLight = light;
      LightPage::show();
      LightPage::updateValues();
    }
    break;
  }

  default:
    LOG_ERROR("Unknown component touched on room view: ", component_id);
    break;
  }
}

void InterfaceManager::_handleLightPageComponentTouch(uint8_t component_id) {
  switch (component_id) {
  case LIGHT_PAGE_BACK_BUTTON_ID: {
    NSPanel::instance->goToPage(ROOM_PAGE_NAME);
    InterfaceManager::instance->_populateRoomPage();
    break;
  }
  case LIGHT_PAGE_BRIGHTNESS_SLIDER_ID: {
    if (LightPage::selectedLight != nullptr) {
      std::list<lightConfig *> lights;
      lights.push_back(LightPage::selectedLight);
      this->_changeLightsToLevel(&lights, LightPage::getBrightnessValue());
      // LightPage::updateValues(); Not needed as slider changes directly
    }
    break;
  }
  case LIGHT_PAGE_KELVIN_SLIDER_ID: {
    if (LightPage::selectedLight != nullptr) {
      std::list<lightConfig *> lights;
      lights.push_back(LightPage::selectedLight);
      if (LightPage::getCurrentMode() == LIGHT_PAGE_MODE::COLOR_TEMP) {
        this->_changeLightsToKelvin(&lights, LightPage::getKelvinSatValue());
      } else if (LightPage::getCurrentMode() == LIGHT_PAGE_MODE::COLOR_RGB) {
        this->_changeLightsToColorSaturation(&lights, LightPage::getKelvinSatValue());
      }
      // LightPage::updateValues(); Not needed as slider changes directly
    }
    break;
  }
  case LIGHT_PAGE_HUE_SLIDER_ID: {
    if (LightPage::selectedLight != nullptr) {
      std::list<lightConfig *> lights;
      lights.push_back(LightPage::selectedLight);
      this->_changeLightsToColorHue(&lights, LightPage::getHueValue());
    }
    break;
  }
  case LIGHT_PAGE_SWITCH_MODE_BUTTON_ID: {
    LightPage::switchMode();
    break;
  }

  default:
    break;
  }
}

void InterfaceManager::_ceilingMasterButtonEvent() {
  if (this->_currentRoomMode == roomMode::room) {
    std::list<lightConfig *> onLights = this->config.currentRoom->getCeilingLightsThatAreOn();
    if (onLights.size() > 0) {
      this->_changeLightsToLevel(&onLights, 0);
    } else {
      std::list<lightConfig *> lightList = this->config.currentRoom->getAllCeilingLights();
      this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    std::list<lightConfig *> onLights = this->config.getCeilingLightsThatAreOn();
    if (onLights.size() > 0) {
      this->_changeLightsToLevel(&onLights, 0);
    } else {
      std::list<lightConfig *> lightList = this->config.getAllCeilingLights();
      this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
    }
  }

  this->_updatePanelLightStatus();
}

void InterfaceManager::_tableMasterButtonEvent() {
  if (this->_currentRoomMode == roomMode::room) {
    std::list<lightConfig *> onLights = this->config.currentRoom->getTableLightsThatAreOn();

    if (onLights.size() > 0) {
      this->_changeLightsToLevel(&onLights, 0);
    } else {
      std::list<lightConfig *> lightList = this->config.currentRoom->getAllTableLights();
      this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
    }
    this->_updatePanelLightStatus();
  } else if (this->_currentRoomMode == roomMode::house) {
    std::list<lightConfig *> onLights = this->config.getTableLightsThatAreOn();
    if (onLights.size() > 0) {
      this->_changeLightsToLevel(&onLights, 0);
    } else {
      std::list<lightConfig *> lightList = this->config.getAllTableLights();
      this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
    }
  }

  this->_updatePanelLightStatus();
}

void InterfaceManager::_updateLightsThatAreOn() {
  std::list<lightConfig *> lights;
  if (this->_currentRoomMode == roomMode::room) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = this->config.currentRoom->getAllLightsThatAreOn();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if (this->config.currentRoom->anyCeilingLightsOn()) {
        lights = this->config.currentRoom->getCeilingLightsThatAreOn();
      } else {
        lights = this->config.currentRoom->getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if (this->config.currentRoom->anyTableLightstOn()) {
        lights = this->config.currentRoom->getTableLightsThatAreOn();
      } else {
        lights = this->config.currentRoom->getAllTableLights();
      }
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = this->config.getAllLightsThatAreOn();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if (this->config.anyCeilingLightsOn()) {
        lights = this->config.getCeilingLightsThatAreOn();
      } else {
        lights = this->config.getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if (this->config.anyTableLightstOn()) {
        lights = this->config.getTableLightsThatAreOn();
      } else {
        lights = this->config.getAllTableLights();
      }
    }
  }

  uint8_t newLevel = HomePage::getDimmingValue();
  this->_changeLightsToLevel(&lights, newLevel);
  this->_updatePanelLightStatus();
}

void InterfaceManager::_updateAllLights() {
  std::list<lightConfig *> lights;
  if (this->_currentRoomMode == roomMode::room) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      if (this->config.currentRoom->anyLightsOn()) {
        lights = this->config.currentRoom->getAllLightsThatAreOn();
      } else {
        lights = this->config.currentRoom->getAllLights();
      }
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if (this->config.currentRoom->anyCeilingLightsOn()) {
        lights = this->config.currentRoom->getCeilingLightsThatAreOn();
      } else {
        lights = this->config.currentRoom->getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if (this->config.currentRoom->anyTableLightstOn()) {
        lights = this->config.currentRoom->getTableLightsThatAreOn();
      } else {
        lights = this->config.currentRoom->getAllTableLights();
      }
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      if (this->config.anyLightsOn()) {
        lights = this->config.getAllLightsThatAreOn();
      } else {
        lights = this->config.getAllLights();
      }
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      if (this->config.anyCeilingLightsOn()) {
        lights = this->config.getCeilingLightsThatAreOn();
      } else {
        lights = this->config.getAllCeilingLights();
      }
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      if (this->config.anyTableLightstOn()) {
        lights = this->config.getTableLightsThatAreOn();
      } else {
        lights = this->config.getAllTableLights();
      }
    }
  }

  uint8_t newLevel = HomePage::getDimmingValue();
  this->_changeLightsToLevel(&lights, newLevel);
  this->_updatePanelLightStatus();
}

void InterfaceManager::_updateLightsColorTemp() {
  std::list<lightConfig *> lights;
  if (this->_currentRoomMode == roomMode::room) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = this->config.currentRoom->getAllLights();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      lights = this->config.currentRoom->getAllCeilingLights();
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      lights = this->config.currentRoom->getAllTableLights();
    }
  } else if (this->_currentRoomMode == roomMode::house) {
    if (this->_currentEditMode == editLightMode::all_lights) {
      lights = this->config.getAllLights();
    } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
      lights = this->config.getAllCeilingLights();
    } else if (this->_currentEditMode == editLightMode::table_lights) {
      lights = this->config.getAllTableLights();
    }
  }

  uint16_t newKelvin = HomePage::getColorTempValue();
  this->_changeLightsToKelvin(&lights, newKelvin);
  this->_updatePanelLightStatus();
}

void InterfaceManager::_populateRoomPage() {
  RoomPage::setCurrentRoomLabel(this->config.currentRoom->name.c_str());
  for (int i = 0; i < 12; i++) {
    lightConfig *displayLight = this->config.currentRoom->getLightAtRoomViewPosition(i + 1);

    if (displayLight != nullptr) {
      // Add two spaces to the left of the name before sending name to panel
      // See issue #22
      std::string display_name = "  ";
      display_name.append(displayLight->name);

      RoomPage::setLightName(displayLight->roomViewPosition, display_name.c_str());
      RoomPage::setLightState(displayLight->roomViewPosition, displayLight->level > 0);
      RoomPage::setLightVisibility(displayLight->roomViewPosition, true);
    } else {
      RoomPage::setLightVisibility(i + 1, false); // If no light was found, hide the position
    }
  }
}

void InterfaceManager::_updateRoomPageStates() {
  for (lightConfig *light : this->config.currentRoom->getAllRoomViewLights()) {
    RoomPage::setLightState(light->roomViewPosition, light->level > 0);
  }
}

void InterfaceManager::_setEditLightMode(editLightMode mode) {
  InterfaceManager::instance->_currentEditMode = mode; // Set current mode
  if (mode == editLightMode::all_lights) {
    HomePage::setSliderLightLevelColor(23243); // Reset to normal color
    HomePage::setSliderColorTempColor(23243);  // Reset to normal color
    HomePage::setHighlightCeilingVisibility(false);
    HomePage::setHighlightTableVisibility(false);
  } else {
    if (mode == editLightMode::ceiling_lights) {
      HomePage::setHighlightCeilingVisibility(true);
      HomePage::setHighlightTableVisibility(false);
    } else if (mode == editLightMode::table_lights) {
      HomePage::setHighlightTableVisibility(true);
      HomePage::setHighlightCeilingVisibility(false);
    }

    HomePage::setSliderLightLevelColor(65024); // Change slider color to indicate special mode
    HomePage::setSliderColorTempColor(65024);  // Change slider color to indicate special mode
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
    if (!InterfaceManager::instance->_isFingerOnDisplay && millis() > InterfaceManager::instance->_lastSpecialModeEventMillis + InterfaceManager::instance->config.special_mode_release_time) {
      break;
    } else if (!InterfaceManager::instance->_isFingerOnDisplay && millis() > InterfaceManager::instance->_lastSpecialModeEventMillis + InterfaceManager::instance->config.special_mode_release_time) {
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

  while (millis() < start + InterfaceManager::instance->config.special_mode_trigger_time) {
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

void InterfaceManager::_processPanelConfig() {
  this->config.homeScreen = (*this->_roomDataJson)["home"].as<uint8_t>();
  this->config.colorTempMin = (*this->_roomDataJson)["color_temp_min"].as<uint16_t>();
  this->config.colorTempMax = (*this->_roomDataJson)["color_temp_max"].as<uint16_t>();
  this->config.reverseColorTempSlider = (*this->_roomDataJson)["reverse_color_temp"].as<String>().equals("True");
  this->config.raiseToMaxLightLevelAbove = (*this->_roomDataJson)["raise_to_100_light_level"].as<uint8_t>();
  this->config.button_min_press_time = (*this->_roomDataJson)["min_button_push_time"].as<uint16_t>();
  this->config.button_long_press_time = (*this->_roomDataJson)["button_long_press_time"].as<uint16_t>();
  this->config.special_mode_trigger_time = (*this->_roomDataJson)["special_mode_trigger_time"].as<uint16_t>();
  this->config.special_mode_release_time = (*this->_roomDataJson)["special_mode_release_time"].as<uint16_t>();
  this->config.mqtt_ignore_time = (*this->_roomDataJson)["mqtt_ignore_time"].as<uint16_t>();
  NSPMConfig::instance->button1_mode = static_cast<BUTTON_MODE>((*this->_roomDataJson)["button1_mode"].as<uint8_t>());
  NSPMConfig::instance->button2_mode = static_cast<BUTTON_MODE>((*this->_roomDataJson)["button2_mode"].as<uint8_t>());

  uint8_t numberOfRooms = (*this->_roomDataJson)["rooms"].as<JsonArray>().size();
  uint8_t currentRoom = 1;
  DynamicJsonDocument *buffer = new DynamicJsonDocument(2048);
  for (uint8_t roomId : (*this->_roomDataJson)["rooms"].as<JsonArray>()) {
    LOG_INFO("Getting config for room ", roomId);
    // Display what room we are getting configuration for.
    std::string info_text = "Loading room ";
    info_text.append(std::to_string(currentRoom));
    info_text.append("/");
    info_text.append(std::to_string(numberOfRooms));
    NspanelManagerPage::setText(info_text.c_str());

    // Try downloading room config for a long as needed
    for (;;) {
      if (this->_getRoomConfig(roomId, buffer)) {
        break;
      } else {
        LOG_ERROR("Failed to download room config, will try again in 5 seconds.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
      }
    }

    roomConfig roomCfg;
    roomCfg.id = roomId;
    roomCfg.name = (*buffer)["name"] | "ERR";
    JsonVariant json_lights = (*buffer)["lights"];
    for (JsonPair lightPair : json_lights.as<JsonObject>()) {
      lightConfig lightCfg;
      lightCfg.id = atoi(lightPair.key().c_str());
      lightCfg.name = lightPair.value()["name"] | "ERR-L";
      lightCfg.canDim = lightPair.value()["can_dim"];
      lightCfg.canTemperature = lightPair.value()["can_temperature"];
      lightCfg.canRgb = lightPair.value()["can_rgb"];
      lightCfg.roomViewPosition = lightPair.value()["view_position"] | 0;
      if (lightPair.value()["ceiling"] == true) {
        roomCfg.ceilingLights.push_back(lightCfg);
      } else {
        roomCfg.tableLights.push_back(lightCfg);
      }
    }

    JsonVariant json_scenes = (*buffer)["scenes"];
    for (JsonPair scenePair : json_scenes.as<JsonObject>()) {
      sceneConfig sceneCfg;
      sceneCfg.id = atoi(scenePair.key().c_str());
      sceneCfg.name = scenePair.value()["name"] | "ERR-S";
      roomCfg.scenes.push_back(sceneCfg);
      LOG_DEBUG("Loaded scene ", sceneCfg.id, "::", sceneCfg.name.c_str());
    }
    this->config.rooms.push_back(roomCfg);

    // Update the reference back from the scene to the room
    for (sceneConfig &sceneCfg : this->config.rooms.back().scenes) {
      sceneCfg.room = &this->config.rooms.back();
    }
    buffer->clear();
  }
  delete buffer;

  // All rooms and lights has loaded, prep buttonmanager
  if (NSPMConfig::instance->button1_mode == BUTTON_MODE::DETACHED) {
    ButtonManager::button1_detached_mode_light = InterfaceManager::instance->config.getLightById((*this->_roomDataJson)["button1_detached_light"].as<uint16_t>());
    if (ButtonManager::button1_detached_mode_light != nullptr) {
      LOG_DEBUG("Button 1 detached mode light: ", ButtonManager::button1_detached_mode_light->name.c_str());
    } else {
      LOG_ERROR("Coudln't find Button 1 detached mode light with ID: ", (*this->_roomDataJson)["button1_detached_light"].as<uint16_t>());
    }
  } else {
    ButtonManager::button1_detached_mode_light = nullptr;
  }
  if (NSPMConfig::instance->button2_mode == BUTTON_MODE::DETACHED) {
    ButtonManager::button2_detached_mode_light = InterfaceManager::instance->config.getLightById((*this->_roomDataJson)["button2_detached_light"].as<uint16_t>());
    if (ButtonManager::button2_detached_mode_light != nullptr) {
      LOG_DEBUG("Button 2 detached mode light: ", ButtonManager::button2_detached_mode_light->name.c_str());
    } else {
      LOG_ERROR("Coudln't find Button 2 detached mode light with ID: ", (*this->_roomDataJson)["button2_detached_light"].as<uint16_t>());
    }
  } else {
    ButtonManager::button2_detached_mode_light = nullptr;
  }

  // Panel config processed, clear Json data
  this->_roomDataJson->clear();

  this->_changeRoom(this->config.homeScreen);
  NSPanel::instance->goToPage("home");
}

void InterfaceManager::_goToNextRoom() {
  this->config.currentRoom++;
  if (this->config.currentRoom == this->config.rooms.end()) {
    this->config.currentRoom = this->config.rooms.begin();
  }
  this->_updatePanelWithNewRoomInfo();
}

void InterfaceManager::_goToPreviousRoom() {
  if (this->config.currentRoom == this->config.rooms.begin()) {
    this->config.currentRoom = this->config.rooms.end();
    this->config.currentRoom--;
  } else {
    this->config.currentRoom--;
  }
  this->_updatePanelWithNewRoomInfo();
}

void InterfaceManager::_changeRoom(uint8_t roomId) {
  bool foundRoom = false;
  for (std::list<roomConfig>::iterator it = this->config.rooms.begin(); it != this->config.rooms.end(); it++) {
    if (it->id == roomId) {
      this->config.currentRoom = it;
      this->_updatePanelWithNewRoomInfo();
      return;
    }
  }

  LOG_ERROR("Did not find requested room. Will cancel operation.");
  return;
}

void InterfaceManager::_updatePanelWithNewRoomInfo() {
  if (this->_currentRoomMode == roomMode::room) {
    NSPanel::instance->setComponentText("home.room", this->config.currentRoom->name.c_str());
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
  } else if (this->_currentRoomMode == roomMode::house) {
    NSPanel::instance->setComponentText("home.mode", "All lights");
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

  if (InterfaceManager::_mqttMessages.size() < 10) {
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
              NSPanel::instance->setDimLevel(100);
              NSPanel::instance->goToPage(HOME_PAGE_NAME);
              InterfaceManager::processWakeEvent(); // Send out state information that panel woke from sleep
            } else if (msg.payload.compare("0") == 0) {
              NSPanel::instance->setDimLevel(0);
              NSPanel::instance->goToPage(SCREENSAVE_PAGE_NAME);
              // InterfaceManager::processSleepEvent(); // This is triggered by "PostInit events" from the Nextion panel.
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
              if (colorTemp > InterfaceManager::instance->config.colorTempMax) {
                colorTemp = InterfaceManager::instance->config.colorTempMax;
              } else if (colorTemp < InterfaceManager::instance->config.colorTempMin) {
                colorTemp = InterfaceManager::instance->config.colorTempMin;
              }

              colorTemp = ((colorTemp - InterfaceManager::instance->config.colorTempMin) * 100) / (InterfaceManager::instance->config.colorTempMax - InterfaceManager::instance->config.colorTempMin);

              if (InterfaceManager::instance->config.reverseColorTempSlider) {
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
  for (roomConfig &roomCfg : InterfaceManager::instance->config.rooms) {
    for (lightConfig &lightCfg : roomCfg.ceilingLights) {
      if (lightCfg.id == light_id) {
        if (lightCfg.level != level) {
          lightCfg.level = level;
          this->_updatePanelLightStatus();
          this->_updateRoomPageStates();
        }
        return;
      }
    }

    for (lightConfig &lightCfg : roomCfg.tableLights) {
      if (lightCfg.id == light_id) {
        if (lightCfg.level != level) {
          lightCfg.level = level;
          this->_updatePanelLightStatus();
          this->_updateRoomPageStates();
        }
        return;
      }
    }
  }
}

void InterfaceManager::_setLightColorTemperature(uint16_t light_id, uint8_t level) {
  // TODO: Only update the displayed light level after all MQTT messages has processed
  for (roomConfig &roomCfg : InterfaceManager::instance->config.rooms) {
    for (lightConfig &lightCfg : roomCfg.ceilingLights) {
      if (lightCfg.id == light_id) {
        if (lightCfg.colorTemperature != level) {
          lightCfg.colorTemperature = level;
          this->_updatePanelLightStatus();
        }
        return;
      }
    }

    for (lightConfig &lightCfg : roomCfg.tableLights) {
      if (lightCfg.id == light_id) {
        if (lightCfg.colorTemperature != level) {
          lightCfg.colorTemperature = level;
          this->_updatePanelLightStatus();
        }
        return;
      }
    }
  }
}

void InterfaceManager::_onOffLight(lightConfig *light) {
  std::list<lightConfig *> lightsToChange;
  lightsToChange.push_back(light);
  if (light->level == 0) {
    this->_changeLightsToLevel(&lightsToChange, HomePage::getDimmingValue());
  } else {
    this->_changeLightsToLevel(&lightsToChange, 0);
  }
  this->_updatePanelLightStatus();
}

void InterfaceManager::_changeLightsToLevel(std::list<lightConfig *> *lights, uint8_t level) {
  this->_ignoreMqttStatusUpdatesUntil = millis() + this->config.mqtt_ignore_time;

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
  doc["method"] = "set";
  doc["attribute"] = "brightness";
  doc["brightness"] = level;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (lightConfig *light : (*lights)) {
    entity_ids.add(light->id);
  }

  char buffer[1024];
  size_t json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (lightConfig *light : (*lights)) {
      light->level = level;
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void InterfaceManager::_changeLightsToKelvin(std::list<lightConfig *> *lights, uint16_t kelvin) {
  this->_ignoreMqttStatusUpdatesUntil = millis() + this->config.mqtt_ignore_time;

  uint16_t sendKelvin = kelvin * ((this->config.colorTempMax - this->config.colorTempMin) / 100);
  if (this->config.reverseColorTempSlider) {
    sendKelvin = this->config.colorTempMax - sendKelvin;
  } else {
    sendKelvin = this->config.colorTempMin + sendKelvin;
  }

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
  doc["method"] = "set";
  doc["attribute"] = "kelvin";
  doc["kelvin"] = sendKelvin;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (lightConfig *light : (*lights)) {
    entity_ids.add(light->id);
  }

  char buffer[1024];
  uint json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (lightConfig *light : (*lights)) {
      light->colorTemperature = kelvin;
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void InterfaceManager::_changeLightsToColorSaturation(std::list<lightConfig *> *lights, uint8_t saturation) {
  this->_ignoreMqttStatusUpdatesUntil = millis() + this->config.mqtt_ignore_time;

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
  doc["method"] = "set";
  doc["attribute"] = "saturation";
  doc["saturation"] = saturation;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (lightConfig *light : (*lights)) {
    entity_ids.add(light->id);
  }

  char buffer[1024];
  uint json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (lightConfig *light : (*lights)) {
      light->colorSat = saturation;
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void InterfaceManager::_changeLightsToColorHue(std::list<lightConfig *> *lights, uint16_t hue) {
  this->_ignoreMqttStatusUpdatesUntil = millis() + this->config.mqtt_ignore_time;

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
  doc["method"] = "set";
  doc["attribute"] = "hue";
  doc["hue"] = hue;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (lightConfig *light : (*lights)) {
    entity_ids.add(light->id);
  }

  char buffer[1024];
  uint json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (lightConfig *light : (*lights)) {
      light->colorHue = hue;
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void InterfaceManager::activateScene(sceneConfig scene) {
  std::string mqtt_topic = "nspanel/";
  mqtt_topic.append(std::to_string(scene.id));
  mqtt_topic.append("/activate");
  MqttManager::publish(mqtt_topic, "1");
}

void InterfaceManager::saveScene(sceneConfig scene) {
  std::string mqtt_topic = "nspanel/";
  mqtt_topic.append(std::to_string(scene.id));
  mqtt_topic.append("/save");
  MqttManager::publish(mqtt_topic, "1");
}

void InterfaceManager::_updatePanelLightStatus() {
  uint totalBrightness = 0;
  uint totalBrightnessLights = 0;
  uint totalKelvinLightsCeiling = 0;
  uint16_t totalKelvinValueCeilingLights = 0;

  // Calculate average for ceiling lights
  if (this->_currentEditMode == editLightMode::all_lights || this->_currentEditMode == editLightMode::ceiling_lights) {
    if (this->_currentRoomMode == roomMode::room) {
      for (lightConfig &light : this->config.currentRoom->ceilingLights) {
        if (light.level > 0) {
          totalBrightnessLights++;
          totalBrightness += light.level;
          if (light.canTemperature) {
            totalKelvinLightsCeiling++;
            totalKelvinValueCeilingLights += light.colorTemperature;
          }
        }
      }
    } else if (this->_currentRoomMode == roomMode::house) {
      for (lightConfig *light : this->config.getAllCeilingLights()) {
        if (light->level > 0) {
          totalBrightnessLights++;
          totalBrightness += light->level;
          if (light->canTemperature) {
            totalKelvinLightsCeiling++;
            totalKelvinValueCeilingLights += light->colorTemperature;
          }
        }
      }
    }
  }

  uint8_t averageCeilingBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
  uint8_t averageCeilingKelvin = totalKelvinLightsCeiling == 0 ? 0 : totalKelvinValueCeilingLights / totalKelvinLightsCeiling;
  HomePage::setCeilingLightsState(averageCeilingBrightness > 0);
  HomePage::setCeilingBrightnessLabelText(averageCeilingBrightness);

  // Calculate average for table lights
  totalBrightness = 0;
  totalBrightnessLights = 0;
  uint8_t totalKelvinLightsTable = 0;
  uint16_t totalKelvinValueTableLights = 0;
  if (this->_currentEditMode == editLightMode::all_lights || this->_currentEditMode == editLightMode::table_lights) {
    if (this->_currentRoomMode == roomMode::room) {
      for (lightConfig &light : this->config.currentRoom->tableLights) {
        if (light.level > 0) {
          totalBrightnessLights++;
          totalBrightness += light.level;
          if (light.canTemperature) {
            totalKelvinLightsTable++;
            totalKelvinValueTableLights += light.colorTemperature;
          }
        }
      }
    } else if (this->_currentRoomMode == roomMode::house) {
      for (lightConfig *light : this->config.getAllTableLights()) {
        if (light->level > 0) {
          totalBrightnessLights++;
          totalBrightness += light->level;
          if (light->canTemperature) {
            totalKelvinLightsTable++;
            totalKelvinValueTableLights += light->colorTemperature;
          }
        }
      }
    }
  }
  uint8_t averageTableBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
  uint8_t averageTableKelvin = totalKelvinLightsTable == 0 ? 0 : totalKelvinValueTableLights / totalKelvinLightsTable;
  HomePage::setTableLightsState(averageTableBrightness > 0);
  HomePage::setTableBrightnessLabelText(averageTableBrightness);

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
  if (totalAverageBrightness > 0 && totalAverageBrightness != HomePage::getDimmingValue()) {
    HomePage::setDimmingValue(totalAverageBrightness);
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
  if (totalAverageKelvin > 0 and totalAverageKelvin != HomePage::getColorTempValue()) {
    HomePage::setColorTempValue(totalAverageKelvin);
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
