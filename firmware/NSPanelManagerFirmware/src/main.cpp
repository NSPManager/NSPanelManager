#include "esp32-hal.h"
#include "freertos/portmacro.h"
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
#include <TftDefines.h>
#include <WarningManager.hpp>
#include <WebManager.hpp>
#include <WiFi.h>
#include <cmath>
#include <math.h>
#include <nspm-bin-version.h>
#include <protobuf_nspanel.pb-c.h>
#include <string>

NSPanel nspanel;
InterfaceManager interfaceManager;
MqttLog logger;
NSPMConfig config;
WebManager webMan;
TaskHandle_t _taskWifiAndMqttManager;
WiFiClient espClient;
MqttManager mqttManager;

unsigned long lastRegistrationRequest = 0;
unsigned long lastStatusReport = 0;
unsigned long lastWiFiconnected = 0;
uint8_t lastUpdateProgress = 0;
// Temperature sensing variables:
#define READ_TEMP_SENSE_DELAY_MS 1000
#define NUM_TEMP_SENSE_AVERAGE 30
float averageTemperature = 0;
uint8_t numberOfTempSamples = 0;
uint8_t temperatureSlotNumber = 0;
float tempSensorReadings[NUM_TEMP_SENSE_AVERAGE];

void readNTCTemperatureTask(void *param) {
  for (;;) {
    float temperature = analogRead(38);
    temperature = temperature * 3.3 / 4095.0;
    temperature = 11200 * temperature / (3.3 - temperature);
    temperature = 1 / (1 / 298.15 + log(temperature / 10000) / 3950);
    temperature = temperature - 273.15; // Celsius

    if (NSPMConfig::instance->use_fahrenheit) {
      temperature = (temperature * 9 / 5) + 32;
    }
    tempSensorReadings[temperatureSlotNumber] = temperature + NSPMConfig::instance->temperature_calibration;
    temperatureSlotNumber++;
    if (temperatureSlotNumber >= NUM_TEMP_SENSE_AVERAGE) {
      temperatureSlotNumber = 0;
    }
    if (numberOfTempSamples < temperatureSlotNumber) {
      numberOfTempSamples = temperatureSlotNumber;
    }

    float tempTotal = 0;
    for (int i = 0; i < numberOfTempSamples; i++) {
      tempTotal += tempSensorReadings[i];
    }
    int averageTempInt = (tempTotal / numberOfTempSamples) * 10;
    averageTemperature = float(std::round(averageTempInt)) / 10;
    vTaskDelay(READ_TEMP_SENSE_DELAY_MS / portTICK_PERIOD_MS);
  }
}

void sendMqttManagerRegistrationRequest() {
  // Only send registration requests with a 3 second interval.
  if (MqttManager::connected() && millis() >= lastRegistrationRequest + 3000) {
    LOG_DEBUG("Sending MQTTManager register request.");
    JsonDocument doc;
    doc["command"] = "register_request";
    doc["mac_origin"] = WiFi.macAddress();
    doc["friendly_name"] = NSPMConfig::instance->wifi_hostname.c_str();
    doc["version"] = NSPanelManagerFirmwareVersion;
    doc["md5_firmware"] = NSPMConfig::instance->md5_firmware;
    doc["md5_data_file"] = NSPMConfig::instance->md5_data_file;
    doc["md5_tft_file"] = NSPMConfig::instance->md5_tft_file;
    doc["address"] = WiFi.localIP();

    char buffer[512];
    serializeJson(doc, buffer);
    MqttManager::publish("nspanel/mqttmanager/command", buffer);
    lastRegistrationRequest = millis();
  }
}

void onWiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
  case ARDUINO_EVENT_WIFI_READY:
    Serial.println("WiFi interface ready");
    break;
  case ARDUINO_EVENT_WIFI_SCAN_DONE:
    Serial.println("Completed scan for access points");
    break;
  case ARDUINO_EVENT_WIFI_STA_START:
    Serial.println("WiFi client started");
    break;
  case ARDUINO_EVENT_WIFI_STA_STOP:
    Serial.println("WiFi clients stopped");
    break;
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    Serial.println("Connected to access point");
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    Serial.println("Disconnected from WiFi access point");
    break;
  case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
    Serial.println("Authentication mode of access point has changed");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    Serial.print("Obtained IP address: ");
    Serial.println(WiFi.localIP());
    break;
  case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    Serial.println("Lost IP address and IP address is reset to 0");
    break;
  case ARDUINO_EVENT_WPS_ER_SUCCESS:
    Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_FAILED:
    Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_TIMEOUT:
    Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_PIN:
    Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
    break;
  case ARDUINO_EVENT_WIFI_AP_START:
    Serial.println("WiFi access point started");
    break;
  case ARDUINO_EVENT_WIFI_AP_STOP:
    Serial.println("WiFi access point  stopped");
    break;
  case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
    Serial.println("Client connected");
    break;
  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
    Serial.println("Client disconnected");
    break;
  case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
    Serial.println("Assigned IP address to client");
    break;
  case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
    Serial.println("Received probe request");
    break;
  case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
    Serial.println("AP IPv6 is preferred");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    Serial.println("STA IPv6 is preferred");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP6:
    Serial.println("Ethernet IPv6 is preferred");
    break;
  case ARDUINO_EVENT_ETH_START:
    Serial.println("Ethernet started");
    break;
  case ARDUINO_EVENT_ETH_STOP:
    Serial.println("Ethernet stopped");
    break;
  case ARDUINO_EVENT_ETH_CONNECTED:
    Serial.println("Ethernet connected");
    break;
  case ARDUINO_EVENT_ETH_DISCONNECTED:
    Serial.println("Ethernet disconnected");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP:
    Serial.println("Obtained IP address");
    break;
  default:
    Serial.println("Unknown WiFi event happened!");
    break;
  }
}

