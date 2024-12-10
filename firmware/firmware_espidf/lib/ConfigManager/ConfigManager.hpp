#pragma once
#include <esp_err.h>
#include <string>

class ConfigManager {
public:
  /**
   * @brief Load the saved config from LittleFS. This requires that LittleFS has already been mounted.
   * @return ESP_OK if everything was loaded correctly from config. ESP_ERR_NOT_FOUND if config was not found on LittleFS. ESP_ERR_INVALID_STATE if failed to parse JSON.
   */
  static esp_err_t load_config();

  /**
   * Create a default config file. This will not be saved to LittleFS. It simply sets default values.
   */
  static void create_default();

  /**
   * @brief Save the current config values to the config file in LittleFS. This requires that LittleFS has already been mounted.
   * @return ESP_OK if everything was saved correctly, otherwise ESP_ERR_NOT_FINISHED.
   */
  static esp_err_t save_config();

  // Hostname of this NSPanel
  static inline std::string wifi_hostname;

  // WiFi SSID to connect to
  static inline std::string wifi_ssid;

  // The password to the WiFi to connect to
  static inline std::string wifi_psk;

  // IP address to the MQTT server to connect to
  static inline std::string mqtt_server;

  // What port to connect to the MQTT server on
  static inline uint16_t mqtt_port;

  // Username for MQTT server authentication
  static inline std::string mqtt_username;

  // Password for MQTT server authentication
  static inline std::string mqtt_password;

  // Use latest Nextion upload protocol?
  static inline bool use_latest_nextion_upload_protocol;

  // Nextion upload protocol baudrate
  static inline uint32_t nextion_upload_baudrate;

  // Selected log level
  static inline uint8_t log_level;

  // MD5 checksum for installed firmware, used to check if new firmware update is available in manager
  static inline std::string md5_firmware;

  // MD5 checksum for installed data_file (LittleFS), used to check if new data_file update is available in manager
  static inline std::string md5_data_file;

  // MD5 checksum for installed gui, used to check if new gui update is available in manager
  static inline std::string md5_gui;
};