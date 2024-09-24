#include "esp32-hal.h"
#include <ArduinoJson.h>
#include <ButtonManager.hpp>
#include <HttpLib.hpp>
#include <InterfaceConfig.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <RoomManager.hpp>
#include <WiFi.h>
#include <map>
#include <string>

void RoomManager::init() {
  RoomManager::currentRoom = RoomManager::rooms.end();
  RoomManager::_lastReloadCommand = millis();
  RoomManager::_notify_task_room_load_complete = nullptr;
}

void RoomManager::loadAllRooms(int32_t *room_ids, uint32_t n_room_ids) {
  RoomManager::_notify_task_room_load_complete = xTaskGetCurrentTaskHandle();
  for (int i = 0; i < n_room_ids; i++) {
    while (!MqttManager::connected()) {
      vTaskDelay(50 / portTICK_PERIOD_MS); // Wait for MQTT to be connected before continuing.
    }

    uint32_t room_id = room_ids[i];
    auto existing_room = std::find_if(RoomManager::rooms.begin(), RoomManager::rooms.end(), [room_id](NSPanelRoomStatus status) {
      return status.id == room_id;
    });

    if (existing_room == RoomManager::rooms.end()) {
      std::string room_status_topic = "nspanel/mqttmanager_";
      room_status_topic.append(NSPMConfig::instance->manager_address);
      room_status_topic.append("/room/");
      room_status_topic.append(std::to_string(room_ids[i]));
      room_status_topic.append("/status");
      MqttManager::subscribeToTopic(room_status_topic.c_str(), &RoomManager::handleNSPanelRoomStatusUpdate);
      if (ulTaskNotifyTake(pdTRUE, 5000 / portTICK_PERIOD_MS) == pdTRUE) {
        vTaskDelay(50 / portTICK_PERIOD_MS); // Wait 50ms for other tasks to compute
      } else {
        LOG_ERROR("Failed to load room ", room_ids[i], " within 5 seconds. Will continue to next room.");
      }
    } else {
      LOG_DEBUG("Room already exists, do not load via loadAllRooms. Updates are handled separately.");
    }
  }
  LOG_INFO("All rooms loaded.");
  RoomManager::_notify_task_room_load_complete = nullptr;
}

void RoomManager::handleNSPanelRoomStatusUpdate(char *topic, byte *payload, unsigned int length) {
  try {
    NSPanelRoomStatus *room_status = nspanel_room_status__unpack(NULL, length, payload);
    LOG_DEBUG("Successfully decoded room status for room ", room_status->id, "::", room_status->name);
    auto existing_room = std::find_if(RoomManager::rooms.begin(), RoomManager::rooms.end(), [room_status](NSPanelRoomStatus status) {
      return status.id == room_status->id;
    });
    bool update_current_room = RoomManager::currentRoom == existing_room; // Check if currently selected room is the same as the one to be removed/updated.

    // if (existing_room == RoomManager::rooms.end()) {
    //   // Room does not exist, simply push to end of list
    //   RoomManager::rooms.push_back(*room_status);
    // } else {
    //   // Room already exists, merge all changes
    //   (*existing_room).average_color_temperature = room_status->average_color_temperature;
    //   (*existing_room).ceiling_lights_dim_level = room_status->ceiling_lights_dim_level;
    //   (*existing_room).table_lights_dim_level = room_status->table_lights_dim_level;
    //   (*existing_room).average_color_temperature = room_status->average_color_temperature;
    //   (*existing_room).ceiling_lights_color_temperature_value = room_status->ceiling_lights_color_temperature_value;
    //   (*existing_room).table_lights_color_temperature_value = room_status->table_lights_color_temperature_value;

    //   RoomManager::_callRoomChangeCallbacks();
    // }

    RoomManager::rooms.push_back(*room_status);
    if (existing_room != RoomManager::rooms.end()) {
      RoomManager::rooms.erase(existing_room);
    }

    if (update_current_room) {
      RoomManager::currentRoom = std::find_if(RoomManager::rooms.begin(), RoomManager::rooms.end(), [room_status](NSPanelRoomStatus status) {
        return status.id == room_status->id;
      });
      RoomManager::_callRoomChangeCallbacks();
    }

    if (RoomManager::_notify_task_room_load_complete != nullptr) {
      xTaskNotifyGive(RoomManager::_notify_task_room_load_complete);
    }
  } catch (std::exception &ex) {
    LOG_ERROR("Caught exception while processing room status update. Exception: ", ex.what());
  }
}

