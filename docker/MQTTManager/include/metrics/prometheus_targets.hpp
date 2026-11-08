#ifndef MQTTMANAGER_PROMETHEUS_TARGETS_HPP
#define MQTTMANAGER_PROMETHEUS_TARGETS_HPP

#include <cstdint>
#include <nspanel/nspanel.hpp>
#include <string>
#include <vector>

/**
 * Prometheus http_sd service discovery, served at GET /prometheus/targets.
 *
 * Lists the manager itself, under the Host the request arrived with, and every panel that can be
 * scraped: accepted, online (not offline, waiting, rebooting or updating), with a known IP and a metrics_port advertised in its
 * register_request. See METRICS.md, "Service discovery endpoint".
 */
class PrometheusTargets {
public:
  struct PanelTarget {
    std::string name;
    std::string mac;
    uint32_t nspm_id;
    std::string ip_address;
    uint16_t metrics_port;
  };

  /**
   * The http_sd JSON for the given request Host header and panels. A Host that is empty or does not
   * look like host[:port] leaves the manager out.
   */
  static std::string build_json(const std::string &host, const std::vector<PanelTarget> &panels);

  /**
   * The panels currently loaded in EntityManager that should be scraped.
   */
  static std::vector<PanelTarget> current_panel_targets();

  /**
   * The panels among the given ones that should be scraped.
   */
  static std::vector<PanelTarget> select_panel_targets(const std::vector<NSPanelMetricsSnapshot> &panels);

  /**
   * Whether host is a plain host[:port] (letters, digits, '.', '-', '_', ':', '[' and ']').
   */
  static bool is_valid_host(const std::string &host);
};

#endif
