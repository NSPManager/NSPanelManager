#include <gtest/gtest.h>
#include <metrics/latency_metrics.hpp>
#include <metrics/manager_metrics.hpp>
#include <metrics/metrics.hpp>
#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace {

bool exposition_contains(const std::string &text) {
  return Metrics::serialize().find(text) != std::string::npos;
}

} // namespace

TEST(LatencyMetricsTest, home_assistant_call_is_timed_from_request_to_result) {
  LatencyMetrics::home_assistant_request_sent(900001, "call_service", "test_call_domain");
  LatencyMetrics::home_assistant_request_sent(900002, "get_states", "");
  LatencyMetrics::home_assistant_result_received(900001, true);
  LatencyMetrics::home_assistant_result_received(900002, true); // Not timed: not a call_service.
  LatencyMetrics::home_assistant_result_received(900001, true); // Already finished.
  EXPECT_TRUE(exposition_contains("nspm_home_assistant_call_duration_seconds_count{domain=\"test_call_domain\"} 1"));
  EXPECT_FALSE(exposition_contains("nspm_home_assistant_call_failures_total{domain=\"test_call_domain\"}"));
}

TEST(LatencyMetricsTest, failed_home_assistant_calls_are_counted_by_domain) {
  LatencyMetrics::home_assistant_request_sent(900011, "call_service", "test_failing_domain");
  LatencyMetrics::home_assistant_request_sent(900012, "call_service", "test_failing_domain");
  LatencyMetrics::home_assistant_result_received(900011, false);
  LatencyMetrics::home_assistant_result_received(900012, true);
  EXPECT_TRUE(exposition_contains("nspm_home_assistant_call_failures_total{domain=\"test_failing_domain\"} 1"));
  EXPECT_TRUE(exposition_contains("nspm_home_assistant_call_duration_seconds_count{domain=\"test_failing_domain\"} 2"));
}

TEST(LatencyMetricsTest, panel_log_line_series_follow_renames_and_deletion) {
  ManagerMetrics::PanelLabels old_labels = {.panel = "TestOldName", .mac = "AA:BB:CC:00:00:51", .nspm_id = "951"};
  ManagerMetrics::PanelLabels new_labels = {.panel = "TestNewName", .mac = "AA:BB:CC:00:00:51", .nspm_id = "951"};
  LatencyMetrics::panel_log_line(old_labels, "warning");
  EXPECT_TRUE(exposition_contains("nspm_panel_log_lines_total{level=\"warning\",mac=\"AA:BB:CC:00:00:51\",nspm_id=\"951\",panel=\"TestOldName\"} 1"));
  // Renamed, noticed by another phase-1 call: ManagerMetrics' removal hook drops our old series too.
  ManagerMetrics::panel_labels(new_labels);
  EXPECT_FALSE(exposition_contains("panel=\"TestOldName\""));
  LatencyMetrics::panel_log_line(new_labels, "warning");
  EXPECT_TRUE(exposition_contains("nspm_panel_log_lines_total{level=\"warning\",mac=\"AA:BB:CC:00:00:51\",nspm_id=\"951\",panel=\"TestNewName\"} 1"));
  // Destroyed (EntityManager::load_nspanels -> ~NSPanel -> ManagerMetrics::panel_removed).
  ManagerMetrics::panel_removed("AA:BB:CC:00:00:51");
  EXPECT_FALSE(exposition_contains("mac=\"AA:BB:CC:00:00:51\""));
}

