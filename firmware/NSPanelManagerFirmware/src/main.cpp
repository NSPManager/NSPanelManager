#include <Arduino.h>
#include <ButtonManager.hpp>
#include <HTTPClient.h>
#include <InterfaceManager.hpp>
#include <LittleFS.h>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <PubSubClient.h>
#include <WebManager.hpp>
#include <WiFi.h>
#include <nspm-bin-version.h>
#include <string>

NSPanel nspanel;
InterfaceManager interfaceManager;
MqttLog logger;
NSPMConfig config;
WebManager webMan;
TaskHandle_t _taskWifiAndMqttManager;
WiFiClient espClient;
MqttManager mqttManager;

unsigned long lastStatusReport = 0;
unsigned long lastWiFiconnected = 0;

float readNTCTemperature(bool farenheit) {
  float temperature = analogRead(38);
  if (temperature > 0) {
    temperature = temperature * 3.3 / 4095.0;
    temperature = 11200 * temperature / (3.3 - temperature);
    temperature = 1 / (1 / 298.15 + log(temperature / 10000) / 3950);
    temperature = temperature - 273.15; // Celsius
    if (farenheit) {
      temperature = (temperature * 9 / 5) + 32;
    }
    return temperature;
  }
  return -254;
}

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

      StaticJsonDocument<512> doc;
      doc["mac_address"] = WiFi.macAddress().c_str();
      doc["friendly_name"] = NSPMConfig::instance->wifi_hostname.c_str();
      doc["version"] = NSPanelManagerFirmwareVersion;
      doc["md5_firmware"] = NSPMConfig::instance->md5_firmware;
      doc["md5_data_file"] = NSPMConfig::instance->md5_data_file;
      doc["md5_tft_file"] = NSPMConfig::instance->md5_tft_file;

      char buffer[512];
      serializeJson(doc, buffer);

      httpClient.begin(wifiClient, url.c_str());
      httpClient.addHeader("Content-Type", "application/json");
      int responseCode = httpClient.POST(buffer);

      if (responseCode == 200) {
        InterfaceManager::hasRegisteredToManager = true;
        LOG_INFO("Registered to manager at: ", url.c_str());
        break;
      } else {
        InterfaceManager::hasRegisteredToManager = false;
        LOG_ERROR("Failed to register panel at: ", url.c_str(), ". Will try again in 5 seconds.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
      }
    }
  }
}

