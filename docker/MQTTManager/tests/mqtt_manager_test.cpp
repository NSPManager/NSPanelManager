#include "test_helpers.hpp"
#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

using nspm_test::ScopedBrokerConnected;

namespace {

const std::string LIVE_TOPIC = "test/mqtt_manager/live";

std::optional<std::string> payload() {
  return "payload";
}

const std::string QUEUED_TOPIC = "test/mqtt_manager/queued";

std::mutex received_mutex;
std::condition_variable received_condition;
std::vector<std::string> received_messages;

void on_queued_message(std::string topic, std::string message) {
  {
    std::lock_guard<std::mutex> lock_guard(received_mutex);
    received_messages.push_back(message);
  }
  received_condition.notify_all();
}

// Waits until count messages have been received on QUEUED_TOPIC, or the timeout passes.
bool wait_for_received(size_t count, std::chrono::milliseconds timeout) {
  std::unique_lock<std::mutex> lock(received_mutex);
  return received_condition.wait_for(lock, timeout, [count] { return received_messages.size() >= count; });
}

// Runs the message processing thread for the duration of a test.
class ScopedProcessingThread {
public:
  ScopedProcessingThread() {
    {
      std::lock_guard<std::mutex> lock_guard(received_mutex);
      received_messages.clear();
    }
    MQTT_Manager::subscribe(QUEUED_TOPIC, &on_queued_message);
    MQTT_Manager::test_start_processing_thread();
  }

  ~ScopedProcessingThread() {
    MQTT_Manager::test_stop_processing_thread();
    MQTT_Manager::detach_callback(QUEUED_TOPIC, &on_queued_message);
  }

  ScopedProcessingThread(const ScopedProcessingThread &) = delete;
  ScopedProcessingThread &operator=(const ScopedProcessingThread &) = delete;
};

} // namespace

TEST(MqttManagerTest, publish_if_connected_sends_unretained) {
  ScopedBrokerConnected connected;
  nspm_test::stomp_mirrored_to(LIVE_TOPIC);

  EXPECT_TRUE(MQTT_Manager::publish_if_connected(LIVE_TOPIC, payload));

  auto messages = nspm_test::mqtt_sent_to(LIVE_TOPIC);
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0].payload, "payload");
  EXPECT_FALSE(messages[0].retain);

  auto mirrored = nspm_test::stomp_mirrored_to(LIVE_TOPIC);
  ASSERT_EQ(mirrored.size(), 1);
  EXPECT_EQ(mirrored[0].payload, "payload");
  EXPECT_FALSE(mirrored[0].retain);
}

TEST(MqttManagerTest, publish_if_connected_drops_messages_while_disconnected) {
  size_t buffered = MQTT_Manager::test_buffered_message_count();

  EXPECT_FALSE(MQTT_Manager::publish_if_connected(LIVE_TOPIC, payload));
  EXPECT_EQ(MQTT_Manager::test_buffered_message_count(), buffered);
}

TEST(MqttManagerTest, publish_if_connected_mirrors_to_stomp_while_disconnected) {
  nspm_test::stomp_mirrored_to(LIVE_TOPIC);

  EXPECT_FALSE(MQTT_Manager::publish_if_connected(LIVE_TOPIC, payload));

  auto mirrored = nspm_test::stomp_mirrored_to(LIVE_TOPIC);
  ASSERT_EQ(mirrored.size(), 1);
  EXPECT_EQ(mirrored[0].payload, "payload");
  EXPECT_FALSE(mirrored[0].retain);
}

TEST(MqttManagerTest, publish_if_connected_drops_messages_until_caught_up_after_reconnecting) {
  ScopedBrokerConnected connected;
  MQTT_Manager::test_set_broker_connected(false);
  MQTT_Manager::publish("test/mqtt_manager/buffered", "published while disconnected");
  MQTT_Manager::test_set_broker_connected(true); // Reconnected, but the buffered message is not sent yet.
  size_t buffered = MQTT_Manager::test_buffered_message_count();
  nspm_test::stomp_mirrored_to(LIVE_TOPIC);

  EXPECT_FALSE(MQTT_Manager::publish_if_connected(LIVE_TOPIC, payload));
  EXPECT_EQ(MQTT_Manager::test_buffered_message_count(), buffered);
  EXPECT_EQ(nspm_test::stomp_mirrored_to(LIVE_TOPIC).size(), 1);

  MQTT_Manager::test_send_buffered_messages();
  EXPECT_TRUE(nspm_test::mqtt_sent_to(LIVE_TOPIC).empty());
}

TEST(MqttManagerTest, publish_if_connected_sends_nothing_without_a_payload) {
  ScopedBrokerConnected connected;
  nspm_test::stomp_mirrored_to(LIVE_TOPIC);

  bool sent = MQTT_Manager::publish_if_connected(LIVE_TOPIC, []() -> std::optional<std::string> {
    return std::nullopt;
  });

  EXPECT_FALSE(sent);
  EXPECT_TRUE(nspm_test::mqtt_sent_to(LIVE_TOPIC).empty());
  EXPECT_TRUE(nspm_test::stomp_mirrored_to(LIVE_TOPIC).empty());
}

TEST(MqttManagerTest, queued_message_is_processed_without_polling_delay) {
  ScopedProcessingThread processing;
  // Let the processing thread reach its wait, so the message has to wake it up.
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  for (int i = 0; i < 5; i++) {
    auto pushed_at = std::chrono::steady_clock::now();
    ASSERT_TRUE(MQTT_Manager::test_enqueue_message(QUEUED_TOPIC, std::to_string(i)));
    ASSERT_TRUE(wait_for_received(i + 1, std::chrono::seconds(2)));
    // The processing thread used to poll the queue every 100 ms.
    EXPECT_LT(std::chrono::steady_clock::now() - pushed_at, std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

TEST(MqttManagerTest, queued_messages_are_processed_in_order) {
  ScopedProcessingThread processing;

  std::vector<std::string> expected;
  for (int i = 0; i < 100; i++) {
    expected.push_back(std::to_string(i));
    ASSERT_TRUE(MQTT_Manager::test_enqueue_message(QUEUED_TOPIC, expected.back()));
  }

  ASSERT_TRUE(wait_for_received(expected.size(), std::chrono::seconds(2)));
  std::lock_guard<std::mutex> lock_guard(received_mutex);
  EXPECT_EQ(received_messages, expected);
}

TEST(MqttManagerTest, processing_thread_stops_promptly) {
  MQTT_Manager::test_start_processing_thread();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  auto stop_started = std::chrono::steady_clock::now();
  MQTT_Manager::test_stop_processing_thread();
  // Woken by the stop rather than the 1 s safety-net timeout.
  EXPECT_LT(std::chrono::steady_clock::now() - stop_started, std::chrono::milliseconds(500));
}
