#include "test_helpers.hpp"
#include <functional>
#include <gtest/gtest.h>
#include <media_player/home_assistant_media_player.hpp>
#include <media_player/home_assistant_source_player.hpp>
#include <protobuf_nspanel.pb.h>
#include <protobuf_nspanel_entity.pb.h>
#include <string>
#include <vector>

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

// Unsigned, as nlohmann::json parses it from Home Assistant. A signed int in the attributes is ignored.
constexpr uint32_t SELECT_SOURCE = HomeAssistantSourcePlayer::feature_select_source;

std::vector<std::string> sources_of(const NSPanelEntityState_MediaPlayer_SourceSelection &selection) {
  return std::vector<std::string>(selection.sources().begin(), selection.sources().end());
}

TEST_F(HomeAssistantMediaPlayerTest, player_sources_are_published_from_home_assistant) {
  receive_state("playing", {{"source_list", nlohmann::json::array({"TV", "Radio", "Spotify"})}, {"source", "Radio"}, {"supported_features", SELECT_SOURCE}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(sources_of(state->player_sources()), std::vector<std::string>({"TV", "Radio", "Spotify"}));
  EXPECT_EQ(state->player_sources().current_source(), "Radio");
  EXPECT_TRUE(state->player_sources().can_select());
  EXPECT_FALSE(state->has_source_player_sources());
}

TEST_F(HomeAssistantMediaPlayerTest, sources_can_not_be_selected_without_the_select_source_feature) {
  receive_state("playing", {{"source_list", nlohmann::json::array({"TV", "Radio"})}, {"source", "TV"}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(sources_of(state->player_sources()), std::vector<std::string>({"TV", "Radio"}));
  EXPECT_FALSE(state->player_sources().can_select());

  send_panel_command([](auto *command) { command->set_select_player_source("Radio"); });

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantMediaPlayerTest, panel_source_selection_calls_home_assistant) {
  receive_state("playing", {{"source_list", nlohmann::json::array({"TV", "Radio"})}, {"source", "TV"}, {"supported_features", SELECT_SOURCE}});

  send_panel_command([](auto *command) { command->set_select_player_source("Radio"); });

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "media_player"}, {"service", "select_source"}, {"target", {{"entity_id", "media_player.lounge"}}}, {"service_data", {{"source", "Radio"}}}}));
}

TEST_F(HomeAssistantMediaPlayerTest, selecting_an_unknown_source_is_ignored) {
  receive_state("playing", {{"source_list", nlohmann::json::array({"TV", "Radio"})}, {"supported_features", SELECT_SOURCE}});

  send_panel_command([](auto *command) { command->set_select_player_source("Vinyl"); });

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantMediaPlayerTest, source_lists_are_limited_for_the_panel) {
  nlohmann::json source_list = nlohmann::json::array();
  for (size_t i = 0; i < MediaPlayerEntity::max_sources + 5; i++) {
    source_list.push_back("Input " + std::to_string(i));
  }
  // 63 bytes then a 2 byte UTF-8 character: cutting at 64 bytes would split it, so the name is cut before it.
  const std::string long_name = std::string(MediaPlayerEntity::max_source_name_length - 1, 'a') + "\xC3\xA9";
  const std::string short_name = std::string(MediaPlayerEntity::max_source_name_length - 1, 'a');
  source_list[0] = long_name;
  receive_state("playing", {{"source_list", source_list}, {"source", long_name}, {"supported_features", SELECT_SOURCE}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  ASSERT_EQ(state->player_sources().sources_size(), static_cast<int>(MediaPlayerEntity::max_sources));
  EXPECT_EQ(state->player_sources().sources(0), short_name);
  EXPECT_EQ(state->player_sources().current_source(), short_name);
  EXPECT_EQ(state->player_sources().sources(MediaPlayerEntity::max_sources - 1), "Input " + std::to_string(MediaPlayerEntity::max_sources - 1));

  // The panel only knows the shortened name, Home Assistant gets the full one.
  send_panel_command([&](auto *command) { command->set_select_player_source(short_name); });
  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["source"], long_name);

  // Sources past the limit were never shown on the panel, so they can not be selected.
  send_panel_command([&](auto *command) { command->set_select_player_source("Input " + std::to_string(MediaPlayerEntity::max_sources)); });
  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantMediaPlayerTest, removed_entity_has_no_sources) {
  receive_state("playing", {{"source_list", nlohmann::json::array({"TV", "Radio"})}, {"supported_features", SELECT_SOURCE}});

  nlohmann::json removed = home_assistant_state_changed("media_player.lounge", "");
  removed["event"]["data"]["new_state"] = nullptr;
  HomeAssistantManager::test_process_event(removed);

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->player_sources().sources_size(), 0);
  EXPECT_FALSE(state->player_sources().can_select());
}

