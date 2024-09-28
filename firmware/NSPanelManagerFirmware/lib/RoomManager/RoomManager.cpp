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
  RoomManager::currentRoom = nullptr;
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

    bool found_and_replaced_existing_room = false;
    for (int i = 0; i < RoomManager::rooms.size(); i++) {
      if (RoomManager::rooms[i].id == room_status->id) {
        bool update_current_room = RoomManager::currentRoom != nullptr && RoomManager::currentRoom->id == room_status->id;
        RoomManager::rooms[i] = (*room_status);
        found_and_replaced_existing_room = true;
        if (update_current_room) {
          RoomManager::currentRoom = &RoomManager::rooms[i];
          RoomManager::_callRoomChangeCallbacks();
        }
        break;
      }
    }

    if (!found_and_replaced_existing_room) {
      RoomManager::rooms.push_back((*room_status));
    }

    if (RoomManager::_notify_task_room_load_complete != nullptr) {
      xTaskNotifyGive(RoomManager::_notify_task_room_load_complete);
    }
  } catch (std::exception &ex) {
    LOG_ERROR("Caught exception while processing room status update. Exception: ", ex.what());
  }
}

void RoomManager::goToNextRoom() {
  if (RoomManager::currentRoom != nullptr) {
    auto room_id_it = std::find_if(InterfaceConfig::room_ids.begin(), InterfaceConfig::room_ids.end(), [](int32_t id) {
      return RoomManager::currentRoom->id == id;
    });
    room_id_it++;
    if (room_id_it == InterfaceConfig::room_ids.end()) {
      // We are at the last room in the order, go to the first room.
      room_id_it = InterfaceConfig::room_ids.begin();
    }

    for (int i = 0; i < RoomManager::rooms.size(); i++) {
      if (RoomManager::rooms[i].id == (*room_id_it)) {
        RoomManager::currentRoom = &RoomManager::rooms[i];
        break;
      }
    }
    RoomManager::_callRoomChangeCallbacks();
  } else {
    LOG_ERROR("Current room is not set!");
  }
}

void RoomManager::goToPreviousRoom() {
  if (RoomManager::currentRoom != nullptr) {
    auto room_id_it = std::find_if(InterfaceConfig::room_ids.begin(), InterfaceConfig::room_ids.end(), [](int32_t id) {
      return RoomManager::currentRoom->id == id;
    });
    if (room_id_it == InterfaceConfig::room_ids.begin()) {
      // We are at the first room in the order, go to the last room.
      room_id_it = InterfaceConfig::room_ids.end()--;
    }

    for (int i = 0; i < RoomManager::rooms.size(); i++) {
      if (RoomManager::rooms[i].id == (*room_id_it)) {
        RoomManager::currentRoom = &RoomManager::rooms[i];
        break;
      }
    }
    RoomManager::_callRoomChangeCallbacks();
  } else {
    LOG_ERROR("Current room is not set!");
  }
}

bool RoomManager::goToRoomId(uint16_t roomId) {
  for (int i = 0; i < RoomManager::rooms.size(); i++) {
    if (RoomManager::rooms[i].id == roomId) {
      RoomManager::currentRoom = &RoomManager::rooms[i];
      RoomManager::_callRoomChangeCallbacks();
      return true;
    }
  }

  LOG_ERROR("Did not find requested room. Will cancel operation.");
  return false;
}

NSPanelRoomStatus *RoomManager::getRoomById(uint16_t roomId) {
  for (int i = 0; i < RoomManager::rooms.size(); i++) {
    if (RoomManager::rooms[i].id == roomId) {
      return &RoomManager::rooms[i];
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
  if (RoomManager::currentRoom != nullptr) {
    for (RoomManagerObserver *observer : RoomManager::_roomChangeObservers) {
      observer->roomChangedCallback();
    }
  } else {
    LOG_ERROR("Current room is nullptr!");
  }
}

bool RoomManager::hasValidCurrentRoom() {
  if (RoomManager::currentRoom == nullptr) {
    LOG_ERROR("Current room is nullptr!");
    return false;
  }
  return true;
}