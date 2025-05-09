#include "RoomManager.hpp"
#include <MqttManager.hpp>
#include <NSPM_ConfigManager.hpp>
#include <NSPM_ConfigManager_event.hpp>
#include <RoomManager_event.hpp>
#include <esp_event.h>
#include <esp_log.h>
#include <vector>

ESP_EVENT_DEFINE_BASE(ROOMMANAGER_EVENT);

void RoomManager::init() {
  ESP_LOGI("RoomManager", "Initializing RoomManager.");
  RoomManager::_room_statuses_mutex = xSemaphoreCreateMutex();
  RoomManager::_load_all_rooms_task_handle = NULL;

  // Hook into MQTT events
  MqttManager::register_handler(MQTT_EVENT_DATA, RoomManager::_mqtt_event_handler, NULL);
  esp_event_handler_register(NSPM_CONFIGMANAGER_EVENT, ESP_EVENT_ANY_ID, RoomManager::_event_handler, NULL);
}

esp_err_t RoomManager::get_room_status(NSPanelRoomStatus *status, uint32_t room_id) {
  if (xSemaphoreTake(RoomManager::_room_statuses_mutex, pdMS_TO_TICKS(250) == pdPASS)) {
    for (auto it = RoomManager::_room_statuses.begin(); it != RoomManager::_room_statuses.end(); it++) {
      if ((*it)->id == room_id) {
        // Found the room, copy data to status reference and return ESP_OK
        *status = *(*it);
        xSemaphoreGive(RoomManager::_room_statuses_mutex);
        return ESP_OK;
      }
    }
    xSemaphoreGive(RoomManager::_room_statuses_mutex);
  } else {
    ESP_LOGE("RoomManager", "Failed to get _room_statuses_mutex while trying to get NSPanelRoomStatus for room %ld!", room_id);
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t RoomManager::get_room_status_mutable(NSPanelRoomStatus **status, uint32_t room_id) {
  if (xSemaphoreTake(RoomManager::_room_statuses_mutex, pdMS_TO_TICKS(250) == pdPASS)) {
    for (auto it = RoomManager::_room_statuses.begin(); it != RoomManager::_room_statuses.end(); it++) {
      if ((*it)->id == room_id) {
        // Found the room, copy data to status reference and return ESP_OK
        // As there is now good way to create a copy of a protobuf we pack it into a buffer
        // and then unpack it into the destination
        size_t status_pack_size = nspanel_room_status__get_packed_size((*it));
        std::vector<uint8_t> buffer(status_pack_size);
        nspanel_room_status__pack((*it), buffer.data());

        (*status) = nspanel_room_status__unpack(NULL, status_pack_size, buffer.data());
        xSemaphoreGive(RoomManager::_room_statuses_mutex);
        return ESP_OK;
      }
    }
    xSemaphoreGive(RoomManager::_room_statuses_mutex);
  } else {
    ESP_LOGE("RoomManager", "Failed to get _room_statuses_mutex while trying to get NSPanelRoomStatus for room %ld!", room_id);
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t RoomManager::get_current_room_status(NSPanelRoomStatus *status) {
  return RoomManager::get_room_status(status, RoomManager::_current_room_id);
}

esp_err_t RoomManager::get_current_room_status_mutable(NSPanelRoomStatus **status) {
  return RoomManager::get_room_status_mutable(status, RoomManager::_current_room_id);
}

esp_err_t RoomManager::get_current_room_id(int32_t *room_id) {
  if (xSemaphoreTake(RoomManager::_room_statuses_mutex, pdMS_TO_TICKS(250) == pdPASS)) {
    *room_id = RoomManager::_current_room_id;
    xSemaphoreGive(RoomManager::_room_statuses_mutex);
    return ESP_OK;
  } else {
    ESP_LOGE("RoomManager", "Failed to get _room_statuses_mutex while trying to get currently selected room ID!");
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t RoomManager::go_to_next_room() {
  NSPanelConfig config;
  if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
    if (xSemaphoreTake(RoomManager::_room_statuses_mutex, pdMS_TO_TICKS(250) == pdPASS)) {
      // Find the current index of the current room
      int i;
      for (i = 0; i < config.n_room_ids; i++) {
        if (config.room_ids[i] == RoomManager::_current_room_id) {
          break;
        }
      }
      // Check if there is more rooms ahead or if we should loop back to the beginning of all rooms
      if (i < config.n_room_ids - 1) {
        i++;
      } else {
        i = 0;
      }
      RoomManager::_current_room_id = config.room_ids[i];
      xSemaphoreGive(RoomManager::_room_statuses_mutex);
      ESP_LOGI("RoomManager", "Switched to room ID %ld. Will post event ROOM_SWITCHED.", RoomManager::_current_room_id);
      esp_event_post(ROOMMANAGER_EVENT, roommanager_event_t::ROOM_SWITCHED, NULL, 0, pdMS_TO_TICKS(250));
    } else {
      ESP_LOGE("RoomManager", "Failed to get _room_statuses_mutex while trying to get currently selected room ID!");
    }
  } else {
    ESP_LOGE("RoomManager", "Failed to get NSPanel Config from NSPM_ConfigManager while trying to go to next room.");
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t RoomManager::replace_room_status(NSPanelRoomStatus *status) {
  if (xSemaphoreTake(RoomManager::_room_statuses_mutex, pdMS_TO_TICKS(250) == pdPASS)) {
    auto existing_room_it = std::__find_if(RoomManager::_room_statuses.begin(), RoomManager::_room_statuses.end(), [status](auto room) {
      return (*room)->id == status->id;
    });

    if (existing_room_it != RoomManager::_room_statuses.end()) {
      // Found existing room with matching ID, replace it.
      // Begin by deleting the old room and then replace value in
      // std::list of room statuses. This is done so that we don't
      // have to free memory when deleting the existing room from
      // the list and then reallocate a new list item in memory

      // TODO: Is this memory safe? Are items (such as strings) freed when
      // doing free_unpacked. If so, the following scenario would cause problems:
      //   NSPanelStatus status;
      //   RoomManager::get_current_room_status(&status);
      //   RoomManager::replace_room_status(&status);
      // as the internal pointers will still point to the same memory region and be
      // freed upon replacing the room.
      nspanel_room_status__free_unpacked((*existing_room_it), NULL);
      (*existing_room_it) = status;

      xSemaphoreGive(RoomManager::_room_statuses_mutex);
      return ESP_OK;
    } else {
      // Existing room with matching ID was not found, insert into list.
      xSemaphoreGive(RoomManager::_room_statuses_mutex);
      return ESP_ERR_NOT_FOUND;
    }
  }

  return ESP_ERR_NOT_FOUND;
}

void RoomManager::_mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  // This function is only registered for MQTT_EVENT_DATA, handle data config data:
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  std::string topic_string = std::string(event->topic, event->topic_len);

  std::string manager_address = NSPM_ConfigManager::get_manager_address();
  std::string mqtt_base_topic = "nspanel/mqttmanager_";
  mqtt_base_topic.append(manager_address);
  mqtt_base_topic.append("/room/");
  if (topic_string.starts_with(mqtt_base_topic)) {
    NSPanelConfig config;
    if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
      std::string compare_string;
      for (int i = 0; i < config.n_room_ids; i++) {
        compare_string = mqtt_base_topic;
        compare_string.append(std::to_string(config.room_ids[i]));
        compare_string.append("/status");

        if (compare_string.compare(topic_string) == 0) { // The message was received from an MQTT topic that matches one of the room status topics
          NSPanelRoomStatus *room_status = nspanel_room_status__unpack(NULL, event->data_len, (const uint8_t *)event->data);
          if (room_status != NULL) {
            // Successfully unpacked protobuf status object, insert into list of room status loaded
            if (xSemaphoreTake(RoomManager::_room_statuses_mutex, pdMS_TO_TICKS(250) == pdPASS)) {
              // Find existing room with matching ID
              auto existing_room_it = std::__find_if(RoomManager::_room_statuses.begin(), RoomManager::_room_statuses.end(), [room_status](auto room) {
                return (*room)->id == room_status->id;
              });

              if (existing_room_it != RoomManager::_room_statuses.end()) {
                // Found existing room with matching ID, replace it.
                // Begin by deleting the old room and then replace value in
                // std::list of room statuses. This is done so that we don't
                // have to free memory when deleting the existing room from
                // the list and then reallocate a new list item in memory
                nspanel_room_status__free_unpacked((*existing_room_it), NULL);
                (*existing_room_it) = room_status;
              } else {
                // Existing room with matching ID was not found, insert into list.
                RoomManager::_room_statuses.push_back(room_status);
              }
              xSemaphoreGive(RoomManager::_room_statuses_mutex);

              ESP_LOGI("RoomManager", "Loaded room config/status for room %ld::%s, will send event.", room_status->id, room_status->name);
              if (room_status->id == RoomManager::_current_room_id) {
                esp_event_post(ROOMMANAGER_EVENT, roommanager_event_t::CURRENT_ROOM_UPDATED, NULL, 0, pdMS_TO_TICKS(250));
              }

              uint32_t room_id = room_status->id;
              esp_event_post(ROOMMANAGER_EVENT, roommanager_event_t::ROOM_LOADED, &room_id, sizeof(room_id), pdMS_TO_TICKS(250));
            } else {
              ESP_LOGE("RoomManager", "Couldn't take _room_statuses_mutex while processing MQTT room status update! Will unload unpacked data and wait for next update.");
              nspanel_room_status__free_unpacked(room_status, NULL);
            }
          } else {
            ESP_LOGE("RoomManager", "Error while unpacking room status from topic %s!", topic_string.c_str());
          }

          return;
        }
      }
    } else {
      ESP_LOGE("RoomManager", "Failed to get NSPM_Config from NSPM_ConfigManager while processing new room config!");
    }
  }
}

void RoomManager::_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {
  case nspm_configmanager_event::CONFIG_LOADED:
    if (RoomManager::_load_all_rooms_task_handle == NULL) {
      xTaskCreatePinnedToCore(RoomManager::_load_all_rooms, "load_all_rooms", 8192, NULL, 2, &RoomManager::_load_all_rooms_task_handle, 1);
    }
    break;

  default:
    break;
  }
}

void RoomManager::_load_all_rooms(void *arg) {
  ESP_LOGI("RoomManager", "Loading all rooms.");

  std::string manager_address = NSPM_ConfigManager::get_manager_address();
  std::string mqtt_base_topic = "nspanel/mqttmanager_";
  mqtt_base_topic.append(manager_address);
  mqtt_base_topic.append("/room/");
  NSPanelConfig config;
  for (;;) {
    if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
      for (;;) {
        std::string subscribe_topic;
        bool all_subscribed_successfully = true;
        for (int i = 0; i < config.n_room_ids; i++) {
          subscribe_topic = mqtt_base_topic;
          subscribe_topic.append(std::to_string(config.room_ids[i]));
          subscribe_topic.append("/status");
          if (MqttManager::subscribe(subscribe_topic) != ESP_OK) {
            ESP_LOGE("RoomManager", "Failed to subscribe to %s while loading rooms. Will try again.", subscribe_topic.c_str());
            all_subscribed_successfully = false;
            break;
          }
          vTaskDelay(pdMS_TO_TICKS(250));
        }
        if (all_subscribed_successfully) {
          // We subscribed to each room config/status topic. Cancel this loop
          break;
        }
      }

      // Wait for all rooms to be loaded
      ESP_LOGI("RoomManager", "Subscribed to all room config/status topics successfully. Waiting for all rooms to load");
      NSPanelRoomStatus status;
      for (int i = 0; i < config.n_room_ids; i++) {
        while (RoomManager::get_room_status(&status, config.room_ids[i]) != ESP_OK) {
          // Failed to get config for one room, what 100ms and check again
          ESP_LOGW("RoomManager", "Failed to get config/status for room ID %ld, will try again to check if all rooms are loaded.", config.room_ids[i]);
          vTaskDelay(pdMS_TO_TICKS(100));
          break;
        }
      }

      ESP_LOGI("RoomManager", "All rooms loaded successfully, will fire event.");
      RoomManager::_current_room_id = config.default_room;
      esp_event_post(ROOMMANAGER_EVENT, roommanager_event_t::ALL_ROOMS_LOADED, NULL, 0, pdMS_TO_TICKS(250));

      // Successfully loaded all rooms, break loop
      break;
    } else {
      ESP_LOGE("RoomManager", "Failed to get NSPM_Config when trying to load all rooms, will try again!");
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }

  // Load complete, delete task:
  RoomManager::_load_all_rooms_task_handle = NULL;
  vTaskDelete(NULL);
}