// A media player fed by a source player, as with a multi-room audio matrix: the player reports the
// entity ID of its current source player in a state attribute.
class HomeAssistantSourcePlayerSourcesTest : public HomeAssistantMediaPlayerTest {
protected:
  void SetUp() override {
    nspm_test::SendCaptureTest::SetUp();
    nlohmann::json entity_data = django_media_player_data("media_player.lounge");
    entity_data["source_volume_strategy"] = "source_entity";
    entity_data["source_entity_attribute"] = "source_entity";
    row = std::make_unique<ScopedEntity>("media_player", "Lounge zone", entity_data);
    player = std::make_unique<HomeAssistantMediaPlayer>(row->id);
  }

  void receive_source_state(const std::string &entity_id, const nlohmann::json &attributes) {
    auto event = home_assistant_state_changed(entity_id, "playing", attributes);
    HomeAssistantManager::test_process_event(event);
  }
};

TEST_F(HomeAssistantSourcePlayerSourcesTest, source_player_sources_are_published) {
  receive_state("playing", {{"source_entity", "media_player.streamer"}});
  receive_source_state("media_player.streamer", {{"source_list", nlohmann::json::array({"Spotify", "AirPlay"})}, {"source", "AirPlay"}, {"supported_features", SELECT_SOURCE}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  ASSERT_TRUE(state->has_source_player_sources());
  EXPECT_EQ(sources_of(state->source_player_sources()), std::vector<std::string>({"Spotify", "AirPlay"}));
  EXPECT_EQ(state->source_player_sources().current_source(), "AirPlay");
  EXPECT_TRUE(state->source_player_sources().can_select());
}

TEST_F(HomeAssistantSourcePlayerSourcesTest, panel_source_player_selection_calls_the_source_player) {
  receive_state("playing", {{"source_entity", "media_player.streamer"}});
  receive_source_state("media_player.streamer", {{"source_list", nlohmann::json::array({"Spotify", "AirPlay"})}, {"supported_features", SELECT_SOURCE}});
  home_assistant_service_calls();

  send_panel_command([](auto *command) { command->set_select_source_player_source("Spotify"); });

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "media_player"}, {"service", "select_source"}, {"target", {{"entity_id", "media_player.streamer"}}}, {"service_data", {{"source", "Spotify"}}}}));
}

TEST_F(HomeAssistantSourcePlayerSourcesTest, switching_source_player_drops_the_previous_sources) {
  receive_state("playing", {{"source_entity", "media_player.streamer"}});
  receive_source_state("media_player.streamer", {{"source_list", nlohmann::json::array({"Spotify", "AirPlay"})}, {"supported_features", SELECT_SOURCE}});
  last_published_state();

  receive_state("playing", {{"source_entity", "media_player.turntable"}});

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  ASSERT_TRUE(state->has_source_player_sources());
  EXPECT_EQ(state->source_player_sources().sources_size(), 0);

  // A late event from the previous source player must not bring its sources back.
  receive_source_state("media_player.streamer", {{"source_list", nlohmann::json::array({"Spotify"})}, {"supported_features", SELECT_SOURCE}});
  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(HomeAssistantMediaPlayerTest, source_player_selection_without_a_source_player_is_ignored) {
  receive_state("playing", {{"source_list", nlohmann::json::array({"TV"})}, {"supported_features", SELECT_SOURCE}});

  send_panel_command([](auto *command) { command->set_select_source_player_source("TV"); });

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
