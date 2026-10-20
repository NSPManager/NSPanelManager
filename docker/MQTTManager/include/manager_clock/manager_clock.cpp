#include "manager_clock.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <cstdlib>
#include <ctime>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

void ManagerClock::start() {
  ManagerClock::init();
  if (!ManagerClock::_instance.joinable()) {
    ManagerClock::_instance = std::thread(ManagerClock::_run_clock_thread);
  }
}

void ManagerClock::init() {
  MQTT_Manager::attach_connected_callback(&ManagerClock::publish_epoch);
}

void ManagerClock::_run_clock_thread() {
  // Set timezone
  setenv("TZ", MqttManagerConfig::timezone.c_str(), 1);
  tzset();

  // Timezone loaded, proceed to update MQTT
  for (;;) {
    char time_buffer[20];
    char ampm_buffer[20];
    std::string time_str;
    char date_buffer[100];
    std::string date_str;

    std::time_t time = std::time({});
    std::strftime(date_buffer, 100, MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::DATE_FORMAT).c_str(), std::localtime(&time));
    date_str = date_buffer;

    if (!MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::CLOCK_US_STYLE)) [[likely]] {
      std::strftime(time_buffer, 20, "%H:%M", std::localtime(&time));
      time_str = time_buffer;
    } else {
      std::strftime(time_buffer, 20, "%I:%M", std::localtime(&time));
      std::strftime(ampm_buffer, 20, "%p", std::localtime(&time));
      time_str = time_buffer;
    }

    MQTT_Manager::publish(fmt::format("nspanel/mqttmanager_{}/status/time", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS)), time_buffer, true);
    if (!MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::CLOCK_US_STYLE)) [[likely]] {
      MQTT_Manager::publish(fmt::format("nspanel/mqttmanager_{}/status/ampm", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS)), "", true);
    } else {
      MQTT_Manager::publish(fmt::format("nspanel/mqttmanager_{}/status/ampm", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS)), ampm_buffer, true);
    }

    MQTT_Manager::publish(fmt::format("nspanel/mqttmanager_{}/status/date", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS)), date_buffer, true);
    ManagerClock::publish_epoch();

    // Sleep until next minute
    auto t_now = std::chrono::system_clock::now();
    auto t_now_c = std::chrono::system_clock::to_time_t(t_now);
    std::tm next_minute_tm = *std::localtime(&t_now_c);
    next_minute_tm.tm_sec = 1; // Always wait to minute change over and not the exact same second

    next_minute_tm.tm_min++;
    if (next_minute_tm.tm_min >= 60) { // We went over to next hour
      next_minute_tm.tm_min = 0;
      next_minute_tm.tm_hour++;
      if (next_minute_tm.tm_hour >= 24) { // We went over to next day
        next_minute_tm.tm_hour = 0;
        next_minute_tm.tm_mday++;
        // mktime (called below) normalises month/year overflow and handles
        // varying month lengths and leap years, so no manual rollover needed.
      }
    }

    auto t_next_minute = std::chrono::system_clock::from_time_t(std::mktime(&next_minute_tm));
    std::this_thread::sleep_until(t_next_minute);
  }
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
