#include "prometheus_targets.hpp"
#include <cctype>
#include <entity_manager/entity_manager.hpp>
#include <metrics/panel_metrics_collector.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

std::string PrometheusTargets::build_json(const std::string &host, const std::vector<PanelTarget> &panels) {
  nlohmann::json targets = nlohmann::json::array();
  if (PrometheusTargets::is_valid_host(host)) {
    targets.push_back({
        {"targets", nlohmann::json::array({host})},
        {"labels", {{"role", "manager"}, {"__metrics_path__", "/prometheus/metrics"}}},
    });
  }
  for (const PanelTarget &panel : panels) {
    targets.push_back({
        {"targets", nlohmann::json::array({panel.ip_address + ":" + std::to_string(panel.metrics_port)})},
        {"labels", {{"role", "panel"}, {"panel", panel.name}, {"mac", panel.mac}, {"nspm_id", std::to_string(panel.nspm_id)}, {"__metrics_path__", "/metrics"}}},
    });
  }
  return targets.dump();
}

bool PrometheusTargets::is_valid_host(const std::string &host) {
  if (host.empty() || host.size() > 255) {
    return false;
  }
  for (unsigned char c : host) {
    if (!std::isalnum(c) && c != '.' && c != '-' && c != '_' && c != ':' && c != '[' && c != ']') {
      return false;
    }
  }
  return true;
}

std::vector<PrometheusTargets::PanelTarget> PrometheusTargets::current_panel_targets() {
  std::vector<NSPanelMetricsSnapshot> snapshots;
  for (const auto &nspanel : EntityManager::get_all_nspanels()) {
    snapshots.push_back(nspanel->get_metrics_snapshot());
  }
  return PrometheusTargets::select_panel_targets(snapshots);
}

std::vector<PrometheusTargets::PanelTarget> PrometheusTargets::select_panel_targets(const std::vector<NSPanelMetricsSnapshot> &panels) {
  std::vector<PanelTarget> targets;
  for (const NSPanelMetricsSnapshot &panel : panels) {
    // Only online panels: an offline one would only be scraped into up == 0 (alert on nspm_panel_up instead),
    // and scraping one that is updating or rebooting adds load and fails anyway.
    if (!PanelMetricsCollector::is_accepted(panel) || panel.state != MQTT_MANAGER_NSPANEL_STATE::ONLINE) {
      continue;
    }
    if (panel.ip_address.empty() || panel.metrics_port == 0) {
      continue;
    }
    targets.push_back({.name = panel.name, .mac = panel.mac, .nspm_id = panel.id, .ip_address = panel.ip_address, .metrics_port = panel.metrics_port});
  }
  return targets;
}
