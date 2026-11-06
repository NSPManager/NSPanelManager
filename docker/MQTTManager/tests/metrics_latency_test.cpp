#include <gtest/gtest.h>
#include <metrics/latency_metrics.hpp>
#include <metrics/metrics.hpp>
#include <string>

namespace {

bool exposition_contains(const std::string &text) {
  return Metrics::serialize().find(text) != std::string::npos;
}

} // namespace

TEST(LatencyMetricsTest, home_assistant_call_is_timed_from_request_to_result) {
  LatencyMetrics::home_assistant_request_sent(900001, "call_service", "test_call_domain");
  LatencyMetrics::home_assistant_request_sent(900002, "get_states", "");
  LatencyMetrics::home_assistant_result_received(900001);
  LatencyMetrics::home_assistant_result_received(900002); // Not timed: not a call_service.
  LatencyMetrics::home_assistant_result_received(900001); // Already finished.
  EXPECT_TRUE(exposition_contains("nspm_home_assistant_call_duration_seconds_count{domain=\"test_call_domain\"} 1"));
}

TEST(LatencyMetricsTest, panel_command_observes_only_the_first_integration_call) {
  LatencyMetrics::integration_call_issued(); // Outside a command: ignored.
  {
    LatencyMetrics::CommandScope scope("test_command_type");
    LatencyMetrics::integration_call_issued();
    LatencyMetrics::integration_call_issued();
  }
  LatencyMetrics::integration_call_issued();
  EXPECT_TRUE(exposition_contains("nspm_panel_command_duration_seconds_count{type=\"test_command_type\"} 1"));
}

TEST(LatencyMetricsTest, state_changes_are_counted_by_domain) {
  LatencyMetrics::home_assistant_state_changed("testdomain.kitchen");
  LatencyMetrics::home_assistant_state_changed("testdomain.office");
  EXPECT_TRUE(exposition_contains("nspm_home_assistant_state_changes_total{domain=\"testdomain\"} 2"));
}

TEST(LatencyMetricsTest, panel_log_levels_are_normalised) {
  EXPECT_STREQ(LatencyMetrics::panel_log_level("E"), "error");
  EXPECT_STREQ(LatencyMetrics::panel_log_level("WARNING"), "warning");
  EXPECT_STREQ(LatencyMetrics::panel_log_level("info"), "info");
  EXPECT_STREQ(LatencyMetrics::panel_log_level("D"), "debug");
  EXPECT_STREQ(LatencyMetrics::panel_log_level("V"), "unknown");
  EXPECT_STREQ(LatencyMetrics::panel_log_level(""), "unknown");
}

TEST(LatencyMetricsTest, mqtt_queue_depth_follows_enqueue_and_dequeue) {
  LatencyMetrics::mqtt_message_enqueued();
  LatencyMetrics::mqtt_message_enqueued();
  LatencyMetrics::mqtt_message_dequeued(Metrics::TopicClass::OTHER, 0.001);
  // Other tests may have pushed real messages through MQTT_Manager, so only check the series exist.
  EXPECT_TRUE(exposition_contains("nspm_mqtt_queue_depth "));
  EXPECT_TRUE(exposition_contains("nspm_mqtt_queue_wait_seconds_bucket{topic_class=\"other\""));
  LatencyMetrics::mqtt_message_dequeued(Metrics::TopicClass::OTHER, 0.001);
}
