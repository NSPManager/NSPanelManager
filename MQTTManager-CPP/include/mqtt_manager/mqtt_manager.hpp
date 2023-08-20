#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <functional>
#include <mqtt/client.h>
#include <mqtt/subscribe_options.h>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

class MQTT_Observer {
public:
  /**
   * Process an MQTT message. Returns true if message was processed or false to continue
   * through all registered listeners.
   */
  virtual bool mqtt_callback(const std::string &topic, const std::string &payload) = 0;
};

class MQTT_Manager {
public:
  static void connect();
  static bool is_connected();
  static void subscribe();

  /**
   * Attach an MQTT_Observer to the list of observers for new MQTT messages.
   * @param observer: Pointer to the observer.
   */
  static void attach_observer(MQTT_Observer *observer);

  /**
   * Detach an MQTT_Observer from the list of observers for new MQTT messages.
   * @param observer: Pointer to the observer.
   */
  static void detach_observer(MQTT_Observer *observer);

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

private:
  static inline mqtt::client *_mqtt_client = nullptr;
  static inline std::list<MQTT_Observer *> _mqtt_observers;
  static inline std::list<std::function<bool(const std::string &topic, const std::string &payload)>> _mqtt_observer_callbacks; // Raw function callbacks
  static const std::vector<std::string> _get_subscribe_topics();
  static const std::vector<int> _get_subscribe_topics_qos();

  static void _process_mqtt_message(const std::string topic, const std::string message);
  static void _process_mqtt_command(nlohmann::json &data);
};

#endif // !MQTT_MANAGER_HPP
