#include "esp32-hal.h"
#include <ArduinoJson.h>
#include <ButtonManager.hpp>
#include <HttpLib.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceManager.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <ReadBufferFixedSize.h>
#include <RoomManager.hpp>
#include <WiFi.h>
#include <map>
#include <protobuf_defines.h>
#include <string>

void RoomManager::init() {
  RoomManager::currentRoom = nullptr;
  RoomManager::_lastReloadCommand = millis();
  RoomManager::_notify_task_room_load_complete = nullptr;
  RoomManager::_room_status_decode_buffer_mutex = xSemaphoreCreateMutex();
}

void RoomManager::loadAllRooms() {
  RoomManager::_notify_task_room_load_complete = xTaskGetCurrentTaskHandle();
  for (int i = 0; i < InterfaceConfig::room_ids.size(); i++) {
    while (!MqttManager::connected()) {
      vTaskDelay(50 / portTICK_PERIOD_MS); // Wait for MQTT to be connected before continuing.
    }

    auto existing_room = RoomManager::rooms.cend();
    uint32_t room_id = InterfaceConfig::room_ids[i];
    for (auto it = RoomManager::rooms.cbegin(); it != RoomManager::rooms.cend(); it++) {
      if ((*it)->id() == room_id) {
        existing_room = it;
        break;
      }
    }

    if (existing_room == RoomManager::rooms.end()) {
      std::string room_status_topic = "nspanel/mqttmanager_";
      room_status_topic.append(NSPMConfig::instance->manager_address);
      room_status_topic.append("/room/");
      room_status_topic.append(std::to_string(InterfaceConfig::room_ids[i]));
      room_status_topic.append("/status");
      MqttManager::subscribeToTopic(room_status_topic.c_str(), &RoomManager::handleNSPanelRoomStatusUpdate);
      if (ulTaskNotifyTake(pdTRUE, 5000 / portTICK_PERIOD_MS) == pdTRUE) {
        vTaskDelay(100 / portTICK_PERIOD_MS); // Wait 50ms for other tasks to compute
      } else {
        LOG_ERROR("Failed to load room ", InterfaceConfig::room_ids[i], " within 5 seconds. Will continue to next room.");
      }
    } else {
      LOG_DEBUG("Room already exists, do not load via loadAllRooms. Updates are handled separately.");
    }
  }
  LOG_INFO("All rooms loaded.");
  RoomManager::_notify_task_room_load_complete = nullptr;
}

void RoomManager::handleNSPanelRoomStatusUpdate(MQTTMessage *message) {
  try {
    if (xSemaphoreTake(RoomManager::_room_status_decode_buffer_mutex, 1000 / portTICK_PERIOD_MS) == pdTRUE) {
      uint64_t start = esp_timer_get_time();
      if (message->get_protobuf_obj<PROTOBUF_NSPANEL_ROOM_STATUS>(&RoomManager::_room_status_decode_buffer)) {
        message->clear(); // We no longer need raw data stored in the message.
      } else {
        LOG_ERROR("Failed to deserialize protobuf!");
        return;
      }
      LOG_DEBUG("Deserialization of room ", RoomManager::_room_status_decode_buffer.id(), " took ", esp_timer_get_time() - start, "us.");

      LOG_DEBUG("Successfully decoded room status for room ", RoomManager::_room_status_decode_buffer.id(), "::", RoomManager::_room_status_decode_buffer.name());
      bool found_and_replaced_existing_room = false;
      for (auto it = RoomManager::rooms.cbegin(); it != RoomManager::rooms.cend(); it++) {
        if ((*it)->id() == RoomManager::_room_status_decode_buffer.id()) {
          bool update_current_room = RoomManager::currentRoom != nullptr && RoomManager::currentRoom->id() == RoomManager::_room_status_decode_buffer.id();
          *(*it) = RoomManager::_room_status_decode_buffer;
          found_and_replaced_existing_room = true;
          if (update_current_room) {
            RoomManager::currentRoom = (*it);
            LOG_DEBUG("Current room updated, calling room change callbacks.");
            RoomManager::_callRoomChangeCallbacks();
          }
          break;
        }
      }

      if (!found_and_replaced_existing_room) {
        LOG_INFO("Adding room ", RoomManager::_room_status_decode_buffer.id(), "::", RoomManager::_room_status_decode_buffer.name(), ".");
        RoomManager::rooms.push_back(new PROTOBUF_NSPANEL_ROOM_STATUS(RoomManager::_room_status_decode_buffer));
      } else {
        LOG_INFO("Updated existing room ", RoomManager::_room_status_decode_buffer.id(), "::", RoomManager::_room_status_decode_buffer.name(), ".");
      }

      xSemaphoreGive(RoomManager::_room_status_decode_buffer_mutex); // Give back Mutex
      if (RoomManager::_notify_task_room_load_complete != nullptr) {
        xTaskNotifyGive(RoomManager::_notify_task_room_load_complete);
      }
    } else {
      LOG_ERROR("Failed to take mutex to decode room status!");
    }
  } catch (std::exception &ex) {
    LOG_ERROR("Caught exception while processing room status update. Exception: ", ex.what());
    xSemaphoreGive(RoomManager::_room_status_decode_buffer_mutex);
  }
}

void RoomManager::goToNextRoom() {
  if (RoomManager::currentRoom != nullptr) {
    auto room_id_it = std::find_if(InterfaceConfig::room_ids.begin(), InterfaceConfig::room_ids.end(), [](int32_t id) {
      return RoomManager::currentRoom->id() == id;
    });
    room_id_it++;
    if (room_id_it == InterfaceConfig::room_ids.end()) {
      // We are at the last room in the order, go to the first room.
      room_id_it = InterfaceConfig::room_ids.begin();
    }

    // Find room instance
    for (auto it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
      if ((*it)->id() == (*room_id_it)) {
        RoomManager::currentRoom = (*it);
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
      return RoomManager::currentRoom->id() == id;
    });
    if (room_id_it == InterfaceConfig::room_ids.begin()) {
      // We are at the first room in the order, go to the last room.
      room_id_it = InterfaceConfig::room_ids.end()--;
    }

    // Find room instance
    for (auto it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
      if ((*it)->id() == (*room_id_it)) {
        RoomManager::currentRoom = (*it);
        break;
      }
    }
    RoomManager::_callRoomChangeCallbacks();
  } else {
    LOG_ERROR("Current room is not set!");
  }
}

bool RoomManager::goToRoomId(uint16_t roomId) {
  // Find room instance
  for (auto it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it)->id() == roomId) {
      RoomManager::currentRoom = (*it);
      return true;
    }
  }

  LOG_ERROR("Did not find requested room. Will cancel operation.");
  return false;
}

PROTOBUF_NSPANEL_ROOM_STATUS *RoomManager::getRoomById(uint16_t roomId) {
  for (auto it = RoomManager::rooms.begin(); it != RoomManager::rooms.end(); it++) {
    if ((*it)->id() == roomId) {
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