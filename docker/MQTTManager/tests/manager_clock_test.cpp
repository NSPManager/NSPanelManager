#include "test_helpers.hpp"
#include <chrono>
#include <cstdint>
#include <gtest/gtest.h>
#include <manager_clock/manager_clock.hpp>
#include <mqtt_manager/mqtt_manager.hpp>
#include <string>

using nspm_test::ScopedErrorLog;
using nspm_test::ScopedBrokerConnected;

namespace {

std::chrono::system_clock::time_point unix_ms(int64_t milliseconds) {
  return std::chrono::system_clock::time_point(std::chrono::milliseconds(milliseconds));
}

int64_t now_unix_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace

TEST(ManagerClockTest, epoch_payload_is_unix_time_in_milliseconds) {
  EXPECT_EQ(ManagerClock::epoch_payload(unix_ms(1760724061234)), "1760724061234");
}

TEST(ManagerClockTest, epoch_payload_drops_parts_of_a_millisecond) {
  auto time = unix_ms(1760724061234) + std::chrono::microseconds(999);
  EXPECT_EQ(ManagerClock::epoch_payload(time), "1760724061234");
}

TEST(ManagerClockTest, epoch_payload_is_empty_before_2025) {
  EXPECT_FALSE(ManagerClock::epoch_payload(unix_ms(0)).has_value());
  EXPECT_FALSE(ManagerClock::epoch_payload(unix_ms(1735689599999)).has_value()); // 2024-12-31T23:59:59.999Z
  EXPECT_EQ(ManagerClock::epoch_payload(unix_ms(1735689600000)), "1735689600000"); // 2025-01-01T00:00:00Z
}

TEST(ManagerClockTest, unset_clock_is_warned_about_once_until_it_is_set) {
  ManagerClock::epoch_payload(unix_ms(1760724061234)); // Start from a set clock.
  ScopedErrorLog log(spdlog::level::warn);

  ManagerClock::epoch_payload(unix_ms(0));
  ManagerClock::epoch_payload(unix_ms(60000));
  EXPECT_EQ(log.errors().size(), 1);

  ManagerClock::epoch_payload(unix_ms(1760724061234));
  ManagerClock::epoch_payload(unix_ms(0));
  EXPECT_EQ(log.errors().size(), 2);
}

TEST(ManagerClockTest, publish_epoch_sends_the_current_time_unretained) {
  ScopedBrokerConnected connected;

  int64_t before = now_unix_ms();
  ManagerClock::publish_epoch();
  int64_t after = now_unix_ms();

  auto messages = nspm_test::mqtt_sent_to(nspm_test::epoch_topic());
  ASSERT_EQ(messages.size(), 1);
  EXPECT_FALSE(messages[0].retain);
  int64_t sent = std::stoll(messages[0].payload);
  EXPECT_GE(sent, before);
  EXPECT_LE(sent, after);
}

TEST(ManagerClockTest, publish_epoch_does_not_buffer_the_time_while_disconnected) {
  size_t buffered = MQTT_Manager::test_buffered_message_count();

  ManagerClock::publish_epoch();

  EXPECT_EQ(MQTT_Manager::test_buffered_message_count(), buffered);
}

TEST(ManagerClockTest, publish_epoch_mirrors_the_time_to_stomp_while_disconnected) {
  nspm_test::stomp_mirrored_to(nspm_test::epoch_topic());

  ManagerClock::publish_epoch();

  auto mirrored = nspm_test::stomp_mirrored_to(nspm_test::epoch_topic());
  ASSERT_EQ(mirrored.size(), 1);
  EXPECT_FALSE(mirrored[0].retain);
}

// ManagerClock::init() attaches for the rest of the test run, so this is the only test calling it.
TEST(ManagerClockTest, time_is_sent_when_mqtt_connects) {
  ManagerClock::init();
  ScopedBrokerConnected connected;

  MQTT_Manager::test_notify_connected();

  EXPECT_EQ(nspm_test::mqtt_sent_to(nspm_test::epoch_topic()).size(), 1);
}
