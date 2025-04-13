#include "mqtt_manager.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/signals2.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <google/protobuf/message.h>
#include <iostream>
#include <mqtt/client.h>
#include <mqtt/message.h>
#include <mqtt/subscribe_options.h>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <mutex>
#include <pthread.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <utility>
#include <vector>

inline bool file_exists(const char *name) {
  struct stat buffer;
  return (stat(name, &buffer) == 0);
}

void MQTT_Manager::init() {
  MQTT_Manager::reload_config(); // This will also start a new thread to handle MQTT messages.
}

void MQTT_Manager::connect() {

  if (!MQTT_Manager::_process_messages_thread.joinable()) {
    SPDLOG_INFO("Starting MQTT message processing thread.");
    MQTT_Manager::_process_messages_thread = std::thread(MQTT_Manager::_process_mqtt_messages);
  }

  MQTT_Manager::_reconnect_mqtt_client();
  {
    std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_settings_mutex);
    if (MQTT_Manager::_mqtt_address.empty()) {
      SPDLOG_ERROR("No MQTT server address configured. Will not try to connect to MQTT.");
      return;
    }
  }

  try {
    // Consume messages
    MQTT_Manager::_mqtt_client->start_consuming();
    while (true) {
      auto msg = MQTT_Manager::_mqtt_client->consume_message();
      if (msg) {
        MQTTMessage message_struct{
            .topic = msg->get_topic(),
            .message = msg->get_payload_str()};
        MQTT_Manager::_mqtt_message_queue.push(message_struct);
      } else if (!MQTT_Manager::_mqtt_client->is_connected()) {
        MQTT_Manager::_reconnect_mqtt_client();
      }
    }

    // Disconnect
    SPDLOG_INFO("Disconnecting from the MQTT server...");
    MQTT_Manager::_mqtt_client->disconnect();
    SPDLOG_INFO("Disconnect OK.");
  } catch (const mqtt::exception &exc) {
    std::cerr << exc.what() << std::endl;
  }
}

void MQTT_Manager::reload_config() {
  std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_settings_mutex);
  std::string address = MqttManagerConfig::get_setting_with_default("mqtt_server", "");
  uint16_t port = std::stoi(MqttManagerConfig::get_setting_with_default("mqtt_port", "1883"));
  std::string username = MqttManagerConfig::get_setting_with_default("mqtt_username", "");
  std::string password = MqttManagerConfig::get_setting_with_default("mqtt_password", "");

  if (MQTT_Manager::_mqtt_address.compare(address) != 0 ||
      MQTT_Manager::_mqtt_port != port ||
      MQTT_Manager::_mqtt_username.compare(username) != 0 ||
      MQTT_Manager::_mqtt_password.compare(password) != 0) {
    MQTT_Manager::_mqtt_address = address;
    MQTT_Manager::_mqtt_port = port;
    MQTT_Manager::_mqtt_username = username;
    MQTT_Manager::_mqtt_password = password;
    SPDLOG_INFO("Reconnecting MQTT as settings has changed.");
    if (MQTT_Manager::_mqtt_client != nullptr) {
      MQTT_Manager::_mqtt_client->disconnect(); // This will cause it to reconnect with new settings.
    } else {
      // Start a new thread to connect via.
      std::thread mqtt_thread = std::thread(&MQTT_Manager::connect);
      mqtt_thread.detach();
    }
  }
}

bool MQTT_Manager::is_connected() {
  if (MQTT_Manager::_mqtt_client != nullptr) {
    return MQTT_Manager::_mqtt_client->is_connected();
  } else {
    return false;
  }
}

