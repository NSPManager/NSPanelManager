#include "mqtt_manager.hpp"
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
#include <pthread.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
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
                      .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
                      .clean_session(false)
                      .finalize();

  try {
    spdlog::info("Connecting to the MQTT Server...");
    mqtt::connect_response rsp = MQTT_Manager::_mqtt_client->connect(connOpts);
    spdlog::info("Connected to server.");

    spdlog::debug("Subscribing to topics...");
    MQTT_Manager::_mqtt_client->subscribe(MQTT_Manager::_get_subscribe_topics(), MQTT_Manager::_get_subscribe_topics_qos());

    // Consume messages
    while (true) {
      auto msg = MQTT_Manager::_mqtt_client->consume_message();

      if (msg) {
        MQTT_Manager::_process_mqtt_message(msg->get_topic(), msg->get_payload());
      } else if (!MQTT_Manager::_mqtt_client->is_connected()) {
        spdlog::error("Lost connection");
        while (!MQTT_Manager::_mqtt_client->is_connected()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        spdlog::info("Re-established connection");
      }
    }

    // Disconnect
    spdlog::info("Disconnecting from the MQTT server...");
    MQTT_Manager::_mqtt_client->disconnect();
    spdlog::info("OK");
  } catch (const mqtt::exception &exc) {
    std::cerr << exc.what() << std::endl;
    // return 1;
  }

  // return 0;
}

bool MQTT_Manager::is_connected() {
  if (MQTT_Manager::_mqtt_client != nullptr) {
    return MQTT_Manager::_mqtt_client->is_connected();
  } else {
    return false;
  }
}

void MQTT_Manager::subscribe() {
}

const std::vector<std::string> MQTT_Manager::_get_subscribe_topics() {
  std::vector<std::string> subscribe_topics;
  subscribe_topics.push_back("nspanel/mqttmanager/command");
  subscribe_topics.push_back("nspanel/+/log");
  subscribe_topics.push_back("nspanel/+/status");
  subscribe_topics.push_back("nspanel/+/status_report");
  subscribe_topics.push_back("nspanel/scenes/room/+/+/save");
  subscribe_topics.push_back("nspanel/scenes/room/+/+/activate");
  subscribe_topics.push_back("nspanel/scenes/global/+/save");
  subscribe_topics.push_back("nspanel/scenes/global/+/activate");
  // subscribe_topics.push_back("nspanel/entities/#");
  return subscribe_topics;
}

const std::vector<int> MQTT_Manager::_get_subscribe_topics_qos() {
  std::vector<int> subscribe_topics;
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  subscribe_topics.push_back(1);
  // subscribe_topics.push_back(1);
  return subscribe_topics;
}

void MQTT_Manager::_process_mqtt_message(const std::string topic, const std::string message) {
  try {
    bool message_handled = false;
    for (MQTT_Observer *observer : MQTT_Manager::_mqtt_observers) {
      if (observer->mqtt_callback(topic, message)) {
        message_handled = true;
        break;
      }
    }
    if (!message_handled) {
      // If message is still unhandled, check all static callbacks.
      for (auto mqtt_callback : MQTT_Manager::_mqtt_observer_callbacks) {
        if (mqtt_callback(topic, message)) {
          message_handled = true;
          break;
        }
      }
    }
    if (!message_handled) {
      spdlog::warn("Got message on topic '{}' that was unhandled.", topic);
    }
  } catch (std::exception ex) {
    spdlog::error("Caught std::exception while processing message on topic '{}'. message: '{}'. Exception: ", topic, message, ex.what());
  } catch (...) {
    spdlog::error("Caught exception of type other than std::exception while processing message on topic '{}'. message: {}", topic, message);
  }
}

void MQTT_Manager::_process_mqtt_command(nlohmann::json &data) {
}

void MQTT_Manager::attach_observer(MQTT_Observer *observer) {
  MQTT_Manager::_mqtt_observers.push_back(observer);
}

void MQTT_Manager::attach_observer(std::function<bool(const std::string &topic, const std::string &payload)> callback) {
  MQTT_Manager::_mqtt_observer_callbacks.push_back(callback);
}

void MQTT_Manager::detach_observer(MQTT_Observer *observer) {
  MQTT_Manager::_mqtt_observers.remove(observer);
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
  if (MQTT_Manager::_mqtt_client != nullptr) {
    if (MQTT_Manager::_mqtt_client->is_connected()) {
      mqtt::message_ptr msg = mqtt::make_message(topic.c_str(), payload.c_str(), 0, retain);
      MQTT_Manager::_mqtt_client->publish(msg);
    } else {
      spdlog::error("Tried sending MQTT message while MQTT is disconnected.");
    }
  }
}
