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
  MqttManager::setBufferSize(32);
}

void MqttManager::start() {
  if (!MqttManager::_hasStarted) {
    MqttManager::_wifiClient = new WiFiClient();                            // Create WifiClient for MQTT Client
    MqttManager::_mqttClient = new PubSubClient(*MqttManager::_wifiClient); // Create MQTT Client used to communicate with MQTT
    MqttManager::_mqttClient->setBufferSize(5000);
    MqttManager::_mqttClient->setCallback(&MqttManager::_mqttClientCallback);
    xTaskCreatePinnedToCore(MqttManager::_taskMqttRunTask, "taskRunMQTT", 10000, NULL, 2, &MqttManager::_taskMqttRunTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(MqttManager::_taskHandleMqttMessageReceiveQueue, "taskHandleMQTTRecvQueue", 10000, NULL, 1, &MqttManager::_taskHandleMqttMessageReceiveQueueHandle, CONFIG_ARDUINO_RUNNING_CORE);
  }
}

void MqttManager::setBufferSize(uint8_t size) {
  // WARNING! Calling this function will create a new buffer so all messages
  // currently in the buffer will be lost.
  MqttManager::_sendQueue = xQueueCreate(size, sizeof(MQTTMessage *));
  MqttManager::_receiveQueue = xQueueCreate(size, sizeof(MQTTMessage *));

  if (MqttManager::_sendQueue == NULL) {
    Serial.println("Failed to create send queue!");
  } else {
    Serial.print("Created _sendQueue of size: ");
    Serial.print(size);
    Serial.print(" * ");
    Serial.println(sizeof(MQTTMessage));
  }

  if (MqttManager::_receiveQueue == NULL) {
    Serial.println("Failed to create receive queue!");
  } else {
    Serial.print("Created _receiveQueue of size: ");
    Serial.print(size);
    Serial.print(" * ");
    Serial.println(sizeof(MQTTMessage));
  }
}

bool MqttManager::connected() {
  if (!MqttManager::_hasStarted) {
    return false;
  } else {
    return MqttManager::_mqttClient->connected();
  }
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

// This is the only function that adds the message to the queue
bool MqttManager::publish(std::string &topic, std::string &message, bool retain) {
  if (MqttManager::_hasStarted) {
    bool result = MqttManager::_mqttClient->publish(topic.c_str(), message.c_str(), retain);
    xTaskNotifyGive(MqttManager::_taskMqttRunTaskHandle);
    return result;
  }
  return false;

  // Verify that a queue exists, else stop processing
  if (MqttManager::_sendQueue == NULL) {
    Serial.println("ERROR: No MQTT send queue!");
    return false;
  }

  MQTTMessage *msg = new MQTTMessage;
  msg->topic = topic;
  msg->data = message;
  msg->retain = retain;

  if (xQueueSendToBack(MqttManager::_sendQueue, (void *)&msg, 250 / portTICK_PERIOD_MS) == pdTRUE) {
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

void MqttManager::subscribeToTopic(const char *topic, std::function<void(MQTTMessage *message)> callback) {
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
  if (MqttManager::connected()) {
    LOG_TRACE("Subscribing to topic ", topic.topic.c_str());
    MqttManager::_mqttClient->subscribe(topic.topic.c_str());
    xTaskNotifyGive(MqttManager::_taskMqttRunTaskHandle);
  }
}

void MqttManager::_subscribeToAllRegisteredTopics() {
  for (SubscribeTopic &topic : MqttManager::_subscribeTopics) {
    MqttManager::_subscribeToTopic(topic);
    vTaskDelay(200 / portTICK_PERIOD_MS); // Wait 50ms between each subscribe to allow for processing of any retained message in the topic
  }
}

void MqttManager::_mqttClientCallback(char *topic, byte *payload, unsigned int length) {
  MQTTMessage *received_message = new MQTTMessage{
      .topic = topic,
      .data = std::string((char *)payload, length),
      .retain = false,
  };
  if (xQueueSendToBack(MqttManager::_receiveQueue, (void *)&received_message, 250 / portTICK_PERIOD_MS) == pdTRUE) {
    xTaskNotifyGive(MqttManager::_taskHandleMqttMessageReceiveQueueHandle);
  } else {
    LOG_ERROR("Failed to add message to receive queue");
  }
}

void MqttManager::_taskHandleMqttMessageReceiveQueue(void *param) {
  for (;;) {
    MQTTMessage *message = nullptr;
    try {
      if (ulTaskNotifyTake(true, portMAX_DELAY) == pdTRUE) {
        if (xQueueReceive(MqttManager::_receiveQueue, &message, 100 / portTICK_PERIOD_MS)) {
          for (auto it = MqttManager::_subscribeTopics.begin(); it != MqttManager::_subscribeTopics.end(); it++) {
            if (it->topic.compare(message->topic) == 0) {
              it->callback(message);
            }
          }
          delete message; // Callback has completed. Delete stored message.
          message = nullptr;
        }
      } else {
        vTaskDelay(50 / portTICK_PERIOD_MS); // We failed to receive a message for whatever reason. Wait to not overload CPU of call itself failed.
      }
    } catch (std::exception &ex) {
      LOG_ERROR("Got exception while trying to process message in queue. Will delete message and continue with others.");
      LOG_ERROR("Error: ", ex.what());
      if (message != nullptr) {
        delete message;
      }
    }
  }
}

void MqttManager::_sendMqttMessage() {
  // Verify that a queue exists, else stop processing
  if (MqttManager::_sendQueue == NULL) {
    Serial.println("ERROR: No MQTT send queue!");
    return;
  }

  if (MqttManager::connected()) {
    if (uxQueueMessagesWaiting(MqttManager::_sendQueue) > 0) {
      MQTTMessage *msg;
      if (xQueuePeek(MqttManager::_sendQueue, &(msg), 5 / portTICK_PERIOD_MS)) {
        if (MqttManager::_mqttClient->publish(msg->topic.c_str(), (uint8_t *)msg->data.c_str(), msg->data.length(), msg->retain)) {
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

  MqttManager::_hasStarted = true;
  for (;;) {
    // Wait for notification to start processing OR 20ms
    (ulTaskNotifyTake(pdTRUE, 20 / portTICK_PERIOD_MS) == pdTRUE);
    if (MqttManager::connected()) {
      // MqttManager::_sendMqttMessage();
      MqttManager::_mqttClient->loop();
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
  if (!MqttManager::_hasStarted) {
    return false;
  }

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
  if (MqttManager::connected()) {
    while (!MqttManager::_mqttClient->publish(NSPMConfig::instance->mqtt_availability_topic.c_str(), online_message_buffer, true)) {
      LOG_ERROR("Failed to send online/offline message");
      vTaskDelay(250 / portTICK_PERIOD_MS);
    }
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
