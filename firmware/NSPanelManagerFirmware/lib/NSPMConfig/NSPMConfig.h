#ifndef NSPMConfig_H
#define NSPMConfig_H

#include <string>

enum BUTTON_MODE {
  DIRECT,
  DETACHED,
  CUSTOM_MQTT,
  FOLLOW
};

class NSPMConfig {
public:
  /// @brief Will try to initialize and load LittleFS
  /// @return True if successful
  bool init();
  /// @brief Load config file from LittleFS
  /// @return True if successful
  bool loadFromLittleFS();
  /// @brief Save current config file to LittleFS
  /// @param remountLittleFs: boolean on wether or not to remount littlefs before trying to save the config
  /// @return True if successful
  bool saveToLittleFS(bool remountLittleFs);
  /// @brief Reset all values to default
  /// @return True if successfuly saved to LittleFS
  bool factoryReset();
  /// @brief The instance of the config manager
  static NSPMConfig *instance;

  /// @brief Indicates that a config has successfully been loaded from the manager.
  bool successful_config_load = false;

  /// @brief The address (hostname or IP) to the manager server
  std::string manager_address = "";
  /// @brief The port to access to manager at
  uint16_t manager_port = 8000;

  /// @brief The logging level set for runtime
  uint8_t logging_level = 3;

  /// @brief The hostname of this device
  std::string wifi_hostname = "";
  /// @brief The WiFi name to connect to
  std::string wifi_ssid = "";
  /// @brief The pre-shared key for the WiFi
  std::string wifi_psk = "";

  /// @brief Address to MQTT server
  std::string mqtt_server = "";
  /// @brief The port to connect to MQTT with
  uint16_t mqtt_port = 1883;
  /// @brief MQTT Username
  std::string mqtt_username = "";
  /// @brief MQTT Password
  std::string mqtt_password = "";
  /// @brief MQTT Availability topic
  std::string mqtt_availability_topic = "";
  /// @brief MQTT panel status topic
  std::string mqtt_panel_status_topic = "";
  /// @brief MQTT panel command topic
  std::string mqtt_panel_cmd_topic = "";
  /// @brief MQTT screen brightness topic
  std::string mqtt_panel_screen_brightness_topic = "";
  /// @brief MQTT screensaver brightness topic
  std::string mqtt_panel_screensaver_brightness = "";
  /// @brief MQTT screensaver mode topic
  std::string mqtt_panel_screensaver_mode = "";
  /// @brief MQTT panel temperature topic
  std::string mqtt_panel_temperature_topic = "";
  /// @brief MQTT topic to send logs to
  std::string mqtt_log_topic = "";
  /// @brief The MQTT topic to send panel on/off status events to
  std::string mqtt_screen_state_topic = "";
  /// @brief The MQTT topic to receive on/off command for the panel from.
  std::string mqtt_screen_cmd_topic = "";
  /// @brief The MQTT topic to receive on/off command for relay 1
  std::string mqtt_relay1_cmd_topic = "";
  /// @brief The MQTT topic to send on/off state updates for relay 1
  std::string mqtt_relay1_state_topic = "";
  /// @brief The MQTT topic to receive on/off command for relay 2
  std::string mqtt_relay2_cmd_topic = "";
  /// @brief The MQTT topic to send on/off state updates for relay 2
  std::string mqtt_relay2_state_topic = "";

  /// @brief The upload baud rate for the serial connection when uploading a new TFT file
  uint32_t tft_upload_baud = 115200;
  /// @brief Wether or not to use the "v1.2" protcol or the v1.0
  bool use_new_upload_protocol = true;

  /// @brief MD5 checksum for currently installed firmware.
  std::string md5_firmware = "";
  /// @brief MD5 checksum for currently installed LittleFS.
  std::string md5_data_file = "";
  /// @brief MD5 checksum for the currently installed TFT file.
  std::string md5_tft_file = "";

  /// @brief Mimimum time for a button to be pressed to register as a press
  uint16_t button_min_press_time = 100;
  /// @brief Mimimum time for a button to be held to register as a long press
  uint16_t button_long_press_time = 1000;

  /// @brief Mimimum time to hold a button to start special mode
  uint16_t special_mode_trigger_time = 2000;
  /// @brief Time to exit special mode after no actions has been taken
  uint16_t special_mode_release_time = 30000;

  /// @brief Time to ignore incomming MQTT messages after a command is sent out
  uint16_t mqtt_ignore_time = 50000;

  // Button modes. 0 = direct mode. 1 = detached mode.
  BUTTON_MODE button1_mode = BUTTON_MODE::DIRECT;
  BUTTON_MODE button2_mode = BUTTON_MODE::DIRECT;

  // Button 1 custom MQTT message paramaters
  std::string button1_mqtt_topic = "";
  std::string button1_mqtt_payload = "";

  // Button 2 custom MQTT message paramaters
  std::string button2_mqtt_topic = "";
  std::string button2_mqtt_payload = "";

  bool reverse_relays = false;

  // @brief default relay state
  bool relay1_default_mode = false;
  bool relay2_default_mode = false;

  // @brief The temperature calibration in degrees
  float temperature_calibration = 0;

  // Indicates wether or not LittleFS was mounted or not.
  bool littlefs_mount_successfull = false;

  /// @brief Should we report time in farenheit or celcius? True = farenheit, false = Celcius
  bool use_fahrenheit = false;

  /// @brief Indicates what TFT and checksums to check.
  bool is_us_panel = false;
};

#endif
