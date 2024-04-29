#include "mqtt_manager.hpp"
#include "light/light.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/signals2.hpp>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <mqtt/client.h>
#include <mqtt/message.h>
#include <mqtt/subscribe_options.h>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <pthread.h>
#include <ratio>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <utility>
#include <vector>

inline bool file_exists(const char *name) {
  struct stat buffer;
  return (stat(name, &buffer) == 0);
}

void MQTT_Manager::connect() {
  if (MQTT_Manager::_mqtt_client == nullptr) {
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
    mac_address_str.erase(std::find_if(mac_address_str.rbegin(), mac_address_str.rend(), [](unsigned char ch) {
                            return !std::isspace(ch) && ch != '\r' && ch != '\n' && ch != '\0';
                          }).base(),
                          mac_address_str.end());

    std::replace(mac_address_str.begin(), mac_address_str.end(), ':', '_');
    std::string mqtt_client_name = "NSPMMQTT_";
    mqtt_client_name.append(mac_address_str);
    SPDLOG_INFO("Will connect to MQTT with Client name {}", mqtt_client_name);

    std::string connection_url = "tcp://";
    connection_url.append(MqttManagerConfig::mqtt_server);
    connection_url.append(":");
    connection_url.append(std::to_string(MqttManagerConfig::mqtt_port));

    MQTT_Manager::_mqtt_client = new mqtt::client(connection_url, mqtt_client_name.c_str());
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

    MQTT_Manager::_resubscribe();
    SPDLOG_DEBUG("Sending buffered messages.");
    try {
      auto it = MQTT_Manager::_mqtt_messages_buffer.cbegin();
      while (it != MQTT_Manager::_mqtt_messages_buffer.cend()) {
        MQTT_Manager::_mqtt_client->publish((*it));
        MQTT_Manager::_mqtt_messages_buffer.erase(it++);
      }
    } catch (std::exception &e) {
      SPDLOG_ERROR("Caught exception when trying to publish message: {}", boost::diagnostic_information(e, true));
    }

    // Consume messages
    MQTT_Manager::_mqtt_client->start_consuming();
    while (true) {
      auto msg = MQTT_Manager::_mqtt_client->consume_message();
      if (msg) {
        MQTT_Manager::_process_mqtt_message(msg->get_topic(), msg->get_payload());
      } else if (!MQTT_Manager::_mqtt_client->is_connected()) {
        SPDLOG_ERROR("Lost connection");
        while (!MQTT_Manager::_mqtt_client->is_connected()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        SPDLOG_INFO("Established connection to MQTT server.");
        MQTT_Manager::_resubscribe();
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
  try {
    SPDLOG_DEBUG("Subscribing to registered MQTT topics.");
    MQTT_Manager::_mqtt_client->subscribe(MQTT_Manager::_get_subscribe_topics(), MQTT_Manager::_get_subscribe_topics_qos());

    mqtt::const_message_ptr msg;
    for (auto mqtt_topic_pair : MQTT_Manager::_subscribed_topics) {
      SPDLOG_DEBUG("Subscribing to topic {}", mqtt_topic_pair.first);
      auto subscribe_result = MQTT_Manager::_mqtt_client->subscribe(mqtt_topic_pair.first, mqtt_topic_pair.second);
      int result_code = subscribe_result.get_reason_codes().front();
      SPDLOG_DEBUG("Subscribed to {}. Got code: {}", mqtt_topic_pair.first, result_code);

      bool received_message;
      do {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait 100ms between each subscribe in order for MQTT to catch up.
        received_message = MQTT_Manager::_mqtt_client->try_consume_message(&msg);
        if (received_message) {
          MQTT_Manager::_process_mqtt_message(msg->get_topic(), msg->get_payload());
        }
      } while (received_message);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception trying to subscribe to topics: {}", boost::diagnostic_information(e, true));
  }
}

const std::vector<std::string> MQTT_Manager::_get_subscribe_topics() {
  // TODO: Remove this type of subscription, perform subscribe in each room and so on.
  std::vector<std::string> subscribe_topics;
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
  return subscribe_topics;
}

void MQTT_Manager::_process_mqtt_message(const std::string topic, const std::string message) {
  try {
    for (auto mqtt_topic_signal_pair : MQTT_Manager::_mqtt_callbacks) {
      if (mqtt_topic_signal_pair.first.compare(topic) == 0) {
        MQTT_Manager::_mqtt_callbacks[mqtt_topic_signal_pair.first](topic, message);
      }
    }

    // Call each observer/listener until a callback return true, ie. the callback was handled.
    for (auto mqtt_callback : MQTT_Manager::_mqtt_observer_callbacks) {
      if (mqtt_callback(topic, message)) {
        break;
      }
    }
  } catch (std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing message on topic '{}'. message: '{}'. Exception: ", topic, message, boost::diagnostic_information(ex, true));
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
    if (MQTT_Manager::is_connected()) {
      MQTT_Manager::_mqtt_client->publish(msg);
    } else {
      MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
    }
  } else {
    MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
  }
}

void MQTT_Manager::clear_retain(const std::string &topic) {
  if (topic.size() > 0) {
    mqtt::message_ptr msg = mqtt::make_message(topic.c_str(), "", 0, 0, true);
    if (MQTT_Manager::_mqtt_client != nullptr) {
      MQTT_Manager::_mqtt_client->publish(msg);
    } else {
      MQTT_Manager::_mqtt_messages_buffer.push_back(msg);
    }
  }
}
