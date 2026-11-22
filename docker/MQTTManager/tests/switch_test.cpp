#include "test_helpers.hpp"
#include <gtest/gtest.h>
#include <switch/home_assistant_switch.hpp>
#include <switch/openhab_switch.hpp>

using nspm_test::django_switch_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::home_assistant_state_changed;
using nspm_test::ScopedEntity;

class HomeAssistantSwitchTest : public nspm_test::SendCaptureTest {};

TEST_F(HomeAssistantSwitchTest, loads_placement_and_controller_from_database) {
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"), 3, 7, 2);

  HomeAssistantSwitch fan(row.id);

  EXPECT_EQ(fan.get_id(), row.id);
  EXPECT_EQ(fan.get_name(), "Fan");
  EXPECT_EQ(fan.get_room_id(), 3);
  EXPECT_EQ(fan.get_entity_page_id(), 7);
  EXPECT_EQ(fan.get_entity_page_slot(), 2);
  EXPECT_EQ(fan.get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
  EXPECT_EQ(fan.get_type(), MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY);
  EXPECT_FALSE(fan.get_state());
}

TEST_F(HomeAssistantSwitchTest, turn_on_and_off_call_the_switch_domain) {
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  HomeAssistantSwitch fan(row.id);

  fan.turn_on(true);
  fan.turn_off(true);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 2);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "switch"}, {"service", "turn_on"}, {"target", {{"entity_id", "switch.fan"}}}}));
  EXPECT_EQ(calls[1]["service"], "turn_off");
}

TEST_F(HomeAssistantSwitchTest, input_boolean_uses_its_own_domain) {
  ScopedEntity row("switch", "Guest mode", django_switch_data("home_assistant", "input_boolean.guest_mode"));
  HomeAssistantSwitch guest_mode(row.id);

  guest_mode.turn_on(true);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["domain"], "input_boolean");
  EXPECT_EQ(calls[0]["target"]["entity_id"], "input_boolean.guest_mode");
}

TEST_F(HomeAssistantSwitchTest, unsupported_domain_sends_nothing) {
  ScopedEntity row("switch", "Porch", django_switch_data("home_assistant", "light.porch"));
  HomeAssistantSwitch porch(row.id);

  porch.turn_on(true);

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantSwitchTest, turn_on_without_send_update_sends_nothing) {
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  HomeAssistantSwitch fan(row.id);

  fan.turn_on(false);

  EXPECT_TRUE(home_assistant_service_calls().empty());
  EXPECT_FALSE(fan.get_state());
}

TEST_F(HomeAssistantSwitchTest, optimistic_mode_shows_the_new_state_immediately) {
  set_optimistic_mode(true);
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  HomeAssistantSwitch fan(row.id);

  fan.turn_on(true);

  EXPECT_TRUE(fan.get_state());
}

TEST_F(HomeAssistantSwitchTest, non_optimistic_mode_waits_for_home_assistant) {
  set_optimistic_mode(false);
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  HomeAssistantSwitch fan(row.id);

  fan.turn_on(true);
  EXPECT_FALSE(fan.get_state());

  auto event = home_assistant_state_changed("switch.fan", "on");
  HomeAssistantManager::test_process_event(event);
  EXPECT_TRUE(fan.get_state());
}

TEST_F(HomeAssistantSwitchTest, follows_state_changes_from_home_assistant) {
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  HomeAssistantSwitch fan(row.id);

  auto on = home_assistant_state_changed("switch.fan", "on");
  HomeAssistantManager::test_process_event(on);
  EXPECT_TRUE(fan.get_state());

  auto other_entity = home_assistant_state_changed("switch.other", "off");
  HomeAssistantManager::test_process_event(other_entity);
  EXPECT_TRUE(fan.get_state());

  auto off = home_assistant_state_changed("switch.fan", "off");
  HomeAssistantManager::test_process_event(off);
  EXPECT_FALSE(fan.get_state());
}

TEST_F(HomeAssistantSwitchTest, toggle_sends_the_opposite_of_the_current_state) {
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  HomeAssistantSwitch fan(row.id);

  fan.toggle();
  auto on = home_assistant_state_changed("switch.fan", "on");
  HomeAssistantManager::test_process_event(on);
  fan.toggle();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 2);
  EXPECT_EQ(calls[0]["service"], "turn_on");
  EXPECT_EQ(calls[1]["service"], "turn_off");
}

TEST_F(HomeAssistantSwitchTest, destroyed_switch_stops_observing_home_assistant) {
  ScopedEntity row("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  { HomeAssistantSwitch fan(row.id); }

  // Would call into the destroyed switch if its observer were still attached.
  auto on = home_assistant_state_changed("switch.fan", "on");
  HomeAssistantManager::test_process_event(on);
}

class OpenhabSwitchTest : public nspm_test::SendCaptureTest {};

TEST_F(OpenhabSwitchTest, turn_on_and_off_send_item_commands) {
  ScopedEntity row("switch", "Fan", django_switch_data("openhab", "", "Fan_Switch"));
  OpenhabSwitch fan(row.id);

  fan.turn_on(true);
  fan.turn_off(true);

  auto messages = OpenhabManager::test_take_sent_messages();
  ASSERT_EQ(messages.size(), 2);
  EXPECT_EQ(messages[0]["type"], "ItemCommandEvent");
  EXPECT_EQ(messages[0]["topic"], "openhab/items/Fan_Switch/command");
  EXPECT_EQ(nlohmann::json::parse(std::string(messages[0]["payload"])), nlohmann::json({{"type", "OnOff"}, {"value", "ON"}}));
  EXPECT_EQ(nlohmann::json::parse(std::string(messages[1]["payload"]))["value"], "OFF");
  EXPECT_EQ(fan.get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB);
}
