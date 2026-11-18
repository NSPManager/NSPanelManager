#ifndef MQTTMANAGER_MANAGER_CLOCK
#define MQTTMANAGER_MANAGER_CLOCK

#include <atomic>
#include <chrono>
#include <optional>
#include <string>
#include <thread>

/**
 * Tells NSPanels the current time.
 *
 * Every minute the time and date to display are sent, retained, on nspanel/mqttmanager_<address>/status/time,
 * status/ampm and status/date, formatted as configured.
 *
 * The time is also sent as Unix time in milliseconds on nspanel/mqttmanager_<address>/status/epoch so that
 * panels can set their own clock. It is never retained, so a panel only ever receives a time that was current
 * when it was sent. It is sent every minute, whenever the manager connects to MQTT, and whenever a panel comes
 * online.
 */
class ManagerClock {
public:
  /**
   * Start the thread that sends the time every minute, and send the time each time the manager connects
   * to MQTT. Call before MQTT_Manager::init().
   */
  static void start();

  /**
   * Send the time each time the manager connects to MQTT. Called by start().
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
  static inline std::thread _instance;

  // Send the time and date at the start of every minute. Never returns.
  static void _run_clock_thread();

  // 2025-01-01T00:00:00Z. Anything earlier means the host clock has not been set.
  static constexpr std::chrono::system_clock::time_point _earliest_plausible_time{std::chrono::seconds(1735689600)};

  // Warn once when the host clock is not set, not every minute.
  static inline std::atomic<bool> _warned_about_unset_clock = false;
};

#endif // !MQTTMANAGER_MANAGER_CLOCK
