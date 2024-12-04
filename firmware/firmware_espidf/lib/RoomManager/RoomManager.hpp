#pragma once
#include <esp_event.h>
#include <list>
#include <protobuf_nspanel.pb-c.h>

class RoomManager {
public:
  /**
   * Load and initialize RoomManager.
   */
  static void init();

  /**
   * @brief Get a room status given the ID
   * @param status: Reference to where to save NSPanelRoomStatus to
   * @param room_id: What room to get the status for
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t get_room_status(NSPanelRoomStatus *status, uint32_t room_id);

  /**
   * @brief Get a room status pointer to a __unpacked protobuf given the ID
   * @param status: Reference to where to save NSPanelRoomStatus to
   * @param room_id: What room to get the status for
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t get_room_status_mutable(NSPanelRoomStatus **status, uint32_t room_id);

  /**
   * @brief Get a room status object for the currently selected room
   * @param status: Reference to where to save NSPanelRoomStatus to
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t get_current_room_status(NSPanelRoomStatus *status);

  /**
   * @brief Get a room status pointer to a __unpacked protobuf to currently selected room
   * @param status: Reference to where to save NSPanelRoomStatus to
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t get_current_room_status_mutable(NSPanelRoomStatus **status);

  /**
   * @brief Get the ID of the currently selected room
   * @param room_id: Reference to where to save the currently selected room ID
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t get_current_room_id(int32_t *room_id);

  /**
   * @brief Get the ID of the currently selected room
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED or ESP_ERR_NOT_FOUND
   */
  static esp_err_t go_to_next_room();

  /**
   * @brief Will search for a matching room (match by room ID) and replace the one in the list of rooms
   * @param status: The new room status to be replaced into the list of rooms.
   * @return ESP_OK if operation was successful, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t replace_room_status(NSPanelRoomStatus *status);

private:
  /**
   * @brief Handle events triggered from MQTT places.
   */
  static void _mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * @brief Handle events triggered from other places.
   */
  static void _event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * The task that will load all rooms into memory.
   */
  static void _task_load_all_room_configs(void *param);

  /**
   * Once the NSPM config has been loaded from the MQTT Manager container, load all rooms
   */
  static void _load_all_rooms(void *arg);

  // Vars:
  // List of all loaded rooms and their statuses
  static inline std::list<NSPanelRoomStatus *> _room_statuses;

  // Mutex to only allow one task at the time to access the list of room statuses
  static inline SemaphoreHandle_t _room_statuses_mutex;

  // Task handle to task that loads all rooms via MQTT
  static inline TaskHandle_t _load_all_rooms_task_handle;

  // The currently selected room
  static inline uint32_t _current_room_id;
};