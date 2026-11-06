#include "metrics.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <prometheus/client_metric.h>
#include <prometheus/gauge.h>
#include <prometheus/metric_family.h>
#include <prometheus/text_serializer.h>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

// Set by CMake (NSPM_VERSION / NSPM_GIT_SHA cache variables), "unknown" when not given.
#ifndef NSPM_VERSION
#define NSPM_VERSION "unknown"
#endif
#ifndef NSPM_GIT_SHA
#define NSPM_GIT_SHA "unknown"
#endif

namespace {

/**
 * process_* metrics read from /proc/self, as the Go and Python clients export them.
 * prometheus-cpp has no process collector of its own.
 */
class ProcessCollector : public prometheus::Collectable {
public:
  std::vector<prometheus::MetricFamily> Collect() const override {
    std::vector<prometheus::MetricFamily> families;
    const double ticks_per_second = static_cast<double>(sysconf(_SC_CLK_TCK));
    const double page_size = static_cast<double>(sysconf(_SC_PAGESIZE));

    std::ifstream stat_file("/proc/self/stat");
    std::string stat;
    std::getline(stat_file, stat);
    // The second field, the command name, is in parentheses and may contain spaces, so start after its closing parenthesis.
    size_t comm_end = stat.rfind(')');
    if (comm_end != std::string::npos && ticks_per_second > 0) {
      std::istringstream fields(stat.substr(comm_end + 1));
      // Fields from 3 (state) onwards, see proc(5).
      std::vector<std::string> values;
      std::string value;
      while (fields >> value) {
        values.push_back(value);
      }
      // Index in values = field number - 3.
      auto field = [&values](size_t number) -> double {
        size_t index = number - 3;
        return index < values.size() ? std::strtod(values[index].c_str(), nullptr) : 0.0;
      };
      if (values.size() >= 22) {
        double cpu_seconds = (field(14) + field(15)) / ticks_per_second; // utime + stime
        families.push_back(ProcessCollector::_family("process_cpu_seconds_total", "Total user and system CPU time spent in seconds.", prometheus::MetricType::Counter, cpu_seconds));
        families.push_back(ProcessCollector::_family("process_threads", "Number of OS threads in the process.", prometheus::MetricType::Gauge, field(20)));
        double boot_time = ProcessCollector::_boot_time();
        if (boot_time > 0) {
          families.push_back(ProcessCollector::_family("process_start_time_seconds", "Start time of the process since unix epoch in seconds.", prometheus::MetricType::Gauge, boot_time + field(22) / ticks_per_second));
        }
        families.push_back(ProcessCollector::_family("process_virtual_memory_bytes", "Virtual memory size in bytes.", prometheus::MetricType::Gauge, field(23)));
        families.push_back(ProcessCollector::_family("process_resident_memory_bytes", "Resident memory size in bytes.", prometheus::MetricType::Gauge, field(24) * page_size));
      }
    }

    std::error_code error;
    size_t open_fds = 0;
    for (auto it = std::filesystem::directory_iterator("/proc/self/fd", error); !error && it != std::filesystem::directory_iterator(); it.increment(error)) {
      open_fds++;
    }
    if (!error) {
      // The iterator's own descriptor is counted too, as in the Go client.
      families.push_back(ProcessCollector::_family("process_open_fds", "Number of open file descriptors.", prometheus::MetricType::Gauge, static_cast<double>(open_fds)));
    }
    return families;
  }

private:
  static prometheus::MetricFamily _family(const std::string &name, const std::string &help, prometheus::MetricType type, double value) {
    prometheus::ClientMetric metric;
    if (type == prometheus::MetricType::Counter) {
      metric.counter.value = value;
    } else {
      metric.gauge.value = value;
    }
    return prometheus::MetricFamily{.name = name, .help = help, .type = type, .metric = {metric}};
  }

