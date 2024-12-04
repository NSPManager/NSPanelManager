#include <ConfigManager.hpp>
#include <InterfaceManager.hpp>
#include <LittleFS.hpp>
#include <MqttManager.hpp>
#include <Nextion.hpp>
#include <PageManager.hpp>
#include <RoomManager.hpp>
#include <WebManager.hpp>
#include <WiFiManager.hpp>
#include <esp_log.h>
#include <nvs_flash.h>

extern "C" void app_main() {
  // Set global log level initially. This is later set from saved config.
  esp_log_level_set("*", ESP_LOG_DEBUG);

  ESP_LOGI("Main", "Starting NSPanel Manager firmware. Version TODO.");

  esp_event_loop_create_default();

  nvs_flash_init();
  if (LittleFS::mount() != ESP_OK) {
    ESP_LOGE("Main", "Failed to mount LittleFS!");
  }

  ConfigManager::create_default(); // Set default values on all config entities
  if (ConfigManager::load_config() != ESP_OK) {
    ESP_LOGE("Main", "Failed to load config from LittleFS. If this is the first time running the panel this is normal as not config has been saved yet.");
    ESP_LOGI("Main", "Default config values has been applied, will save those to create a config file.");
    esp_err_t config_save_result = ConfigManager::save_config();
    if (config_save_result != ESP_OK) {
      ESP_LOGE("Main", "Failed to save config to LittleFS, got error %s!", esp_err_to_name(config_save_result));
    }
  } else {
    if (ConfigManager::wifi_hostname.empty()) {
      ESP_LOGE("Main", "Successfully loaded config from LittleFS but the config is not valid. Empty WiFi hostname, will load default values.");
      ConfigManager::create_default();
    } else {
      ESP_LOGI("Main", "Config loaded successfully. Starting NSPanel as '%s'.", ConfigManager::wifi_hostname.c_str());
      // Set global log level
      esp_log_level_set("*", static_cast<esp_log_level_t>(ConfigManager::log_level));
    }
  }

  // Initialize PageManager memory
  PageManager::init();

  // Start task that handles WiFi connection
  WiFiManager::start_client(&ConfigManager::wifi_ssid, &ConfigManager::wifi_psk, &ConfigManager::wifi_hostname);

  // Start task that handles MQTT connection
  MqttManager::start(&ConfigManager::mqtt_server, &ConfigManager::mqtt_port, &ConfigManager::mqtt_username, &ConfigManager::mqtt_password);

  // Start RoomManager
  RoomManager::init();

  // Start task that handles the HTTP server
  WebManager::start();

  // Start the interface and load config
  InterfaceManager::init();
}