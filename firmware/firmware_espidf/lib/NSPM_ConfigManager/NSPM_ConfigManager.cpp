#include "NSPM_ConfigManager.hpp"
#include <ConfigManager.hpp>
#include <MqttManager.hpp>
#include <NSPM_ConfigManager_event.hpp>
#include <NSPM_version.hpp>
#include <WiFiManager.hpp>
#include <cJSON.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_wifi.h>

ESP_EVENT_DEFINE_BASE(NSPM_CONFIGMANAGER_EVENT);

void NSPM_ConfigManager::init() {
  NSPM_ConfigManager::_config_mutex = xSemaphoreCreateMutex();
  MqttManager::register_handler(MQTT_EVENT_DATA, &NSPM_ConfigManager::_mqtt_event_handler, NULL);

  // Subscribe to MQTT command topic
  NSPM_ConfigManager::_mqtt_command_topic = "nspanel/";
  NSPM_ConfigManager::_mqtt_command_topic.append(ConfigManager::wifi_hostname);
  NSPM_ConfigManager::_mqtt_command_topic.append("/command");
  // Wait until subscribe is successful
  while (MqttManager::subscribe(NSPM_ConfigManager::_mqtt_command_topic.c_str()) != ESP_OK) {
    ESP_LOGE("NSPM_ConfigManager", "Tried to subscribe to NSPanel command topic but subscribe call was unsuccessful! Will try again.");
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  // We have now subscribed to MQTT command topic, start the task to send MQTT register_requests for managers to answer to
  NSPM_ConfigManager::_send_register_requests = true;
  xTaskCreate(NSPM_ConfigManager::_task_send_register_request, "register_request_task", 4096, NULL, 2, &NSPM_ConfigManager::_task_send_register_request_handle);
}

void NSPM_ConfigManager::_mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  std::string topic_string = std::string(event->topic, event->topic_len);
  // esp_mqtt_client_handle_t client = event->client;

  switch ((esp_mqtt_event_id_t)event_id) {
  case MQTT_EVENT_DATA: {
    if (NSPM_ConfigManager::_mqtt_command_topic.compare(topic_string) == 0) {
      NSPM_ConfigManager::_handle_register_accept(event->data, event->data_len);
    } else if (NSPM_ConfigManager::_mqtt_config_topic.compare(topic_string) == 0) {
      NSPM_ConfigManager::_handle_new_config_data(event->data, event->data_len);
    }
    break;
  }

  default:
    break;
  }
}

void NSPM_ConfigManager::_handle_register_accept(const char *data, size_t data_length) {
  cJSON *json = cJSON_ParseWithLength(data, data_length);
  if (json != NULL) {
    cJSON *item = cJSON_GetObjectItem(json, "command");
    if (cJSON_IsString(item) && item->valuestring != NULL) {
      if (strcmp("register_accept", item->valuestring) != 0) {
        cJSON_Delete(json); // Cleanup
        return;             // Command we received was not a "register_accept" from manager, cancel processing.
      }
    }

    item = cJSON_GetObjectItem(json, "address");
    if (cJSON_IsString(item) && item->valuestring != NULL) {
      NSPM_ConfigManager::_manager_address = item->valuestring;
    } else {
      ESP_LOGE("NSPM_ConfigManager", "register_accept does not contain valid 'address' field.");
      cJSON_Delete(json); // Cleanup
      return;
    }

    item = cJSON_GetObjectItem(json, "port");
    if (cJSON_IsNumber(item) && item->valueint != 0) {
      NSPM_ConfigManager::_manager_port = item->valueint;
    } else {
      ESP_LOGE("NSPM_ConfigManager", "register_accept does not contain valid 'port' field.");
      cJSON_Delete(json); // Cleanup
      return;
    }

    item = cJSON_GetObjectItem(json, "config_topic");
    if (cJSON_IsString(item) && item->valuestring != NULL) {
      NSPM_ConfigManager::_mqtt_config_topic = item->valuestring;
    } else {
      ESP_LOGE("NSPM_ConfigManager", "register_accept does not contain valid 'config_topic' field.");
      cJSON_Delete(json); // Cleanup
      return;
    }

    ESP_LOGI("NSPM_ConfigManager", "Received register_accept from manager. Registered to manager at %s:%d", NSPM_ConfigManager::_manager_address.c_str(), NSPM_ConfigManager::_manager_port);
    NSPM_ConfigManager::_send_register_requests = false;
    NSPM_ConfigManager::_mqtt_manager_command_topic = "nspanel/mqttmanager_";
    NSPM_ConfigManager::_mqtt_manager_command_topic.append(NSPM_ConfigManager::_manager_address);
    NSPM_ConfigManager::_mqtt_manager_command_topic.append("/command");
    // Subscribe to where the NSPanel Manager container will send the config for this panel
    while (MqttManager::subscribe(NSPM_ConfigManager::_mqtt_config_topic) != ESP_OK) {
      ESP_LOGE("NSPM_ConfigManager", "Failed to subscribe to NSPanel config topic '%s'.", NSPM_ConfigManager::_mqtt_config_topic.c_str());
      vTaskDelay(pdMS_TO_TICKS(500));
    }
    cJSON_Delete(json); // Cleanup

    ESP_LOGI("NSPM_ConfigManager", "Register accept fully processed.");
  }
}

