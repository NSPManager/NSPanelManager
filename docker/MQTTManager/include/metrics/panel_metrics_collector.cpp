#include "panel_metrics_collector.hpp"
#include "metrics.hpp"
#include <entity_manager/entity_manager.hpp>
#include <expected>
#include <map>
#include <memory>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <mutex>
#include <prometheus/client_metric.h>
#include <string>
#include <utility>
#include <vector>

namespace {

using Labels = std::vector<prometheus::ClientMetric::Label>;

prometheus::MetricFamily gauge_family(const std::string &name, const std::string &help) {
  return prometheus::MetricFamily{.name = name, .help = help, .type = prometheus::MetricType::Gauge, .metric = {}};
}

void add_gauge(prometheus::MetricFamily &family, Labels labels, double value) {
  prometheus::ClientMetric metric;
  metric.label = std::move(labels);
  metric.gauge.value = value;
  family.metric.push_back(std::move(metric));
}

constexpr MQTT_MANAGER_NSPANEL_STATE all_states[] = {
    MQTT_MANAGER_NSPANEL_STATE::UNKNOWN,
    MQTT_MANAGER_NSPANEL_STATE::WAITING,
    MQTT_MANAGER_NSPANEL_STATE::ONLINE,
    MQTT_MANAGER_NSPANEL_STATE::OFFLINE,
    MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE,
    MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA,
    MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT,
    MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT,
    MQTT_MANAGER_NSPANEL_STATE::DENIED,
    MQTT_MANAGER_NSPANEL_STATE::REBOOTING,
};

bool md5_known(const std::string &md5) {
  return !md5.empty() && md5 != "UNKNOWN";
}

} // namespace

bool PanelMetricsCollector::is_accepted(const NSPanelMetricsSnapshot &panel) {
  return panel.registered && panel.state != MQTT_MANAGER_NSPANEL_STATE::DENIED && panel.state != MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT;
}

bool PanelMetricsCollector::is_up(const NSPanelMetricsSnapshot &panel) {
  switch (panel.state) {
  case MQTT_MANAGER_NSPANEL_STATE::ONLINE:
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE:
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA:
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT:
    return true;
  default:
    return false;
  }
}

const char *PanelMetricsCollector::state_label(MQTT_MANAGER_NSPANEL_STATE state) {
  switch (state) {
  case MQTT_MANAGER_NSPANEL_STATE::UNKNOWN:
    return "unknown";
  case MQTT_MANAGER_NSPANEL_STATE::WAITING:
    return "waiting";
  case MQTT_MANAGER_NSPANEL_STATE::ONLINE:
    return "online";
  case MQTT_MANAGER_NSPANEL_STATE::OFFLINE:
    return "offline";
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE:
    return "updating_firmware";
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA:
    return "updating_data";
  case MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT:
    return "updating_tft";
  case MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT:
    return "awaiting_accept";
  case MQTT_MANAGER_NSPANEL_STATE::DENIED:
    return "denied";
  case MQTT_MANAGER_NSPANEL_STATE::REBOOTING:
    return "rebooting";
  }
  return "unknown";
}

void PanelMetricsCollector::install() {
  static std::once_flag installed;
  std::call_once(installed, []() {
    // Never destroyed, Metrics holds it weakly.
    static std::shared_ptr<PanelMetricsCollector> collector = std::make_shared<PanelMetricsCollector>();
    Metrics::add_collectable(collector);
  });
}

