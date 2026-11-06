#include <gtest/gtest.h>
#include <metrics/metrics.hpp>
#include <metrics/prometheus_targets.hpp>
#include <nlohmann/json.hpp>
#include <string>

TEST(MetricsTest, classify_topic) {
  using TopicClass = Metrics::TopicClass;
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/status_report"), TopicClass::STATUS_REPORT);
  EXPECT_EQ(Metrics::classify_topic("nspanel/Office/status_report"), TopicClass::STATUS_REPORT);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/log"), TopicClass::LOG);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/log_backlog"), TopicClass::LOG);
  EXPECT_EQ(Metrics::classify_topic("nspanel/mqttmanager/command"), TopicClass::REGISTER);
  EXPECT_EQ(Metrics::classify_topic("nspanel/mqttmanager_192.168.1.67/command"), TopicClass::COMMAND);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/command"), TopicClass::COMMAND);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/buzzer_mute_cmd"), TopicClass::COMMAND);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/config"), TopicClass::CONFIG);
  EXPECT_EQ(Metrics::classify_topic("nspanel/config/reload"), TopicClass::CONFIG);
  EXPECT_EQ(Metrics::classify_topic("homeassistant/switch/nspanelmanager/c0_49_ef_d1_a7_30_relay1/config"), TopicClass::HA_DISCOVERY);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/status"), TopicClass::OTHER);
  EXPECT_EQ(Metrics::classify_topic("nspanel/C0:49:EF:D1:A7:30/relay1_state"), TopicClass::OTHER);
  EXPECT_EQ(Metrics::classify_topic(""), TopicClass::OTHER);
}

TEST(MetricsTest, topic_class_labels) {
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::STATUS_REPORT), "status_report");
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::LOG), "log");
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::COMMAND), "command");
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::CONFIG), "config");
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::REGISTER), "register");
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::HA_DISCOVERY), "ha_discovery");
  EXPECT_STREQ(Metrics::topic_class_label(Metrics::TopicClass::OTHER), "other");
}

TEST(MetricsTest, serialize_includes_build_info_and_process_metrics) {
  std::string text = Metrics::serialize();
  EXPECT_NE(text.find("nspm_build_info{"), std::string::npos) << text;
  EXPECT_NE(text.find("process_cpu_seconds_total "), std::string::npos) << text;
  EXPECT_NE(text.find("process_resident_memory_bytes "), std::string::npos) << text;
  EXPECT_NE(text.find("process_start_time_seconds "), std::string::npos) << text;
  EXPECT_NE(text.find("process_open_fds "), std::string::npos) << text;
  EXPECT_NE(text.find("process_threads "), std::string::npos) << text;
}

TEST(MetricsTest, buckets_are_increasing) {
  for (const auto *buckets : {&Metrics::latency_buckets(), &Metrics::fast_buckets()}) {
    ASSERT_FALSE(buckets->empty());
    for (size_t i = 1; i < buckets->size(); i++) {
      EXPECT_LT((*buckets)[i - 1], (*buckets)[i]);
    }
  }
}

TEST(PrometheusTargetsTest, lists_manager_by_host_and_panels) {
  std::vector<PrometheusTargets::PanelTarget> panels = {
      {.name = "Office", .mac = "C0:49:EF:D1:A7:30", .nspm_id = 4, .ip_address = "10.8.0.79", .metrics_port = 80},
  };
  nlohmann::json targets = nlohmann::json::parse(PrometheusTargets::build_json("nspm.manque.net:8000", panels));
  nlohmann::json expected = nlohmann::json::parse(R"([
    {"targets": ["nspm.manque.net:8000"], "labels": {"role": "manager", "__metrics_path__": "/prometheus/metrics"}},
    {"targets": ["10.8.0.79:80"], "labels": {"role": "panel", "panel": "Office", "mac": "C0:49:EF:D1:A7:30", "nspm_id": "4", "__metrics_path__": "/metrics"}}
  ])");
  EXPECT_EQ(targets, expected);
}

TEST(PrometheusTargetsTest, leaves_out_manager_without_valid_host) {
  EXPECT_EQ(PrometheusTargets::build_json("", {}), "[]");
  EXPECT_EQ(PrometheusTargets::build_json("evil\"host", {}), "[]");
  EXPECT_EQ(PrometheusTargets::build_json("host/path", {}), "[]");
  EXPECT_TRUE(PrometheusTargets::is_valid_host("[::1]:8000"));
  EXPECT_TRUE(PrometheusTargets::is_valid_host("192.168.1.67:8000"));
}
