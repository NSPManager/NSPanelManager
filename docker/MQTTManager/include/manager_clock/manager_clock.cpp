#include "manager_clock.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

void ManagerClock::init() {
  MQTT_Manager::attach_connected_callback(&ManagerClock::publish_epoch);
}

void ManagerClock::publish_epoch() {
  std::string topic = fmt::format("nspanel/mqttmanager_{}/status/epoch", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS));
  MQTT_Manager::publish_if_connected(topic, [] {
    return ManagerClock::epoch_payload(std::chrono::system_clock::now());
  });
}

std::optional<std::string> ManagerClock::epoch_payload(std::chrono::system_clock::time_point now) {
  if (now < ManagerClock::_earliest_plausible_time) {
    if (!ManagerClock::_warned_about_unset_clock.exchange(true)) {
      SPDLOG_WARN("System clock reads {} ms since the Unix epoch, which is too early to be right. Will not send the time to NSPanels until the clock is set.", std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
    }
    return std::nullopt;
  }

  ManagerClock::_warned_about_unset_clock = false;
  return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
}
