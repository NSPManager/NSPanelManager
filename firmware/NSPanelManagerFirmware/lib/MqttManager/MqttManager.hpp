#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <Arduino.h>
#include <MqttLog.hpp>
#include <PubSubClient.h>
#include <ReadBufferFixedSize.h>
#include <WiFiClient.h>
#include <protobuf_defines.h>
#include <string>
#include <vector>

class MQTTMessage : public EmbeddedProto::ReadBufferInterface {
public:
  std::string topic;
  std::string data;
  bool retain;

  // Read and deserialize the data in this message into the specified protobuf type
  template <class PROTOBUF_TYPE>
  bool get_protobuf_obj(PROTOBUF_TYPE *dest) {
    if (dest != NULL) {
      try {
        auto result = dest->deserialize(*this);
        if (result == EmbeddedProto::Error::NO_ERRORS) {
          return true;
        } else {
          LOG_ERROR("Error while deserializing protobuf. Error code: ", (uint32_t)result);
        }
      } catch (std::exception &ex) {
        LOG_ERROR("Caught exception ", ex.what(), " while processing message into protobuf.");
      }
    } else {
      LOG_ERROR("Trying to deserialize message into NULL-buffer!");
    }
    return false;
  }

  // Clear all data in message buffer.
  void clear() {
    this->data.clear();
  }

  // Implementation for EmbeddedProto::ReadBufferInterface follows below
  // This is done in order to keep dynamic lengths of MQTT message payloads and not
  // allocate a fixed amount of data per message.

  uint32_t get_size() const override {
    return this->data.size();
  }

  uint32_t get_max_size() const override {
    return this->data.size();
  }

  bool peek(uint8_t &byte) const override {
    const bool return_value = this->data.size() > this->_read_index;
    if (return_value) {
      byte = this->data[this->_read_index];
    }
    return return_value;
  }

  bool advance() override {
    const bool return_value = this->data.size() > this->_read_index;
    if (return_value) {
      this->_read_index++;
      return true;
    }
    return false;
  }

  bool advance(const uint32_t n_bytes) override {
    const uint32_t new_read_index = this->_read_index + n_bytes;
    const bool return_value = this->data.size() >= new_read_index;
    if (return_value) {
      this->_read_index = new_read_index;
      return true;
    }
    return false;
  }

  bool pop(uint8_t &byte) override {
    const bool return_value = this->data.size() > this->_read_index;
    if (return_value) {
      byte = this->data[this->_read_index];
      this->_read_index++;
      return true;
    }
    return false;
  }

private:
  uint32_t _read_index = 0;
};

struct SubscribeTopic {
  std::string topic;
  std::function<void(MQTTMessage *message)> callback;
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
  static void subscribeToTopic(const char *topic, std::function<void(MQTTMessage *message)>);
  static void subscribeToTopic(SubscribeTopic &topic);
  static bool connected();

  // The actual client used to communicate with MQTT
  static inline PubSubClient *mqttClient = NULL;

private:
  static bool _connect();
  static void _subscribeToTopic(SubscribeTopic &topic);
  static void _sendMqttMessage();
  static void _subscribeToAllRegisteredTopics(); // Will subscribe to any topics listed in the _subscribeTopics
  static void _taskMqttRunTask(void *params);
  static void _taskHandleMqttMessageReceiveQueue(void *params);
  static void _mqttClientCallback(char *topic, byte *payload, unsigned int length);

  static inline bool _hasStarted;
  static inline std::vector<SubscribeTopic> _subscribeTopics;
  static inline QueueHandle_t _sendQueue;
  static inline QueueHandle_t _receiveQueue;
  static inline WiFiClient *_wifiClient = NULL;
  static inline TaskHandle_t _taskMqttRunTaskHandle;
  static inline TaskHandle_t _taskHandleMqttMessageReceiveQueueHandle;
};

#endif
