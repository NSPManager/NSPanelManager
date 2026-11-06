#ifndef MQTTMANAGER_METRICS_HPP
#define MQTTMANAGER_METRICS_HPP

#include <memory>
#include <prometheus/collectable.h>
#include <prometheus/histogram.h>
#include <prometheus/registry.h>
#include <string>

/**
 * Prometheus metrics for MQTTManager.
 *
 * Every metric lives in one registry, which the manager's HTTP server (NextionImageServer) serves as
 * text at GET /prometheus/metrics. Only prometheus-cpp's core library is used: there is no
 * prometheus-cpp HTTP server, the text is returned from our own handler.
 *
 * Rules (see METRICS.md):
 *  - Names start with nspm_ (process_* for the process collector).
 *  - No per-entity, per-topic or per-IP labels. Classify MQTT topics with classify_topic() first.
 *  - md5s only on *_info gauges with the value 1.
 *
 * Counters and gauges are thread safe, so they can be incremented from any thread without a lock.
 * Look a labelled child up once (Family::Add) and keep the reference where the code is hot.
 */
class Metrics {
public:
  /**
   * The registry every metric is registered in.
   */
  static prometheus::Registry &registry();

  /**
   * The text exposition format (version 0.0.4) of the registry, the /proc/self process collector and
   * every collectable added with add_collectable().
   */
  static std::string serialize();

  /**
   * Add a collectable that is collected at serialize time, after the registry. Use this for values that
   * are cheaper to sample at scrape time than to keep in sync, and for series whose labels come and go
   * (panels being added, renamed or deleted). Held weakly: it is dropped once the collectable is destroyed.
   */
  static void add_collectable(const std::shared_ptr<prometheus::Collectable> &collectable);

  /**
   * The fixed set of MQTT topic classes used as the topic_class label.
   */
  enum class TopicClass {
    STATUS_REPORT,
    LOG,
    COMMAND,
    CONFIG,
    REGISTER,
    HA_DISCOVERY,
    OTHER,
  };

  /**
   * Number of TopicClass values, for arrays indexed by topic class.
   */
  static constexpr size_t topic_class_count = 7;

  /**
   * Classify an MQTT topic into a TopicClass.
   */
  static TopicClass classify_topic(const std::string &topic);

  /**
   * The label value of a topic class: status_report, log, command, config, register, ha_discovery or other.
   */
  static const char *topic_class_label(TopicClass topic_class);

  /**
   * Histogram buckets in seconds for operations that may take a while (network round trips, handlers):
   * 0.5 ms to 10 s.
   */
  static const prometheus::Histogram::BucketBoundaries &latency_buckets();

  /**
   * Histogram buckets in seconds for operations that should be quick (queue waits, building a message):
   * 50 us to 0.5 s.
   */
  static const prometheus::Histogram::BucketBoundaries &fast_buckets();
};

#endif