// void RoomManager::loadRoom(uint16_t roomId, bool is_update) {
// JsonDocument *roomData = new JsonDocument;
// uint8_t tries = 0;
// bool successDownloadingConfig = false;

// do {
//   std::string roomDataJsonUrl = "http://";
//   roomDataJsonUrl.append(NSPMConfig::instance->manager_address);
//   roomDataJsonUrl.append(":");
//   roomDataJsonUrl.append(std::to_string(NSPMConfig::instance->manager_port));
//   roomDataJsonUrl.append("/api/get_nspanel_config/room/");
//   roomDataJsonUrl.append(std::to_string(roomId));
//   LOG_INFO("Downloading room config from: ", roomDataJsonUrl.c_str());

//   successDownloadingConfig = HttpLib::DownloadJSON(roomDataJsonUrl.c_str(), roomData);

//   if (!successDownloadingConfig) {
//     tries++;
//     LOG_ERROR("Failed to download config for room ", roomId, " will try again in 5 seconds.");
//     vTaskDelay(5000 / portTICK_PERIOD_MS);

//     // 30 failed tries to download config, restart and try again.
//     if (tries == 30) {
//       LOG_ERROR("Failed to download config for room ", roomId, ", will restart and try again.");
//       // NspanelManagerPage::setText("Restarting...");
//       vTaskDelay(5000 / portTICK_PERIOD_MS);
//       ESP.restart();
//     }
//   }
// } while (!successDownloadingConfig);
// // Successfully downloaded config, proceed to process it
// // Load already existing room or if a nullptr was returned, create a new room.
// Room *newRoom = RoomManager::getRoomById(roomId);
// if (newRoom == nullptr) {
//   newRoom = new Room(roomId);
// }
// newRoom->name = (*roomData)["name"] | "ERR";
// std::string mqtt_room_status_topic = "nspanel/mqttmanager_";
// mqtt_room_status_topic.append(NSPMConfig::instance->manager_address);
// mqtt_room_status_topic.append("/command");
// MqttManager::subscribeToTopic(mqtt_room_status_topic.c_str(), &RoomManager::mqtt_callback);

// Load and init all lights for the room
// JsonVariant json_lights = (*roomData)["lights"];
// for (JsonPair lightPair : json_lights.as<JsonObject>()) {
//   bool existing_light;
//   Light *newLight = newRoom->getLightById(atoi(lightPair.key().c_str()));
//   if (newLight == nullptr) {
//     existing_light = false;
//     newLight = new Light();
//   } else {
//     existing_light = true;
//   }
//   std::map<std::string, std::string> data;
//   data["id"] = lightPair.key().c_str();
//   for (JsonPair lightSettingsPair : lightPair.value().as<JsonObject>()) {
//     data[lightSettingsPair.key().c_str()] = lightSettingsPair.value().as<String>().c_str();
//   }
//   newLight->initFromMap(data);

//   //  If the light is new (ie. not updated from existing) push it into the correct list.
//   if (!existing_light) {
//     if (lightPair.value()["ceiling"] == true) {
//       newRoom->ceilingLights.insert(std::make_pair(newLight->getId(), newLight));
//     } else {
//       newRoom->tableLights.insert(std::make_pair(newLight->getId(), newLight));
//     }
//   }
// }

// if (is_update) {
//   for (auto it = newRoom->ceilingLights.cbegin(); it != newRoom->ceilingLights.cend();) {
//     // Check if this light ID exist in JSON config
//     bool light_id_found = false;
//     for (JsonPair jsonLightPair : json_lights.as<JsonObject>()) {
//       if (atoi(jsonLightPair.key().c_str()) == (*it).first) {
//         light_id_found = true;
//         break;
//       }
//     }

