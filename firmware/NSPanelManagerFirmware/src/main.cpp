#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <InterfaceManager.hpp>
#include <LittleFS.h>
#include <MqttLog.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <PubSubClient.h>
#include <WebManager.hpp>
#include <WiFi.h>
#include <nspm-bin-version.h>
#include <string>
#include <MqttManager.hpp>

NSPanel nspanel;
InterfaceManager interfaceManager;
MqttLog logger;
NSPMConfig config;
WebManager webMan;
TaskHandle_t _taskWifiAndMqttManager;
WiFiClient espClient;
MqttManager mqttManager;

unsigned long lastStatusReport = 0;

void registerToNSPanelManager() {
  if (WiFi.isConnected()) {
    while (true) {
      WiFiClient wifiClient;
      HTTPClient httpClient;
      std::string url = "http://";
      url.append(NSPMConfig::instance->manager_address);
      url.append(":");
      url.append(std::to_string(NSPMConfig::instance->manager_port));
      url.append("/api/register_nspanel");

      StaticJsonDocument<128> doc;
      doc["mac_address"] = WiFi.macAddress().c_str();
      doc["friendly_name"] = NSPMConfig::instance->wifi_hostname.c_str();
      doc["version"] = NSPanelManagerFirmwareVersion;

      char buffer[128];
      serializeJson(doc, buffer);

      httpClient.begin(wifiClient, url.c_str());
      httpClient.addHeader("Content-Type", "application/json");
      int responseCode = httpClient.POST(buffer);

      if (responseCode == 200) {
        InterfaceManager::hasRegisteredToManager = true;
        LOG_INFO("Registered to manager at: ", url.c_str());
        break;
      } else {
        LOG_ERROR("Failed to register panel at: ", url.c_str(), ". Will try again in 5 seconds.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
      }
    }
  }
}

void taskManageWifiAndMqtt(void *param) {
  LOG_INFO("taskWiFiMqttHandler started!");
  Serial.print("Configured to connect to WiFi: "); Serial.println(config.wifi_ssid.c_str());
  Serial.print("with hostname: "); Serial.println(config.wifi_hostname.c_str());
  if (!NSPMConfig::instance->wifi_ssid.empty()) {
    for (;;) {
      if (!WiFi.isConnected()) {
        LOG_ERROR("WiFi not connected!");
        Serial.println("WiFi not connected!");
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(config.wifi_hostname.c_str());
        while (!WiFi.isConnected()) {
          Serial.print("Connecting to WiFi "); Serial.println(config.wifi_ssid.c_str());
          WiFi.begin(config.wifi_ssid.c_str(), config.wifi_psk.c_str());
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          if (WiFi.isConnected()) {
            Serial.println("Connected to WiFi!");
            LOG_INFO("Connected to WiFi ", config.wifi_ssid.c_str());
            Serial.print("Connected to WiFi "); Serial.println(config.wifi_ssid.c_str());
            LOG_INFO("IP Address: ", WiFi.localIP());
            LOG_INFO("Netmask:    ", WiFi.subnetMask());
            LOG_INFO("Gateway:    ", WiFi.gatewayIP());
            // Start web server
            webMan.init(NSPanelManagerFirmwareVersion);
            registerToNSPanelManager();
          } else {
            LOG_ERROR("Failed to connect to WiFi. Will try again in 10 seconds");
            Serial.println("Failed to connect to WiFi. Will try again in 10 seconds");
          }
        }
      } else if (config.wifi_ssid.empty()) {
        LOG_ERROR("No WiFi SSID configured!");
      }

      if (WiFi.isConnected() && MqttManager::connected() && NSPanel::instance->getUpdateState()) {
        DynamicJsonDocument *status_report_doc = new DynamicJsonDocument(512);
        (*status_report_doc)["rssi"] = WiFi.RSSI();
        (*status_report_doc)["heap_used_pct"] = round((float(ESP.getFreeHeap()) / float(ESP.getHeapSize())) * 100);
        (*status_report_doc)["mac"] = WiFi.macAddress().c_str();
        (*status_report_doc)["state"] = "updating_tft";
        (*status_report_doc)["progress"] = NSPanel::instance->getUpdateProgress();

        char buffer[512];
        uint json_length = serializeJson(*status_report_doc, buffer);
        delete status_report_doc;
        MqttManager::publish(NSPMConfig::instance->mqtt_panel_status_topic, buffer);
        lastStatusReport = millis();
      } else if (WiFi.isConnected() && MqttManager::connected() && WebManager::getState() == WebManagerState::UPDATING_FIRMWARE) {
        DynamicJsonDocument *status_report_doc = new DynamicJsonDocument(512);
        (*status_report_doc)["rssi"] = WiFi.RSSI();
        (*status_report_doc)["heap_used_pct"] = round((float(ESP.getFreeHeap()) / float(ESP.getHeapSize())) * 100);
        (*status_report_doc)["mac"] = WiFi.macAddress().c_str();
        (*status_report_doc)["state"] = "updating_fw";
        (*status_report_doc)["progress"] = WebManager::getUpdateProgress();

        char buffer[512];
        uint json_length = serializeJson(*status_report_doc, buffer);
        delete status_report_doc;
        MqttManager::publish(NSPMConfig::instance->mqtt_panel_status_topic, buffer);
        lastStatusReport = millis();
      } else if (WiFi.isConnected() && MqttManager::connected() && WebManager::getState() == WebManagerState::UPDATING_LITTLEFS) {
        DynamicJsonDocument *status_report_doc = new DynamicJsonDocument(512);
        (*status_report_doc)["rssi"] = WiFi.RSSI();
        (*status_report_doc)["heap_used_pct"] = round((float(ESP.getFreeHeap()) / float(ESP.getHeapSize())) * 100);
        (*status_report_doc)["mac"] = WiFi.macAddress().c_str();
        (*status_report_doc)["state"] = "updating_fs";
        (*status_report_doc)["progress"] = WebManager::getUpdateProgress();

        char buffer[512];
        uint json_length = serializeJson(*status_report_doc, buffer);
        delete status_report_doc;
        MqttManager::publish(NSPMConfig::instance->mqtt_panel_status_topic, buffer);
        lastStatusReport = millis();
      } else if (WiFi.isConnected() && MqttManager::connected() && InterfaceManager::hasRegisteredToManager && lastStatusReport + 30000 <= millis()) {
        // Report state every 30 seconds
        DynamicJsonDocument *status_report_doc = new DynamicJsonDocument(512);
        (*status_report_doc)["rssi"] = WiFi.RSSI();
        (*status_report_doc)["heap_used_pct"] = round((float(ESP.getFreeHeap()) / float(ESP.getHeapSize())) * 100);
        (*status_report_doc)["mac"] = WiFi.macAddress().c_str();
        (*status_report_doc)["state"] = "online";

        char buffer[512];
        uint json_length = serializeJson(*status_report_doc, buffer);
        delete status_report_doc;
        MqttManager::publish(NSPMConfig::instance->mqtt_panel_status_topic, buffer);
        lastStatusReport = millis();
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } else {
    Serial.println("No WiFi configuration exists. Starting AP!");
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    if (WiFi.softAPConfig(local_ip, gateway, subnet)) {
      Serial.println("Soft-AP configuration applied.");
      if (WiFi.softAP("NSPMPanel", "password")) {
        Serial.println("Soft-AP started.");

        Serial.println("WiFi SSID: NSPMPanel");
        Serial.println("WiFi PSK : password");
        Serial.print("WiFi IP Address: "); Serial.println(WiFi.softAPIP().toString().c_str());
        webMan.init(NSPanelManagerFirmwareVersion);
        // Wait indefinitly
        for (;;) {
          vTaskDelay(portMAX_DELAY);
        }
      } else {
        LOG_ERROR("Failed to start Soft-AP!");
      }
    } else {
      LOG_ERROR("Failed to apply Soft-AP configuration!");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Load config if any, and if it fails. Factory reset!
  if (!(config.init() && config.loadFromLittleFS())) {
    config.factoryReset();
  }

  // Setup logging
  logger.init(&(NSPMConfig::instance->mqtt_log_topic));
  logger.setLogLevel(static_cast<MqttLogLevel>(config.logging_level));
  // logger.setLogLevel(MqttLogLevel::Debug);
  //
  mqttManager.init();

  LOG_INFO("Starting tasks");
  Serial.println("Starting wifi and mqtt task.");
  xTaskCreatePinnedToCore(taskManageWifiAndMqtt, "taskManageWifi", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);

  nspanel.init();
  interfaceManager.init();
}

void loop() { vTaskDelay(portMAX_DELAY); }
