#pragma once

#include <esp_event_base.h>
#include <esp_task.h>
#include <protobuf_nspanel.pb-c.h>
#include <string>

/**
 * NSPM_ConfigManager does not handle the config stored on the panel. NSPM_ConfigManager is responsible for loading and keeping the stored config from
 * NSPanel Manager manager container in sync. When changes occurs it fires the relevant events defined in NSPM_ConfigManager_events.hpp
 */
class NSPM_ConfigManager {
public:
  /**
   * @brief the config manager and load all relevant configuration from the manager.
   */
  static void init();

  /**
   * @brief Get a copy of the config object in order to read data from it.
   * This is done so that the internal mutex can handle any collisions even though this is suboptimal.
   * It would be better to return a pointer to the config but that leaves the risk of it becoming invalid
   * as a new config is loaded.
   * @param config: The place to copy the NSPanelConfig object to
   * @return ESP_OK if everything was done correctly, otherwise ESP_ERR_NOT_FINISHED
   */
  static esp_err_t get_config(NSPanelConfig *config);

  /**
   * @brief Get the manager IP address
   */
  static std::string get_manager_address();

  /**
   * @brief Get the port used to communicate with the manager
   */
  static uint16_t get_manager_port();

  /**
   * @brief Get the MQTT topic to send commands to the manager
   */
  static std::string get_manager_command_topic();

private:
  /**
   * @brief MQTT events, such as "reload config" and "room config" updates.
   */
  static void _mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * @brief sending request_requests to any manager.
   */
  static void _task_send_register_request(void *arg);

  /**
   * @brief Handle a "register_accept" request from MQTT
   * @param data: Received data from MQTT
   * @param data_length: Number of bytes that were received
   */
  static void _handle_register_accept(const char *data, size_t data_length);

  /**
   * @brief Handle data received on the MQTT config topic
   * @param data: Received data from MQTT
   * @param data_length: Number of bytes that were received
   */
  static void _handle_new_config_data(const char *data, size_t data_length);

  // Vars:
  // Task handle for the task responsible for sending all register requests to the manager. This is used to stop the task once a register_accept has been received.
  static inline TaskHandle_t _task_send_register_request_handle;

  // Topic that the MQTT Manager container send commands to panel on:
  static inline std::string _mqtt_command_topic;

  // Topic that NSPanels can send command to the MQTTManager on:
  static inline std::string _mqtt_manager_command_topic;

  // Used to stop "register_request" task so that it cleans up the JSON data.
  static inline bool _send_register_requests;

  // IP address to the MQTT Manager container
  static inline std::string _manager_address = "";

  // Port used to download files from MQTT Manager container
  static inline uint16_t _manager_port;

  // MQTT topic used from manager to send new config data to panel
  static inline std::string _mqtt_config_topic;

  // Mutex used to lock the writing to the NSPanel Config object.
  static inline SemaphoreHandle_t _config_mutex;

  // The protobuf NSPanelConfig object decoded from MQTT.
  static inline NSPanelConfig *_config = NULL;
};