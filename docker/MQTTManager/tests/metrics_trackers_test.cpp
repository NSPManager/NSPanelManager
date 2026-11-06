#include <array>
#include <chrono>
#include <gtest/gtest.h>
#include <metrics/drop_log_limiter.hpp>
#include <metrics/ha_call_tracker.hpp>
#include <metrics/panel_update_tracker.hpp>
#include <vector>

using namespace std::chrono_literals;

namespace {

using Phase = PanelUpdateTracker::Phase;
using Type = PanelUpdateTracker::Type;
using Kind = PanelUpdateTracker::Event::Kind;

// Events with the duration left out, for comparing everything but FINISHED's seconds.
std::vector<std::pair<Type, Kind>> kinds(const std::vector<PanelUpdateTracker::Event> &events) {
  std::vector<std::pair<Type, Kind>> result;
  for (const auto &event : events) {
    result.push_back({event.type, event.kind});
  }
  return result;
}

constexpr std::array<bool, 3> none_match = {false, false, false};
constexpr std::array<bool, 3> all_match = {true, true, true};
constexpr std::array<bool, 3> littlefs_matches = {false, true, false};

} // namespace

TEST(HaCallTrackerTest, finish_returns_domain_and_elapsed_time) {
  HaCallTracker tracker;
  auto start = HaCallTracker::clock::time_point(10s);
  tracker.start(7, "light", start);
  auto result = tracker.finish(7, start + 250ms);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->domain, "light");
  EXPECT_DOUBLE_EQ(result->seconds, 0.25);
  EXPECT_EQ(tracker.pending(), 0);
}

TEST(HaCallTrackerTest, unknown_or_already_finished_ids_return_nothing) {
  HaCallTracker tracker;
  auto start = HaCallTracker::clock::time_point(10s);
  EXPECT_FALSE(tracker.finish(1, start).has_value());
  tracker.start(1, "switch", start);
  EXPECT_TRUE(tracker.finish(1, start).has_value());
  EXPECT_FALSE(tracker.finish(1, start).has_value());
}

TEST(HaCallTrackerTest, old_requests_are_pruned_when_new_ones_start) {
  HaCallTracker tracker(60s, 1024);
  auto start = HaCallTracker::clock::time_point(10s);
  tracker.start(1, "light", start);
  tracker.start(2, "light", start + 30s);
  tracker.start(3, "light", start + 61s);
  EXPECT_EQ(tracker.pending(), 2);
  EXPECT_FALSE(tracker.finish(1, start + 62s).has_value());
  EXPECT_TRUE(tracker.finish(2, start + 62s).has_value());
}

TEST(HaCallTrackerTest, number_of_pending_requests_is_capped_dropping_the_oldest) {
  HaCallTracker tracker(60s, 3);
  auto start = HaCallTracker::clock::time_point(10s);
  for (long id = 1; id <= 5; id++) {
    tracker.start(id, "light", start);
  }
  EXPECT_EQ(tracker.pending(), 3);
  EXPECT_FALSE(tracker.finish(2, start).has_value());
  EXPECT_TRUE(tracker.finish(3, start).has_value());
  EXPECT_TRUE(tracker.finish(5, start).has_value());
}

TEST(HaCallTrackerTest, clear_forgets_pending_requests) {
  HaCallTracker tracker;
  tracker.start(1, "light");
  tracker.clear();
  EXPECT_EQ(tracker.pending(), 0);
  EXPECT_FALSE(tracker.finish(1).has_value());
}

