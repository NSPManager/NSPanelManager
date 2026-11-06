#include "manager_metrics.hpp"
#include "metrics.hpp"
#include <array>
#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <prometheus/counter.h>
#include <prometheus/family.h>
#include <prometheus/gauge.h>
#include <string>
#include <vector>

namespace {

prometheus::Family<prometheus::Counter> &counter_family(const std::string &name, const std::string &help) {
  return prometheus::BuildCounter().Name(name).Help(help).Register(Metrics::registry());
}

prometheus::Family<prometheus::Gauge> &gauge_family(const std::string &name, const std::string &help) {
  return prometheus::BuildGauge().Name(name).Help(help).Register(Metrics::registry());
}

struct MqttMetrics {
  prometheus::Gauge &connected = gauge_family("nspm_mqtt_connected", "Whether MQTTManager is connected to the MQTT broker.").Add({});
  prometheus::Counter &connects = counter_family("nspm_mqtt_connects_total", "Connections established to the MQTT broker.").Add({});
  prometheus::Counter &disconnects = counter_family("nspm_mqtt_disconnects_total", "Connections to the MQTT broker that were lost or closed.").Add({});
  std::array<prometheus::Counter *, Metrics::topic_class_count> received_messages;
  std::array<prometheus::Counter *, Metrics::topic_class_count> received_bytes;
  std::array<prometheus::Counter *, 2> published_messages; // Indexed by retained.
  std::array<prometheus::Counter *, 2> published_bytes;
  prometheus::Counter &publish_errors = counter_family("nspm_mqtt_publish_errors_total", "Publishes to the MQTT broker that threw, including attempts that were retried.").Add({});
  prometheus::Gauge &buffered_messages = gauge_family("nspm_mqtt_buffered_messages", "Messages held to be sent once connected to the MQTT broker.").Add({});
  prometheus::Gauge &retained_topics = gauge_family("nspm_mqtt_retained_topics", "Retained topics MQTTManager republishes after reconnecting.").Add({});
  std::atomic<bool> was_connected = false;

  MqttMetrics() {
    auto &received_messages_family = counter_family("nspm_mqtt_messages_received_total", "MQTT messages received, by topic class.");
    auto &received_bytes_family = counter_family("nspm_mqtt_received_bytes_total", "Payload bytes of MQTT messages received, by topic class.");
    for (size_t i = 0; i < Metrics::topic_class_count; i++) {
      const char *label = Metrics::topic_class_label(static_cast<Metrics::TopicClass>(i));
      this->received_messages[i] = &received_messages_family.Add({{"topic_class", label}});
      this->received_bytes[i] = &received_bytes_family.Add({{"topic_class", label}});
    }
    auto &published_messages_family = counter_family("nspm_mqtt_messages_published_total", "MQTT messages handed to the MQTT client.");
    auto &published_bytes_family = counter_family("nspm_mqtt_published_bytes_total", "Payload bytes of MQTT messages handed to the MQTT client.");
    for (int retained = 0; retained < 2; retained++) {
      const char *label = retained ? "true" : "false";
      this->published_messages[retained] = &published_messages_family.Add({{"retained", label}});
      this->published_bytes[retained] = &published_bytes_family.Add({{"retained", label}});
    }
  }
};

MqttMetrics &mqtt() {
  static MqttMetrics *metrics = new MqttMetrics();
  return *metrics;
}

prometheus::Labels panel_labels_map(const ManagerMetrics::PanelLabels &panel) {
  return {{"panel", panel.panel}, {"mac", panel.mac}, {"nspm_id", panel.nspm_id}};
}

struct PanelMetrics {
  prometheus::Family<prometheus::Counter> &status_reports = counter_family("nspm_panel_status_reports_total", "Status reports received from the panel.");
  prometheus::Family<prometheus::Gauge> &last_status_report = gauge_family("nspm_panel_last_status_report_timestamp_seconds", "When the last status report from the panel was received, in seconds since the epoch.");
  prometheus::Family<prometheus::Counter> &register_requests = counter_family("nspm_panel_register_requests_total", "register_request messages received from the panel.");
  prometheus::Family<prometheus::Counter> &configs_sent = counter_family("nspm_panel_configs_sent_total", "Configs published to the panel.");
  prometheus::Family<prometheus::Counter> &config_bytes = counter_family("nspm_panel_config_bytes_total", "Bytes of config published to the panel.");

