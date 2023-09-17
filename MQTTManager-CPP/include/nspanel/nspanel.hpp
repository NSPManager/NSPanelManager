#ifndef MQTT_MANAGER_NSPANEL
#define MQTT_MANAGER_NSPANEL
#include <cstdint>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <string>

enum MQTT_MANAGER_NSPANEL_STATE {
  UNKNOWN,
  ONLINE,
  OFFLINE,
  UPDATING_FIRMWARE,
  UPDATING_DATA,
  UPDATING_TFT
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
   * Get the JSON message that will be sent over the websocket when a client requests the state of the NSPAnel.
   */
  nlohmann::json get_websocket_json_representation();

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

  std::string _mqtt_log_topic;
  std::string _mqtt_status_topic;
  std::string _mqtt_status_report_topic;
  std::string _mqtt_command_topic;
};

#endif // !MQTT_MANAGER_NSPANEL
