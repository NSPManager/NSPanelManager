#ifndef MQTTMANAGER_PANEL_METRICS_COLLECTOR_HPP
#define MQTTMANAGER_PANEL_METRICS_COLLECTOR_HPP

#include <nspanel/nspanel.hpp>
#include <prometheus/collectable.h>
#include <prometheus/metric_family.h>
#include <vector>

/**
 * Panel and staged-image metrics, sampled from EntityManager and MqttManagerConfig at scrape time
 * rather than kept in sync on every change. Built afresh on every scrape, so deleted and renamed
 * panels disappear instead of leaving stale series behind.
 *
 *   nspm_panels{state}                       panels in each state, every state listed
 *   nspm_panel_up{panel,mac,nspm_id}         1 while online or updating, for accepted panels
 *   nspm_panel_info{panel,mac,nspm_id,ip,model,md5_firmware,md5_littlefs,md5_tft} = 1
 *   nspm_staged_image_info{type,variant,md5} = 1
 *   nspm_panel_update_pending{panel,type}    the staged image for the panel differs from what it runs
 *   nspm_panel_wifi_rssi_dbm, nspm_panel_heap_used_ratio, nspm_panel_temperature_celsius,
 *   nspm_panel_humidity, nspm_panel_pressure, nspm_panel_update_progress_ratio
 *                                            from the latest status report, while the panel is not offline
 */
class PanelMetricsCollector : public prometheus::Collectable {
public:
  std::vector<prometheus::MetricFamily> Collect() const override;

  /**
   * Register a collector with Metrics. Safe to call more than once.
   */
  static void install();

  /**
   * Whether a panel has been accepted into the manager (registered, neither denied nor awaiting accept).
   */
  static bool is_accepted(const NSPanelMetricsSnapshot &panel);

  /**
   * Whether a panel counts as up: online or updating.
   */
  static bool is_up(const NSPanelMetricsSnapshot &panel);

  /**
   * The label value for a panel state, e.g. online, updating_tft or awaiting_accept.
   */
  static const char *state_label(MQTT_MANAGER_NSPANEL_STATE state);
};

#endif
