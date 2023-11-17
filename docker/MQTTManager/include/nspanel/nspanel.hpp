#ifndef MQTT_MANAGER_NSPANEL
#define MQTT_MANAGER_NSPANEL
#include <cstdint>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <string>

enum MQTT_MANAGER_NSPANEL_STATE {
  UNKNOWN,
  WAITING, // Waiting to be accepted by register request.
  ONLINE,
  OFFLINE,
  UPDATING_FIRMWARE,
  UPDATING_DATA,
  UPDATING_TFT
};

class NSPanelLogMessage {
public:
  std::string time;
  std::string level;
  std::string message;
};

class NSPanel : public MQTT_Observer {
public:
  NSPanel(nlohmann::json &init_data);

  /**
   * Get the ID of this NSPanel.
   */
  uint get_id();
  std::string get_mac();
  std::string get_name();
  bool mqtt_callback(const std::string &topic, const std::string &payload);

  /**
   * Dump JSON as string and send to NSPanel command topic.
   */
  void send_command(nlohmann::json &command);

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
   * Request warnings from maanager (Django) and update internal state.
   */
  void update_warnings_from_manager();

  /**
   * Send current state to the websocket.
   */
  void send_websocket_update();

private:
  uint _id;
  std::string _mac;
  std::string _name;
  bool _is_us_panel;
  std::string _ip_address;
  int16_t _rssi;
  int16_t _temperature;
  uint8_t _heap_used_pct;
  uint8_t _update_progress;
  MQTT_MANAGER_NSPANEL_STATE _state;
  std::string _nspanel_warnings;
  std::string _nspanel_warnings_from_manager;

  std::string _mqtt_log_topic;
  std::string _mqtt_status_topic;
  std::string _mqtt_status_report_topic;
  std::string _mqtt_command_topic;

  std::list<NSPanelLogMessage> _log_messages;
};

#endif // !MQTT_MANAGER_NSPANEL
