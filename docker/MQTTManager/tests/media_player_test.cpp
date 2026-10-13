#include "test_helpers.hpp"
#include <functional>
#include <gtest/gtest.h>
#include <media_player/home_assistant_media_player.hpp>
#include <protobuf_nspanel.pb.h>
#include <protobuf_nspanel_entity.pb.h>

using nspm_test::django_media_player_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::home_assistant_state_changed;
using nspm_test::ScopedEntity;

class HomeAssistantMediaPlayerTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    nspm_test::SendCaptureTest::SetUp();
    row = std::make_unique<ScopedEntity>("media_player", "Lounge speaker", django_media_player_data("media_player.lounge", 10), 5, 11, 4);
    player = std::make_unique<HomeAssistantMediaPlayer>(row->id);
  }

  void TearDown() override {
    player.reset();
    row.reset();
  }

  void receive_state(const std::string &state, const nlohmann::json &attributes) {
    auto event = home_assistant_state_changed("media_player.lounge", state, attributes);
    HomeAssistantManager::test_process_event(event);
  }

  // The last state published for the NSPanels since the previous call.
  std::optional<NSPanelEntityState_MediaPlayer> last_published_state() {
    auto messages = nspm_test::mqtt_published_to(player->get_mqtt_state_topic());
    if (messages.empty()) {
      return std::nullopt;
    }
    EXPECT_TRUE(messages.back().retain);
    NSPanelEntityState state;
    EXPECT_TRUE(state.ParseFromString(messages.back().payload));
    return state.media_player();
  }

  void send_panel_command(const std::function<void(NSPanelMQTTManagerCommand_MediaPlayerCommand *)> &fill) {
    NSPanelMQTTManagerCommand command;
    auto *media_player_command = command.mutable_media_player_command();
    media_player_command->set_media_player_id(row->id);
    fill(media_player_command);
    player->command_callback(command);
  }

  std::unique_ptr<ScopedEntity> row;
  std::unique_ptr<HomeAssistantMediaPlayer> player;
};

TEST_F(HomeAssistantMediaPlayerTest, loads_config_and_publishes_an_initial_state) {
  EXPECT_EQ(player->get_name(), "Lounge speaker");
  EXPECT_EQ(player->get_room_id(), 5);
  EXPECT_EQ(player->get_entity_page_id(), 11);
  EXPECT_EQ(player->get_entity_page_slot(), 4);
  EXPECT_EQ(player->get_type(), MQTT_MANAGER_ENTITY_TYPE::MEDIA_PLAYER);
  EXPECT_FALSE(player->can_toggle());

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->media_player_id(), row->id);
  EXPECT_EQ(state->name(), "Lounge speaker");
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN);
  EXPECT_EQ(state->volume_step(), 10);
}

TEST_F(HomeAssistantMediaPlayerTest, publishes_state_changes_from_home_assistant) {
  last_published_state();

  receive_state("playing", {{"media_title", "So What"}, {"media_artist", "Miles Davis"}, {"volume_level", 0.35}, {"is_volume_muted", false}, {"supported_features", 1 | 4096 | 16384}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING);
  EXPECT_EQ(state->media_title(), "So What");
  EXPECT_EQ(state->media_artist(), "Miles Davis");
  EXPECT_EQ(state->volume(), 35);
  EXPECT_FALSE(state->is_muted());
  EXPECT_FALSE(state->has_source_volume());
}

TEST_F(HomeAssistantMediaPlayerTest, unchanged_state_is_not_published_again) {
  receive_state("paused", {{"volume_level", 0.5}});
  last_published_state();

  receive_state("paused", {{"volume_level", 0.5}});

  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(HomeAssistantMediaPlayerTest, volume_is_kept_while_the_player_is_off) {
  receive_state("playing", {{"volume_level", 0.4}});
  receive_state("off", nlohmann::json::object());

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_OFF);
  EXPECT_EQ(state->volume(), 40);
}

TEST_F(HomeAssistantMediaPlayerTest, removed_entity_becomes_unknown) {
  receive_state("playing", nlohmann::json::object());

  nlohmann::json removed = home_assistant_state_changed("media_player.lounge", "");
  removed["event"]["data"]["new_state"] = nullptr;
  HomeAssistantManager::test_process_event(removed);

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN);
}

TEST_F(HomeAssistantMediaPlayerTest, panel_playback_commands_call_home_assistant) {
  const std::pair<NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction, std::string> cases[] = {
      {NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PLAY, "media_play"},
      {NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PAUSE, "media_pause"},
      {NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_NEXT_TRACK, "media_next_track"},
      {NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PREVIOUS_TRACK, "media_previous_track"},
  };
  for (auto &[action, service] : cases) {
    SCOPED_TRACE(service);
    send_panel_command([&](auto *command) { command->set_playback_action(action); });

    auto calls = home_assistant_service_calls();
    ASSERT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "media_player"}, {"service", service}, {"target", {{"entity_id", "media_player.lounge"}}}}));
  }
}

TEST_F(HomeAssistantMediaPlayerTest, panel_volume_and_mute_commands_call_home_assistant) {
  send_panel_command([](auto *command) {
    command->set_has_volume(true);
    command->set_volume(45);
    command->set_has_muted(true);
    command->set_muted(true);
  });

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 2);
  EXPECT_EQ(calls[0]["service"], "volume_set");
  EXPECT_DOUBLE_EQ(calls[0]["service_data"]["volume_level"].get<double>(), 0.45);
  EXPECT_EQ(calls[1]["service"], "volume_mute");
  EXPECT_EQ(calls[1]["service_data"], nlohmann::json({{"is_volume_muted", true}}));
}

TEST_F(HomeAssistantMediaPlayerTest, panel_volume_is_clamped) {
  send_panel_command([](auto *command) {
    command->set_has_volume(true);
    command->set_volume(150);
  });

  EXPECT_DOUBLE_EQ(home_assistant_service_calls().at(0)["service_data"]["volume_level"].get<double>(), 1.0);
}

TEST_F(HomeAssistantMediaPlayerTest, commands_for_other_media_players_are_ignored) {
  NSPanelMQTTManagerCommand command;
  command.mutable_media_player_command()->set_media_player_id(row->id + 1000);
  command.mutable_media_player_command()->set_playback_action(NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PLAY);

  player->command_callback(command);

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST(MediaPlayerConfig, invalid_volume_step_falls_back_to_five) {
  for (auto volume_step : {0, 101}) {
    SCOPED_TRACE(volume_step);
    ScopedEntity row("media_player", "Speaker", django_media_player_data("media_player.speaker", volume_step));
    MQTT_Manager::test_take_published_messages();

    HomeAssistantMediaPlayer player(row.id);

    NSPanelEntityState state;
    auto messages = nspm_test::mqtt_published_to(player.get_mqtt_state_topic());
    ASSERT_FALSE(messages.empty());
    ASSERT_TRUE(state.ParseFromString(messages.back().payload));
    EXPECT_EQ(state.media_player().volume_step(), 5);
  }
}