std::vector<prometheus::MetricFamily> PanelMetricsCollector::Collect() const {
  auto panels_by_state = gauge_family("nspm_panels", "Panels known to the manager, by state.");
  auto panel_up = gauge_family("nspm_panel_up", "Whether an accepted panel is online (1, also while updating) or not (0).");
  auto panel_info = gauge_family("nspm_panel_info", "What an accepted panel runs. Always 1.");
  auto staged_image_info = gauge_family("nspm_staged_image_info", "Images staged on the manager for panels to update to. Always 1.");
  auto update_pending = gauge_family("nspm_panel_update_pending", "Whether the staged image for the panel differs from what it runs.");
  auto rssi = gauge_family("nspm_panel_wifi_rssi_dbm", "Wi-Fi signal strength from the panel's latest status report.");
  auto heap_used = gauge_family("nspm_panel_heap_used_ratio", "Heap in use from the panel's latest status report, 0 to 1.");
  auto temperature = gauge_family("nspm_panel_temperature_celsius", "Temperature from the panel's latest status report, in degrees Celsius.");
  auto humidity = gauge_family("nspm_panel_humidity", "Relative humidity in percent from the panel's latest status report.");
  auto pressure = gauge_family("nspm_panel_pressure", "Air pressure in pascals from the panel's latest status report.");
  auto update_progress = gauge_family("nspm_panel_update_progress_ratio", "Progress of the panel's running update from its latest status report, 0 to 1.");

  std::map<MQTT_MANAGER_NSPANEL_STATE, size_t> state_counts;
  for (MQTT_MANAGER_NSPANEL_STATE state : all_states) {
    state_counts[state] = 0;
  }

  // Panels report temperature in the unit the manager is configured for.
  const bool fahrenheit = MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::USE_FAHRENHEIT);

  auto have = [](const std::expected<std::string, bool> &checksum) { return checksum.has_value(); };
  const bool have_firmware[2] = {have(MqttManagerConfig::get_firmware_sonoff_checksum()), have(MqttManagerConfig::get_firmware_custom_checksum())};
  const bool have_littlefs[2] = {have(MqttManagerConfig::get_littlefs_sonoff_checksum()), have(MqttManagerConfig::get_littlefs_custom_checksum())};

  for (const std::shared_ptr<NSPanel> &nspanel : EntityManager::get_all_nspanels()) {
    NSPanelMetricsSnapshot panel = nspanel->get_metrics_snapshot();
    state_counts[panel.state]++;
    if (!PanelMetricsCollector::is_accepted(panel)) {
      continue;
    }

    const std::string nspm_id = std::to_string(panel.id);
    const Labels labels = {{"panel", panel.name}, {"mac", panel.mac}, {"nspm_id", nspm_id}};
    auto with = [&labels](const char *name, const std::string &value) {
      Labels extended = labels;
      extended.push_back({name, value});
      return extended;
    };
    add_gauge(panel_up, labels, PanelMetricsCollector::is_up(panel) ? 1 : 0);
    add_gauge(panel_info,
              {{"panel", panel.name}, {"mac", panel.mac}, {"nspm_id", nspm_id}, {"ip", panel.ip_address}, {"model", panel.model}, {"md5_firmware", panel.md5_firmware}, {"md5_littlefs", panel.md5_littlefs}, {"md5_tft", panel.md5_tft}},
              1);

    // has_*_update() log an error when the staged checksum is missing, so only ask where there is one to compare with.
    if (panel.model != "web") {
      size_t model_index = panel.model == "custom" ? 1 : 0;
      if (md5_known(panel.md5_firmware) && have_firmware[model_index]) {
        add_gauge(update_pending, with("type", "firmware"), nspanel->has_firmware_update() ? 1 : 0);
      }
      if (md5_known(panel.md5_littlefs) && have_littlefs[model_index]) {
        add_gauge(update_pending, with("type", "littlefs"), nspanel->has_littlefs_update() ? 1 : 0);
      }
      if (md5_known(panel.md5_tft)) {
        add_gauge(update_pending, with("type", "tft"), nspanel->has_tft_update() ? 1 : 0);
      }
    }

    if (panel.status_report_received && panel.state != MQTT_MANAGER_NSPANEL_STATE::OFFLINE) {
      add_gauge(rssi, labels, panel.rssi);
      add_gauge(heap_used, labels, panel.heap_used_pct / 100.0);
      add_gauge(temperature, labels, fahrenheit ? (panel.temperature - 32.0) * 5.0 / 9.0 : panel.temperature);
      if (panel.humidity) {
        add_gauge(humidity, labels, *panel.humidity);
      }
      if (panel.pressure) {
        add_gauge(pressure, labels, *panel.pressure);
      }
      add_gauge(update_progress, labels, panel.update_progress / 100.0);
    }
  }

  for (const auto &[state, count] : state_counts) {
    add_gauge(panels_by_state, {{"state", PanelMetricsCollector::state_label(state)}}, static_cast<double>(count));
  }

  const std::vector<std::tuple<const char *, const char *, std::expected<std::string, bool>>> staged_images = {
      {"firmware", "sonoff", MqttManagerConfig::get_firmware_sonoff_checksum()},
      {"firmware", "custom", MqttManagerConfig::get_firmware_custom_checksum()},
      {"littlefs", "sonoff", MqttManagerConfig::get_littlefs_sonoff_checksum()},
      {"littlefs", "custom", MqttManagerConfig::get_littlefs_custom_checksum()},
      {"tft", "eu_tft1", MqttManagerConfig::get_eu_tft1_checksum()},
      {"tft", "eu_tft2", MqttManagerConfig::get_eu_tft2_checksum()},
      {"tft", "eu_tft3", MqttManagerConfig::get_eu_tft3_checksum()},
      {"tft", "eu_tft4", MqttManagerConfig::get_eu_tft4_checksum()},
      {"tft", "us_tft1", MqttManagerConfig::get_us_tft1_checksum()},
      {"tft", "us_tft2", MqttManagerConfig::get_us_tft2_checksum()},
      {"tft", "us_tft3", MqttManagerConfig::get_us_tft3_checksum()},
      {"tft", "us_tft4", MqttManagerConfig::get_us_tft4_checksum()},
      {"tft", "us_horizontal_mirrored_tft1", MqttManagerConfig::get_us_horizontal_mirrored_tft1_checksum()},
      {"tft", "us_horizontal_mirrored_tft2", MqttManagerConfig::get_us_horizontal_mirrored_tft2_checksum()},
      {"tft", "us_horizontal_mirrored_tft3", MqttManagerConfig::get_us_horizontal_mirrored_tft3_checksum()},
      {"tft", "us_horizontal_mirrored_tft4", MqttManagerConfig::get_us_horizontal_mirrored_tft4_checksum()},
  };
  for (const auto &[type, variant, checksum] : staged_images) {
    if (checksum.has_value()) {
      add_gauge(staged_image_info, {{"type", type}, {"variant", variant}, {"md5", *checksum}}, 1);
    }
  }

  std::vector<prometheus::MetricFamily> families;
  for (auto *family : {&panels_by_state, &panel_up, &panel_info, &staged_image_info, &update_pending, &rssi, &heap_used, &temperature, &humidity, &pressure, &update_progress}) {
    if (!family->metric.empty()) {
      families.push_back(std::move(*family));
    }
  }
  return families;
}
