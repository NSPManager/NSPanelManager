#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include "websocket_server/websocket_server.hpp"
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <functional>
#include <google/protobuf/message.h>
#include <mqtt/client.h>
#include <mqtt/message.h>
#include <mqtt/subscribe_options.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

#if defined(TEST_MODE) && TEST_MODE == 1
#include <utility>
#include <vector>
#endif

struct MQTTMessage {
  std::string topic;
  std::string message;
};

class MQTT_Manager {
public:
  static void init(); // Load config and connect to MQTT
  static void connect();

  /*
   * Reload config from DB and reconnect if needed.
   */
  static void reload_config();

  /*
   * Whether messages are sent to the MQTT server as they are published. False from losing the
   * connection until, after reconnecting, the messages buffered meanwhile have been sent: until then
   * new messages are buffered behind them, so that nothing overtakes an older message.
   */
  static bool is_connected();

  template <typename CALLBACK_BIND>
  static void subscribe(std::string topic, int qos, CALLBACK_BIND callback) {
    if (topic.empty()) {
      SPDLOG_ERROR("Subscribing to '{}' is not allowed. Will not subscribe.", topic);
      return;
    }

    std::lock_guard<std::mutex> mutex_guard(MQTT_Manager::_mqtt_client_mutex);
    bool already_subscribed = MQTT_Manager::_mqtt_callbacks[topic].num_slots() > 0;
    MQTT_Manager::_mqtt_callbacks[topic].disconnect(callback); // Disconnect before doing a connect in case we were already connected.
    MQTT_Manager::_mqtt_callbacks[topic].connect(callback);
    if (!already_subscribed) {
      MQTT_Manager::_subscribed_topics[topic] = qos;
      if (MQTT_Manager::is_connected()) {
        SPDLOG_TRACE("Subscribing to MQTT Topic '{}'.", topic);
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
    std::lock_guard<std::mutex> mutex_guard(MQTT_Manager::_mqtt_client_mutex);
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
   * Will send an MQTT message on given topic with the given payload.
   * @param topic: The MQTT topic to send payload to.
   * @param payload: MQTT message payload.
   * @param retain: Wether to set the MQTT retain flag or not.
   */
  static void publish_protobuf(const std::string &topic, google::protobuf::Message &payload, bool retain);

  /**
   * Will send a non-retained MQTT message with QoS 2, but only if connected right now. The message is
   * never buffered while disconnected or retried on failure, so it suits payloads that are only valid at
   * the moment they are sent, such as the current time. (The MQTT client itself may still resend it
   * after reconnecting if the connection dropped before the server acknowledged it.)
   * The message is mirrored to STOMP whether or not MQTT is connected.
   * @param topic: The MQTT topic to send payload to.
   * @param make_payload: Called with the MQTT client locked, immediately before sending. Must not call
   *                      back into MQTT_Manager. Return std::nullopt to send nothing.
   * @return true if the message was sent over MQTT.
   */
  static bool publish_if_connected(const std::string &topic, std::function<std::optional<std::string>()> make_payload);

  /**
   * Attach a callback that is called each time the connection to the MQTT server has been
   * (re)established and every topic has been subscribed to again.
   * @param callback: Function to call.
   */
  static void attach_connected_callback(std::function<void()> callback);

  /**
   * Will clear retain on a topic (not recursive)
   * @param topic: The MQTT topic to send payload to.
   */
  static void clear_retain(const std::string &topic);

#if defined(TEST_MODE) && TEST_MODE == 1
  struct TestPublishedMessage {
    std::string topic;
    std::string payload;
    bool retain;
  };

  // Everything passed to publish since the last call, whether or not the MQTT client is connected.
  static std::vector<TestPublishedMessage> test_take_published_messages() {
    std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_test_published_messages_mutex);
    return std::exchange(MQTT_Manager::_test_published_messages, {});
  }

  // Number of messages waiting to be sent once connected.
  static size_t test_buffered_message_count() {
    std::lock_guard<std::mutex> mutex_guard(MQTT_Manager::_mqtt_client_mutex);
    return MQTT_Manager::_mqtt_messages_buffer.size();
  }

  // Calls the connected callbacks as if a connection to the MQTT server had just been established.
  static void test_notify_connected() {
    MQTT_Manager::_connected_callbacks();
  }

  // Number of callbacks attached to a topic.
  static size_t test_callback_count(const std::string &topic) {
    std::lock_guard<std::mutex> mutex_guard(MQTT_Manager::_mqtt_client_mutex);
    return MQTT_Manager::_mqtt_callbacks.count(topic) > 0 ? MQTT_Manager::_mqtt_callbacks.at(topic).num_slots() : 0;
  }

  // While set, behave as if connected to the MQTT server, but record what would have been sent to it
  // rather than sending it. Clearing it behaves as losing the connection.
  static void test_set_broker_connected(bool connected) {
    MQTT_Manager::_test_broker_connected = connected;
    if (!connected) {
      MQTT_Manager::_caught_up = false;
    }
  }

  // Send what was buffered while disconnected, as after reconnecting.
  static void test_send_buffered_messages() {
    MQTT_Manager::_send_buffered_messages();
  }

  // Everything sent to the MQTT server since the last call, in order. Only recorded while
  // test_set_broker_connected(true).
  static std::vector<TestPublishedMessage> test_take_sent_messages() {
    std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_test_published_messages_mutex);
    return std::exchange(MQTT_Manager::_test_sent_messages, {});
  }

  // Everything mirrored to STOMP since the last call, in order, with the MQTT topic (without the
  // "mqtt/" prefix) and the payload before base64 encoding.
  static std::vector<TestPublishedMessage> test_take_mirrored_messages() {
    std::lock_guard<std::mutex> lock_guard(MQTT_Manager::_test_published_messages_mutex);
    return std::exchange(MQTT_Manager::_test_mirrored_messages, {});
  }
#endif

private:
#if defined(TEST_MODE) && TEST_MODE == 1
  static inline std::mutex _test_published_messages_mutex;
  static inline std::vector<TestPublishedMessage> _test_published_messages;
  static inline std::vector<TestPublishedMessage> _test_sent_messages;
  static inline std::vector<TestPublishedMessage> _test_mirrored_messages;
  static inline std::atomic<bool> _test_broker_connected = false;
#endif
  static inline boost::lockfree::spsc_queue<MQTTMessage, boost::lockfree::capacity<256>> _mqtt_message_queue;
  static inline std::thread _process_messages_thread;
  static inline mqtt::client *_mqtt_client = nullptr;
  static inline std::mutex _mqtt_client_mutex;
  static inline std::mutex _mqtt_message_mutex;
  static inline std::list<mqtt::message_ptr> _mqtt_messages_buffer;
  static inline std::unordered_map<std::string, std::string> _mqtt_retain_buffer; // Used so that when we reconnect to an MQTT server that has restarted we can repopulate the retained messages.
  static inline std::mutex _mqtt_retain_buffer_mutex;

  static void _reconnect_mqtt_client();

  // Whether the MQTT client is connected to the server, regardless of _caught_up.
  static bool _broker_connected();

  // Set once the messages buffered while disconnected have been sent, see is_connected().
  static inline std::atomic<bool> _caught_up = false;

  /**
   * Send the messages buffered while disconnected, in the order they were published, then the latest
   * value of every retained topic in case the server restarted and lost them, and mark the client as
   * caught up. Holds the client lock throughout, so nothing published meanwhile can overtake them.
   */
  static void _send_buffered_messages();

  // Hand a message to the MQTT client. Called with _mqtt_client_mutex held.
  static void _send(mqtt::const_message_ptr msg);

  static inline boost::ptr_map<std::string, boost::signals2::signal<void(std::string, std::string)>> _mqtt_callbacks;
  static inline std::unordered_map<std::string, int> _subscribed_topics;
  static inline boost::signals2::signal<void()> _connected_callbacks;

  /**
   * Replicate a sent MQTT message into the websocket STOMP topic "mqtt/<topic>".
   */
  static void _mirror_to_stomp(const std::string &topic, const std::string &payload, bool retain);

  static void _process_mqtt_messages();
  static void _process_mqtt_command(nlohmann::json &data);

  static inline std::atomic<bool> _reconnect_on_disconnect = true;
  static inline std::mutex _settings_mutex;
  static inline std::string _mqtt_address;
  static inline uint16_t _mqtt_port;
  static inline std::string _mqtt_username;
  static inline std::string _mqtt_password;
  static inline std::atomic<bool> _stop_consuming;

  /**
   * Will process a STOMP message received the websocket through WebsocketManager.
   * This is used to bridge STOMP and MQTT to be able to hook into React web frontend.
   * @param topic: The STOMP topic the message was received on.
   * @param message: The STOMP message payload.
   */
  static void _process_stomp_message(StompFrame frame);
};

#endif // !MQTT_MANAGER_HPP
