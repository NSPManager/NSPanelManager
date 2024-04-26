#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <list>
#include <string>

class mqttMessage {
public:
  std::string topic;
  std::string payload;
};

struct SubscribeTopic {
  std::string topic;
  // void *callback(char *topic, byte *payload, unsigned int length);
  std::function<void(char *topic, byte *payload, unsigned int length)> callback;
};

struct PublishMessage {
  std::string topic;
  std::string data;
  bool retain;
};

class MqttManager {
public:
  static void init();
  static void start();
  static void setBufferSize(uint8_t bufferSize);
  static bool publish(const char *topic, std::string &message);
  static bool publish(const char *topic, std::string &message, bool retain);
  static bool publish(std::string &topic, const char *message);
  static bool publish(std::string &topic, const char *message, bool retain);
  static bool publish(std::string &topic, std::string &message);
  static bool publish(std::string &topic, std::string &message, bool retain);
  static bool publish(const char *topic, const char *message);
  static bool publish(const char *topic, const char *message, bool retain);
  static void subscribeToTopic(const char *topic, std::function<void(char *topic, byte *payload, unsigned int length)>);
  // static void subscribeToTopic(const char* topic, void (*callback)(char* topic, byte* payload, unsigned int length));
  static void subscribeToTopic(SubscribeTopic &topic);
  static bool connected();

private:
  static bool _connect();
  static void _subscribeToTopic(SubscribeTopic &topic);
  static void _sendMqttMessage();
  static void _subscribeToAllRegisteredTopics(); // Will subscribe to any topics listed in the _subscribeTopics
  static void _taskMqttRunTask(void *params);
  static void _mqttClientCallback(char *topic, byte *payload, unsigned int length);

  static inline bool _hasStarted;
  static inline std::list<SubscribeTopic> _subscribeTopics;
  static inline QueueHandle_t _sendQueue;
  static inline WiFiClient *_wifiClient;
  static inline PubSubClient *_mqttClient;
};

#endif
