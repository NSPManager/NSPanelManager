#include <array>
#include <cctype>
#include <metrics/ha_call_tracker.hpp>
#include <metrics/latency_metrics.hpp>
#include <map>
#include <mutex>
#include <optional>
#include <vector>
#include <prometheus/counter.h>
#include <prometheus/family.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>

namespace {

// OTA updates take minutes: a TFT over UART can take a quarter of an hour.
const prometheus::Histogram::BucketBoundaries &update_buckets() {
  static const prometheus::Histogram::BucketBoundaries buckets = {5, 10, 30, 60, 120, 300, 600, 900, 1200, 1800, 3600};
  return buckets;
}

// One child per topic class, looked up once since these are on the path of every MQTT message.
struct MqttQueueMetrics {
  prometheus::Gauge *depth;
  std::array<prometheus::Histogram *, Metrics::topic_class_count> wait;
  std::array<prometheus::Histogram *, Metrics::topic_class_count> handler_duration;
  std::array<prometheus::Counter *, Metrics::topic_class_count> handler_exceptions;
  std::array<prometheus::Counter *, Metrics::topic_class_count> dropped;

  static MqttQueueMetrics &get() {
    static MqttQueueMetrics metrics = []() {
      auto &registry = Metrics::registry();
      auto &depth = prometheus::BuildGauge()
                        .Name("nspm_mqtt_queue_depth")
                        .Help("MQTT messages received and waiting to be processed.")
                        .Register(registry);
      auto &wait = prometheus::BuildHistogram()
                       .Name("nspm_mqtt_queue_wait_seconds")
                       .Help("Time a received MQTT message waited in the queue before its callbacks ran.")
                       .Register(registry);
      auto &duration = prometheus::BuildHistogram()
                           .Name("nspm_mqtt_handler_duration_seconds")
                           .Help("Time the callbacks for a received MQTT message took.")
                           .Register(registry);
      auto &exceptions = prometheus::BuildCounter()
                             .Name("nspm_mqtt_handler_exceptions_total")
                             .Help("Received MQTT messages whose callbacks ended in an exception.")
                             .Register(registry);
      auto &dropped = prometheus::BuildCounter()
                          .Name("nspm_mqtt_messages_dropped_total")
                          .Help("Received MQTT messages dropped because the processing queue was full.")
                          .Register(registry);
      MqttQueueMetrics result;
      result.depth = &depth.Add({});
      for (size_t i = 0; i < Metrics::topic_class_count; i++) {
        prometheus::Labels labels = {{"topic_class", Metrics::topic_class_label(static_cast<Metrics::TopicClass>(i))}};
        result.wait[i] = &wait.Add(labels, Metrics::fast_buckets());
        result.handler_duration[i] = &duration.Add(labels, Metrics::latency_buckets());
        result.handler_exceptions[i] = &exceptions.Add(labels);
        result.dropped[i] = &dropped.Add(labels);
      }
      return result;
    }();
    return metrics;
  }
};

HaCallTracker &ha_call_tracker() {
  static HaCallTracker tracker;
  return tracker;
}

// Text before the first '.', eg. "light" for "light.kitchen".
std::string entity_domain(const std::string &entity_id) {
  size_t dot = entity_id.find('.');
  if (dot == std::string::npos || dot == 0) {
    return "unknown";
  }
  return entity_id.substr(0, dot);
}

struct CommandInFlight {
  LatencyMetrics::clock::time_point started;
  std::string type;
  bool observed = false;
};

thread_local std::optional<CommandInFlight> command_in_flight;

} // namespace

void LatencyMetrics::mqtt_message_enqueued() {
  MqttQueueMetrics::get().depth->Increment();
}

void LatencyMetrics::mqtt_message_dropped(Metrics::TopicClass topic_class) {
  MqttQueueMetrics::get().dropped[static_cast<size_t>(topic_class)]->Increment();
}

void LatencyMetrics::mqtt_message_dequeued(Metrics::TopicClass topic_class, double wait_seconds) {
  auto &metrics = MqttQueueMetrics::get();
  metrics.depth->Decrement();
  metrics.wait[static_cast<size_t>(topic_class)]->Observe(wait_seconds);
}

