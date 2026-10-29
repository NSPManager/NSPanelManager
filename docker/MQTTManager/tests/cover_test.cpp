#include "test_helpers.hpp"
#include <cover/home_assistant_cover.hpp>
#include <entity_manager/entity_manager.hpp>
#include <fmt/format.h>
#include <functional>
#include <gtest/gtest.h>
#include <protobuf_nspanel.pb.h>
#include <protobuf_nspanel_entity.pb.h>
#include <room/room_entities_page.hpp>
#include <string>

using nspm_test::django_cover_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::home_assistant_state_changed;
using nspm_test::ScopedEntity;

class HomeAssistantCoverTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    nspm_test::SendCaptureTest::SetUp();
    row = std::make_unique<ScopedEntity>("cover", "Living room blind", django_cover_data("home_assistant", "cover.living_room_blind"), 5, 11, 4);
    cover = std::make_unique<HomeAssistantCover>(row->id);
  }

  void TearDown() override {
    cover.reset();
    row.reset();
  }

  void receive_state(const std::string &state, const nlohmann::json &attributes = nlohmann::json::object()) {
    auto event = home_assistant_state_changed("cover.living_room_blind", state, attributes);
    HomeAssistantManager::test_process_event(event);
  }

  std::optional<NSPanelEntityState_Cover> last_published_state() {
    auto messages = nspm_test::mqtt_published_to(cover->get_mqtt_state_topic());
    if (messages.empty()) {
      return std::nullopt;
    }
    EXPECT_TRUE(messages.back().retain);
    NSPanelEntityState state;
    EXPECT_TRUE(state.ParseFromString(messages.back().payload));
    return state.cover();
  }

  void send_panel_command(const std::function<void(NSPanelMQTTManagerCommand_CoverCommand *)> &fill) {
    NSPanelMQTTManagerCommand command;
    auto *cover_command = command.mutable_cover_command();
    cover_command->set_cover_id(row->id);
    fill(cover_command);
    cover->command_callback(command);
  }

  std::unique_ptr<ScopedEntity> row;
  std::unique_ptr<HomeAssistantCover> cover;
};

TEST_F(HomeAssistantCoverTest, loads_config_and_publishes_an_initial_state) {
  EXPECT_EQ(cover->get_name(), "Living room blind");
  EXPECT_EQ(cover->get_room_id(), 5);
  EXPECT_EQ(cover->get_entity_page_id(), 11);
  EXPECT_EQ(cover->get_entity_page_slot(), 4);
  EXPECT_EQ(cover->get_type(), MQTT_MANAGER_ENTITY_TYPE::COVER);
  EXPECT_EQ(cover->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
  EXPECT_FALSE(cover->can_toggle());

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->cover_id(), row->id);
  EXPECT_EQ(state->name(), "Living room blind");
  EXPECT_EQ(state->state(), NSPanelEntityState_Cover_CoverState_UNKNOWN);
  EXPECT_FALSE(state->has_position());
  EXPECT_FALSE(state->can_open());
}

