#include "test_helpers.hpp"
#include <button/home_assistant_button.hpp>
#include <button/nspm_button.hpp>
#include <gtest/gtest.h>

using nspm_test::django_button_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::mqtt_published_to;
using nspm_test::ScopedEntity;

class HomeAssistantButtonTest : public nspm_test::SendCaptureTest {};

TEST_F(HomeAssistantButtonTest, loads_placement_and_controller_from_database) {
  ScopedEntity row("button", "Doorbell", django_button_data("home_assistant", "button.doorbell"), 4, 9, 5);

  HomeAssistantButton doorbell(row.id);

  EXPECT_EQ(doorbell.get_name(), "Doorbell");
  EXPECT_EQ(doorbell.get_room_id(), 4);
  EXPECT_EQ(doorbell.get_entity_page_id(), 9);
  EXPECT_EQ(doorbell.get_entity_page_slot(), 5);
  EXPECT_EQ(doorbell.get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
  EXPECT_EQ(doorbell.get_type(), MQTT_MANAGER_ENTITY_TYPE::BUTTON);
  EXPECT_TRUE(doorbell.can_toggle());
}

TEST_F(HomeAssistantButtonTest, press_calls_the_button_domain) {
  ScopedEntity row("button", "Doorbell", django_button_data("home_assistant", "button.doorbell"));
  HomeAssistantButton doorbell(row.id);

  doorbell.toggle();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "button"}, {"service", "press"}, {"target", {{"entity_id", "button.doorbell"}}}}));
}

TEST_F(HomeAssistantButtonTest, input_button_uses_its_own_domain) {
  ScopedEntity row("button", "Reset", django_button_data("home_assistant", "input_button.reset"));
  HomeAssistantButton reset(row.id);

  reset.toggle();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["domain"], "input_button");
}

TEST_F(HomeAssistantButtonTest, every_press_is_sent) {
  ScopedEntity row("button", "Doorbell", django_button_data("home_assistant", "button.doorbell"));
  HomeAssistantButton doorbell(row.id);

  doorbell.toggle();
  doorbell.toggle();

  EXPECT_EQ(home_assistant_service_calls().size(), 2);
}

TEST_F(HomeAssistantButtonTest, reload_config_picks_up_a_new_entity) {
  ScopedEntity row("button", "Doorbell", django_button_data("home_assistant", "button.doorbell"));
  HomeAssistantButton doorbell(row.id);

  auto db_row = database_manager::database.get<database_manager::Entity>(row.id);
  db_row.set_entity_data_json(django_button_data("home_assistant", "button.gate"));
  database_manager::database.update(db_row);
  doorbell.reload_config();
  doorbell.toggle();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["target"]["entity_id"], "button.gate");
}

class NSPMButtonTest : public nspm_test::SendCaptureTest {};

TEST_F(NSPMButtonTest, press_publishes_the_configured_mqtt_message) {
  ScopedEntity row("button", "Garage door", django_button_data("nspm", "", "garage/door/set", "toggle"));
  NSPMButton garage(row.id);

  garage.toggle();

  auto messages = mqtt_published_to("garage/door/set");
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0].payload, "toggle");
  EXPECT_FALSE(messages[0].retain);
  EXPECT_EQ(garage.get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::NSPM);
  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(NSPMButtonTest, press_uses_the_current_database_values) {
  ScopedEntity row("button", "Garage door", django_button_data("nspm", "", "garage/door/set", "toggle"));
  NSPMButton garage(row.id);

  auto db_row = database_manager::database.get<database_manager::Entity>(row.id);
  db_row.set_entity_data_json(django_button_data("nspm", "", "garage/door/set", "open"));
  database_manager::database.update(db_row);
  garage.toggle();

  auto messages = mqtt_published_to("garage/door/set");
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0].payload, "open");
}

TEST_F(NSPMButtonTest, loads_without_errors) {
  ScopedEntity row("button", "Garage door", django_button_data("nspm", "", "garage/door/set", "toggle"));
  nspm_test::ScopedErrorLog log;

  NSPMButton garage(row.id);

  EXPECT_EQ(log.errors(), std::vector<std::string>());
}
