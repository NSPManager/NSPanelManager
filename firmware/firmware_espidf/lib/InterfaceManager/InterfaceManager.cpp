#include <ConfigManager.hpp>
#include <GUI_data.hpp>
#include <HomePage.hpp>
#include <InterfaceManager.hpp>
#include <LoadingPage.hpp>
#include <MqttManager.hpp>
#include <NSPM_ConfigManager.hpp>
#include <Nextion.hpp>
#include <Nextion_event.hpp>
#include <RoomManager_event.hpp>
#include <WiFiManager.hpp>
#include <esp_log.h>

void InterfaceManager::init() {
  esp_err_t nextion_init_result = Nextion::init();
  if (nextion_init_result != ESP_OK) {
    ESP_LOGE("InterfaceManager", "Failed to initialize Nextion display. Will not continue with InterfaceManager!");
    return;
  }

  esp_event_handler_register(NEXTION_EVENT, ESP_EVENT_ANY_ID, InterfaceManager::_nextion_event_handler, NULL);
  esp_event_handler_register(ROOMMANAGER_EVENT, ESP_EVENT_ANY_ID, InterfaceManager::_room_manager_event_handler, NULL);

  // Show boot page
  LoadingPage::show();

  // Wait for WiFi
  std::string append_string = "";
  if (!WiFiManager::connected() || WiFiManager::ip_info().ip.addr == 0) {
    std::string connection_text = "Connecting to ";
    connection_text.append(ConfigManager::wifi_ssid);

    std::string set_string = connection_text;
    set_string.append(append_string);
    LoadingPage::set_loading_text(set_string);
    LoadingPage::set_secondary_text("");

    if (append_string.size() < 3) {
      append_string.append(".");
    } else {
      append_string.clear();
    }
    vTaskDelay(pdMS_TO_TICKS(250));
  }

  // Wait for MQTT
  append_string.clear();
  while (!MqttManager::connected()) {
    std::string connection_text = "Connecting to MQTT";
    char ip_address_str[IP4ADDR_STRLEN_MAX];
    esp_netif_ip_info_t ip_info = WiFiManager::ip_info();
    sprintf(ip_address_str, IPSTR, IP2STR(&ip_info.ip));

    std::string set_string = connection_text;
    set_string.append(append_string);
    LoadingPage::set_loading_text(set_string);
    LoadingPage::set_secondary_text(ip_address_str);

    if (append_string.size() < 3) {
      append_string.append(".");
    } else {
      append_string.clear();
    }
    vTaskDelay(pdMS_TO_TICKS(250));
  }

  LoadingPage::set_loading_text("Loading config");
  NSPM_ConfigManager::init(); // Register to manager and load all config

  // WiFi and MQTT connected.
  // RoomManager will take over and load the config, once the config has been
  // successfully loaded the event handler for RoomManager will take over and send the Nextion display to the correct page
}

void InterfaceManager::_nextion_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {

  default:
    break;
  }
}

void InterfaceManager::_room_manager_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {
  case roommanager_event_t::ALL_ROOMS_LOADED:
    HomePage::show();
    break;

  default:
    break;
  }
}