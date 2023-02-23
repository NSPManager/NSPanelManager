#include <Arduino.h>
#include <ArduinoJson.h>
#include <nspm-bin-version.h>
#include <string>
#include <LittleFS.h>
#include <NSPMConfig.h>
#include <ArduLog.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <WebManager.h>

NSPMConfig config;
ArduLog logger;
WebManager webMan;
TaskHandle_t _taskWifiAndMqttManager;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void registerToNSPanelManager()
{
  if (WiFi.isConnected())
  {
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

    char buffer[128];
    serializeJson(doc, buffer);

    httpClient.begin(wifiClient, url.c_str());
    httpClient.addHeader("Content-Type", "application/json");
    int responseCode = httpClient.POST(buffer);

    LOG_INFO("Registration at manager got response code: ", responseCode);
  }
}

void taskManageWifiAndMqtt(void *param)
{
  LOG_INFO("taskWiFiMqttHandler started!");
  if (!NSPMConfig::instance->wifi_ssid.empty())
  {
    for (;;)
    {
      if (!WiFi.isConnected() && !config.wifi_ssid.empty())
      {
        LOG_ERROR("WiFi not connected!");
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(config.wifi_hostname.c_str());
        while (!WiFi.isConnected())
        {
          WiFi.begin(config.wifi_ssid.c_str(), config.wifi_psk.c_str());
          LOG_INFO("Connecting to WiFi ", LOG_BOLD, config.wifi_ssid.c_str());
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          if (WiFi.isConnected())
          {
            LOG_INFO("Connected to WiFi ", LOG_BOLD, config.wifi_ssid.c_str());
            LOG_INFO("IP Address: ", LOG_BOLD, WiFi.localIP());
            LOG_INFO("Netmask:    ", LOG_BOLD, WiFi.subnetMask());
            LOG_INFO("Gateway:    ", LOG_BOLD, WiFi.gatewayIP());
            // Start web server
            webMan.init(NSPanelManagerFirmwareVersion);
            registerToNSPanelManager();
          }
          else
          {
            LOG_ERROR("Failed to connect to WiFi. Will try again in 10 seconds");
          }
        }
      }
      else if (config.wifi_ssid.empty())
      {
        LOG_ERROR("No WiFi SSID configured!");
      }

      if (WiFi.isConnected() && !mqttClient.connected() && !config.mqtt_server.empty())
      {
        LOG_ERROR("MQTT not connected!");
        while (WiFi.isConnected() && !mqttClient.connected())
        {
          mqttClient.setServer(config.mqtt_server.c_str(), config.mqtt_port);
          // mqttClient.setCallback(mqttCallback);
          // TODO: MQTT Callback
          mqttClient.setBufferSize(2048);
          LOG_INFO("Connecting to MQTT server ", LOG_BOLD, config.mqtt_server.c_str());
          // mqttClient.connect(config.wifi_hostname.c_str(), config.mqtt_username.c_str(), config.mqtt_password.c_str());
          mqttClient.connect(config.wifi_hostname.c_str(), config.mqtt_username.c_str(), config.mqtt_password.c_str(), NSPMConfig::instance->mqtt_avalability_topic.c_str(), 1, 1, "offline");
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          if (mqttClient.connected())
          {
            LOG_INFO("Connected to MQTT server ", LOG_BOLD, config.mqtt_server.c_str());
            // TODO: Subscribe
          }
          else
          {
            LOG_ERROR("Failed to connect to MQTT. Will try again in 10 seconds");
          }
        }
      }
      else if (config.mqtt_server.empty())
      {
        LOG_ERROR("No MQTT server configured!");
      }
      vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
  }
  else
  {
    LOG_ERROR("No WiFi configuration exists. Starting AP!");
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    if (WiFi.softAPConfig(local_ip, gateway, subnet))
    {
      LOG_INFO("Soft-AP configuration applied.");
      if (WiFi.softAP("Light Controller", "password"))
      {
        LOG_INFO("Soft-AP started.");

        LOG_INFO("WiFi SSID: Light Controller");
        LOG_INFO("WiFi PSK : password");
        LOG_INFO("WiFi IP Address: ", WiFi.softAPIP().toString().c_str());
        webMan.init(NSPanelManagerFirmwareVersion);
        vTaskDelete(NULL); // This task is complete. Stop processing.
      }
      else
      {
        LOG_ERROR("Failed to start Soft-AP!");
      }
    }
    else
    {
      LOG_ERROR("Failed to apply Soft-AP configuration!");
    }
  }
}

void setup()
{
  // Setup logging
  Serial.begin(115200);
  logger.init();
  logger.SetSerial(&Serial);
  logger.SetLogLevel(ArduLogLevel::Trace);

  // Load config if any, and if it fails. Factory reset!
  if (!(config.init() && config.loadFromLittleFS()))
  {
    config.factoryReset();
  }

  LOG_INFO("Starting tasks");
  xTaskCreatePinnedToCore(taskManageWifiAndMqtt, "taskManageWifiAndMqtt", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void loop()
{
  mqttClient.loop();
  vTaskDelay(10 / portTICK_PERIOD_MS);
}