  // Labels last seen for each panel, by MAC, to remove the old series when they change.
  std::mutex labels_mutex;
  std::map<std::string, ManagerMetrics::PanelLabels> labels_by_mac;
  std::vector<std::function<void(const ManagerMetrics::PanelLabels &)>> removed_callbacks;

  // Remove every series under these labels. Family::Add returns the existing child, or creates one that is removed straight away.
  void remove(const ManagerMetrics::PanelLabels &old_labels) {
    prometheus::Labels labels = panel_labels_map(old_labels);
    this->status_reports.Remove(&this->status_reports.Add(labels));
    this->last_status_report.Remove(&this->last_status_report.Add(labels));
    this->register_requests.Remove(&this->register_requests.Add(labels));
    this->configs_sent.Remove(&this->configs_sent.Add(labels));
    this->config_bytes.Remove(&this->config_bytes.Add(labels));
  }

  // Record the panel's labels and remove its old series if they changed. Returns the old labels if they were removed.
  std::optional<ManagerMetrics::PanelLabels> update(const ManagerMetrics::PanelLabels &panel) {
    std::lock_guard<std::mutex> lock_guard(this->labels_mutex);
    auto [it, inserted] = this->labels_by_mac.try_emplace(panel.mac, panel);
    if (inserted || it->second == panel) {
      return std::nullopt;
    }
    ManagerMetrics::PanelLabels old_labels = it->second;
    it->second = panel;
    this->remove(old_labels);
    return old_labels;
  }

  void notify_removed(const ManagerMetrics::PanelLabels &old_labels) {
    std::vector<std::function<void(const ManagerMetrics::PanelLabels &)>> callbacks;
    {
      std::lock_guard<std::mutex> lock_guard(this->labels_mutex);
      callbacks = this->removed_callbacks;
    }
    for (const auto &callback : callbacks) {
      callback(old_labels);
    }
  }
};

PanelMetrics &panels() {
  static PanelMetrics *metrics = new PanelMetrics();
  return *metrics;
}

struct IntegrationMetrics {
  prometheus::Gauge &connected;
  prometheus::Gauge &authenticated;
  prometheus::Counter &reconnects;
  prometheus::Counter *auth_failures; // nullptr where the integration has no separate authentication step.
  prometheus::Counter &messages_received;
  prometheus::Counter &messages_sent;
  std::atomic<bool> has_connected = false;

