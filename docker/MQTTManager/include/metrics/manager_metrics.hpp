#ifndef MQTTMANAGER_MANAGER_METRICS_HPP
#define MQTTMANAGER_MANAGER_METRICS_HPP

#include <array>
#include <cstddef>
#include <functional>
#include <string>

/**
 * Phase 1 manager metrics (METRICS.md): counters and connection state, recorded at the choke points
 * that already exist. Every function is cheap and thread safe, and may be called from any thread.
 *
 * Values that are cheaper to sample than to keep in sync (panel states, status report values,
 * staged images) are not here: PanelMetricsCollector computes them at scrape time.
 */
class ManagerMetrics {
public:
  /**
   * Register every unlabelled series, so they are exported (as 0) before anything has happened.
   */
  static void init();

  // MQTT

  /**
   * The MQTT client connected (true) or lost its connection (false). Counts a connect on every change
   * to true and a disconnect on every change from true to false.
   */
  static void mqtt_connection_changed(bool connected);
  static void mqtt_message_received(const std::string &topic, size_t bytes);
  static void mqtt_message_published(bool retained, size_t bytes);
  static void mqtt_publish_error();
  static void mqtt_buffered_messages(size_t count);
  static void mqtt_retained_topics(size_t count);

  // Panels. Every per-panel series is labelled with panel (name), mac and nspm_id.

  struct PanelLabels {
    std::string panel;
    std::string mac;
    std::string nspm_id;

    bool operator==(const PanelLabels &other) const = default;
  };

  static void panel_status_report(const PanelLabels &panel);
  static void panel_register_request(const PanelLabels &panel);
  static void panel_config_sent(const PanelLabels &panel, size_t bytes);

  /**
   * Record the panel's current labels (keyed by MAC). When they differ from the ones last seen for the
   * MAC (the panel was renamed or got a new id), the series under the old labels are removed. The
   * panel_* functions above call this themselves; call it directly when a panel's settings reload.
   */
  static void panel_labels(const PanelLabels &panel);

  /**
   * Remove every series of the panel with this MAC, as when the panel is deleted from the manager.
   */
  static void panel_removed(const std::string &mac);

  /**
   * Called with a panel's old labels whenever its series are removed (renamed or deleted), for metrics
   * kept outside ManagerMetrics that carry the same labels to remove theirs too. Callbacks run on the
   * thread that noticed the change, with no lock held.
   */
  static void on_panel_labels_removed(std::function<void(const PanelLabels &old_labels)> callback);

  // Home Assistant and OpenHAB.

  enum class Integration {
    HOME_ASSISTANT,
    OPENHAB,
  };

  /**
   * The integration's websocket opened (true) or closed or failed (false). Every open after the first
   * counts as a reconnect.
   */
  static void integration_connection_changed(Integration integration, bool connected);
  static void integration_authenticated(Integration integration, bool authenticated);
  static void integration_auth_failure(Integration integration);
  static void integration_message_received(Integration integration);
  static void integration_message_sent(Integration integration);

  // Web interface websocket (STOMP).

  static void websocket_clients(size_t count);
  static void websocket_message_received();
  static void websocket_message_sent();

  // Manager HTTP server (:8003).

  enum class HttpRoute {
    IMAGE,
    TARGETS,
    METRICS,
    OTHER,
  };
  static HttpRoute http_route(const std::string &uri);
  static void http_request(HttpRoute route, int status_code);

  // Active manager warnings shown in the web interface, by level: error, warning, info, debug.

  static void warnings_active(const std::array<size_t, 4> &count_by_level);
};

#endif
