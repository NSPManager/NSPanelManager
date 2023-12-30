#include "mqtt_manager.hpp"
#include "light/light.hpp"
#include <boost/signals2.hpp>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <mqtt/client.h>
#include <mqtt/message.h>
#include <mqtt/subscribe_options.h>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <mutex>
#include <pthread.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

void MQTT_Manager::connect() {
  if (MQTT_Manager::_mqtt_client == nullptr) {
    std::string connection_url = "tcp://";
    connection_url.append(MqttManagerConfig::mqtt_server);
    connection_url.append(":");
    connection_url.append(std::to_string(MqttManagerConfig::mqtt_port));

    MQTT_Manager::_mqtt_client = new mqtt::client(connection_url, "NSPM_MqttManager");
  }

  auto connOpts = mqtt::connect_options_builder()
                      .user_name(MqttManagerConfig::mqtt_username)
                      .password(MqttManagerConfig::mqtt_password)
                      .keep_alive_interval(std::chrono::seconds(30))
                      .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(10))
                      .clean_session(false)
                      .finalize();

  try {
    SPDLOG_INFO("Connecting to the MQTT Server...");
    mqtt::connect_response rsp = MQTT_Manager::_mqtt_client->connect(connOpts);
    SPDLOG_INFO("Connected to server.");

    SPDLOG_DEBUG("Subscribing to topics...");
    MQTT_Manager::_mqtt_client->subscribe(MQTT_Manager::_get_subscribe_topics(), MQTT_Manager::_get_subscribe_topics_qos());
    MQTT_Manager::_resubscribe();
    // Send buffered messages if any
    auto it = MQTT_Manager::_mqtt_messages_buffer.begin();
    while (it != MQTT_Manager::_mqtt_messages_buffer.end()) {
      MQTT_Manager::_mqtt_client->publish((*it));
      MQTT_Manager::_mqtt_messages_buffer.erase(it++);
    }

    // Consume messages
    while (true) {
      auto msg = MQTT_Manager::_mqtt_client->consume_message();

      if (msg) {
        MQTT_Manager::_process_mqtt_message(msg->get_topic(), msg->get_payload());
      } else if (!MQTT_Manager::_mqtt_client->is_connected()) {
        SPDLOG_ERROR("Lost connection");
        while (!MQTT_Manager::_mqtt_client->is_connected()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        SPDLOG_INFO("Re-established connection");
        MQTT_Manager::_resubscribe();
        // Send buffered messages if any
        auto it = MQTT_Manager::_mqtt_messages_buffer.begin();
        while (it != MQTT_Manager::_mqtt_messages_buffer.end()) {
          MQTT_Manager::_mqtt_client->publish((*it));
          MQTT_Manager::_mqtt_messages_buffer.erase(it++);
        }
      }
    }

    // Disconnect
    SPDLOG_INFO("Disconnecting from the MQTT server...");
    MQTT_Manager::_mqtt_client->disconnect();
    SPDLOG_INFO("OK");
  } catch (const mqtt::exception &exc) {
    std::cerr << exc.what() << std::endl;
  }
}

bool MQTT_Manager::is_connected() {
  if (MQTT_Manager::_mqtt_client != nullptr) {
    return MQTT_Manager::_mqtt_client->is_connected();
  } else {
    return false;
  }
}

void MQTT_Manager::_resubscribe() {
  SPDLOG_DEBUG("Subscribing to registered MQTT topics.");
  for (auto mqtt_topic_pair : MQTT_Manager::_subscribed_topics) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait 100ms between each subscribe in order for MQTT to catch up.
    SPDLOG_DEBUG("Subscribing to topic {}", mqtt_topic_pair.first);
    MQTT_Manager::_mqtt_client->subscribe(mqtt_topic_pair.first, mqtt_topic_pair.second);
  }
}

const std::vector<std::string> MQTT_Manager::_get_subscribe_topics() {
  std::vector<std::string> subscribe_topics;
  subscribe_topics.push_back("nspanel/+/log");
  subscribe_topics.push_back("nspanel/scenes/room/+/+/save");
  subscribe_topics.push_back("nspanel/scenes/room/+/+/activate");
  subscribe_topics.push_back("nspanel/scenes/global/+/save");
  subscribe_topics.push_back("nspanel/scenes/global/+/activate");
  return subscribe_topics;
}

const std::vector<int> MQTT_Manager::_get_subscribe_topics_qos() {
  std::vector<int> subscribe_topics;
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  return subscribe_topics;
}

void MQTT_Manager::_process_mqtt_message(const std::string topic, const std::string message) {
  for (auto mqtt_topic_signal_pair : MQTT_Manager::_mqtt_callbacks) {
    if (mqtt_topic_signal_pair.first.compare(topic) == 0) {
      MQTT_Manager::_mqtt_callbacks[mqtt_topic_signal_pair.first](topic, message);
    }
  }

  try {
    SPDLOG_TRACE("Got message: {} --> {}", topic, message);
    // Call each observer/listener until a callback return true, ie. the callback was handled.
    for (auto mqtt_callback : MQTT_Manager::_mqtt_observer_callbacks) {
      if (mqtt_callback(topic, message)) {
        break;
      }
    }
  } catch (std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing message on topic '{}'. message: '{}'. Exception: ", topic, message, ex.what());
  } catch (...) {
    SPDLOG_ERROR("Caught exception of type other than std::exception while processing message on topic '{}'. message: {}", topic, message);
  }
}

void MQTT_Manager::_process_mqtt_command(nlohmann::json &data) {
}

void MQTT_Manager::attach_observer(std::function<bool(const std::string &topic, const std::string &payload)> callback) {
  MQTT_Manager::_mqtt_observer_callbacks.push_back(callback);
}

void MQTT_Manager::detach_observer(std::function<bool(const std::string &topic, const std::string &payload)> callback) {
  for (auto it = MQTT_Manager::_mqtt_observer_callbacks.begin(); it != MQTT_Manager::_mqtt_observer_callbacks.end(); ++it) {
    if (callback.target_type() == it->target_type()) {
      if (callback.target<bool (*)(const std::string &topic, const std::string &payload)>() == it->target<bool (*)(const std::string &topic, const std::string &payload)>()) {
        it = MQTT_Manager::_mqtt_observer_callbacks.erase(it);
      }
    }
  }
}

void MQTT_Manager::publish(const std::string &topic, const std::string &payload) {
  MQTT_Manager::publish(topic, payload, false);
}

void MQTT_Manager::publish(const std::string &topic, const std::string &payload, bool retain) {
  mqtt::message_ptr msg = mqtt::make_message(topic.c_str(), payload.c_str(), 0, retain);
  if (MQTT_Manager::_mqtt_client != nullptr) {
    std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_mqtt_client_mutex);
    MQTT_Manager::_mqtt_client->publish(msg);
  } else {
    MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
  }
}

void MQTT_Manager::clear_retain(const std::string &topic) {
  mqtt::message_ptr msg = mqtt::make_message(topic.c_str(), "", 0, 0, true);
  if (MQTT_Manager::_mqtt_client != nullptr) {
    std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_mqtt_client_mutex);
    MQTT_Manager::_mqtt_client->publish(msg);
  } else {
    MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
  }
}