  IntegrationMetrics(const std::string &prefix, const std::string &name, bool with_auth_failures)
      : connected(gauge_family(prefix + "_connected", "Whether the " + name + " websocket is connected.").Add({})),
        authenticated(gauge_family(prefix + "_authenticated", "Whether MQTTManager is authenticated to " + name + ".").Add({})),
        reconnects(counter_family(prefix + "_reconnects_total", "Times the " + name + " websocket connected again after its first connection.").Add({})),
        auth_failures(with_auth_failures ? &counter_family(prefix + "_auth_failures_total", name + " authentication attempts that were rejected.").Add({}) : nullptr),
        messages_received(counter_family(prefix + "_messages_received_total", "Websocket messages received from " + name + ".").Add({})),
        messages_sent(counter_family(prefix + "_messages_sent_total", "Websocket messages sent to " + name + ".").Add({})) {}
};

IntegrationMetrics &integration(ManagerMetrics::Integration which) {
  static IntegrationMetrics *home_assistant = new IntegrationMetrics("nspm_home_assistant", "Home Assistant", true);
  // OpenHAB takes its token in the URL, a bad one shows up as a failed connection rather than an auth reply.
  static IntegrationMetrics *openhab = new IntegrationMetrics("nspm_openhab", "OpenHAB", false);
  return which == ManagerMetrics::Integration::HOME_ASSISTANT ? *home_assistant : *openhab;
}

struct WebsocketMetrics {
  prometheus::Gauge &clients = gauge_family("nspm_websocket_clients", "Web interface STOMP websocket clients connected.").Add({});
  prometheus::Family<prometheus::Counter> &messages = counter_family("nspm_websocket_messages_total", "Web interface websocket messages, by direction (STOMP heartbeats excluded).");
  prometheus::Counter &received = messages.Add({{"direction", "received"}});
  prometheus::Counter &sent = messages.Add({{"direction", "sent"}});
};

WebsocketMetrics &websocket() {
  static WebsocketMetrics *metrics = new WebsocketMetrics();
  return *metrics;
}

const char *http_route_label(ManagerMetrics::HttpRoute route) {
  switch (route) {
  case ManagerMetrics::HttpRoute::IMAGE:
    return "image";
  case ManagerMetrics::HttpRoute::TARGETS:
    return "targets";
  case ManagerMetrics::HttpRoute::METRICS:
    return "metrics";
  case ManagerMetrics::HttpRoute::OTHER:
    return "other";
  }
  return "other";
}

} // namespace

void ManagerMetrics::init() {
  mqtt();
  panels();
  integration(Integration::HOME_ASSISTANT);
  integration(Integration::OPENHAB);
  websocket();
  ManagerMetrics::warnings_active({0, 0, 0, 0});
}

void ManagerMetrics::mqtt_connection_changed(bool connected) {
  MqttMetrics &metrics = mqtt();
  metrics.connected.Set(connected ? 1 : 0);
  bool was_connected = metrics.was_connected.exchange(connected);
  if (connected && !was_connected) {
    metrics.connects.Increment();
  } else if (!connected && was_connected) {
    metrics.disconnects.Increment();
  }
}

void ManagerMetrics::mqtt_message_received(const std::string &topic, size_t bytes) {
  MqttMetrics &metrics = mqtt();
  size_t index = static_cast<size_t>(Metrics::classify_topic(topic));
  metrics.received_messages[index]->Increment();
  metrics.received_bytes[index]->Increment(static_cast<double>(bytes));
}

void ManagerMetrics::mqtt_message_published(bool retained, size_t bytes) {
  MqttMetrics &metrics = mqtt();
  metrics.published_messages[retained ? 1 : 0]->Increment();
  metrics.published_bytes[retained ? 1 : 0]->Increment(static_cast<double>(bytes));
}

void ManagerMetrics::mqtt_publish_error() {
  mqtt().publish_errors.Increment();
}

void ManagerMetrics::mqtt_buffered_messages(size_t count) {
  mqtt().buffered_messages.Set(static_cast<double>(count));
}

void ManagerMetrics::mqtt_retained_topics(size_t count) {
  mqtt().retained_topics.Set(static_cast<double>(count));
}

void ManagerMetrics::panel_labels(const PanelLabels &panel) {
  PanelMetrics &metrics = panels();
  if (auto old_labels = metrics.update(panel)) {
    metrics.notify_removed(*old_labels);
  }
}

void ManagerMetrics::panel_removed(const std::string &mac) {
  PanelMetrics &metrics = panels();
  std::optional<PanelLabels> old_labels;
  {
    std::lock_guard<std::mutex> lock_guard(metrics.labels_mutex);
    auto it = metrics.labels_by_mac.find(mac);
    if (it != metrics.labels_by_mac.end()) {
      old_labels = it->second;
      metrics.labels_by_mac.erase(it);
      metrics.remove(*old_labels);
    }
  }
  if (old_labels) {
    metrics.notify_removed(*old_labels);
  }
}