void MQTT_Manager::_reconnect_mqtt_client() {
  std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_mqtt_client_mutex);
  std::lock_guard<std::mutex> mutex_guard_settings(MQTT_Manager::_settings_mutex);

  std::stringstream mac_address_str_stream;
  if (file_exists("/sys/class/net/eth0/address")) {
    SPDLOG_INFO("Appending mac data from eth0.");
    std::ifstream f("/sys/class/net/eth0/address", std::ios::in | std::ios::binary);
    mac_address_str_stream << f.rdbuf();
    f.close();
    mac_address_str_stream << '\0';
  } else if (file_exists("/sys/class/net/tap0/address")) {
    SPDLOG_INFO("Appending mac data from tap0.");
    std::ifstream f("/sys/class/net/tap0/address", std::ios::in | std::ios::binary);
    mac_address_str_stream << f.rdbuf();
    f.close();
    mac_address_str_stream << '\0';
  } else {
    SPDLOG_WARN("Neither eth0 or tap0 interfaces was found to use as address suffixes for MQTTManager MQTT name. This could result in instability in the case of 2 or more MQTTManager running at the same time!");
  }

  // Cleanup text
  std::string mac_address_str = mac_address_str_stream.str();
  boost::algorithm::trim(mac_address_str);
  boost::algorithm::replace_all(mac_address_str, " ", "");
  boost::algorithm::replace_all(mac_address_str, ":", "_");
  boost::algorithm::replace_all(mac_address_str, "\n", "");

  std::string mqtt_client_name = "NSPMMQTT_";
  mqtt_client_name.append(mac_address_str);
  SPDLOG_INFO("Will connect to MQTT with Client name {}", mqtt_client_name);

  if (MQTT_Manager::_mqtt_address.empty()) {
    SPDLOG_ERROR("MQTT Server address is empty. Aborting.");
    return;
  }

  std::string connection_url = "tcp://";
  connection_url.append(MQTT_Manager::_mqtt_address);
  connection_url.append(":");
  connection_url.append(std::to_string(MQTT_Manager::_mqtt_port));
  SPDLOG_INFO("Will connect to MQTT via {} as client {}", connection_url, mqtt_client_name);

  if (MQTT_Manager::_mqtt_client != nullptr && MQTT_Manager::_mqtt_client->is_connected()) {
    MQTT_Manager::_mqtt_client->disconnect();
    delete MQTT_Manager::_mqtt_client;
  }

  MQTT_Manager::_mqtt_client = new mqtt::client(connection_url, mqtt_client_name.c_str());

  auto connOpts = mqtt::connect_options_builder()
                      .user_name(MQTT_Manager::_mqtt_username)
                      .password(MQTT_Manager::_mqtt_password)
                      .keep_alive_interval(std::chrono::seconds(30))
                      .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(10))
                      .clean_session(false)
                      .finalize();

  MQTT_Manager::_mqtt_client->connect(connOpts);

  while (!MQTT_Manager::_mqtt_client->is_connected()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
  SPDLOG_INFO("Established connection to MQTT server.");

  try {
    SPDLOG_DEBUG("Subscribing to registered MQTT topics.");

    mqtt::const_message_ptr msg;
    for (auto mqtt_topic_pair : MQTT_Manager::_subscribed_topics) {
      SPDLOG_TRACE("Subscribing to topic {}", mqtt_topic_pair.first);
      auto subscribe_result = MQTT_Manager::_mqtt_client->subscribe(mqtt_topic_pair.first, mqtt_topic_pair.second);
      int result_code = subscribe_result.get_reason_codes().front();
      SPDLOG_DEBUG("Subscribed to {}. Got code: {}", mqtt_topic_pair.first, result_code);

      bool received_message;
      do {
        received_message = MQTT_Manager::_mqtt_client->try_consume_message_for(&msg, std::chrono::milliseconds(5)); // Wait for max 100ms to see if a message is to be receved on the recently subscribed topic.
        if (received_message) {
          MQTTMessage message_struct{
              .topic = msg->get_topic(),
              .message = msg->get_payload_str()};
          while (!MQTT_Manager::_mqtt_message_queue.push(message_struct)) {
          }
        }
      } while (received_message);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception trying to subscribe to topics: {}", boost::diagnostic_information(e, true));
  }

  // Send buffered messages if any
  try {
    auto it = MQTT_Manager::_mqtt_messages_buffer.cbegin();
    while (it != MQTT_Manager::_mqtt_messages_buffer.cend()) {
      MQTT_Manager::_mqtt_client->publish((*it));
      MQTT_Manager::_mqtt_messages_buffer.erase(it++);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to publish message: {}", boost::diagnostic_information(e, true));
  }
}

void MQTT_Manager::_process_mqtt_messages() {
  while (true) {
    while (MQTT_Manager::_mqtt_message_queue.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    MQTT_Manager::_mqtt_message_queue.consume_all([](MQTTMessage message) {
      try {
        SPDLOG_TRACE("Processing message from topic {}", message.topic);
        // Call each observer/listener until a callback return true, ie. the callback was handled.
        MQTT_Manager::_mqtt_callbacks[message.topic](message.topic, message.message);
      } catch (std::exception &ex) {
        SPDLOG_ERROR("Caught std::exception while processing message on topic '{}'. message: '{}'. Exception: ", message.topic, message.message, boost::diagnostic_information(ex, true));
      } catch (...) {
        SPDLOG_ERROR("Caught exception of type other than std::exception while processing message on topic '{}'. message: {}", message.topic, message.message);
      }
    });
  }
}

void MQTT_Manager::_process_mqtt_command(nlohmann::json &data) {
}

void MQTT_Manager::publish(const std::string &topic, const std::string &payload) {
  MQTT_Manager::publish(topic, payload, false);
}

void MQTT_Manager::publish(const std::string &topic, const std::string &payload, bool retain) {
  if (topic.empty()) {
    SPDLOG_ERROR("Tried to publish payload to empty topic. Payload: {}", payload);
    return; // It's not allowed to publish to empty topic.
  }

  std::lock_guard<std::mutex> mutex_guard(MQTT_Manager::_mqtt_client_mutex);
  mqtt::message_ptr msg = mqtt::make_message(topic.c_str(), payload.c_str(), payload.size(), 0, retain);
  if (MQTT_Manager::_mqtt_client != nullptr) {
    if (MQTT_Manager::is_connected()) {
      SPDLOG_TRACE("Publising '{}' -> '{}'", topic, payload);
      MQTT_Manager::_mqtt_client->publish(msg);
    } else {
      MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
    }
  } else {
    MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
  }
}

void MQTT_Manager::publish_protobuf(const std::string &topic, google::protobuf::Message &payload, bool retain) {
  std::string payload_string;
  if (payload.SerializeToString(&payload_string)) {
    MQTT_Manager::publish(topic, payload_string.c_str(), retain);
  } else {
    SPDLOG_ERROR("Tried to send message to topic '{}' but serialization of protobuf object failed.", topic);
  }
}

void MQTT_Manager::clear_retain(const std::string &topic) {
  if (topic.empty()) {
    SPDLOG_ERROR("Topic is empty, will not clear retian.");
    return;
  }

  std::lock_guard<std::mutex> mutex_guard(MQTT_Manager::_mqtt_client_mutex);
  if (topic.size() > 0) {
    mqtt::message_ptr msg = mqtt::make_message(topic.c_str(), "", 0, 0, true);
    if (MQTT_Manager::_mqtt_client != nullptr) {
      MQTT_Manager::_mqtt_client->publish(msg);
    } else {
      MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
    }
  }
}
