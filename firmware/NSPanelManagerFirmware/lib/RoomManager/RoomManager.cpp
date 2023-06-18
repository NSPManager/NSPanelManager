#include <ArduinoJson.h>
#include <ButtonManager.hpp>
#include <HttpLib.hpp>
#include <InterfaceConfig.hpp>
#include <Light.hpp>
#include <LightManager.hpp>
#include <MqttLog.hpp>
#include <NSPMConfig.h>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <Scene.hpp>
#include <WiFi.h>

void RoomManager::loadAllRooms() {
  DynamicJsonDocument *roomData = new DynamicJsonDocument(2048);
  uint8_t tries = 0;
  bool successDownloadingConfig = false;
  do {
    // Sometimes when running WiFi.macAddress garbage is returned.
    // Keep creation of download URL in the loop.
    std::string roomDataJsonUrl = "http://";
    roomDataJsonUrl.append(NSPMConfig::instance->manager_address);
    roomDataJsonUrl.append(":");
    roomDataJsonUrl.append(std::to_string(NSPMConfig::instance->manager_port));
    roomDataJsonUrl.append("/api/get_nspanel_config?mac=");
    roomDataJsonUrl.append(WiFi.macAddress().c_str());
    successDownloadingConfig = HttpLib::DownloadJSON(roomDataJsonUrl.c_str(), roomData);
    roomDataJsonUrl.end();

    if (!successDownloadingConfig) {
      tries++;
      LOG_ERROR("Failed to download config, will try again in 5 seconds.");
      vTaskDelay(5000 / portTICK_PERIOD_MS);

      // 30 failed tries to download config, restart and try again.
      if (tries == 30) {
        LOG_ERROR("Failed to download config, will restart and try again.");
        // NspanelManagerPage::setText("Restarting...");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP.restart();
      }
    }
  } while (!successDownloadingConfig);
  // Config downloaded, process the raw data
  // The config also contains other config values for the interface. Populate InterfaceConfig
  InterfaceConfig::homeScreen = (*roomData)["home"].as<uint8_t>();
  InterfaceConfig::colorTempMin = (*roomData)["color_temp_min"].as<uint16_t>();
  InterfaceConfig::colorTempMax = (*roomData)["color_temp_max"].as<uint16_t>();
  InterfaceConfig::reverseColorTempSlider = (*roomData)["reverse_color_temp"].as<String>().equals("True");
  InterfaceConfig::raiseToMaxLightLevelAbove = (*roomData)["raise_to_100_light_level"].as<uint8_t>();
  InterfaceConfig::button_min_press_time = (*roomData)["min_button_push_time"].as<uint16_t>();
  InterfaceConfig::button_long_press_time = (*roomData)["button_long_press_time"].as<uint16_t>();
  InterfaceConfig::special_mode_trigger_time = (*roomData)["special_mode_trigger_time"].as<uint16_t>();
  InterfaceConfig::special_mode_release_time = (*roomData)["special_mode_release_time"].as<uint16_t>();
  InterfaceConfig::mqtt_ignore_time = (*roomData)["mqtt_ignore_time"].as<uint16_t>();
  InterfaceConfig::screen_dim_level = (*roomData)["screen_dim_level"].as<uint8_t>();
  InterfaceConfig::screensaver_dim_level = (*roomData)["screensaver_dim_level"].as<uint8_t>();
  InterfaceConfig::screensaver_activation_timeout = (*roomData)["screensaver_activation_timeout"].as<uint16_t>();
  InterfaceConfig::show_screensaver_clock = (*roomData)["show_screensaver_clock"].as<String>().equals("True");
  InterfaceConfig::clock_us_style = (*roomData)["clock_us_style"].as<String>().equals("True");
  NSPMConfig::instance->button1_mode = static_cast<BUTTON_MODE>((*roomData)["button1_mode"].as<uint8_t>());
  NSPMConfig::instance->button2_mode = static_cast<BUTTON_MODE>((*roomData)["button2_mode"].as<uint8_t>());
  // Init rooms
  uint8_t numberOfRooms = (*roomData)["rooms"].as<JsonArray>().size();
  RoomManager::rooms.resize(numberOfRooms);

  uint8_t i = 0;
  for (uint16_t roomId : (*roomData)["rooms"].as<JsonArray>()) {
    LOG_INFO("Getting config for room ", roomId);
    // Display what room we are getting configuration for.
    // std::string info_text = "Loading room ";
    // info_text.append(std::to_string(roomId));
    // info_text.append("/");
    // info_text.append(std::to_string(numberOfRooms));
    // NspanelManagerPage::setText(info_text.c_str());
    // Download and process room JSON from NspanelManager into a Room instance
    Room *room = RoomManager::loadRoom(roomId);
    RoomManager::rooms[i] = room;
    i++;
  }

  RoomManager::goToRoomId(InterfaceConfig::homeScreen); // Set currentRoom to the default room for this panel

  // All rooms and lights has loaded, prep buttonmanager
  if (NSPMConfig::instance->button1_mode == BUTTON_MODE::DETACHED) {
    ButtonManager::button1_detached_mode_light = LightManager::getLightById((*roomData)["button1_detached_light"].as<uint16_t>());
    if (ButtonManager::button1_detached_mode_light != nullptr) {
      LOG_DEBUG("Button 1 detached mode light: ", ButtonManager::button1_detached_mode_light->getName().c_str());
    } else {
      LOG_ERROR("Coudln't find Button 1 detached mode light with ID: ", (*roomData)["button1_detached_light"].as<uint16_t>());
    }
  } else {
    ButtonManager::button1_detached_mode_light = nullptr;
  }
  if (NSPMConfig::instance->button2_mode == BUTTON_MODE::DETACHED) {
    ButtonManager::button2_detached_mode_light = LightManager::getLightById((*roomData)["button2_detached_light"].as<uint16_t>());
    if (ButtonManager::button2_detached_mode_light != nullptr) {
      LOG_DEBUG("Button 2 detached mode light: ", ButtonManager::button2_detached_mode_light->getName().c_str());
    } else {
      LOG_ERROR("Coudln't find Button 2 detached mode light with ID: ", (*roomData)["button2_detached_light"].as<uint16_t>());
    }
  } else {
    ButtonManager::button2_detached_mode_light = nullptr;
  }
}