  // System boot time in seconds since the epoch, from the btime line of /proc/stat. 0 if unknown.
  static double _boot_time() {
    static const double boot_time = []() {
      std::ifstream stat_file("/proc/stat");
      std::string line;
      while (std::getline(stat_file, line)) {
        if (line.rfind("btime ", 0) == 0) {
          return std::strtod(line.c_str() + 6, nullptr);
        }
      }
      return 0.0;
    }();
    return boot_time;
  }
};

std::mutex collectables_mutex;
std::vector<std::weak_ptr<prometheus::Collectable>> collectables;

bool ends_with(std::string_view value, std::string_view suffix) {
  return value.size() >= suffix.size() && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool starts_with(std::string_view value, std::string_view prefix) {
  return value.compare(0, prefix.size(), prefix) == 0;
}

} // namespace

prometheus::Registry &Metrics::registry() {
  static prometheus::Registry *registry = []() {
    // Never destroyed: metrics are incremented from threads that may outlive static destruction at exit.
    auto *registry = new prometheus::Registry();
    prometheus::BuildGauge()
        .Name("nspm_build_info")
        .Help("MQTTManager build information. Always 1.")
        .Register(*registry)
        .Add({{"version", NSPM_VERSION}, {"git_sha", NSPM_GIT_SHA}})
        .Set(1);
    return registry;
  }();
  return *registry;
}

void Metrics::add_collectable(const std::shared_ptr<prometheus::Collectable> &collectable) {
  std::lock_guard<std::mutex> lock_guard(collectables_mutex);
  collectables.push_back(collectable);
}

std::string Metrics::serialize() {
  static const ProcessCollector process_collector;

  std::vector<prometheus::MetricFamily> families = Metrics::registry().Collect();
  std::vector<prometheus::MetricFamily> process_families = process_collector.Collect();
  families.insert(families.end(), std::make_move_iterator(process_families.begin()), std::make_move_iterator(process_families.end()));

  std::vector<std::shared_ptr<prometheus::Collectable>> live_collectables;
  {
    std::lock_guard<std::mutex> lock_guard(collectables_mutex);
    collectables.erase(std::remove_if(collectables.begin(), collectables.end(), [](const auto &collectable) { return collectable.expired(); }), collectables.end());
    for (const auto &collectable : collectables) {
      if (auto locked = collectable.lock()) {
        live_collectables.push_back(std::move(locked));
      }
    }
  }
  // Collected outside the lock: a collectable may read state guarded by other mutexes.
  for (const auto &collectable : live_collectables) {
    std::vector<prometheus::MetricFamily> collected = collectable->Collect();
    families.insert(families.end(), std::make_move_iterator(collected.begin()), std::make_move_iterator(collected.end()));
  }

  return prometheus::TextSerializer().Serialize(families);
}

Metrics::TopicClass Metrics::classify_topic(const std::string &topic) {
  if (starts_with(topic, "homeassistant/")) {
    return TopicClass::HA_DISCOVERY;
  }
  if (ends_with(topic, "/status_report")) {
    return TopicClass::STATUS_REPORT;
  }
  if (ends_with(topic, "/log") || ends_with(topic, "/log_backlog")) {
    return TopicClass::LOG;
  }
  // Panels send register_request (and nothing else the manager acts on) to the shared manager topic.
  if (topic == "nspanel/mqttmanager/command") {
    return TopicClass::REGISTER;
  }
  if (ends_with(topic, "/command") || ends_with(topic, "_cmd")) {
    return TopicClass::COMMAND;
  }
  if (ends_with(topic, "/config") || starts_with(topic, "nspanel/config/")) {
    return TopicClass::CONFIG;
  }
  return TopicClass::OTHER;
}

const char *Metrics::topic_class_label(TopicClass topic_class) {
  switch (topic_class) {
  case TopicClass::STATUS_REPORT:
    return "status_report";
  case TopicClass::LOG:
    return "log";
  case TopicClass::COMMAND:
    return "command";
  case TopicClass::CONFIG:
    return "config";
  case TopicClass::REGISTER:
    return "register";
  case TopicClass::HA_DISCOVERY:
    return "ha_discovery";
  case TopicClass::OTHER:
    return "other";
  }
  return "other";
}

const prometheus::Histogram::BucketBoundaries &Metrics::latency_buckets() {
  static const prometheus::Histogram::BucketBoundaries buckets = {0.0005, 0.001, 0.0025, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1, 2.5, 5, 10};
  return buckets;
}

const prometheus::Histogram::BucketBoundaries &Metrics::fast_buckets() {
  static const prometheus::Histogram::BucketBoundaries buckets = {0.00005, 0.0001, 0.00025, 0.0005, 0.001, 0.0025, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5};
  return buckets;
}
