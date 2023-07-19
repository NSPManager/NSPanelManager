#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"
#include <WiFiManager.hpp>
#include <cstring>
#include <stdio.h>

void WiFiManager::init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  WiFiManager::_wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  WiFiManager::_wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&WiFiManager::_wifi_init_config));
}

bool WiFiManager::is_connected() {
  return WiFiManager::_connected;
}

void WiFiManager::event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    printf("Starting connect to WiFi.\n");
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
    WiFiManager::_connected = true;
    printf("Connected to WiFi.\n");
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    wifi_event_sta_disconnected_t *sta_disconnect_evt = (wifi_event_sta_disconnected_t *)event_data;
    printf("Failed to connect to WiFi. Reason: '%d'. Retrying...\n", sta_disconnect_evt->reason);
    WiFiManager::_connected = false;
    esp_wifi_connect();
    WiFiManager::_wifi_connect_retries++;
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    printf("Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    WiFiManager::_wifi_connect_retries = 0;
    xEventGroupSetBits(WiFiManager::_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void WiFiManager::try_connect(std::string ssid, std::string psk) {
  if (WiFiManager::_is_running) {
    return;
  }

  WiFiManager::_is_running = true;
  printf("Trying to connect to ");
  printf(ssid.c_str());
  printf(" with password ");
  printf(psk.c_str());
  printf("\n");

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &WiFiManager::event_handler,
                                                      NULL,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &WiFiManager::event_handler,
                                                      NULL,
                                                      &instance_got_ip));

  // Create WiFi config
  wifi_config_t wifi_config = {};
  memcpy(wifi_config.sta.ssid, ssid.c_str(), std::min(strlen(ssid.c_str()), sizeof(wifi_config.sta.ssid)));
  memcpy(wifi_config.sta.password, psk.c_str(), std::min(strlen(psk.c_str()), sizeof(wifi_config.sta.password)));
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.pmf_cfg.capable = true;
  wifi_config.sta.pmf_cfg.required = false;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  printf("wifi_init_sta finished.\n");

  EventBits_t bits = xEventGroupWaitBits(WiFiManager::_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
   * happened. */
  if (bits & WIFI_CONNECTED_BIT) {
    printf("Connected to WiFi.\n");
  } else if (bits & WIFI_FAIL_BIT) {
    printf("Failed to connect to WiFi.\n");
  } else {
    printf("ERROR! Unknown Wifi connect event.");
  }

  /* The event will not be processed after unregister */
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
  vEventGroupDelete(WiFiManager::_wifi_event_group);
}
