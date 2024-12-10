#include <MqttLog.hpp>
#include <PubSubClient.h>

void MqttLog::init(std::string *mqtt_log_topic, PubSubClient **mqtt_client) {
  MqttLog::instance = this;
  this->_mqttLogTopic = mqtt_log_topic;
  this->_messageBuild = "";
  this->_messageBuilderMutex = xSemaphoreCreateMutex();
  this->_mqtt_client = mqtt_client;

  Serial.println("MqttLog initialized.");
}

void MqttLog::setLogLevel(MqttLogLevel logLevel) {
  Serial.print("MqttLog setting log level to ");
  Serial.println(static_cast<int>(this->_logLevel));
  LOG_INFO("Setting log level to ", static_cast<int>(this->_logLevel));
  this->_logLevel = logLevel;
}

MqttLogLevel MqttLog::getLogLevel() {
  return this->_logLevel;
}
