#include <Arduino.h>
#include <ArduinoJson.h>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

void MqttManager::init() {
  MqttManager::_wifiClient = new WiFiClient();                            // Create WifiClient for MQTT Client
  MqttManager::_mqttClient = new PubSubClient(*MqttManager::_wifiClient); // Create MQTT Client used to communicate with MQTT
  MqttManager::_mqttClient->setBufferSize(2048);
  MqttManager::_mqttClient->setCallback(&MqttManager::_mqttClientCallback);
  if (MqttLog::instance->getLogLevel() == MqttLogLevel::Debug) {
    MqttManager::setBufferSize(32);
  } else {
    MqttManager::setBufferSize(8);
  }
  xTaskCreatePinnedToCore(MqttManager::_taskMqttRunTask, "taskRunMQTT", 50000, NULL, 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void MqttManager::setBufferSize(uint8_t size) {
  // WARNING! Calling this function will create a new buffer so all messages
  // currently in the buffer will be lost.
  MqttManager::_sendQueue = xQueueCreate(size, sizeof(PublishMessage *));

  if (MqttManager::_sendQueue == NULL) {
    Serial.println("Failed to create queue!");
  } else {
    Serial.print("Created _sendQueue of size: ");
    Serial.print(size);
    Serial.print(" * ");
    Serial.println(sizeof(PublishMessage));
  }
}

bool MqttManager::connected() {
  return MqttManager::_mqttClient->connected();
}

bool MqttManager::publish(const char *topic, std::string &message) {
  std::string str_topic = std::string(topic);
  return MqttManager::publish(str_topic, message);
}

bool MqttManager::publish(const char *topic, std::string &message, bool retain) {
  std::string str_topic = std::string(topic);
  return MqttManager::publish(str_topic, message, retain);
}

bool MqttManager::publish(std::string &topic, const char *message) {
  std::string str_message = std::string(message);
  return MqttManager::publish(topic, str_message);
}

bool MqttManager::publish(std::string &topic, const char *message, bool retain) {
  std::string str_message = std::string(message);
  return MqttManager::publish(topic, str_message, retain);
}

// Send message without retain
bool MqttManager::publish(std::string &topic, std::string &message) {
  return MqttManager::publish(topic, message, false);
}

// This is the only function that addes the message to the queue
bool MqttManager::publish(std::string &topic, std::string &message, bool retain) {
  // Verify that a queue exists, else stop processing
  if (MqttManager::_sendQueue == NULL) {
    Serial.println("ERROR: No MQTT send queue!");
    return false;
  }

  PublishMessage *msg = new PublishMessage;
  msg->topic = topic;
  msg->data = message;
  msg->retain = retain;

  if (xQueueSendToBack(MqttManager::_sendQueue, (void *)&msg, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    return true;
  } else {
    delete msg;
    return false;
  }
}

bool MqttManager::publish(const char *topic, const char *message) {
  return MqttManager::publish(topic, message, false);
}

bool MqttManager::publish(const char *topic, const char *message, bool retain) {
  std::string str_topic = std::string(topic);
  std::string str_message = std::string(message);
  return MqttManager::publish(str_topic, str_message, retain);
}

void MqttManager::subscribeToTopic(const char *topic, std::function<void(char *topic, byte *payload, unsigned int length)> callback) {
  LOG_TRACE("Adding topic ", topic, " to subscribe list.");
  SubscribeTopic tpc;
  tpc.topic = std::string(topic);
  tpc.callback = callback;
  MqttManager::subscribeToTopic(tpc);
}

void MqttManager::subscribeToTopic(SubscribeTopic &topic) {
  // Add to list of topics so that we know what callback to use
  // and also what to re-subscribe to if we reconnect to MQTT
  MqttManager::_subscribeTopics.push_back(topic);
  // Do the actual subscription to the topic
  MqttManager::_subscribeToTopic(topic);
}

void MqttManager::_subscribeToTopic(SubscribeTopic &topic) {
  if (MqttManager::_mqttClient->connected()) {
    LOG_TRACE("Subscribing to topic ", topic.topic.c_str());
    MqttManager::_mqttClient->subscribe(topic.topic.c_str());
    MqttManager::_mqttClient->loop();
  }
}

void MqttManager::_subscribeToAllRegisteredTopics() {
  for (SubscribeTopic &topic : MqttManager::_subscribeTopics) {
    MqttManager::_subscribeToTopic(topic);
    vTaskDelay(50 / portTICK_PERIOD_MS); // Wait 50ms between each subscribe to allow for processing of any retained message in the topic
  }
}

void MqttManager::_mqttClientCallback(char *topic, byte *payload, unsigned int length) {
  for (SubscribeTopic &tpc : MqttManager::_subscribeTopics) {
    if (tpc.topic.compare(topic) == 0) {
      tpc.callback(topic, payload, length);
      break;
    }
  }
}

void MqttManager::_sendMqttMessage() {
  // Verify that a queue exists, else stop processing
  if (MqttManager::_sendQueue == NULL) {
    Serial.println("ERROR: No MQTT send queue!");
    return;
  }

  if (MqttManager::_mqttClient->connected()) {
    if (uxQueueMessagesWaiting(MqttManager::_sendQueue) > 0) {
      PublishMessage *msg;
      if (xQueuePeek(MqttManager::_sendQueue, &(msg), 5 / portTICK_PERIOD_MS)) {
        if (MqttManager::_mqttClient->publish(msg->topic.c_str(), msg->data.c_str(), msg->retain)) {
          // Message was successfully sent, pop item from queue
          if (xQueueReceive(MqttManager::_sendQueue, &(msg), 100 / portTICK_PERIOD_MS)) {
            delete msg;
          }
        }
      }
    }
  }
}

void MqttManager::_taskMqttRunTask(void *param) {
  if (NSPMConfig::instance->mqtt_server.empty() || NSPMConfig::instance->mqtt_port == 0) { // No server configured, stop processing
    Serial.println("No MQTT server configured, will not start MQTT task.");
    vTaskDelete(NULL);
  }

  for (;;) {
    if (MqttManager::connected()) {
      MqttManager::_sendMqttMessage();
      MqttManager::_mqttClient->loop();
      vTaskDelay(25 / portTICK_PERIOD_MS); // Wait 25ms between each tick of MQTT client
    } else {
      if (MqttManager::_connect()) {
        MqttManager::_subscribeToAllRegisteredTopics();
      } else {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
    }
  }
}

bool MqttManager::_connect() {
  // Stop processing if not connected to WiFi
  if (!WiFi.isConnected()) {
    return false;
  }

  // Build "offline" message used for last will
  JsonDocument *offline_message_doc = new JsonDocument;
  (*offline_message_doc)["mac"] = WiFi.macAddress().c_str();
  (*offline_message_doc)["state"] = "offline";
  char offline_message_buffer[512];
  serializeJson(*offline_message_doc, offline_message_buffer);
  delete offline_message_doc;

  // Build "online" message used for last will
  JsonDocument *online_message_doc = new JsonDocument;
  (*online_message_doc)["mac"] = WiFi.macAddress().c_str();
  (*online_message_doc)["state"] = "online";
  char online_message_buffer[512];
  serializeJson(*online_message_doc, online_message_buffer);
  delete online_message_doc;

  std::string mqtt_device_name = "NSPM_";
  mqtt_device_name.append(NSPMConfig::instance->wifi_hostname);
  mqtt_device_name.append(WiFi.macAddress().c_str());

  LOG_INFO("Connecting to MQTT server ", NSPMConfig::instance->mqtt_server.c_str());
  Serial.print("Connecting to MQTT server ");
  MqttManager::_mqttClient->setSocketTimeout(5); // Set tighter timeout. Default: 15 seconds.
  Serial.println(NSPMConfig::instance->mqtt_server.c_str());
  MqttManager::_mqttClient->setServer(NSPMConfig::instance->mqtt_server.c_str(), NSPMConfig::instance->mqtt_port);
  MqttManager::_mqttClient->connect(mqtt_device_name.c_str(), NSPMConfig::instance->mqtt_username.c_str(), NSPMConfig::instance->mqtt_password.c_str(), NSPMConfig::instance->mqtt_availability_topic.c_str(), 1, true, offline_message_buffer);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  if (MqttManager::_mqttClient->connected()) {
    MqttManager::_mqttClient->publish(NSPMConfig::instance->mqtt_availability_topic.c_str(), online_message_buffer, true);
    Serial.print("Connected to MQTT server ");
    Serial.println(NSPMConfig::instance->mqtt_server.c_str());
    LOG_INFO("Connected to MQTT server ", NSPMConfig::instance->mqtt_server.c_str());
    return true;
  } else {
    LOG_ERROR("Failed to connect to MQTT. Will try again in 1 seconds");
    Serial.println("Failed to connect to MQTT. Will try again in 1 seconds");
    return false;
  }
}
