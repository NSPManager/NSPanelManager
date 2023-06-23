#include <ArduinoJson.h>
#include <ButtonManager.hpp>
#include <HttpLib.hpp>
#include <InterfaceConfig.hpp>
#include <Light.hpp>
#include <LightManager.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <Scene.hpp>
#include <WiFi.h>

void RoomManager::init() {
  MqttManager::subscribeToTopic("nspanel/config/reload", &RoomManager::reloadCallback);
  RoomManager::currentRoom = RoomManager::rooms.end();
}

void RoomManager::reloadCallback(char *topic, byte *payload, unsigned int length) {
  std::string payload_str = std::string((char *)payload, 1);
  if (payload_str.compare("1") == 0) {
    LOG_DEBUG("Got reload command, reloading rooms.");
    RoomManager::loadAllRooms(true);
  }
}

void RoomManager::loadAllRooms(bool is_update) {
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

  for (uint16_t roomId : (*roomData)["rooms"].as<JsonArray>()) {
    LOG_INFO("Getting config for room ", roomId);
    Room *room = RoomManager::loadRoom(roomId, is_update);
    if (RoomManager::getRoomById(roomId) == nullptr) {
      // If a nullptr is returned, a room by that id was not found
      RoomManager::rooms.push_back(room);
    }
  }

  if (!is_update) {
    RoomManager::goToRoomId(InterfaceConfig::homeScreen); // Set currentRoom to the default room for this panel
  } else {
    // TODO: Implement function that remove any rooms currecntly loaded but not configured (ie. removed through manager)
    LOG_DEBUG("Calling roomChangedCallback");
    RoomManager::_callRoomChangeCallbacks(); // Call room change update to force an update for other components
  }

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

Room *RoomManager::loadRoom(uint16_t roomId, bool is_update) {
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
  // Load already existing room or if a nullptr was returned, create a new room.
  Room *newRoom = RoomManager::getRoomById(roomId);
  if (newRoom == nullptr) {
    newRoom = new Room();
  }
  newRoom->id = roomId;
  newRoom->name = (*roomData)["name"] | "ERR";

  // Load and init all lights for the room
  JsonVariant json_lights = (*roomData)["lights"];
  for (JsonPair lightPair : json_lights.as<JsonObject>()) {
    bool existing_light;
    Light *newLight = newRoom->getLightById(atoi(lightPair.key().c_str()));
    if (newLight == nullptr) {
      existing_light = false;
      newLight = new Light();
    } else {
      existing_light = true;
    }
    newLight->initFromJson(&lightPair);
    // If the light is new (ie. not updated from existing) push it into the correct list.
    if (!existing_light) {
      if (lightPair.value()["ceiling"] == true) {
        newRoom->ceilingLights.insert(std::make_pair(newLight->getId(), newLight));
      } else {
        newRoom->tableLights.insert(std::make_pair(newLight->getId(), newLight));
      }
    }
  }

  if (is_update) {
    for (auto it = newRoom->ceilingLights.cbegin(); it != newRoom->ceilingLights.cend();) {
      // Check if this light ID exist in JSON config
      bool light_id_found = false;
      for (JsonPair jsonLightPair : json_lights.as<JsonObject>()) {
        if (atoi(jsonLightPair.key().c_str()) == (*it).first) {
          light_id_found = true;
          break;
        }
      }

      if (!light_id_found) {
        Light *light_to_remove = (*it).second;
        LOG_DEBUG("Removing light: ", light_to_remove->getName().c_str(), ". ID: ", light_to_remove->getId());
        newRoom->ceilingLights.erase(it++);
        light_to_remove->callDeconstructCallbacks();
        delete light_to_remove;
      } else {
        it++;
      }
    }

    for (auto it = newRoom->tableLights.cbegin(); it != newRoom->tableLights.cend();) {
      // Check if this light ID exist in JSON config
      bool light_id_found = false;
      for (JsonPair jsonLightPair : json_lights.as<JsonObject>()) {
        if (atoi(jsonLightPair.key().c_str()) == (*it).first) {
          light_id_found = true;
          break;
        }
      }

      if (!light_id_found) {
        Light *light_to_remove = (*it).second;
        LOG_DEBUG("Removing light: ", light_to_remove->getName().c_str(), ". ID: ", light_to_remove->getId());
        newRoom->tableLights.erase(it++);
        light_to_remove->callDeconstructCallbacks();
        delete light_to_remove;
      } else {
        it++;
      }
    }
  }

  // Load and init all the scenes for the room
  JsonVariant json_scenes = (*roomData)["scenes"];
  for (JsonPair scenePair : json_scenes.as<JsonObject>()) {
    bool existing_scene;
    Scene *newScene = newRoom->getSceneById(atoi(scenePair.key().c_str()));
    if (newScene == nullptr) {
      existing_scene = false;
      newScene = new Scene();
    } else {
      existing_scene = true;
    }
    newScene->room = newRoom;
    newScene->id = atoi(scenePair.key().c_str());
    newScene->name = scenePair.value()["name"] | "ERR-S";
    if (!existing_scene) {
      newRoom->scenes.push_back(newScene);
      LOG_TRACE("Loaded scene ", newScene->id, "::", newScene->name.c_str());
    }
  }

  delete roomData;
  return newRoom;
}

void RoomManager::goToNextRoom() {
  RoomManager::currentRoom++;
  if (RoomManager::currentRoom == RoomManager::rooms.end()) {
    RoomManager::currentRoom = RoomManager::rooms.begin();
  }
  RoomManager::_callRoomChangeCallbacks();
}

void RoomManager::goToPreviousRoom() {
  if (RoomManager::currentRoom == RoomManager::rooms.begin()) {
    RoomManager::currentRoom = RoomManager::rooms.end();
    RoomManager::currentRoom--;
  } else {
    RoomManager::currentRoom--;
  }
  RoomManager::_callRoomChangeCallbacks();
}

void RoomManager::goToRoomId(uint16_t roomId) {
  bool foundRoom = false;
  for (std::list<Room *>::iterator it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it)->id == roomId) {
      RoomManager::currentRoom = it;
      RoomManager::_callRoomChangeCallbacks();
      return;
    }
  }

  LOG_ERROR("Did not find requested room. Will cancel operation.");
  return;
}

Room *RoomManager::getRoomById(uint16_t roomId) {
  bool foundRoom = false;
  for (std::list<Room *>::iterator it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it)->id == roomId) {
      return (*it);
    }
  }

  return nullptr;
}

void RoomManager::attachRoomChangeCallback(RoomManagerObserver *observer) {
  RoomManager::_roomChangeObservers.push_back(observer);
}

void RoomManager::detachRoomChangeCallback(RoomManagerObserver *observer) {
  RoomManager::_roomChangeObservers.remove(observer);
}

void RoomManager::_callRoomChangeCallbacks() {
  if (RoomManager::currentRoom != RoomManager::rooms.end()) {
    for (RoomManagerObserver *observer : RoomManager::_roomChangeObservers) {
      observer->roomChangedCallback();
    }
  }
}
