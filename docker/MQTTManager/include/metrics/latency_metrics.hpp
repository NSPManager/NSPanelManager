#ifndef MQTTMANAGER_LATENCY_METRICS_HPP
#define MQTTMANAGER_LATENCY_METRICS_HPP

#include <chrono>
#include <atomic>
#include <cstdint>
#include <metrics/manager_metrics.hpp>
#include <metrics/metrics.hpp>
#include <string>

/**
 * Phase 2 metrics (METRICS.md): latency, queueing and the outcomes of long operations.
 *
 * Call sites only call these helpers, so the metric definitions, label sets and buckets are in one
 * place. Every helper is thread safe.
 */
class LatencyMetrics {
public:
  using clock = std::chrono::steady_clock;

  static double seconds_since(clock::time_point start) {
    return std::chrono::duration<double>(clock::now() - start).count();
  }

  // --- MQTT message queue (MQTT_Manager) ---

  // A received message was put on the processing queue.
  static void mqtt_message_enqueued();
  // The subscribe path found the queue full and spun for `seconds` until it could push a message.
  static void mqtt_queue_full_wait(double seconds);

  // Raise `max` to `value` if it is higher, safely against other threads doing the same. True if raised.
  static bool raise_max(std::atomic<long> &max, long value) {
    long current = max.load();
    while (value > current) {
      if (max.compare_exchange_weak(current, value)) {
        return true;
      }
    }
    return false;
  }

  // A received message was dropped because the processing queue was full.
  static void mqtt_message_dropped(Metrics::TopicClass topic_class);
  // A message was taken off the queue after waiting `wait_seconds`.
  static void mqtt_message_dequeued(Metrics::TopicClass topic_class, double wait_seconds);
  // The callbacks for a message ran for `seconds`. `threw` when they ended in an exception.
  static void mqtt_handler_finished(Metrics::TopicClass topic_class, double seconds, bool threw);

  // --- NSPanel config ---

  // Building and serialising an NSPanelConfig took `seconds`.
  static void panel_config_built(double seconds);

  // --- Home Assistant ---

  // A websocket request with this id was sent. Only call_service requests are timed.
  static void home_assistant_request_sent(long id, const std::string &type, const std::string &domain);
  // The result for request `id` arrived. `success` is the result's success field.
  static void home_assistant_result_received(long id, bool success);
  // The websocket closed: results for requests in flight will never arrive.
  static void home_assistant_disconnected();
  // A state_changed event for this entity arrived from Home Assistant.
  static void home_assistant_state_changed(const std::string &entity_id);

  // --- Panel command -> integration call ---

  /**
   * While alive, a panel command of type `type` is being handled on this thread. The first call to
   * integration_call_issued() on the same thread observes the time since the scope was opened.
   */
  class CommandScope {
  public:
    explicit CommandScope(const std::string &type);
    ~CommandScope();
    CommandScope(const CommandScope &) = delete;
    CommandScope &operator=(const CommandScope &) = delete;
  };

  // A call to Home Assistant or OpenHAB is being sent.
  static void integration_call_issued();

  // --- Panels ---

  // A log line with this level (error, warning, info, debug or unknown) arrived from a panel.
  // Series are labelled panel, mac and nspm_id. They are removed with the panel's other series when
  // ManagerMetrics sees its labels change or the panel is destroyed (ManagerMetrics::on_panel_labels_removed).
  static void panel_log_line(const ManagerMetrics::PanelLabels &panel, const char *level);
  // Map a panel's log level (a letter or a word, any case) to error, warning, info, debug or unknown.
  static const char *panel_log_level(const std::string &level);
  // An OTA update of `type` (firmware, littlefs, tft) was requested / started / succeeded / failed.
  static void panel_update(const char *type, const char *outcome);
  // A panel was in the updating state for `type` for `seconds`.
  static void panel_update_duration(const char *type, double seconds);

  // --- Image server ---

  static void image_request(bool cache_hit, const char *format);
  static void image_converted(double seconds);

  // --- Weather and clock ---

  // outcome: success, http_error or parse_error.
  static void weather_fetch(const char *outcome);
  // The time was broadcast to the panels.
  static void clock_broadcast();
};

#endif