void LatencyMetrics::mqtt_handler_finished(Metrics::TopicClass topic_class, double seconds, bool threw) {
  auto &metrics = MqttQueueMetrics::get();
  metrics.handler_duration[static_cast<size_t>(topic_class)]->Observe(seconds);
  if (threw) {
    metrics.handler_exceptions[static_cast<size_t>(topic_class)]->Increment();
  }
}

void LatencyMetrics::panel_config_built(double seconds) {
  static prometheus::Histogram &histogram = prometheus::BuildHistogram()
                                                .Name("nspm_panel_config_build_seconds")
                                                .Help("Time to build and serialise an NSPanelConfig for a panel.")
                                                .Register(Metrics::registry())
                                                .Add({}, Metrics::fast_buckets());
  histogram.Observe(seconds);
}

void LatencyMetrics::home_assistant_request_sent(long id, const std::string &type, const std::string &domain) {
  if (type.compare("call_service") == 0) {
    ha_call_tracker().start(id, domain.empty() ? "unknown" : domain);
  }
}

void LatencyMetrics::home_assistant_result_received(long id, bool success) {
  static auto &family = prometheus::BuildHistogram()
                            .Name("nspm_home_assistant_call_duration_seconds")
                            .Help("Time from sending a call_service request to Home Assistant to receiving its result.")
                            .Register(Metrics::registry());
  static auto &failures = prometheus::BuildCounter()
                              .Name("nspm_home_assistant_call_failures_total")
                              .Help("call_service requests to Home Assistant whose result reported success: false.")
                              .Register(Metrics::registry());
  auto result = ha_call_tracker().finish(id);
  if (result) {
    family.Add({{"domain", result->domain}}, Metrics::latency_buckets()).Observe(result->seconds);
    if (!success) {
      failures.Add({{"domain", result->domain}}).Increment();
    }
  }
}

void LatencyMetrics::home_assistant_disconnected() {
  ha_call_tracker().clear();
}

void LatencyMetrics::home_assistant_state_changed(const std::string &entity_id) {
  static auto &family = prometheus::BuildCounter()
                            .Name("nspm_home_assistant_state_changes_total")
                            .Help("state_changed events received from Home Assistant.")
                            .Register(Metrics::registry());
  family.Add({{"domain", entity_domain(entity_id)}}).Increment();
}

LatencyMetrics::CommandScope::CommandScope(const std::string &type) {
  command_in_flight = CommandInFlight{.started = LatencyMetrics::clock::now(), .type = type};
}

LatencyMetrics::CommandScope::~CommandScope() {
  command_in_flight.reset();
}

void LatencyMetrics::integration_call_issued() {
  if (!command_in_flight || command_in_flight->observed) {
    return;
  }
  static auto &family = prometheus::BuildHistogram()
                            .Name("nspm_panel_command_duration_seconds")
                            .Help("Time from a panel command arriving to the first call to Home Assistant or OpenHAB it caused.")
                            .Register(Metrics::registry());
  command_in_flight->observed = true;
  family.Add({{"type", command_in_flight->type}}, Metrics::fast_buckets()).Observe(LatencyMetrics::seconds_since(command_in_flight->started));
}

namespace {

/**
 * Per-panel series of nspm_panel_log_lines_total, keyed by MAC, so that the series under a panel's
 * old name or id can be removed when it changes, and all of them when the panel is deleted.
 */
struct PanelLogLines {
  struct Panel {
    std::string name;
    uint32_t nspm_id;
    std::map<std::string, prometheus::Counter *> by_level;
  };

  std::mutex mutex;
  std::map<std::string, Panel> panels;

  static PanelLogLines &get() {
    static PanelLogLines instance;
    return instance;
  }

  static prometheus::Family<prometheus::Counter> &family() {
    static auto &family = prometheus::BuildCounter()
                              .Name("nspm_panel_log_lines_total")
                              .Help("Log lines received from a panel over MQTT, by level.")
                              .Register(Metrics::registry());
    return family;
  }

  // Called with mutex held.
  void remove(Panel &panel) {
    for (auto &[level, counter] : panel.by_level) {
      PanelLogLines::family().Remove(counter);
    }
    panel.by_level.clear();
  }
};

} // namespace

