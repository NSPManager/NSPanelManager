#include <gtest/gtest.h>
#include <metrics/manager_metrics.hpp>
#include <metrics/metrics.hpp>
#include <metrics/panel_metrics_collector.hpp>
#include <metrics/prometheus_targets.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

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

TEST(MetricsTest, serialize_includes_process_metrics) {
  std::string text = Metrics::serialize();
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

namespace {

NSPanelMetricsSnapshot scrapable_panel() {
  return NSPanelMetricsSnapshot{
      .id = 4,
      .name = "Office",
      .mac = "C0:49:EF:D1:A7:30",
      .ip_address = "10.8.0.79",
      .model = "sonoff",
      .state = MQTT_MANAGER_NSPANEL_STATE::ONLINE,
      .registered = true,
      .rssi = -60,
      .heap_used_pct = 50,
      .temperature = 70,
      .humidity = std::nullopt,
      .pressure = std::nullopt,
      .update_progress = 0,
      .md5_firmware = "d7a9ecce",
      .md5_littlefs = "706c2b5f",
      .md5_tft = "13bf1937",
      .status_report_received = true,
      .metrics_port = 80,
  };
}

} // namespace

TEST(PrometheusTargetsTest, selects_accepted_online_panels_that_advertise_metrics) {
  auto targets = PrometheusTargets::select_panel_targets({scrapable_panel()});
  ASSERT_EQ(targets.size(), 1u);
  EXPECT_EQ(targets[0].name, "Office");
  EXPECT_EQ(targets[0].mac, "C0:49:EF:D1:A7:30");
  EXPECT_EQ(targets[0].nspm_id, 4u);
  EXPECT_EQ(targets[0].ip_address, "10.8.0.79");
  EXPECT_EQ(targets[0].metrics_port, 80);

}

TEST(PrometheusTargetsTest, leaves_out_panels_that_cannot_be_scraped) {
  std::vector<NSPanelMetricsSnapshot> panels;
  for (auto state : {MQTT_MANAGER_NSPANEL_STATE::OFFLINE, MQTT_MANAGER_NSPANEL_STATE::UNKNOWN, MQTT_MANAGER_NSPANEL_STATE::DENIED, MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT,
                     MQTT_MANAGER_NSPANEL_STATE::WAITING, MQTT_MANAGER_NSPANEL_STATE::REBOOTING, MQTT_MANAGER_NSPANEL_STATE::UPDATING_FIRMWARE, MQTT_MANAGER_NSPANEL_STATE::UPDATING_DATA,
                     MQTT_MANAGER_NSPANEL_STATE::UPDATING_TFT}) {
    auto panel = scrapable_panel();
    panel.state = state;
    panels.push_back(panel);
  }
  auto no_port = scrapable_panel();
  no_port.metrics_port = 0; // Stock firmware: no metrics_port in register_request.
  panels.push_back(no_port);
  auto no_ip = scrapable_panel();
  no_ip.ip_address = "";
  panels.push_back(no_ip);
  auto not_registered = scrapable_panel();
  not_registered.registered = false;
  panels.push_back(not_registered);
  EXPECT_TRUE(PrometheusTargets::select_panel_targets(panels).empty());
}

TEST(ManagerMetricsTest, http_route) {
  EXPECT_EQ(ManagerMetrics::http_route("/nextion-img/media_player/3/album_art?w=100"), ManagerMetrics::HttpRoute::IMAGE);
  EXPECT_EQ(ManagerMetrics::http_route("/prometheus/targets"), ManagerMetrics::HttpRoute::TARGETS);
  EXPECT_EQ(ManagerMetrics::http_route("/prometheus/metrics?x=1"), ManagerMetrics::HttpRoute::METRICS);
  EXPECT_EQ(ManagerMetrics::http_route("/"), ManagerMetrics::HttpRoute::OTHER);
}

TEST(ManagerMetricsTest, counts_show_up_in_serialized_text) {
  ManagerMetrics::init();
  ManagerMetrics::mqtt_message_received("nspanel/C0:49:EF:D1:A7:30/status_report", 120);
  ManagerMetrics::mqtt_message_published(true, 10);
  ManagerMetrics::panel_config_sent({.panel = "MetricsTestPanel", .mac = "AA:BB:CC:00:00:01", .nspm_id = "901"}, 512, "test");
  ManagerMetrics::http_request(ManagerMetrics::HttpRoute::TARGETS, 200);
  std::string text = Metrics::serialize();
  EXPECT_NE(text.find("nspm_mqtt_messages_received_total{topic_class=\"status_report\"}"), std::string::npos) << text;
  EXPECT_NE(text.find("nspm_mqtt_messages_received_total{topic_class=\"ha_discovery\"} 0"), std::string::npos) << text;
  EXPECT_NE(text.find("nspm_mqtt_published_bytes_total{retained=\"true\"}"), std::string::npos) << text;
  EXPECT_NE(text.find("nspm_panel_config_bytes_total{mac=\"AA:BB:CC:00:00:01\",nspm_id=\"901\",panel=\"MetricsTestPanel\"} 512"), std::string::npos) << text;
  EXPECT_NE(text.find("nspm_http_requests_total{code=\"200\",route=\"targets\"}"), std::string::npos) << text;
  EXPECT_NE(text.find("nspm_home_assistant_connected 0"), std::string::npos) << text;
  EXPECT_NE(text.find("nspm_warnings_active{level=\"error\"}"), std::string::npos) << text;
  EXPECT_EQ(text.find("nspm_openhab_auth_failures_total"), std::string::npos) << text;
}

TEST(ManagerMetricsTest, mqtt_connects_and_disconnects_count_transitions) {
  auto value = [](const std::string &name) {
    std::string text = Metrics::serialize();
    size_t start = text.find("\n" + name + " ");
    EXPECT_NE(start, std::string::npos) << name;
    return std::stod(text.substr(start + name.size() + 2));
  };
  ManagerMetrics::mqtt_connection_changed(false);
  double connects = value("nspm_mqtt_connects_total");
  double disconnects = value("nspm_mqtt_disconnects_total");
  ManagerMetrics::mqtt_connection_changed(false); // Already disconnected: not counted again.
  ManagerMetrics::mqtt_connection_changed(true);
  ManagerMetrics::mqtt_connection_changed(true);
  EXPECT_EQ(value("nspm_mqtt_connected"), 1);
  ManagerMetrics::mqtt_connection_changed(false);
  EXPECT_EQ(value("nspm_mqtt_connects_total"), connects + 1);
  EXPECT_EQ(value("nspm_mqtt_disconnects_total"), disconnects + 1);
  EXPECT_EQ(value("nspm_mqtt_connected"), 0);
}

TEST(ManagerMetricsTest, renaming_a_panel_removes_its_old_series) {
  ManagerMetrics::PanelLabels old_labels = {.panel = "Rename Before", .mac = "AA:BB:CC:00:00:02", .nspm_id = "902"};
  ManagerMetrics::PanelLabels new_labels = {.panel = "Rename After", .mac = "AA:BB:CC:00:00:02", .nspm_id = "902"};
  // Static: the callback stays registered after the test.
  static std::vector<ManagerMetrics::PanelLabels> removed;
  ManagerMetrics::on_panel_labels_removed([](const ManagerMetrics::PanelLabels &labels) {
    if (labels.mac == "AA:BB:CC:00:00:02") {
      removed.push_back(labels);
    }
  });

  ManagerMetrics::panel_config_sent(old_labels, 10, "test");
  ManagerMetrics::panel_status_report(old_labels);
  EXPECT_NE(Metrics::serialize().find("panel=\"Rename Before\""), std::string::npos);

  ManagerMetrics::panel_labels(new_labels);
  std::string text = Metrics::serialize();
  EXPECT_EQ(text.find("panel=\"Rename Before\""), std::string::npos) << text;
  ASSERT_EQ(removed.size(), 1u);
  EXPECT_EQ(removed[0], old_labels);

  ManagerMetrics::panel_config_sent(new_labels, 10, "test");
  EXPECT_NE(Metrics::serialize().find("panel=\"Rename After\""), std::string::npos);
  ManagerMetrics::panel_labels(new_labels); // Unchanged: nothing removed.
  EXPECT_EQ(removed.size(), 1u);

  ManagerMetrics::panel_removed("AA:BB:CC:00:00:02");
  EXPECT_EQ(Metrics::serialize().find("panel=\"Rename After\""), std::string::npos);
  EXPECT_EQ(removed.size(), 2u);
}
