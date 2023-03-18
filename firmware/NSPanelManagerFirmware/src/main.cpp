#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <InterfaceManager.h>
#include <LittleFS.h>
#include <MqttLog.h>
#include <NSPMConfig.h>
#include <NSPanel.h>
#include <PubSubClient.h>
#include <WebManager.h>
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
    doc["version"] = NSPanelManagerFirmwareVersion;

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
          LOG_INFO("Connecting to WiFi ", config.wifi_ssid.c_str());
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          if (WiFi.isConnected())
          {
            LOG_INFO("Connected to WiFi ", config.wifi_ssid.c_str());
            LOG_INFO("IP Address: ", WiFi.localIP());
            LOG_INFO("Netmask:    ", WiFi.subnetMask());
            LOG_INFO("Gateway:    ", WiFi.gatewayIP());
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

      if (WiFi.isConnected() && !mqttClient.connected() &&
          !config.mqtt_server.empty())
      {
        LOG_ERROR("MQTT not connected!");
        while (WiFi.isConnected() && !mqttClient.connected())
        {
          mqttClient.setServer(config.mqtt_server.c_str(), config.mqtt_port);
          LOG_INFO("Connecting to MQTT server ", config.mqtt_server.c_str());
          mqttClient.connect(config.wifi_hostname.c_str(), config.mqtt_username.c_str(), config.mqtt_password.c_str(), NSPMConfig::instance->mqtt_availability_topic.c_str(), 1, 1, "offline");
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          if (mqttClient.connected())
          {
            // This task only handles connection. The InterfaceManager will take
            // care of subscribing to relevant topics once the connection is
            // MQTT is established.
            LOG_INFO("Connected to MQTT server ", config.mqtt_server.c_str());
            InterfaceManager::subscribeToMqttTopics();
            mqttClient.publish(NSPMConfig::instance->mqtt_availability_topic.c_str(), "online", true);
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
      if (WiFi.softAP("NSPMPanel", "password"))
      {
        LOG_INFO("Soft-AP started.");

        LOG_INFO("WiFi SSID: NSPMPanel");
        LOG_INFO("WiFi PSK : password");
        LOG_INFO("WiFi IP Address: ", WiFi.softAPIP().toString().c_str());
        webMan.init(NSPanelManagerFirmwareVersion);
        // Wait indefinitly
        for(;;) {
          vTaskDelay(portMAX_DELAY);
        }
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

void taskMqttLoop(void *param)
{
  for (;;)
  {
    mqttClient.loop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  mqttClient.setBufferSize(2048);

  // Load config if any, and if it fails. Factory reset!
  if (!(config.init() && config.loadFromLittleFS()))
  {
    config.factoryReset();
  }

  // Setup logging
  logger.init(&mqttClient, &(NSPMConfig::instance->mqtt_log_topic));
  logger.setLogLevel(static_cast<MqttLogLevel>(config.logging_level));
  // logger.setLogLevel(MqttLogLevel::Debug);

  LOG_INFO("Starting tasks");
  xTaskCreatePinnedToCore(taskManageWifiAndMqtt, "taskManageWifiAndMqtt", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(taskMqttLoop, "taskMqttLoop", 5000, NULL, 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);

  nspanel.init();
  interfaceManager.init(&mqttClient);
}

void loop() { vTaskDelay(portMAX_DELAY); }
