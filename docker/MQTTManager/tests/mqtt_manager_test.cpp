#include "test_helpers.hpp"
#include <gtest/gtest.h>
#include <mqtt_manager/mqtt_manager.hpp>
#include <optional>
#include <string>

using nspm_test::ScopedBrokerConnected;

namespace {

const std::string LIVE_TOPIC = "test/mqtt_manager/live";

std::optional<std::string> payload() {
  return "payload";
}

} // namespace

TEST(MqttManagerTest, publish_if_connected_sends_unretained) {
  ScopedBrokerConnected connected;

  EXPECT_TRUE(MQTT_Manager::publish_if_connected(LIVE_TOPIC, payload));

  auto messages = nspm_test::mqtt_sent_to(LIVE_TOPIC);
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0].payload, "payload");
  EXPECT_FALSE(messages[0].retain);
}

TEST(MqttManagerTest, publish_if_connected_drops_messages_while_disconnected) {
  size_t buffered = MQTT_Manager::test_buffered_message_count();
  bool payload_made = false;

  bool sent = MQTT_Manager::publish_if_connected(LIVE_TOPIC, [&]() -> std::optional<std::string> {
    payload_made = true;
    return "payload";
  });

  EXPECT_FALSE(sent);
  EXPECT_FALSE(payload_made) << "The payload should only be made when it can be sent at once.";
  EXPECT_EQ(MQTT_Manager::test_buffered_message_count(), buffered);
}

TEST(MqttManagerTest, publish_if_connected_drops_messages_until_caught_up_after_reconnecting) {
  ScopedBrokerConnected connected;
  MQTT_Manager::test_set_broker_connected(false);
  MQTT_Manager::publish("test/mqtt_manager/buffered", "published while disconnected");
  MQTT_Manager::test_set_broker_connected(true); // Reconnected, but the buffered message is not sent yet.
  size_t buffered = MQTT_Manager::test_buffered_message_count();

  EXPECT_FALSE(MQTT_Manager::publish_if_connected(LIVE_TOPIC, payload));
  EXPECT_EQ(MQTT_Manager::test_buffered_message_count(), buffered);

  MQTT_Manager::test_send_buffered_messages();
  EXPECT_TRUE(nspm_test::mqtt_sent_to(LIVE_TOPIC).empty());
}

TEST(MqttManagerTest, publish_if_connected_sends_nothing_without_a_payload) {
  ScopedBrokerConnected connected;

  bool sent = MQTT_Manager::publish_if_connected(LIVE_TOPIC, []() -> std::optional<std::string> {
    return std::nullopt;
  });

  EXPECT_FALSE(sent);
  EXPECT_TRUE(nspm_test::mqtt_sent_to(LIVE_TOPIC).empty());
}