TEST(PanelUpdateTrackerTest, firmware_update_succeeds_once_the_new_md5_is_reported_after_rebooting) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  EXPECT_TRUE(tracker.observe(Phase::ONLINE, true, none_match, t).empty());
  EXPECT_EQ(kinds(tracker.observe(Phase::UPDATING_FIRMWARE, true, none_match, t + 1s)), (std::vector<std::pair<Type, Kind>>{{Type::FIRMWARE, Kind::STARTED}}));

  // FIRMWARE_UPDATE_FINISHED reports REBOOTING before LittleFS starts.
  auto events = tracker.observe(Phase::DOWN, true, none_match, t + 61s);
  ASSERT_EQ(kinds(events), (std::vector<std::pair<Type, Kind>>{{Type::FIRMWARE, Kind::FINISHED}}));
  EXPECT_DOUBLE_EQ(events[0].seconds, 60);

  EXPECT_EQ(kinds(tracker.observe(Phase::UPDATING_LITTLEFS, true, none_match, t + 62s)), (std::vector<std::pair<Type, Kind>>{{Type::LITTLEFS, Kind::STARTED}}));
  // LittleFS md5 is stored before the reboot, firmware md5 only on the next boot.
  EXPECT_EQ(kinds(tracker.observe(Phase::DOWN, true, littlefs_matches, t + 80s)), (std::vector<std::pair<Type, Kind>>{{Type::LITTLEFS, Kind::FINISHED}, {Type::LITTLEFS, Kind::SUCCEEDED}}));
  EXPECT_TRUE(tracker.observe(Phase::DOWN, false, none_match, t + 85s).empty()); // Offline (LWT)
  // First boot still reports the old firmware md5.
  EXPECT_TRUE(tracker.observe(Phase::ONLINE, true, littlefs_matches, t + 100s).empty());
  EXPECT_EQ(kinds(tracker.observe(Phase::ONLINE, true, all_match, t + 120s)), (std::vector<std::pair<Type, Kind>>{{Type::FIRMWARE, Kind::SUCCEEDED}}));
  EXPECT_TRUE(tracker.observe(Phase::ONLINE, true, all_match, t + 150s).empty());
}

TEST(PanelUpdateTrackerTest, update_that_returns_online_without_rebooting_failed) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  tracker.observe(Phase::UPDATING_FIRMWARE, true, none_match, t);
  EXPECT_EQ(kinds(tracker.observe(Phase::ONLINE, true, none_match, t + 5s)), (std::vector<std::pair<Type, Kind>>{{Type::FIRMWARE, Kind::FINISHED}, {Type::FIRMWARE, Kind::FAILED}}));
}

TEST(PanelUpdateTrackerTest, status_topic_online_alone_does_not_decide) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  tracker.observe(Phase::UPDATING_TFT, true, none_match, t);
  tracker.observe(Phase::DOWN, true, none_match, t + 300s);
  EXPECT_TRUE(tracker.observe(Phase::ONLINE, false, none_match, t + 320s).empty());
  EXPECT_EQ(kinds(tracker.observe(Phase::ONLINE, true, {false, false, true}, t + 321s)), (std::vector<std::pair<Type, Kind>>{{Type::TFT, Kind::SUCCEEDED}}));
}

TEST(PanelUpdateTrackerTest, update_whose_md5_never_arrives_fails_after_the_timeout) {
  PanelUpdateTracker tracker(15min);
  auto t = PanelUpdateTracker::clock::time_point(100s);
  tracker.observe(Phase::UPDATING_FIRMWARE, true, none_match, t);
  tracker.observe(Phase::DOWN, true, none_match, t + 60s);
  EXPECT_TRUE(tracker.observe(Phase::ONLINE, true, none_match, t + 120s).empty()); // Rolled back, old md5.
  EXPECT_TRUE(tracker.observe(Phase::ONLINE, true, none_match, t + 60s + 15min).empty());
  EXPECT_EQ(kinds(tracker.observe(Phase::ONLINE, true, none_match, t + 61s + 15min)), (std::vector<std::pair<Type, Kind>>{{Type::FIRMWARE, Kind::FAILED}}));
}

TEST(PanelUpdateTrackerTest, forced_reflash_of_the_staged_image_succeeds_after_reboot) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  tracker.observe(Phase::UPDATING_FIRMWARE, true, all_match, t);
  tracker.observe(Phase::DOWN, true, all_match, t + 60s);
  // Already matching while DOWN after leaving: counted as soon as it has rebooted.
  tracker.observe(Phase::UPDATING_LITTLEFS, true, all_match, t + 61s);
  auto events = kinds(tracker.observe(Phase::DOWN, true, all_match, t + 70s));
  EXPECT_EQ(events, (std::vector<std::pair<Type, Kind>>{{Type::LITTLEFS, Kind::FINISHED}, {Type::FIRMWARE, Kind::SUCCEEDED}, {Type::LITTLEFS, Kind::SUCCEEDED}}));
}

TEST(PanelUpdateTrackerTest, restarting_an_unconfirmed_update_counts_the_previous_one_as_failed) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  tracker.observe(Phase::UPDATING_TFT, true, none_match, t);
  tracker.observe(Phase::DOWN, true, none_match, t + 30s); // Rebooted after too many timeouts.
  EXPECT_EQ(kinds(tracker.observe(Phase::UPDATING_TFT, true, none_match, t + 60s)), (std::vector<std::pair<Type, Kind>>{{Type::TFT, Kind::FAILED}, {Type::TFT, Kind::STARTED}}));
}