TEST_F(HomeAssistantCoverTest, publishes_state_position_and_features_from_home_assistant) {
  last_published_state();

  receive_state("open", {{"current_position", 75}, {"supported_features", 1 | 2 | 4 | 8}, {"device_class", "blind"}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_Cover_CoverState_OPEN);
  EXPECT_TRUE(state->has_position());
  EXPECT_EQ(state->position(), 75);
  EXPECT_TRUE(state->can_open());
  EXPECT_TRUE(state->can_close());
  EXPECT_TRUE(state->can_set_position());
  EXPECT_TRUE(state->can_stop());
  EXPECT_FALSE(state->can_open_tilt());
  EXPECT_EQ(state->device_class(), "blind");
}

TEST_F(HomeAssistantCoverTest, maps_every_cover_state) {
  const std::pair<std::string, NSPanelEntityState_Cover_CoverState> cases[] = {
      {"open", NSPanelEntityState_Cover_CoverState_OPEN},
      {"closed", NSPanelEntityState_Cover_CoverState_CLOSED},
      {"opening", NSPanelEntityState_Cover_CoverState_OPENING},
      {"closing", NSPanelEntityState_Cover_CoverState_CLOSING},
      {"unavailable", NSPanelEntityState_Cover_CoverState_UNAVAILABLE},
      {"unknown", NSPanelEntityState_Cover_CoverState_UNKNOWN},
      {"something-new", NSPanelEntityState_Cover_CoverState_UNKNOWN},
  };
  for (auto &[ha_state, expected] : cases) {
    SCOPED_TRACE(ha_state);
    receive_state(ha_state, {{"current_position", 10}});
    auto state = last_published_state();
    ASSERT_TRUE(state.has_value());
    EXPECT_EQ(state->state(), expected);
    // Make the next state different even when two cases map to the same enum (unknown/something-new).
    receive_state("open", {{"current_position", 10}});
    last_published_state();
  }
}

TEST_F(HomeAssistantCoverTest, publishes_tilt_state_and_features) {
  receive_state("open", {{"current_position", 50}, {"current_tilt_position", 30}, {"supported_features", 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_TRUE(state->has_tilt_position());
  EXPECT_EQ(state->tilt_position(), 30);
  EXPECT_TRUE(state->can_open_tilt());
  EXPECT_TRUE(state->can_close_tilt());
  EXPECT_TRUE(state->can_stop_tilt());
  EXPECT_TRUE(state->can_set_tilt_position());
}

TEST_F(HomeAssistantCoverTest, positions_are_clamped_and_rounded) {
  receive_state("open", {{"current_position", 140}, {"supported_features", 4}});
  EXPECT_EQ(last_published_state()->position(), 100);

  receive_state("open", {{"current_position", -20}, {"supported_features", 4}});
  EXPECT_EQ(last_published_state()->position(), 0);

  receive_state("open", {{"current_position", 42.6}, {"supported_features", 4}});
  EXPECT_EQ(last_published_state()->position(), 43);
}

TEST_F(HomeAssistantCoverTest, unchanged_state_is_not_published_again) {
  receive_state("open", {{"current_position", 50}, {"supported_features", 4}});
  last_published_state();

  receive_state("open", {{"current_position", 50}, {"supported_features", 4}});

  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(HomeAssistantCoverTest, removed_entity_becomes_unavailable) {
  receive_state("open", {{"current_position", 50}, {"supported_features", 4 | 8}});

  nlohmann::json removed = home_assistant_state_changed("cover.living_room_blind", "");
  removed["event"]["data"]["new_state"] = nullptr;
  HomeAssistantManager::test_process_event(removed);

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_Cover_CoverState_UNAVAILABLE);
  EXPECT_FALSE(state->has_position());
  EXPECT_FALSE(state->can_open());
  EXPECT_FALSE(state->can_close());
}

TEST_F(HomeAssistantCoverTest, panel_actions_call_home_assistant_services) {
  receive_state("open", {{"current_position", 50}, {"supported_features", 1 | 2 | 4 | 8}});

  const std::pair<NSPanelMQTTManagerCommand_CoverCommand_Action, std::string> cases[] = {
      {NSPanelMQTTManagerCommand_CoverCommand_Action_OPEN, "open_cover"},
      {NSPanelMQTTManagerCommand_CoverCommand_Action_CLOSE, "close_cover"},
      {NSPanelMQTTManagerCommand_CoverCommand_Action_STOP, "stop_cover"},
  };
  for (auto &[action, service] : cases) {
    SCOPED_TRACE(service);
    send_panel_command([&](auto *command) { command->set_action(action); });

    auto calls = home_assistant_service_calls();
    ASSERT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "cover"}, {"service", service}, {"target", {{"entity_id", "cover.living_room_blind"}}}}));
  }
}

TEST_F(HomeAssistantCoverTest, panel_position_command_calls_home_assistant) {
  receive_state("open", {{"current_position", 50}, {"supported_features", 4}});

  send_panel_command([](auto *command) {
    command->set_has_position(true);
    command->set_position(25);
  });

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "cover"}, {"service", "set_cover_position"}, {"target", {{"entity_id", "cover.living_room_blind"}}}, {"service_data", {{"position", 25}}}}));
}

TEST_F(HomeAssistantCoverTest, panel_tilt_commands_call_home_assistant) {
  receive_state("open", {{"supported_features", 16 | 32 | 64 | 128}});

  const std::pair<NSPanelMQTTManagerCommand_CoverCommand_Action, std::string> cases[] = {
      {NSPanelMQTTManagerCommand_CoverCommand_Action_OPEN_TILT, "open_cover_tilt"},
      {NSPanelMQTTManagerCommand_CoverCommand_Action_CLOSE_TILT, "close_cover_tilt"},
      {NSPanelMQTTManagerCommand_CoverCommand_Action_STOP_TILT, "stop_cover_tilt"},
  };
  for (auto &[action, service] : cases) {
    SCOPED_TRACE(service);
    send_panel_command([&](auto *command) { command->set_action(action); });

    auto calls = home_assistant_service_calls();
    ASSERT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "cover"}, {"service", service}, {"target", {{"entity_id", "cover.living_room_blind"}}}}));
  }
}

