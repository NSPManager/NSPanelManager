#include "nspanel.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include <spdlog/spdlog.h>

NSPanel::NSPanel(nlohmann::json &init_data) {
  this->_id = init_data["id"];
  this->_name = init_data["name"];
  this->_mac = init_data["mac"];
  this->_ip_address = init_data["address"];
  this->_is_us_panel = std::string(init_data["is_us_panel"]).compare("True") == 0;
  SPDLOG_DEBUG("Loaded NSPanel {}::{}.", this->_id, this->_name);

  this->_mqtt_log_topic = "nspanel/";
  this->_mqtt_log_topic.append(this->_name);
  this->_mqtt_log_topic.append("/log");

  this->_mqtt_status_topic = "nspanel/";
  this->_mqtt_status_topic.append(this->_name);
  this->_mqtt_status_topic.append("/status");

  this->_mqtt_status_report_topic = "nspanel/";
  this->_mqtt_status_report_topic.append(this->_name);
  this->_mqtt_status_report_topic.append("/status_report");

  MQTT_Manager::attach_observer(this);
}

uint NSPanel::get_id() {
  return this->_id;
}

std::string NSPanel::get_mac() {
  return this->_mac;
}

bool NSPanel::mqtt_callback(const std::string &topic, const std::string &payload) {
  if (topic.compare(this->_mqtt_log_topic) == 0) {
    // TODO: Handle log
    return true;
  } else if (topic.compare(this->_mqtt_status_topic) == 0) {
    // TODO: Handle status
    return true;
  } else if (topic.compare(this->_mqtt_status_report_topic) == 0) {
    // TODO: Handle status report
    return true;
  } else {
    return false;
  }
}
