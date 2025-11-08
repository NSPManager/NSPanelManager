#include "ConfigManager.hpp"
#include <LittleFS.hpp>
#include <cJSON.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_system.h>
#include <lfs.h>
#include <string.h>

esp_err_t ConfigManager::load_config() {
  FILE *f = fopen("/littlefs/config.json", "r");
  if (f == NULL) {
    ESP_LOGE("ConfigManager", "Failed to open /littlefs/config.json for reading.");
    return ESP_ERR_NOT_FOUND;
  }

  // Get file size.
  fseek(f, 0, SEEK_END);
  long file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *read_buffer = (char *)malloc(file_size);
  if (read_buffer == NULL) {
    ESP_LOGE("ConfigManager", "Failed to allocate %ld bytes for read buffer while reading config from LittleFS.", file_size);
    fclose(f);
    return ESP_ERR_NOT_FINISHED;
  }

  fread(read_buffer, 1, file_size, f);
  fclose(f);

  cJSON *json = cJSON_Parse(read_buffer);
  if (json == NULL) {
    ESP_LOGE("ConfigManager", "Failed to parse JSON while reading config from LittleFS!");
    free(read_buffer);
    return ESP_ERR_NOT_FINISHED;
  }

  cJSON *item = cJSON_GetObjectItem(json, "log_level");
  if (cJSON_IsNumber(item)) {
    ConfigManager::log_level = item->valueint;
  }

  item = cJSON_GetObjectItem(json, "wifi_hostname");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::wifi_hostname = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "wifi_ssid");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::wifi_ssid = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "wifi_psk");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::wifi_psk = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "mqtt_server");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::mqtt_server = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "mqtt_port");
  if (cJSON_IsNumber(item)) {
    ConfigManager::mqtt_port = item->valueint;
  }

  item = cJSON_GetObjectItem(json, "mqtt_username");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::mqtt_username = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "mqtt_password");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::mqtt_password = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "md5_firmware");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::md5_firmware = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "md5_data_file");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::md5_data_file = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "md5_gui");
  if (cJSON_IsString(item) && (item->valuestring != NULL)) {
    ConfigManager::md5_gui = item->valuestring;
  }

  item = cJSON_GetObjectItem(json, "use_latest_nextion_upload_protocol");
  if (cJSON_IsBool(item)) {
    ConfigManager::use_latest_nextion_upload_protocol = cJSON_IsTrue(item);
  }

  item = cJSON_GetObjectItem(json, "nextion_upload_baudrate");
  if (cJSON_IsNumber(item)) {
    ConfigManager::nextion_upload_baudrate = item->valueint;
  }

  cJSON_Delete(json);
  free(read_buffer);

  return esp_err_t();
}

void ConfigManager::create_default() {
  // Load MAC-address. Last three bytes are specific to this device and will be used as a unique ID.
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA); // Read MAC address for Wi-Fi Station
  char mac_str[7];                     // Format: XXXXXX
  snprintf(mac_str, sizeof(mac_str), "%02X%02X%02X", mac[3], mac[4], mac[5]);

  ConfigManager::wifi_hostname = "NSPMPanel-";
  ConfigManager::wifi_hostname.append(mac_str);

  ConfigManager::wifi_ssid = "";
  ConfigManager::wifi_psk = "";

  ConfigManager::mqtt_server = "";
  ConfigManager::mqtt_port = 1883;
  ConfigManager::mqtt_username = "";
  ConfigManager::mqtt_password = "";

  ConfigManager::md5_firmware = "";
  ConfigManager::md5_data_file = "";
  ConfigManager::md5_gui = "";

  ConfigManager::use_latest_nextion_upload_protocol = true;
  ConfigManager::nextion_upload_baudrate = 115200;
  ConfigManager::log_level = ESP_LOG_WARN;
}

esp_err_t ConfigManager::save_config() {
  FILE *f = fopen("/littlefs/config.json", "w");
  if (f == NULL) {
    ESP_LOGE("ConfigManager", "Failed to open /littlefs/config.json for writing.");
    return ESP_ERR_NOT_FINISHED;
  }

  cJSON *json = cJSON_CreateObject();
  cJSON_AddNumberToObject(json, "log_level", ConfigManager::log_level);
  cJSON_AddStringToObject(json, "wifi_hostname", ConfigManager::wifi_hostname.c_str());
  cJSON_AddStringToObject(json, "wifi_ssid", ConfigManager::wifi_ssid.c_str());
  cJSON_AddStringToObject(json, "wifi_psk", ConfigManager::wifi_psk.c_str());
  cJSON_AddStringToObject(json, "mqtt_server", ConfigManager::mqtt_server.c_str());
  cJSON_AddNumberToObject(json, "mqtt_port", ConfigManager::mqtt_port);
  cJSON_AddStringToObject(json, "mqtt_username", ConfigManager::mqtt_username.c_str());
  cJSON_AddStringToObject(json, "mqtt_password", ConfigManager::mqtt_password.c_str());
  cJSON_AddStringToObject(json, "md5_firmware", ConfigManager::md5_firmware.c_str());
  cJSON_AddStringToObject(json, "md5_data_file", ConfigManager::md5_data_file.c_str());
  cJSON_AddStringToObject(json, "md5_gui", ConfigManager::md5_gui.c_str());

  if (ConfigManager::use_latest_nextion_upload_protocol) {
    cJSON_AddTrueToObject(json, "use_latest_nextion_upload_protocol");
  } else {
    cJSON_AddFalseToObject(json, "use_latest_nextion_upload_protocol");
  }
  cJSON_AddNumberToObject(json, "nextion_upload_baudrate", ConfigManager::nextion_upload_baudrate);

  char *json_string = cJSON_Print(json);
  size_t bytes_written = fwrite(json_string, sizeof(char), strlen(json_string), f);

  ESP_LOGI("ConfigManager", "Saved %d bytes to /littlefs/config.json", bytes_written);

  bool wrote_correct_num_bytes = (bytes_written == (sizeof(char) * strlen(json_string)));
  cJSON_Delete(json);
  free(json_string);
  fclose(f);

  if (wrote_correct_num_bytes) {
    return ESP_OK;
  } else {
    return ESP_ERR_NOT_FINISHED;
  }
}