void NSPM_ConfigManager::_handle_new_config_data(const char *data, size_t data_length) {
  ESP_LOGI("NSPM_ConfigManager", "Received new config data from MQTT.");

  if (xSemaphoreTake(NSPM_ConfigManager::_config_mutex, pdMS_TO_TICKS(5000))) {
    if (NSPM_ConfigManager::_config != NULL) {
      nspanel_config__free_unpacked(NSPM_ConfigManager::_config, NULL);
    }
    NSPM_ConfigManager::_config = nspanel_config__unpack(NULL, data_length, (const uint8_t *)data);
    xSemaphoreGive(NSPM_ConfigManager::_config_mutex);

    esp_event_post(NSPM_CONFIGMANAGER_EVENT, nspm_configmanager_event::CONFIG_LOADED, NULL, 0, pdMS_TO_TICKS(250));
  } else {
    ESP_LOGE("NSPM_ConfigManager", "Failed to gain config mutex while processing new config from MQTT!");
  }
}

esp_err_t NSPM_ConfigManager::get_config(NSPanelConfig *config) {
  if (NSPM_ConfigManager::_config != NULL) {
    if (xSemaphoreTake(NSPM_ConfigManager::_config_mutex, pdMS_TO_TICKS(5000))) {
      *config = *NSPM_ConfigManager::_config;
      xSemaphoreGive(NSPM_ConfigManager::_config_mutex);
      return ESP_OK;
    } else {
      ESP_LOGE("NSPM_ConfigManager", "Failed to gain config mutex while processing request for config from other task!");
    }
  }
  return ESP_ERR_NOT_FINISHED;
}

std::string NSPM_ConfigManager::get_manager_address() {
  return NSPM_ConfigManager::_manager_address;
}

uint16_t NSPM_ConfigManager::get_manager_port() {
  return NSPM_ConfigManager::_manager_port;
}

std::string NSPM_ConfigManager::get_manager_command_topic() {
  return NSPM_ConfigManager::_mqtt_manager_command_topic;
}

void NSPM_ConfigManager::_task_send_register_request(void *arg) {
  // Get MAC address string
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA); // Read MAC address for Wi-Fi Station
  char mac_str[18];                    // Format: AA:BB:CC:DD:EE:FF
  snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Get IP address string
  char ip_address_str[IP4ADDR_STRLEN_MAX];
  esp_netif_ip_info_t ip_info = WiFiManager::ip_info();
  sprintf(ip_address_str, IPSTR, IP2STR(&ip_info.ip));

  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "command", "register_request");
  cJSON_AddStringToObject(json, "mac_origin", mac_str);
  cJSON_AddStringToObject(json, "friendly_name", ConfigManager::wifi_hostname.c_str());
  cJSON_AddStringToObject(json, "version", NSPM_VERSION);
  cJSON_AddStringToObject(json, "md5_firmware", ConfigManager::md5_firmware.c_str());
  cJSON_AddStringToObject(json, "md5_data_file", ConfigManager::md5_data_file.c_str());
  cJSON_AddStringToObject(json, "md5_tft_file", ConfigManager::md5_gui.c_str());
  cJSON_AddStringToObject(json, "address", ip_address_str);
  char *json_string = cJSON_Print(json);

  while (NSPM_ConfigManager::_send_register_requests) {
    MqttManager::publish("nspanel/mqttmanager/command", json_string, strlen(json_string), false);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }

  cJSON_Delete(json);
  vTaskDelete(NULL); // Delete own task.
}