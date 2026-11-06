#include "prometheus_targets.hpp"
#include <cctype>
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
  // Panels are added to the list once they advertise metrics_port in their register_request.
  return {};
}
