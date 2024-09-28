#include "NSPMConfig.h"
#include "freertos/portmacro.h"
#include <ArduinoJson.h>
#include <HomePage.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceManager.hpp>
#include <LightPage.hpp>
#include <MqttLog.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <RoomManager.hpp>
#include <TftDefines.h>
#include <WebManager.hpp>
#include <WiFi.h>
#include <WiFiClient.h>
#include <string>

void InterfaceManager::init() {
  this->instance = this;
  this->_processMqttMessages = true;
  this->_config_loaded = false;
  RoomManager::init();
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1");
  xTaskCreatePinnedToCore(_taskLoadConfigAndInit, "taskLoadConfigAndInit", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::stop() {
  LOG_INFO("Stopping interface manager.");
  vTaskDelay(50 / portTICK_PERIOD_MS);
  try {
    InterfaceManager::instance->_processMqttMessages = false; // Stop processing of MQTT messages.
    if (InterfaceManager::_taskHandleSpecialModeTimer != NULL) {
      vTaskDelete(InterfaceManager::_taskHandleSpecialModeTimer);
      InterfaceManager::_taskHandleSpecialModeTimer = NULL;
    }

    // TODO: Clear rooms with RoomManager
    // for (Room *room : RoomManager::rooms) {
    //   for (auto lightPair : room->ceilingLights) {
    //     delete lightPair.second;
    //   }
    //   room->ceilingLights.clear();
    //   for (auto lightPair : room->tableLights) {
    //     delete lightPair.second;
    //   }
    //   room->tableLights.clear();
    // }

    RoomManager::rooms.clear();
  } catch (const std::exception &e) {
    LOG_ERROR("Error while stopping Interface Manager: ", e.what());
  }

  LOG_INFO("InterfaceManager stopped.");
  vTaskDelay(50 / portTICK_PERIOD_MS);
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
          std::string wifi_connect_text = "Connecting to ";
          wifi_connect_text.append(NSPMConfig::instance->wifi_ssid.c_str());
          wifi_connect_text.append("...");
          PageManager::GetNSPanelManagerPage()->setText(wifi_connect_text);
          secondary_text = "";
        }
      } else if (!MqttManager::connected()) {
        PageManager::GetNSPanelManagerPage()->setText("Connecting to MQTT...");
      } else if (!InterfaceManager::hasRegisteredToManager) {
        PageManager::GetNSPanelManagerPage()->setText("Registering to manager...");
      }

      if (WiFi.isConnected()) {
        secondary_text = "Panel IP: ";
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

  if (NSPanel::instance->ready()) {
    PageManager::GetNSPanelManagerPage()->setText("Loading config...");
    while (!InterfaceManager::instance->_config_loaded) {
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Update Home page cache
    PageManager::GetHomePage()->updateDimmerValueCache();
    PageManager::GetHomePage()->updateColorTempValueCache();

    // As there may be may be MANY topics to subscribe to, do it in checks of 5 with delays
    // between them to allow for processing all the incoming data.
    PageManager::GetNSPanelManagerPage()->setText("Subscribing...");

    // Loading is done, show Home page
    NSPanel::instance->setDimLevel(InterfaceConfig::screen_dim_level);
    PageManager::GetHomePage()->setScreensaverTimeout(InterfaceConfig::screensaver_activation_timeout);
    InterfaceManager::showDefaultPage();
  }

  // NSPanel::attachTouchEventCallback(InterfaceManager::processTouchEvent);
  NSPanel::attachSleepCallback(InterfaceManager::processSleepEvent);
  NSPanel::attachWakeCallback(InterfaceManager::processWakeEvent);

  // Attach screen clock, weather and mode MQTT callback
  PageManager::GetScreensaverPage()->init();
  PageManager::GetScreensaverPage()->attachMqttCallback();

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

void InterfaceManager::handleNSPanelCommand(MQTTMessage *message) {
  JsonDocument json;
  DeserializationError error = deserializeJson(json, message->data);
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
    NSPMConfig::instance->mqttmanager_command_topic = "nspanel/mqttmanager_";
    NSPMConfig::instance->mqttmanager_command_topic.append(NSPMConfig::instance->manager_address);
    NSPMConfig::instance->mqttmanager_command_topic.append("/command");
    NSPMConfig::instance->nspanel_config_topic = json["config_topic"].as<String>().c_str();

    Serial.print("Received register accept from manager: ");
    Serial.print(NSPMConfig::instance->manager_address.c_str());
    Serial.print(" at port: ");
    Serial.println(NSPMConfig::instance->manager_port);
    LOG_DEBUG("Config topic: ", NSPMConfig::instance->nspanel_config_topic.c_str());
    LOG_INFO("Received register accept from manager ", NSPMConfig::instance->manager_address.c_str(), " with port: ", NSPMConfig::instance->manager_port);
    MqttManager::subscribeToTopic(NSPMConfig::instance->nspanel_config_topic.c_str(), &InterfaceManager::handleNSPanelConfigUpdate);
    InterfaceManager::hasRegisteredToManager = true;
  } else if (command.compare("reload") == 0) {
    if (InterfaceManager::hasRegisteredToManager && NSPMConfig::instance->successful_config_load) {
      // RoomManager::performConfigReload(); // TODO: Is this still needed?
    } else {
      LOG_ERROR("Received command to reload config when the panel hasn't yet registered to a manager or yet once successfully downloaded a config from the manager.");
    }
  } else {
    LOG_WARNING("Received unknown command on MQTT: ", command.c_str());
  }
}

void InterfaceManager::handleNSPanelScreenBrightnessCommand(MQTTMessage *message) {
  int new_brightness = std::stoi(message->data);
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

void InterfaceManager::handleNSPanelScreensaverBrightnessCommand(MQTTMessage *message) {
  int new_brightness = std::stoi(message->data);
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

void InterfaceManager::handleNSPanelConfigUpdate(MQTTMessage *message) {
  try {
    NSPanelConfig *config = nspanel_config__unpack(NULL, message->data.size(), (const uint8_t *)message->data.c_str());
    InterfaceManager::_new_config = *config;
    LOG_INFO("Received new config and successfully parsed it into protobuf. Will start _taskHandleConfigData.");
    xTaskCreatePinnedToCore(&InterfaceManager::_taskHandleConfigData, "handle_config", 5000, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
  } catch (const std::exception &e) {
    LOG_ERROR("Caught error while processing protobuf object on config topic.");
  }
}

void InterfaceManager::_taskHandleConfigData(void *param) {
  InterfaceConfig::homeScreen = InterfaceManager::_new_config.default_room;
  InterfaceConfig::default_page = static_cast<DEFAULT_PAGE>(InterfaceManager::_new_config.default_page);
  InterfaceConfig::button_min_press_time = InterfaceManager::_new_config.min_button_push_time;
  InterfaceConfig::button_long_press_time = InterfaceManager::_new_config.button_long_press_time;
  InterfaceConfig::special_mode_trigger_time = InterfaceManager::_new_config.special_mode_trigger_time;
  InterfaceConfig::special_mode_release_time = InterfaceManager::_new_config.special_mode_release_time;
  InterfaceConfig::screen_dim_level = InterfaceManager::_new_config.screen_dim_level;
  InterfaceConfig::screensaver_dim_level = InterfaceManager::_new_config.screensaver_dim_level;
  InterfaceConfig::screensaver_activation_timeout = InterfaceManager::_new_config.screensaver_activation_timeout;
  InterfaceConfig::screensaver_mode = InterfaceManager::_new_config.screensaver_mode;
  InterfaceConfig::clock_us_style = InterfaceManager::_new_config.clock_us_style;
  InterfaceConfig::lock_to_default_room = false; // TODO: Remove as only "allowed" rooms are loaded.
  InterfaceConfig::optimistic_mode = InterfaceManager::_new_config.optimistic_mode;

  LOG_DEBUG("Loaded screensaver mode: ", InterfaceConfig::screensaver_mode);
  LOG_DEBUG("Screensaver activation timeout: ", InterfaceConfig::screensaver_activation_timeout);

  NSPMConfig::instance->is_us_panel = InterfaceManager::_new_config.is_us_panel;
  NSPMConfig::instance->use_fahrenheit = InterfaceManager::_new_config.use_fahrenheit;
  NSPMConfig::instance->temperature_calibration = InterfaceManager::_new_config.temperature_calibration;
  NSPMConfig::instance->reverse_relays = InterfaceManager::_new_config.reverse_relays;
  NSPMConfig::instance->button1_mode = static_cast<BUTTON_MODE>(InterfaceManager::_new_config.button1_mode);
  NSPMConfig::instance->button1_mqtt_topic = InterfaceManager::_new_config.button1_mqtt_topic;
  NSPMConfig::instance->button1_mqtt_payload = InterfaceManager::_new_config.button1_mqtt_payload;
  NSPMConfig::instance->button2_mode = static_cast<BUTTON_MODE>(InterfaceManager::_new_config.button2_mode);
  NSPMConfig::instance->button2_mqtt_topic = InterfaceManager::_new_config.button2_mqtt_topic;
  NSPMConfig::instance->button2_mqtt_payload = InterfaceManager::_new_config.button2_mqtt_payload;

  bool save_new_config_to_littlefs_at_end = false;
  bool reboot_after_config_saved = false;

  if (NSPMConfig::instance->relay1_default_mode != InterfaceManager::_new_config.relay1_default_mode) {
    LOG_INFO("Saving new relay 1 default mode: ", InterfaceManager::_new_config.relay1_default_mode ? "ON" : "OFF");
    NSPMConfig::instance->relay1_default_mode = InterfaceManager::_new_config.relay1_default_mode;
    save_new_config_to_littlefs_at_end = true;
  }

  if (NSPMConfig::instance->relay2_default_mode != InterfaceManager::_new_config.relay2_default_mode) {
    LOG_INFO("Saving new relay 2 default mode: ", InterfaceManager::_new_config.relay2_default_mode ? "ON" : "OFF");
    NSPMConfig::instance->relay2_default_mode = InterfaceManager::_new_config.relay2_default_mode;
    save_new_config_to_littlefs_at_end = true;
  }

  if (NSPMConfig::instance->wifi_hostname.compare(InterfaceManager::_new_config.name) != 0) {
    save_new_config_to_littlefs_at_end = true;
    reboot_after_config_saved = true;
    NSPMConfig::instance->wifi_hostname = InterfaceManager::_new_config.name;
    LOG_INFO("Name has changed. Restarting.");
  }

  if (save_new_config_to_littlefs_at_end) {
    while (!NSPMConfig::instance->saveToLittleFS(false)) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if (reboot_after_config_saved) {
      ESP.restart();
      vTaskDelay(portMAX_DELAY);
      vTaskDelete(NULL);
    }
  }

  // Start loading scenes and rooms
  LOG_DEBUG("Loading global scenes.");
  InterfaceConfig::global_scenes.clear();
  for (int i = 0; i < InterfaceManager::_new_config.n_global_scenes; i++) {
    InterfaceConfig::global_scenes.push_back(*InterfaceManager::_new_config.global_scenes[i]);
    LOG_DEBUG("Loaded global scene ", InterfaceManager::_new_config.global_scenes[i]->name);
  }
  LOG_INFO("Loaded ", InterfaceConfig::global_scenes.size(), " global scenes.");

  InterfaceConfig::room_ids.clear();
  InterfaceConfig::room_ids.insert(InterfaceConfig::room_ids.end(), InterfaceManager::_new_config.room_ids, InterfaceManager::_new_config.room_ids + InterfaceManager::_new_config.n_room_ids);
  RoomManager::loadAllRooms(InterfaceManager::_new_config.room_ids, InterfaceManager::_new_config.n_room_ids);
  if (!RoomManager::hasValidCurrentRoom()) {
    LOG_DEBUG("Config loaded, will go to default room ID: ", InterfaceConfig::homeScreen);
    if (!RoomManager::goToRoomId(InterfaceConfig::homeScreen)) {
      LOG_ERROR("Failed to go to default room. Will go to first valid room in list.");
      if (RoomManager::rooms.size() > 0) {
        RoomManager::goToRoomId(RoomManager::rooms[0].id);
      } else {
        LOG_ERROR("No rooms loaded!");
      }
    }
  }

  InterfaceManager::instance->_config_loaded = true;
  LOG_INFO("Config loaded successfully. Will delete task.");
  vTaskDelete(NULL);
}

void InterfaceManager::subscribeToMqttTopics() {
  // Subscribe to command to wake/put to sleep the display
  vTaskDelay(100 / portTICK_PERIOD_MS);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_screen_cmd_topic.c_str(), &InterfaceManager::handleNSPanelScreenCommand);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_cmd_topic.c_str(), &InterfaceManager::handleNSPanelCommand);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_screen_brightness_topic.c_str(), &InterfaceManager::handleNSPanelScreenBrightnessCommand);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_screensaver_brightness.c_str(), &InterfaceManager::handleNSPanelScreensaverBrightnessCommand);
}

void InterfaceManager::handleNSPanelScreenCommand(MQTTMessage *message) {
  if (message->data.compare("1") == 0) {
    InterfaceManager::showDefaultPage();
    MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1"); // Send out state information that panel woke from sleep
  } else if (message->data.compare("0") == 0) {
    PageManager::GetScreensaverPage()->show();
  } else {
    LOG_ERROR("Invalid payload for screen cmd. Valid payload: 1 or 0");
  }
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

bool InterfaceManager::_getRoomConfig(int room_id, JsonDocument *buffer) {
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
