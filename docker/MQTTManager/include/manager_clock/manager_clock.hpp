#ifndef MQTTMANAGER_MANAGER_CLOCK
#define MQTTMANAGER_MANAGER_CLOCK

#include <atomic>
#include <chrono>
#include <optional>
#include <string>

/**
 * Tells NSPanels the current time so they can set their own clock.
 *
 * The time is sent as Unix time in milliseconds on nspanel/mqttmanager_<address>/status/epoch.
 * It is never retained, so a panel only ever receives a time that was current when it was sent.
 * It is sent every minute, whenever the manager connects to MQTT, and whenever a panel comes online.
 */
class ManagerClock {
public:
  /**
   * Send the time each time the manager connects to MQTT. Call before MQTT_Manager::init().
   */
  static void init();

  /**
   * Send the current time now, if connected to MQTT.
   */
  static void publish_epoch();

  /**
   * The epoch payload for the given time: Unix time in milliseconds. Empty if the time is too early
   * to be right, as when the host clock has not been set yet.
   */
  static std::optional<std::string> epoch_payload(std::chrono::system_clock::time_point now);

private:
  // 2025-01-01T00:00:00Z. Anything earlier means the host clock has not been set.
  static constexpr std::chrono::system_clock::time_point _earliest_plausible_time{std::chrono::seconds(1735689600)};

  // Warn once when the host clock is not set, not every minute.
  static inline std::atomic<bool> _warned_about_unset_clock = false;
};

#endif // !MQTTMANAGER_MANAGER_CLOCK
