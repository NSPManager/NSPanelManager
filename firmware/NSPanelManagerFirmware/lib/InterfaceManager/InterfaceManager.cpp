#include "NSPMConfig.h"
#include "freertos/portmacro.h"
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
#include <WebManager.hpp>
#include <WiFi.h>
#include <WiFiClient.h>
#include <string>

void InterfaceManager::init() {
  this->instance = this;
  this->_processMqttMessages = true;
  RoomManager::init();
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1");
  xTaskCreatePinnedToCore(_taskLoadConfigAndInit, "taskLoadConfigAndInit", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::stop() {
  LOG_INFO("Stopping interface manager.");
  try {
    InterfaceManager::instance->_processMqttMessages = false; // Stop processing of MQTT messages.
    if (InterfaceManager::_taskHandleSpecialModeTimer != NULL) {
      vTaskDelete(InterfaceManager::_taskHandleSpecialModeTimer);
      InterfaceManager::_taskHandleSpecialModeTimer = NULL;
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
  InterfaceManager::subscribeToMqttTopics();
  std::string last_shown_secondary_text = "";
  while (!WiFi.isConnected() || !MqttManager::connected() || !InterfaceManager::hasRegisteredToManager || !NSPMConfig::instance->littlefs_mount_successfull) {
    if (NSPanel::instance->ready()) {
      std::string secondary_text = "";
      if (!NSPMConfig::instance->littlefs_mount_successfull) {
        PageManager::GetNSPanelManagerPage()->setText("LittleFS mount failed!");
      } else if (!WiFi.isConnected()) {
        if (WiFi.getMode() == WIFI_MODE_AP) {
          PageManager::GetNSPanelManagerPage()->setText("Connect to AP NSPMPanel");
          secondary_text = "Connect to IP 192.168.1.1";
        } else {
          PageManager::GetNSPanelManagerPage()->setText("Connecting to WiFi...");
          secondary_text = "";
        }
      } else if (!InterfaceManager::hasRegisteredToManager) {
        PageManager::GetNSPanelManagerPage()->setText("Registering to manager...");
      } else if (!MqttManager::connected()) {
        PageManager::GetNSPanelManagerPage()->setText("Connecting to MQTT...");
      }

      if (WiFi.isConnected()) {
        secondary_text = "IP: ";
        secondary_text.append(WiFi.localIP().toString().c_str());
      } else {
        secondary_text = "";
      }
      if (secondary_text.compare(last_shown_secondary_text) != 0) {
        PageManager::GetNSPanelManagerPage()->setSecondaryText(secondary_text);
      }
    }
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }

  // Start task for MQTT processing
  xTaskCreatePinnedToCore(_taskProcessMqttMessages, "taskProcessMqttMessages", 5000, NULL, 1, &InterfaceManager::_taskHandleProcessMqttMessages, CONFIG_ARDUINO_RUNNING_CORE);

  if (NSPanel::instance->ready()) {
    PageManager::GetNSPanelManagerPage()->setText("Loading config...");
    RoomManager::loadAllRooms(false);

    // Update Home page cache
    PageManager::GetHomePage()->updateDimmerValueCache();
    PageManager::GetHomePage()->updateColorTempValueCache();

    // As there may be may be MANY topics to subscribe to, do it in checks of 5 with delays
    // between them to allow for processing all the incoming data.
    PageManager::GetNSPanelManagerPage()->setText("Subscribing...");
    LightManager::subscribeToMqttLightUpdates();

    // Loading is done, show Home page
    NSPanel::instance->setDimLevel(InterfaceConfig::screen_dim_level);
    InterfaceManager::showDefaultPage();
    PageManager::GetHomePage()->setScreensaverTimeout(InterfaceConfig::screensaver_activation_timeout);
  }

  // NSPanel::attachTouchEventCallback(InterfaceManager::processTouchEvent);
  NSPanel::attachSleepCallback(InterfaceManager::processSleepEvent);
  NSPanel::attachWakeCallback(InterfaceManager::processWakeEvent);

  // Attach screen clock MQTT callback if configured from manager.
  PageManager::GetScreensaverPage()->init();
  if (InterfaceConfig::screensaver_mode.compare("no_screensaver") != 0) {
    PageManager::GetScreensaverPage()->attachMqttCallback();
  } else {
    LOG_DEBUG("Not attaching MQTT callback as panel is configured to not show screensaver.");
  }

  LOG_INFO("Config initialized. Closing taskLoadConfigAndInit");
  vTaskDelete(NULL); // Delete task, we are done
}

void InterfaceManager::showDefaultPage() {
  if (InterfaceConfig::default_page == DEFAULT_PAGE::MAIN_PAGE) {
    PageManager::GetHomePage()->show();
  } else if (InterfaceConfig::default_page == DEFAULT_PAGE::SCENES_PAGE) {
    PageManager::GetScenePage()->show();
  } else if (InterfaceConfig::default_page == DEFAULT_PAGE::ROOM_PAGE) {
    PageManager::GetRoomPage()->show();
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
          if (InterfaceManager::instance->_processMqttMessages) {
            if (msg.topic.compare(NSPMConfig::instance->mqtt_screen_cmd_topic) == 0) {
              if (msg.payload.compare("1") == 0) {
                InterfaceManager::showDefaultPage();
                MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1"); // Send out state information that panel woke from sleep
              } else if (msg.payload.compare("0") == 0) {
                PageManager::GetScreensaverPage()->show();
              } else {
                LOG_ERROR("Invalid payload for screen cmd. Valid payload: 1 or 0");
              }
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

void InterfaceManager::mqttCallback(char *topic, byte *payload, unsigned int length) {
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

void InterfaceManager::handleNSPanelCommand(char *topic, byte *payload, unsigned int length) {
  std::string payload_str = std::string((char *)payload, length);
  StaticJsonDocument<256> json;
  DeserializationError error = deserializeJson(json, payload_str);
  if (error) {
    LOG_ERROR("Failed to serialize NSPanel command.");
    return;
  }

  std::string command = json["command"].as<String>().c_str();
  if (command.compare("reboot") == 0) {
    ESP.restart();
  } else if (command.compare("firmware_update") == 0) {
    WebManager::startOTAUpdate();
  } else if (command.compare("tft_update") == 0) {
    InterfaceManager::stop();
    NSPanel::instance->startOTAUpdate();
  } else if (command.compare("register_accept") == 0) {
    NSPMConfig::instance->manager_address = json["address"].as<String>().c_str();
    NSPMConfig::instance->manager_port = json["port"].as<uint16_t>();
    InterfaceManager::hasRegisteredToManager = true;
  } else if (command.compare("reload") == 0) {
    RoomManager::performConfigReload();
  } else {
    LOG_WARNING("Received unknown command on MQTT: ", command.c_str());
  }
}

void InterfaceManager::handleNSPanelScreenBrightnessCommand(char *topic, byte *payload, unsigned int length) {
  std::string payload_str = std::string((char *)payload, length);
  int new_brightness = std::stoi(payload_str);
  if (new_brightness < 1) {
    new_brightness = 1;
  } else if (new_brightness > 100) {
    new_brightness = 100;
  }
  InterfaceConfig::screen_dim_level = new_brightness;
  if (PageManager::GetCurrentPage() != PageManager::GetScreensaverPage()) {
    NSPanel::instance->setDimLevel(new_brightness);
  }
}

void InterfaceManager::handleNSPanelScreensaverBrightnessCommand(char *topic, byte *payload, unsigned int length) {
  std::string payload_str = std::string((char *)payload, length);
  int new_brightness = std::stoi(payload_str);
  if (new_brightness < 0) {
    new_brightness = 0;
  } else if (new_brightness > 100) {
    new_brightness = 100;
  }
  InterfaceConfig::screensaver_dim_level = new_brightness;
  if (PageManager::GetCurrentPage() == PageManager::GetScreensaverPage()) {
    NSPanel::instance->setDimLevel(new_brightness);
  }
}

void InterfaceManager::subscribeToMqttTopics() {
  // Subscribe to command to wake/put to sleep the display
  vTaskDelay(100 / portTICK_PERIOD_MS);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_screen_cmd_topic.c_str(), &InterfaceManager::mqttCallback);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_cmd_topic.c_str(), &InterfaceManager::handleNSPanelCommand);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_screen_brightness_topic.c_str(), &InterfaceManager::handleNSPanelScreenBrightnessCommand);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_screensaver_brightness.c_str(), &InterfaceManager::handleNSPanelScreensaverBrightnessCommand);
}

void InterfaceManager::processWakeEvent() {
  LOG_DEBUG("Got wake event from panel, activating home page.");
  InterfaceManager::showDefaultPage();
}

void InterfaceManager::processSleepEvent() {
  // Display went to sleep, reset everything
  LOG_DEBUG("Got sleep event from panel, activating screensaver.");
  RoomManager::goToRoomId(InterfaceConfig::homeScreen);
  // Display auto-magically goes to screensaver page. Only ajust dim-level
  PageManager::GetScreensaverPage()->show();
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