TEST_F(HomeAssistantCoverTest, unsupported_actions_are_ignored) {
  receive_state("open", {{"supported_features", 1}});

  send_panel_command([](auto *command) { command->set_action(NSPanelMQTTManagerCommand_CoverCommand_Action_CLOSE); });
  send_panel_command([](auto *command) { command->set_action(NSPanelMQTTManagerCommand_CoverCommand_Action_STOP); });
  send_panel_command([](auto *command) {
    command->set_has_position(true);
    command->set_position(50);
  });

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantCoverTest, panel_position_is_clamped) {
  receive_state("open", {{"supported_features", 4}});

  send_panel_command([](auto *command) {
    command->set_has_position(true);
    command->set_position(150);
  });

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["position"], 100);
}

TEST_F(HomeAssistantCoverTest, quick_toggle_opens_and_closes_blinds) {
  receive_state("closed", {{"current_position", 0}, {"supported_features", 1 | 2 | 4 | 8}, {"device_class", "blind"}});
  EXPECT_TRUE(cover->can_toggle());

  cover->toggle();
  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service"], "open_cover");

  receive_state("open", {{"current_position", 100}, {"supported_features", 1 | 2 | 4 | 8}, {"device_class", "blind"}});
  cover->toggle();
  calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service"], "close_cover");
}

TEST_F(HomeAssistantCoverTest, quick_toggle_stops_a_moving_blind) {
  receive_state("opening", {{"current_position", 20}, {"supported_features", 1 | 2 | 4 | 8}, {"device_class", "blind"}});

  cover->toggle();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service"], "stop_cover");
}

TEST_F(HomeAssistantCoverTest, garage_doors_are_not_quick_toggled) {
  receive_state("closed", {{"current_position", 0}, {"supported_features", 1 | 2}, {"device_class", "garage"}});

  EXPECT_FALSE(cover->can_toggle());
  cover->toggle();

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantCoverTest, unknown_covers_are_not_quick_toggled) {
  EXPECT_FALSE(cover->can_toggle());

  cover->toggle();

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantCoverTest, commands_for_other_covers_are_ignored) {
  NSPanelMQTTManagerCommand command;
  command.mutable_cover_command()->set_cover_id(row->id + 1000);
  command.mutable_cover_command()->set_action(NSPanelMQTTManagerCommand_CoverCommand_Action_OPEN);

  cover->command_callback(command);

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST(CoverEntityPage, publishes_a_cover_slot_to_the_panel) {
  database_manager::RoomEntitiesPage page_row;
  page_row.display_order = 0;
  page_row.page_type = 4;
  page_row.is_scenes_page = false;
  page_row.room_id = std::nullopt;
  int page_id = database_manager::database.insert(page_row);

  {
    ScopedEntity row("cover", "Living room blind", django_cover_data("home_assistant", "cover.living_room_blind"), 1, page_id, 2);
    EntityManager::load_covers();

    {
      RoomEntitiesPage page(page_id, nullptr);
      auto topic = fmt::format("nspanel/mqttmanager_{}/entity_pages/{}/state", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS), page_id);
      auto messages = nspm_test::mqtt_published_to(topic);
      ASSERT_FALSE(messages.empty());

      NSPanelRoomEntitiesPage state;
      ASSERT_TRUE(state.ParseFromString(messages.back().payload));
      ASSERT_EQ(state.entities_size(), 1);
      EXPECT_EQ(state.entities(0).room_view_position(), 2);
      EXPECT_EQ(state.entities(0).name(), "Living room blind");
      EXPECT_EQ(state.entities(0).type(), NSPanelRoomEntitiesPage_EntitySlot_EntityType_ENTITY_TYPE_COVER);
      EXPECT_EQ(state.entities(0).id(), row.id);
      EXPECT_FALSE(state.entities(0).mqtt_state_topic().empty());
      EXPECT_FALSE(state.entities(0).can_save_scene());
    }
  }

  EntityManager::load_covers();
  database_manager::database.remove<database_manager::RoomEntitiesPage>(page_id);
}