void LatencyMetrics::panel_log_line(const std::string &panel, const std::string &mac, uint32_t nspm_id, const char *level) {
  auto &log_lines = PanelLogLines::get();
  std::lock_guard<std::mutex> lock_guard(log_lines.mutex);
  auto [it, inserted] = log_lines.panels.try_emplace(mac, PanelLogLines::Panel{.name = panel, .nspm_id = nspm_id, .by_level = {}});
  if (!inserted && (it->second.name != panel || it->second.nspm_id != nspm_id)) {
    log_lines.remove(it->second);
    it->second.name = panel;
    it->second.nspm_id = nspm_id;
  }
  auto counter = it->second.by_level.find(level);
  if (counter == it->second.by_level.end()) {
    prometheus::Counter *added = &PanelLogLines::family().Add({{"panel", panel}, {"mac", mac}, {"nspm_id", std::to_string(nspm_id)}, {"level", level}});
    counter = it->second.by_level.emplace(level, added).first;
  }
  counter->second->Increment();
}

void LatencyMetrics::panel_removed(const std::string &mac) {
  auto &log_lines = PanelLogLines::get();
  std::lock_guard<std::mutex> lock_guard(log_lines.mutex);
  auto it = log_lines.panels.find(mac);
  if (it != log_lines.panels.end()) {
    log_lines.remove(it->second);
    log_lines.panels.erase(it);
  }
}

const char *LatencyMetrics::panel_log_level(const std::string &level) {
  if (level.empty()) {
    return "unknown";
  }
  switch (std::toupper(static_cast<unsigned char>(level[0]))) {
  case 'E':
    return "error";
  case 'W':
    return "warning";
  case 'I':
    return "info";
  case 'D':
    return "debug";
  default:
    return "unknown";
  }
}

void LatencyMetrics::panel_update(const char *type, const char *outcome) {
  static auto &family = prometheus::BuildCounter()
                            .Name("nspm_panel_updates_total")
                            .Help("Panel OTA updates by outcome: requested (command sent), started (panel reported updating), succeeded, failed.")
                            .Register(Metrics::registry());
  family.Add({{"type", type}, {"outcome", outcome}}).Increment();
}

void LatencyMetrics::panel_update_duration(const char *type, double seconds) {
  static auto &family = prometheus::BuildHistogram()
                            .Name("nspm_panel_update_duration_seconds")
                            .Help("Time a panel reported being in the updating state for an image type.")
                            .Register(Metrics::registry());
  family.Add({{"type", type}}, update_buckets()).Observe(seconds);
}

void LatencyMetrics::image_request(bool cache_hit, const char *format) {
  static auto &family = prometheus::BuildCounter()
                            .Name("nspm_image_requests_total")
                            .Help("Album art requests that reached the converted image cache.")
                            .Register(Metrics::registry());
  family.Add({{"cache", cache_hit ? "hit" : "miss"}, {"format", format}}).Increment();
}

void LatencyMetrics::image_converted(double seconds) {
  static prometheus::Histogram &histogram = prometheus::BuildHistogram()
                                                .Name("nspm_image_convert_duration_seconds")
                                                .Help("Time to decode, resize and encode an album art image.")
                                                .Register(Metrics::registry())
                                                .Add({}, Metrics::latency_buckets());
  histogram.Observe(seconds);
}

void LatencyMetrics::weather_fetch(const char *outcome) {
  static auto &fetches = prometheus::BuildCounter()
                             .Name("nspm_weather_fetches_total")
                             .Help("Weather forecast fetches from Open-Meteo by outcome.")
                             .Register(Metrics::registry());
  static prometheus::Gauge &last_success = prometheus::BuildGauge()
                                               .Name("nspm_weather_last_success_timestamp_seconds")
                                               .Help("Unix time of the last successful weather fetch.")
                                               .Register(Metrics::registry())
                                               .Add({});
  fetches.Add({{"outcome", outcome}}).Increment();
  if (std::string(outcome).compare("success") == 0) {
    last_success.SetToCurrentTime();
  }
}

void LatencyMetrics::clock_broadcast() {
  static prometheus::Counter &counter = prometheus::BuildCounter()
                                            .Name("nspm_clock_broadcasts_total")
                                            .Help("Times the current time was sent to the panels.")
                                            .Register(Metrics::registry())
                                            .Add({});
  counter.Increment();
}