TEST(PanelUpdateTrackerTest, staying_in_one_updating_state_emits_nothing_more) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  tracker.observe(Phase::UPDATING_TFT, true, none_match, t);
  EXPECT_TRUE(tracker.observe(Phase::UPDATING_TFT, true, none_match, t + 5s).empty());
  EXPECT_TRUE(tracker.observe(Phase::UPDATING_TFT, true, none_match, t + 10s).empty());
}

TEST(PanelUpdateTrackerTest, tracking_only_while_an_update_runs_or_awaits_its_outcome) {
  PanelUpdateTracker tracker;
  auto t = PanelUpdateTracker::clock::time_point(100s);
  EXPECT_FALSE(tracker.tracking());
  tracker.observe(Phase::UPDATING_TFT, true, none_match, t);
  EXPECT_TRUE(tracker.tracking());
  tracker.observe(Phase::DOWN, true, none_match, t + 60s);
  EXPECT_TRUE(tracker.tracking());
  tracker.observe(Phase::ONLINE, true, {false, false, true}, t + 90s);
  EXPECT_FALSE(tracker.tracking());
}

namespace {

using Line = DropLogLimiter::Line;
using Level = DropLogLimiter::Line::Level;

} // namespace

TEST(DropLogLimiterTest, first_drop_warns_with_topic_class_and_capacity) {
  DropLogLimiter limiter(256);
  auto t = DropLogLimiter::clock::time_point(100s);
  EXPECT_TRUE(limiter.tick(t).empty());
  EXPECT_EQ(limiter.on_drop("status_report", t), (std::vector<Line>{{Level::WARNING, "Dropped an MQTT message (status_report): processing queue full (256)."}}));
}

TEST(DropLogLimiterTest, continued_drops_are_summarised_at_most_once_per_window) {
  DropLogLimiter limiter(256);
  auto t = DropLogLimiter::clock::time_point(100s);
  limiter.on_drop("status_report", t);
  for (int i = 0; i < 30; i++) {
    EXPECT_TRUE(limiter.on_drop("status_report", t + 1s).empty());
  }
  for (int i = 0; i < 7; i++) {
    EXPECT_TRUE(limiter.on_drop("other", t + 2s).empty());
  }
  EXPECT_TRUE(limiter.tick(t + 9s).empty());
  EXPECT_EQ(limiter.tick(t + 10s), (std::vector<Line>{{Level::WARNING, "Dropped 37 MQTT messages in the last 10 s (other: 7, status_report: 30); processing queue full (256)."}}));
  EXPECT_TRUE(limiter.tick(t + 11s).empty());
  // A drop in the next window is held for that window's summary, which a later drop can trigger too.
  EXPECT_TRUE(limiter.on_drop("log", t + 12s).empty());
  EXPECT_EQ(limiter.on_drop("log", t + 20s), (std::vector<Line>{{Level::WARNING, "Dropped 1 MQTT messages in the last 10 s (log: 1); processing queue full (256)."}}));
}

TEST(DropLogLimiterTest, a_quiet_window_logs_recovery_once_and_the_next_drop_warns_again) {
  DropLogLimiter limiter(256);
  auto t = DropLogLimiter::clock::time_point(100s);
  limiter.on_drop("status_report", t);
  limiter.on_drop("status_report", t + 1s);
  EXPECT_EQ(limiter.tick(t + 10s).size(), 1); // Summary of the second drop.
  EXPECT_EQ(limiter.tick(t + 20s), (std::vector<Line>{{Level::INFO, "No MQTT messages dropped in the last 10 s; 2 dropped in total since the queue filled up."}}));
  EXPECT_TRUE(limiter.tick(t + 30s).empty());
  EXPECT_EQ(limiter.on_drop("command", t + 40s), (std::vector<Line>{{Level::WARNING, "Dropped an MQTT message (command): processing queue full (256)."}}));
}

TEST(DropLogLimiterTest, single_drop_then_quiet_logs_recovery_without_a_summary) {
  DropLogLimiter limiter(256);
  auto t = DropLogLimiter::clock::time_point(100s);
  limiter.on_drop("other", t);
  EXPECT_EQ(limiter.tick(t + 10s), (std::vector<Line>{{Level::INFO, "No MQTT messages dropped in the last 10 s; 1 dropped in total since the queue filled up."}}));
}
