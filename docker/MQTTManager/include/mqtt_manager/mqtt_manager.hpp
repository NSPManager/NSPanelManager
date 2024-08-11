#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <functional>
#include <mqtt/client.h>
#include <mqtt/message.h>
#include <mqtt/subscribe_options.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>
#include <vector>

struct MQTTMessage {
  std::string topic;
  std::string message;
};

class MQTT_Manager {
public:
  static void connect();
  static bool is_connected();

  template <typename CALLBACK_BIND>
  static void subscribe(std::string topic, int qos, CALLBACK_BIND callback) {
    if (topic.empty()) {
      SPDLOG_ERROR("Subscribing to '{}' is not allowed. Will not subscribe.", topic);
      return;
    }

    bool already_subscribed = MQTT_Manager::_mqtt_callbacks[topic].num_slots() > 0;
    MQTT_Manager::_mqtt_callbacks[topic].disconnect(callback); // Disconnect before doing a connect in case we were already connected.
    MQTT_Manager::_mqtt_callbacks[topic].connect(callback);
    if (!already_subscribed) {
      SPDLOG_DEBUG("Adding '{}' to the list of topics to subscribe to.", topic);
      MQTT_Manager::_subscribed_topics[topic] = qos;
      if (MQTT_Manager::is_connected()) {
        SPDLOG_DEBUG("MQTT is connected, subscribing to MQTT topic '{}'.", topic);
        MQTT_Manager::_mqtt_client->subscribe(topic, qos);
      }
    }
  }

  template <typename CALLBACK_BIND>
  static void subscribe(std::string topic, CALLBACK_BIND callback) {
    MQTT_Manager::subscribe(topic, 0, callback);
  }

  template <typename CALLBACK_BIND>
  static void detach_callback(std::string topic, CALLBACK_BIND callback) {
    MQTT_Manager::_mqtt_callbacks[topic].disconnect(callback);
  }

  /**
   * Attach a function callback to the list of observers for new MQTT messages.
   * @param observer: Pointer to the observer.
   */
  static void attach_observer(std::function<bool(const std::string &topic, const std::string &payload)> callback);

  /**
   * Detach a function callback from the list of observers for new MQTT messages.
   * @param observer: Pointer to the observer.
   */
  static void detach_observer(std::function<bool(const std::string &topic, const std::string &payload)> callback);

  /**
   * Will send an MQTT message on given topic with the given payload without retain.
   * @param topic: The MQTT topic to send payload to.
   * @param payload: MQTT message payload.
   */
  static void publish(const std::string &topic, const std::string &payload);

  /**
   * Will send an MQTT message on given topic with the given payload.
   * @param topic: The MQTT topic to send payload to.
   * @param payload: MQTT message payload.
   * @param retain: Wether to set the MQTT retain flag or not.
   */
  static void publish(const std::string &topic, const std::string &payload, bool retain);

  /**
   * Will clear retain on a topic (not recursive)
   * @param topic: The MQTT topic to send payload to.
   */
  static void clear_retain(const std::string &topic);

private:
  static inline boost::lockfree::spsc_queue<MQTTMessage, boost::lockfree::capacity<256>> _mqtt_message_queue;
  static inline std::thread _process_messages_thread;
  static inline mqtt::client *_mqtt_client = nullptr;
  static inline std::mutex _mqtt_client_mutex;
  static inline std::mutex _mqtt_message_mutex;
  static inline std::list<mqtt::message_ptr> _mqtt_messages_buffer;
  static const std::vector<std::string> _get_subscribe_topics();
  static const std::vector<int> _get_subscribe_topics_qos();

  static void _resubscribe();

  static inline boost::ptr_map<std::string, boost::signals2::signal<void(std::string, std::string)>> _mqtt_callbacks;
  static inline std::unordered_map<std::string, int> _subscribed_topics;

  static void _process_mqtt_messages();
  static void _process_mqtt_command(nlohmann::json &data);
};

#endif // !MQTT_MANAGER_HPP