Room *RoomManager::loadRoom(uint16_t roomId) {
  DynamicJsonDocument *roomData = new DynamicJsonDocument(2048);
  uint8_t tries = 0;
  bool successDownloadingConfig = false;
  std::string roomDataJsonUrl = "http://";
  roomDataJsonUrl.append(NSPMConfig::instance->manager_address);
  roomDataJsonUrl.append(":");
  roomDataJsonUrl.append(std::to_string(NSPMConfig::instance->manager_port));
  roomDataJsonUrl.append("/api/get_nspanel_config/room/");
  roomDataJsonUrl.append(std::to_string(roomId));

  do {

    successDownloadingConfig = HttpLib::DownloadJSON(roomDataJsonUrl.c_str(), roomData);

    if (!successDownloadingConfig) {
      tries++;
      LOG_ERROR("Failed to download config, will try again in 5 seconds.");
      vTaskDelay(5000 / portTICK_PERIOD_MS);

      // 30 failed tries to download config, restart and try again.
      if (tries == 30) {
        LOG_ERROR("Failed to download config, will restart and try again.");
        // NspanelManagerPage::setText("Restarting...");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP.restart();
      }
    }
  } while (!successDownloadingConfig);
  roomDataJsonUrl.end();
  // Sucessfully downloaded config, proceed to process it
  Room *newRoom = new Room();
  newRoom->id = roomId;
  newRoom->name = (*roomData)["name"] | "ERR";

  // Load and init all lights for the room
  JsonVariant json_lights = (*roomData)["lights"];
  for (JsonPair lightPair : json_lights.as<JsonObject>()) {
    Light *newLight = new Light();
    newLight->initFromJson(&lightPair);
    if (lightPair.value()["ceiling"] == true) {
      newRoom->ceilingLights.insert(std::make_pair(newLight->getId(), newLight));
    } else {
      newRoom->tableLights.insert(std::make_pair(newLight->getId(), newLight));
    }
  }

  // Load and init all the scenes for the room
  JsonVariant json_scenes = (*roomData)["scenes"];
  for (JsonPair scenePair : json_scenes.as<JsonObject>()) {
    Scene *newScene = new Scene();
    newScene->room = newRoom;
    newScene->id = atoi(scenePair.key().c_str());
    newScene->name = scenePair.value()["name"] | "ERR-S";
    newRoom->scenes.push_back(newScene);
    LOG_TRACE("Loaded scene ", newScene->id, "::", newScene->name.c_str());
  }

  delete roomData;
  return newRoom;
}

void RoomManager::goToNextRoom() {
  RoomManager::currentRoom++;
  if (RoomManager::currentRoom == RoomManager::rooms.end()) {
    RoomManager::currentRoom = RoomManager::rooms.begin();
  }
}

void RoomManager::goToPreviousRoom() {
  if (RoomManager::currentRoom == RoomManager::rooms.begin()) {
    RoomManager::currentRoom = RoomManager::rooms.end();
    RoomManager::currentRoom--;
  } else {
    RoomManager::currentRoom--;
  }
}

void RoomManager::goToRoomId(uint16_t roomId) {
  bool foundRoom = false;
  for (std::vector<Room *>::iterator it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it)->id == roomId) {
      RoomManager::currentRoom = it;
      return;
    }
  }

  LOG_ERROR("Did not find requested room. Will cancel operation.");
  return;
}
