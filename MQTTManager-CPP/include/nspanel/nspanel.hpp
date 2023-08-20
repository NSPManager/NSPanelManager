#ifndef MQTT_MANAGER_NSPANEL
#define MQTT_MANAGER_NSPANEL
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>

class NSPanel : public MQTT_Observer {
public:
  NSPanel(nlohmann::json &init_data);

  /**
   * Get the ID of this NSPanel.
   */
  uint get_id();
  std::string get_mac();
  bool mqtt_callback(const std::string &topic, const std::string &payload);

private:
  uint _id;
  std::string _mac;
  std::string _name;
  bool _is_us_panel;
  std::string _ip_address;

  std::string _mqtt_log_topic;
  std::string _mqtt_status_topic;
  std::string _mqtt_status_report_topic;
};

#endif // !MQTT_MANAGER_NSPANEL