//     if (!light_id_found) {
//       Light *light_to_remove = (*it).second;
//       LOG_DEBUG("Removing light: ", light_to_remove->getName().c_str(), ". ID: ", light_to_remove->getId());
//       it = newRoom->ceilingLights.erase(it);
//       light_to_remove->callDeconstructCallbacks();
//       delete light_to_remove;
//     } else {
//       it++;
//     }
//   }

//   for (auto it = newRoom->tableLights.cbegin(); it != newRoom->tableLights.cend();) {
//     // Check if this light ID exist in JSON config
//     bool light_id_found = false;
//     for (JsonPair jsonLightPair : json_lights.as<JsonObject>()) {
//       if (atoi(jsonLightPair.key().c_str()) == (*it).first) {
//         light_id_found = true;
//         break;
//       }
//     }

//     if (!light_id_found) {
//       Light *light_to_remove = (*it).second;
//       LOG_DEBUG("Removing light: ", light_to_remove->getName().c_str(), ". ID: ", light_to_remove->getId());
//       it = newRoom->tableLights.erase(it);
//       light_to_remove->callDeconstructCallbacks();
//       delete light_to_remove;
//     } else {
//       it++;
//     }
//   }

//   for (auto it = newRoom->scenes.cbegin(); it != newRoom->scenes.cend();) {
//     // Check if this light ID exist in JSON config
//     bool light_id_found = false;
//     for (JsonPair jsonLightPair : json_lights.as<JsonObject>()) {
//       if (atoi(jsonLightPair.key().c_str()) == (*it)->getId()) {
//         light_id_found = true;
//         break;
//       }
//     }

//     if (!light_id_found) {
//       Scene *scene_to_remove = (*it);
//       LOG_DEBUG("Removing scene: ", scene_to_remove->getName().c_str(), ". ID: ", scene_to_remove->getId());
//       it = newRoom->scenes.erase(it);
//       scene_to_remove->callDeconstructCallbacks();
//       delete scene_to_remove;
//     } else {
//       it++;
//     }
//   }
// }

// // Load and init all the scenes for the room
// JsonVariant json_scenes = (*roomData)["scenes"];
// for (JsonPair scenePair : json_scenes.as<JsonObject>()) {
//   bool existing_scene;
//   Scene *newScene = newRoom->getSceneById(atoi(scenePair.key().c_str()));
//   if (newScene == nullptr) {
//     existing_scene = false;
//     newScene = new Scene();
//   } else {
//     existing_scene = true;
//   }
//   newScene->room = newRoom;
//   newScene->id = atoi(scenePair.key().c_str());
//   newScene->name = scenePair.value()["name"] | "ERR-S";
//   newScene->canSave = scenePair.value()["can_save"].as<String>().equals("true");
//   newScene->callUpdateCallbacks();
//   if (!existing_scene) {
//     newRoom->scenes.push_back(newScene);
//     LOG_TRACE("Loaded scene ", newScene->id, "::", newScene->name.c_str());
//   }
// }

// delete roomData;
//   return newRoom;
// }

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

bool RoomManager::goToRoomId(uint16_t roomId) {
  bool foundRoom = false;
  for (std::list<NSPanelRoomStatus>::iterator it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it).id == roomId) {
      RoomManager::currentRoom = it;
      RoomManager::_callRoomChangeCallbacks();
      return true;
    }
  }

  LOG_ERROR("Did not find requested room. Will cancel operation.");
  return false;
}

NSPanelRoomStatus *RoomManager::getRoomById(uint16_t roomId) {
  bool foundRoom = false;
  for (std::list<NSPanelRoomStatus>::iterator it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it).id == roomId) {
      return &(*it);
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

bool RoomManager::hasValidCurrentRoom() {
  if (RoomManager::currentRoom == RoomManager::rooms.end()) {
    LOG_ERROR("Current room is invalid iterator!");
    return false;
  }
  return true;
}