void startAndManageWiFiAccessPoint() {
  for (;;) {
    Serial.println("Starting AP!");
    WiFi.mode(WIFI_MODE_AP);
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    if (WiFi.softAPConfig(local_ip, gateway, subnet)) {
      Serial.println("Soft-AP configuration applied.");
      if (WiFi.softAP("NSPMPanel", "password")) {
        Serial.println("Soft-AP started.");

        Serial.println("WiFi SSID: NSPMPanel");
        Serial.println("WiFi PSK : password");
        Serial.print("WiFi IP Address: ");
        Serial.println(WiFi.softAPIP().toString().c_str());
        webMan.init(NSPanelManagerFirmwareVersion);
        // Wait indefinitly
        for (;;) {
          vTaskDelay(60000 / portTICK_PERIOD_MS); // Scan for the configured network every 60 seconds.

          int n = WiFi.scanComplete();
          if (n == -2) {
            WiFi.scanNetworks(true);
          } else if (n) {
            for (int i = 0; i < n; ++i) {
              if (!WiFi.SSID(i).isEmpty()) {
                if (WiFi.SSID(i).equals(config.wifi_ssid.c_str())) {
                  ESP.restart(); // The configured network was discovered. Reboot.
                }
              }
            }
          }
          WiFi.scanDelete();
        }
      } else {
        LOG_ERROR("Failed to start Soft-AP!");
      }
    } else {
      LOG_ERROR("Failed to apply Soft-AP configuration!");
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void taskManageWifiAndMqtt(void *param) {
  LOG_INFO("taskWiFiMqttHandler started!");
  Serial.print("Configured to connect to WiFi: ");
  Serial.println(config.wifi_ssid.c_str());
  Serial.print("with hostname: ");
  Serial.println(config.wifi_hostname.c_str());
  if (!NSPMConfig::instance->wifi_ssid.empty()) {
    for (;;) {
      if (!WiFi.isConnected() && millis() - lastWiFiconnected < 180 * 1000) {
        LOG_ERROR("WiFi not connected!");
        Serial.println("WiFi not connected!");
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(config.wifi_hostname.c_str());
        for (uint8_t wifi_connect_tries = 0; wifi_connect_tries < 10 && !WiFi.isConnected(); wifi_connect_tries++) {
          Serial.print("Connecting to WiFi ");
          Serial.println(config.wifi_ssid.c_str());
          WiFi.begin(config.wifi_ssid.c_str(), config.wifi_psk.c_str());
          vTaskDelay(2000 / portTICK_PERIOD_MS);
          if (WiFi.isConnected()) {
            Serial.println("Connected to WiFi!");
            LOG_INFO("Connected to WiFi ", config.wifi_ssid.c_str());
            Serial.print("Connected to WiFi ");
            Serial.println(config.wifi_ssid.c_str());
            LOG_INFO("IP Address: ", WiFi.localIP().toString());
            LOG_INFO("Netmask:    ", WiFi.subnetMask().toString());
            LOG_INFO("Gateway:    ", WiFi.gatewayIP().toString());
            // Start web server
            webMan.init(NSPanelManagerFirmwareVersion);
            registerToNSPanelManager();
          } else {
            LOG_ERROR("Failed to connect to WiFi. Will try again in 5 seconds");
            Serial.println("Failed to connect to WiFi. Will try again in 5 seconds");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
          }
        }
      } else if (!config.wifi_ssid.empty() && !WiFi.isConnected() && millis() - lastWiFiconnected >= 180 * 1000) {
        // Three minutes or more has passed since last successfull WiFi connection. Start the AP by breaking the loop.
        startAndManageWiFiAccessPoint();
      }
      if (WiFi.isConnected() && MqttManager::connected()) {
        DynamicJsonDocument *status_report_doc = new DynamicJsonDocument(512);
        if (NSPanel::instance->getUpdateState()) {
          (*status_report_doc)["state"] = "updating_tft";
          (*status_report_doc)["progress"] = NSPanel::instance->getUpdateProgress();
        } else if (WebManager::getState() == WebManagerState::UPDATING_FIRMWARE) {
          (*status_report_doc)["state"] = "updating_fw";
          (*status_report_doc)["progress"] = WebManager::getUpdateProgress();
        } else if (WebManager::getState() == WebManagerState::UPDATING_LITTLEFS) {
          (*status_report_doc)["state"] = "updating_fs";
          (*status_report_doc)["progress"] = WebManager::getUpdateProgress();
        } else {
          (*status_report_doc)["state"] = "online";
        }

        if ((*status_report_doc)["state"] != "online" || ((*status_report_doc)["state"] == "online" && millis() >= lastStatusReport + 30000)) {
          float temperature = readNTCTemperature(NSPMConfig::instance->use_farenheit);
          (*status_report_doc)["rssi"] = WiFi.RSSI();
          (*status_report_doc)["heap_used_pct"] = round((float(ESP.getFreeHeap()) / float(ESP.getHeapSize())) * 100);
          (*status_report_doc)["mac"] = WiFi.macAddress();
          (*status_report_doc)["temperature"] = temperature;
          MqttManager::publish(NSPMConfig::instance->mqtt_panel_temperature_topic, std::to_string(temperature).c_str());

          char buffer[512];
          uint json_length = serializeJson(*status_report_doc, buffer);
          MqttManager::publish(NSPMConfig::instance->mqtt_panel_status_topic, buffer);
          lastStatusReport = millis();
        }
        delete status_report_doc;
      }

      if (WiFi.isConnected()) {
        lastWiFiconnected = millis();
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } else {
    // No WiFi SSID configured.
    startAndManageWiFiAccessPoint();
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

  mqttManager.init();

  LOG_INFO("Initializing NSPanel communication");
  if (nspanel.init()) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    PageManager::init(); // Attach event callbacks

    LOG_INFO("Starting tasks");
    interfaceManager.init();
    ButtonManager::init();
  } else {
    LOG_ERROR("Failed to initialize NSPanel");
  }

  pinMode(38, INPUT);
}

void loop() {
  taskManageWifiAndMqtt(NULL);
}
