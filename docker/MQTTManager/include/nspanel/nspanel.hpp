#ifndef MQTT_MANAGER_NSPANEL
#define MQTT_MANAGER_NSPANEL
#include "entity/entity.hpp"
#include <cstdint>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

enum MQTT_MANAGER_NSPANEL_STATE {
  UNKNOWN,
  WAITING,
  ONLINE,
  OFFLINE,
  UPDATING_FIRMWARE,
  UPDATING_DATA,
  UPDATING_TFT,
  AWAITING_ACCEPT,
  DENIED
};

class NSPanelLogMessage {
public:
  std::string time;
  std::string level;
  std::string message;
};

class NSPanelRelayGroup : public MqttManagerEntity {
public:
  NSPanelRelayGroup(nlohmann::json &config);
  ~NSPanelRelayGroup();

  void update_config(nlohmann::json &config);
  bool contains(int nspanel_id, int relay_num);
  void turn_on();
  void turn_off();

  uint16_t get_id();
  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();
  void post_init();

private:
  int _id;
  std::string _name;
  // Map of NSPanelID:relayID of relays in this group.
  std::unordered_map<int, int> _nspanel_relays;
};

class NSPanel {
public:
  NSPanel(nlohmann::json &init_data);
  void update_config(nlohmann::json &init_data);
  ~NSPanel();
  void reset_mqtt_topics();
  /**
   * Reset MQTT topics used to register entities to HA.
   */
  void reset_ha_mqtt_topics();

  /**
   * Get the ID of this NSPanel.
   */
  uint get_id();
  std::string get_mac();
  std::string get_name();
  MQTT_MANAGER_NSPANEL_STATE get_state();
  void mqtt_callback(std::string topic, std::string payload);

  /**
   * Dump JSON as string and send to NSPanel command topic.
   */
  void send_command(nlohmann::json &command);

  /**
   * Send a reload command to the panel to reload the config
   */
  void send_reload_command();

  /**
   * Get the JSON message that will be sent over the websocket when a client requests the state of the NSPanel.
   */
  nlohmann::json get_websocket_json_representation();

  /**
   * Get the JSON message that will be sent over the websocket when a client requests the logs from the given NSPanel.
   */
  nlohmann::json get_websocket_json_logs();

  /**
   * Sends a reboot command to the panel over MQTT.
   */
  void reboot();

  /**
   * Sends a command to start firmware update.
   */
  void firmware_update();

  /**
   * Sends a command to start TFT screen update.
   */
  void tft_update();

  /**
   * This panel has been deleted from the manager and we wish to completly remove all details about it every existing.
   */
  void erase();

  /**
   * Request warnings from maanager (Django) and update internal state.
   */
  void update_warnings_from_manager();

  /**
   * Accept register request from this panel.
   */
  void accept_register_request();

  /**
   * Deny register request from this panel and add to "ignore"-list.
   */
  void deny_register_request();

  /**
   * Returns true if the NSPanel is register in manager, otherwise false.
   */
  bool has_registered_to_manager();

  /**
   * Send current state to the websocket.
   */
  void send_websocket_update();

  /**
   * Register NSPanel to manager.
   */
  bool register_to_manager(const nlohmann::json &register_request_payload);

  /**
   * Register NSPanel to manager.
   */
  void register_to_home_assistant();

  /**
   * Turn a relay on or off
   */
  void set_relay_state(uint8_t relay, bool state);

private:
  uint _id;
  std::string _mac;
  std::string _name;
  bool _is_us_panel;
  bool _is_register_accepted;
  bool _is_register_denied;
  bool _has_registered_to_manager;
  std::string _ip_address;
  int16_t _rssi;
  float _temperature;
  uint8_t _heap_used_pct;
  uint8_t _update_progress;
  MQTT_MANAGER_NSPANEL_STATE _state;
  std::string _nspanel_warnings;
  std::string _nspanel_warnings_from_manager;
  std::string _mqtt_register_mac;

  // MQTT Stuff:
  // Wether or not relay1 should be registered to Home Assistant as a switch or light (true = register as light).
  bool _relay1_is_mqtt_light;
  // The topic to send commands to the relay1
  std::string _mqtt_relay1_command_topic;
  // The topic where relay1 state is published
  std::string _mqtt_relay1_state_topic;
  // Wether or not relay1 is on
  bool _relay1_state;
  // Wether or not relay2 should be registered to Home Assistant as a switch or light (true = register as light).
  bool _relay2_is_mqtt_light;
  // The topic to send commands to the relay2
  std::string _mqtt_relay2_command_topic;
  // The topic where relay2 state is published
  std::string _mqtt_relay2_state_topic;
  // Wether or not relay2 is on
  bool _relay2_state;
  // The topic to capture logs from MQTT
  std::string _mqtt_log_topic;
  // The topic to capture status (online/offline) from MQTT
  std::string _mqtt_status_topic;
  // The topic to capture status reports from MQTT
  std::string _mqtt_status_report_topic;
  // The topic to send commands to panel to via MQTT
  std::string _mqtt_command_topic;
  // Home Assistant MQTT registration topics:
  std::string _mqtt_sensor_temperature_topic;
  std::string _mqtt_switch_relay1_topic;
  std::string _mqtt_light_relay1_topic;
  std::string _mqtt_switch_relay2_topic;
  std::string _mqtt_light_relay2_topic;
  std::string _mqtt_switch_screen_topic;
  std::string _mqtt_number_screen_brightness_topic;
  std::string _mqtt_number_screensaver_brightness_topic;
  std::string _mqtt_select_screensaver_topic;

  std::list<NSPanelLogMessage> _log_messages;
};
#endif // !MQTT_MANAGER_NSPANEL