void startAndManageWiFiAccessPoint() {
  for (;;) {
    Serial.println("Setting Wifi mode to AP.");
    WiFi.mode(WIFI_AP);
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    Serial.println("Starting AP!");
    if (WiFi.softAP("NSPMPanel", NULL)) {
      Serial.println("Soft-AP started.");
      if (WiFi.softAPConfig(local_ip, local_ip, subnet)) {
        Serial.println("Soft-AP configuration applied.");

        Serial.println("WiFi SSID: NSPMPanel");
        Serial.println("No WiFi password.");
        Serial.print("WiFi IP Address: 192.168.1.1");

        vTaskDelay(500 / portTICK_PERIOD_MS);
        webMan.init(NSPanelManagerFirmwareVersion);
        // Wait indefinitely
        if (!NSPMConfig::instance->wifi_ssid.empty()) {
          for (;;) {
            vTaskDelay(60000 / portTICK_PERIOD_MS); // Scan for the configured network every 60 seconds.
            Serial.println("Scanning for configured wifi.");

            int n = WiFi.scanComplete();
            if (n == -2) {
              WiFi.scanNetworks(true);
            } else if (n) {
              for (int i = 0; i < n; ++i) {
                if (!WiFi.SSID(i).isEmpty()) {
                  if (WiFi.SSID(i).equals(config.wifi_ssid.c_str())) {
                    Serial.println("Found configured WiFi. Will restart and try to connect.");
                    vTaskDelay(250 / portTICK_PERIOD_MS);
                    ESP.restart(); // The configured network was discovered. Reboot.
                  }
                }
              }
            }
            WiFi.scanDelete();
          }
        } else {
          // Wait indefinitely
          for (;;) {
            vTaskDelay(portMAX_DELAY);
          }
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
  WiFi.onEvent(onWiFiEvent);

  if (!NSPMConfig::instance->wifi_ssid.empty()) {
    for (;;) {
      if (!WiFi.isConnected() && millis() - lastWiFiconnected < 180 * 1000) {
        LOG_ERROR("WiFi not connected!");
        Serial.println("WiFi not connected!");
        WiFi.setHostname(config.wifi_hostname.c_str());
        WiFi.begin(config.wifi_ssid.c_str(), config.wifi_psk.c_str());
        WiFi.setAutoConnect(true);
        WiFi.mode(WIFI_STA);
        for (uint8_t wifi_connect_tries = 0; wifi_connect_tries < 10; wifi_connect_tries++) {
          Serial.print("Connecting to WiFi ");
          Serial.println(config.wifi_ssid.c_str());
          if (WiFi.isConnected()) {
            Serial.println("Connected to WiFi!");
            LOG_INFO("Connected to WiFi ", config.wifi_ssid.c_str());
            Serial.print("Connected to WiFi ");
            Serial.println(config.wifi_ssid.c_str());
            LOG_INFO("IP Address: ", WiFi.localIP().toString());
            LOG_INFO("Netmask:    ", WiFi.subnetMask().toString());
            LOG_INFO("Gateway:    ", WiFi.gatewayIP().toString());

            // We successfully connected to WiFi. Init the rest of the components.
            webMan.init(NSPanelManagerFirmwareVersion);
            mqttManager.start();
            break; // We successfully connected to WiFi.
          } else {
            LOG_ERROR("Failed to connect to WiFi. Will try again.");
            // Serial.println("Failed to connect to WiFi. Will try again.");
          }
          vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
      } else if (!config.wifi_ssid.empty() && !WiFi.isConnected() && millis() - lastWiFiconnected >= 180 * 1000) {
        // Three minutes or more has passed since last successfull WiFi connection. Start the AP by breaking the loop.
        startAndManageWiFiAccessPoint();
      }
      if (WiFi.isConnected() && MqttManager::connected()) {
        if (!InterfaceManager::hasRegisteredToManager) {
          sendMqttManagerRegistrationRequest();
        }
        bool force_send_mqtt_update = false;
        // NSPanelStatusReport report = NSPanelStatusReport_init_zero;
        NSPanelStatusReport report = NSPANEL_STATUS_REPORT__INIT;

        if (NSPanel::instance->getUpdateState()) {
          force_send_mqtt_update = true;
          report.nspanel_state = NSPanelStatusReport__State::NSPANEL_STATUS_REPORT__STATE__UPDATING_TFT;
          report.update_progress = NSPanel::instance->getUpdateProgress();
        } else if (WebManager::getState() == WebManagerState::UPDATING_FIRMWARE) {
          force_send_mqtt_update = true;
          report.nspanel_state = NSPanelStatusReport__State::NSPANEL_STATUS_REPORT__STATE__UPDATING_FIRMWARE;
          report.update_progress = WebManager::getUpdateProgress();
        } else if (WebManager::getState() == WebManagerState::UPDATING_LITTLEFS) {
          force_send_mqtt_update = true;
          report.nspanel_state = NSPanelStatusReport__State::NSPANEL_STATUS_REPORT__STATE__UPDATING_LITTLEFS;
          report.update_progress = WebManager::getUpdateProgress();
        } else {
          report.nspanel_state = NSPanelStatusReport__State::NSPANEL_STATUS_REPORT__STATE__ONLINE;
          report.update_progress = 0;
        }

        if ((force_send_mqtt_update && lastUpdateProgress != report.update_progress) || millis() >= lastStatusReport + 30000) {
          char display_temp[7]; // Displayed temperature should NEVER have to be more than 7 chars. Example, 1000.0 is 6 chars, -100.0 is 6 chars. Neither should happen!
          std::snprintf(display_temp, sizeof(display_temp), "%.1f", averageTemperature);
          std::string ip_address = WiFi.localIP().toString().c_str();
          std::string mac_address = WiFi.macAddress().c_str();

          report.rssi = WiFi.RSSI();
          report.heap_used_pct = round((float(ESP.getFreeHeap()) / float(ESP.getHeapSize())) * 100);
          report.ip_address = (char *)ip_address.c_str();
          report.mac_address = (char *)mac_address.c_str();
          report.temperature = display_temp;

          std::vector<NSPanelWarning> warnings = WarningManager::get_warnings();
          report.n_warnings = warnings.size();
          report.warnings = (NSPanelWarning **)malloc(sizeof(NSPanelWarning *) * warnings.size());
          for (int i = 0; i < warnings.size(); i++) {
            report.warnings[i] = &(warnings[i]);
          }
          LOG_TRACE("Loaded ", warnings.size(), " warnings.");

          size_t pack_length = nspanel_status_report__get_packed_size(&report);
          uint8_t buffer[pack_length];
          size_t pack_size = nspanel_status_report__pack(&report, buffer);

          std::string full_buffer = std::string(buffer, buffer + pack_size);
          MqttManager::publish(NSPMConfig::instance->mqtt_panel_status_topic, full_buffer, true);
          // Cleanup buffers
          free(report.warnings);
          warnings.clear();

          // Send temperature update
          MqttManager::publish(NSPMConfig::instance->mqtt_panel_temperature_topic, display_temp);

          // std::string display_temp = std::to_string((int)round(temperature));
          std::string display_temp_display = display_temp;
          display_temp_display.append("°");
          PageManager::GetScreensaverPage()->updateRoomTemp(display_temp_display);

          lastStatusReport = millis();
        }
      }

      if (WiFi.isConnected()) {
        lastWiFiconnected = millis();
      }
      yield();
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } else {
    // No WiFi SSID configured.
    startAndManageWiFiAccessPoint();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print("Starting NSPanel Manager firmware v");
  Serial.println(NSPanelManagerFirmwareVersion);
  vTaskDelay(250 / portTICK_PERIOD_MS);
  // Load config if any, and if it fails. Factory reset!
  if (!(config.init() && config.loadFromLittleFS())) {
    config.factoryReset();
  }

  // Setup logging
  logger.init(&(NSPMConfig::instance->mqtt_log_topic));
  logger.setLogLevel(static_cast<MqttLogLevel>(config.logging_level));

  mqttManager.init();
  ButtonManager::init();

  vTaskDelay(250 / portTICK_PERIOD_MS);
  LOG_INFO("Initializing NSPanel communication");
  if (nspanel.init()) {
    LOG_INFO("Successfully initiated NSPanel.");
  } else {
    LOG_ERROR("Failed to initiate NSPanel.");
  }
  vTaskDelay(500 / portTICK_PERIOD_MS);
  PageManager::init(); // Attach event callbacks

  LOG_INFO("Starting tasks");
  interfaceManager.init();
  xTaskCreatePinnedToCore(readNTCTemperatureTask, "readTempTask", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);

  pinMode(38, INPUT);
}

void loop() {
  taskManageWifiAndMqtt(NULL);
}