void ManagerMetrics::on_panel_labels_removed(std::function<void(const PanelLabels &old_labels)> callback) {
  PanelMetrics &metrics = panels();
  std::lock_guard<std::mutex> lock_guard(metrics.labels_mutex);
  metrics.removed_callbacks.push_back(std::move(callback));
}

void ManagerMetrics::panel_status_report(const PanelLabels &panel) {
  ManagerMetrics::panel_labels(panel);
  PanelMetrics &metrics = panels();
  prometheus::Labels labels = panel_labels_map(panel);
  metrics.status_reports.Add(labels).Increment();
  metrics.last_status_report.Add(labels).SetToCurrentTime();
}

void ManagerMetrics::panel_register_request(const PanelLabels &panel) {
  ManagerMetrics::panel_labels(panel);
  panels().register_requests.Add(panel_labels_map(panel)).Increment();
}

void ManagerMetrics::panel_config_sent(const PanelLabels &panel, size_t bytes) {
  ManagerMetrics::panel_labels(panel);
  PanelMetrics &metrics = panels();
  prometheus::Labels labels = panel_labels_map(panel);
  metrics.configs_sent.Add(labels).Increment();
  metrics.config_bytes.Add(labels).Increment(static_cast<double>(bytes));
}

void ManagerMetrics::integration_connection_changed(Integration which, bool connected) {
  IntegrationMetrics &metrics = integration(which);
  metrics.connected.Set(connected ? 1 : 0);
  if (!connected) {
    metrics.authenticated.Set(0);
  } else if (metrics.has_connected.exchange(true)) {
    metrics.reconnects.Increment();
  }
}

void ManagerMetrics::integration_authenticated(Integration which, bool authenticated) {
  integration(which).authenticated.Set(authenticated ? 1 : 0);
}

void ManagerMetrics::integration_auth_failure(Integration which) {
  IntegrationMetrics &metrics = integration(which);
  metrics.authenticated.Set(0);
  if (metrics.auth_failures != nullptr) {
    metrics.auth_failures->Increment();
  }
}

void ManagerMetrics::integration_message_received(Integration which) {
  integration(which).messages_received.Increment();
}

void ManagerMetrics::integration_message_sent(Integration which) {
  integration(which).messages_sent.Increment();
}

void ManagerMetrics::websocket_clients(size_t count) {
  websocket().clients.Set(static_cast<double>(count));
}

void ManagerMetrics::websocket_message_received() {
  websocket().received.Increment();
}

void ManagerMetrics::websocket_message_sent() {
  websocket().sent.Increment();
}

ManagerMetrics::HttpRoute ManagerMetrics::http_route(const std::string &uri) {
  std::string path = uri.substr(0, uri.find('?'));
  if (path.rfind("/nextion-img/", 0) == 0) {
    return HttpRoute::IMAGE;
  }
  if (path == "/prometheus/targets") {
    return HttpRoute::TARGETS;
  }
  if (path == "/prometheus/metrics") {
    return HttpRoute::METRICS;
  }
  return HttpRoute::OTHER;
}

void ManagerMetrics::http_request(HttpRoute route, int status_code) {
  static prometheus::Family<prometheus::Counter> *family = &counter_family("nspm_http_requests_total", "Requests to the manager's HTTP server, by route and status code.");
  family->Add({{"route", http_route_label(route)}, {"code", std::to_string(status_code)}}).Increment();
}

void ManagerMetrics::warnings_active(const std::array<size_t, 4> &count_by_level) {
  static prometheus::Family<prometheus::Gauge> *family = &gauge_family("nspm_warnings_active", "Active manager warnings shown in the web interface, by level.");
  static const std::array<const char *, 4> levels = {"error", "warning", "info", "debug"};
  for (size_t i = 0; i < levels.size(); i++) {
    family->Add({{"level", levels[i]}}).Set(static_cast<double>(count_by_level[i]));
  }
}