TEST(LatencyMetricsTest, configs_sent_carry_reason_and_are_removed_with_the_panel) {
  ManagerMetrics::PanelLabels labels = {.panel = "TestReasonPanel", .mac = "AA:BB:CC:00:00:52", .nspm_id = "952"};
  ManagerMetrics::panel_config_sent(labels, 100, "reload");
  ManagerMetrics::panel_config_sent(labels, 100, "reload_resend");
  ManagerMetrics::panel_config_sent(labels, 100, "reload");
  EXPECT_TRUE(exposition_contains("nspm_panel_configs_sent_total{mac=\"AA:BB:CC:00:00:52\",nspm_id=\"952\",panel=\"TestReasonPanel\",reason=\"reload\"} 2"));
  EXPECT_TRUE(exposition_contains("nspm_panel_configs_sent_total{mac=\"AA:BB:CC:00:00:52\",nspm_id=\"952\",panel=\"TestReasonPanel\",reason=\"reload_resend\"} 1"));
  EXPECT_TRUE(exposition_contains("nspm_panel_config_bytes_total{mac=\"AA:BB:CC:00:00:52\",nspm_id=\"952\",panel=\"TestReasonPanel\"} 300"));
  ManagerMetrics::panel_removed("AA:BB:CC:00:00:52");
  EXPECT_FALSE(exposition_contains("mac=\"AA:BB:CC:00:00:52\""));
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

TEST(LatencyMetricsTest, dropped_messages_are_counted_by_topic_class) {
  LatencyMetrics::mqtt_message_dropped(Metrics::TopicClass::HA_DISCOVERY);
  LatencyMetrics::mqtt_message_dropped(Metrics::TopicClass::HA_DISCOVERY);
  EXPECT_TRUE(exposition_contains("nspm_mqtt_messages_dropped_total{topic_class=\"ha_discovery\"} 2"));
}

TEST(LatencyMetricsTest, raise_max_only_raises) {
  std::atomic<long> max = 5;
  EXPECT_FALSE(LatencyMetrics::raise_max(max, 3));
  EXPECT_FALSE(LatencyMetrics::raise_max(max, 5));
  EXPECT_EQ(max.load(), 5);
  EXPECT_TRUE(LatencyMetrics::raise_max(max, 9));
  EXPECT_EQ(max.load(), 9);
}

TEST(LatencyMetricsTest, raise_max_from_several_threads_ends_at_the_highest_value) {
  std::atomic<long> max = 0;
  std::vector<std::thread> threads;
  for (int t = 0; t < 4; t++) {
    threads.emplace_back([&max, t]() {
      for (long value = 0; value < 10000; value++) {
        LatencyMetrics::raise_max(max, value * 4 + t);
      }
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }
  EXPECT_EQ(max.load(), 9999 * 4 + 3);
}

TEST(LatencyMetricsTest, queue_depth_max_keeps_the_highest_depth) {
  auto value = [](const std::string &name) {
    std::string text = Metrics::serialize();
    size_t start = text.find("\n" + name + " ");
    EXPECT_NE(start, std::string::npos) << name;
    return start == std::string::npos ? -1.0 : std::stod(text.substr(start + name.size() + 2));
  };
  LatencyMetrics::mqtt_message_enqueued(); // Registers the series.
  LatencyMetrics::mqtt_message_dequeued(Metrics::TopicClass::OTHER, 0);
  double depth = value("nspm_mqtt_queue_depth");
  double max_before = value("nspm_mqtt_queue_depth_max");
  // Push far enough past anything else in the test binary to raise the maximum.
  int pushes = static_cast<int>(max_before) + 5;
  for (int i = 0; i < pushes; i++) {
    LatencyMetrics::mqtt_message_enqueued();
  }
  for (int i = 0; i < pushes; i++) {
    LatencyMetrics::mqtt_message_dequeued(Metrics::TopicClass::OTHER, 0);
  }
  EXPECT_EQ(value("nspm_mqtt_queue_depth"), depth);
  EXPECT_EQ(value("nspm_mqtt_queue_depth_max"), depth + pushes);
}

TEST(LatencyMetricsTest, queue_full_waits_count_once_and_add_their_time) {
  auto value = [](const std::string &name) {
    std::string text = Metrics::serialize();
    size_t start = text.find("\n" + name + " ");
    return start == std::string::npos ? 0.0 : std::stod(text.substr(start + name.size() + 2));
  };
  double waits = value("nspm_mqtt_queue_full_waits_total");
  double seconds = value("nspm_mqtt_queue_full_wait_seconds_total");
  LatencyMetrics::mqtt_queue_full_wait(0.25);
  LatencyMetrics::mqtt_queue_full_wait(0.5);
  EXPECT_EQ(value("nspm_mqtt_queue_full_waits_total"), waits + 2);
  EXPECT_DOUBLE_EQ(value("nspm_mqtt_queue_full_wait_seconds_total"), seconds + 0.75);
}
