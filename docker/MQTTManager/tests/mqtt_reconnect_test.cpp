#include <gtest/gtest.h>
#include <mqtt_manager/mqtt_manager.hpp>
#include <string>
#include <vector>

// Messages published while disconnected are buffered and sent after reconnecting. Nothing published
// after reconnecting may overtake them: a retained message that did would leave the server holding
// the older value.
class MqttReconnectTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Other tests publish while disconnected too. Send all of that now, so each test starts
    // disconnected with nothing buffered.
    MQTT_Manager::test_set_broker_connected(true);
    MQTT_Manager::test_send_buffered_messages();
    MQTT_Manager::test_set_broker_connected(false);
    MQTT_Manager::test_take_sent_messages();
  }

  void TearDown() override {
    MQTT_Manager::test_set_broker_connected(false);
  }

  // Payloads sent to the server on a topic, in order.
  static std::vector<std::string> sent_to(const std::string &topic) {
    std::vector<std::string> payloads;
    for (auto &message : MQTT_Manager::test_take_sent_messages()) {
      if (message.topic == topic) {
        payloads.push_back(message.payload);
      }
    }
    return payloads;
  }
};

TEST_F(MqttReconnectTest, publishing_after_reconnecting_waits_for_the_buffered_messages) {
  const std::string topic = "test/mqtt_reconnect/state";
  MQTT_Manager::publish(topic, "published while disconnected", true);

  // Connected, but not caught up: this is where a newer message used to go out first.
  MQTT_Manager::test_set_broker_connected(true);
  EXPECT_FALSE(MQTT_Manager::is_connected());
  MQTT_Manager::publish(topic, "published after reconnecting", true);
  EXPECT_TRUE(sent_to(topic).empty());

  MQTT_Manager::test_send_buffered_messages();

  EXPECT_TRUE(MQTT_Manager::is_connected());
  std::vector<std::string> sent = sent_to(topic);
  ASSERT_GE(sent.size(), 2);
  EXPECT_EQ(sent[0], "published while disconnected");
  EXPECT_EQ(sent.back(), "published after reconnecting");
}

TEST_F(MqttReconnectTest, each_retained_topic_ends_on_its_latest_value) {
  const std::string topic = "test/mqtt_reconnect/retained";
  MQTT_Manager::publish(topic, "old", true);
  MQTT_Manager::publish(topic, "new", true);

  MQTT_Manager::test_set_broker_connected(true);
  MQTT_Manager::test_send_buffered_messages();

  std::vector<std::string> sent = sent_to(topic);
  ASSERT_FALSE(sent.empty());
  EXPECT_EQ(sent.back(), "new");
}

TEST_F(MqttReconnectTest, buffered_messages_keep_their_order_and_are_not_replayed_unless_retained) {
  const std::string topic = "test/mqtt_reconnect/events";
  MQTT_Manager::publish(topic, "first", false);
  MQTT_Manager::publish(topic, "second", false);

  MQTT_Manager::test_set_broker_connected(true);
  MQTT_Manager::test_send_buffered_messages();

  EXPECT_EQ(sent_to(topic), (std::vector<std::string>{"first", "second"}));
}

TEST_F(MqttReconnectTest, losing_the_connection_buffers_again) {
  MQTT_Manager::test_set_broker_connected(true);
  MQTT_Manager::test_send_buffered_messages();
  EXPECT_TRUE(MQTT_Manager::is_connected());

  MQTT_Manager::test_set_broker_connected(false);
  EXPECT_FALSE(MQTT_Manager::is_connected());

  MQTT_Manager::test_set_broker_connected(true);
  EXPECT_FALSE(MQTT_Manager::is_connected());
}
