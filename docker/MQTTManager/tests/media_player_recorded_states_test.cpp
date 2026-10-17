#include "test_helpers.hpp"
#include <algorithm>
#include <fstream>
#include <functional>
#include <gtest/gtest.h>
#include <media_player/home_assistant_media_player.hpp>
#include <protobuf_nspanel.pb.h>
#include <protobuf_nspanel_entity.pb.h>
#include <string>
#include <vector>

// The same media players as media_player_test.cpp, fed with states recorded from a real Home
// Assistant (tests/fixtures/home_assistant_media_players.json) instead of hand written ones.

using nspm_test::django_media_player_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::home_assistant_state_changed;
using nspm_test::ScopedEntity;
using nspm_test::ScopedErrorLog;

namespace {

const nlohmann::json &recorded_states() {
  static const nlohmann::json states = [] {
    std::ifstream file(NSPM_TEST_FIXTURES_DIR "/home_assistant_media_players.json");
    return nlohmann::json::parse(file);
  }();
  return states;
}

const nlohmann::json &recorded(const std::string &name) {
  return recorded_states().at(name);
}

std::vector<std::string> sources_of(const NSPanelEntityState_MediaPlayer_SourceSelection &selection) {
  return std::vector<std::string>(selection.sources().begin(), selection.sources().end());
}

std::vector<std::string> recorded_sources(const std::string &name) {
  return recorded(name)["attributes"]["source_list"].get<std::vector<std::string>>();
}

} // namespace

class RecordedMediaPlayerTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    nspm_test::SendCaptureTest::SetUp();
    // Album art URLs point the panel at the manager, as set up on every real install.
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::MANAGER_ADDRESS, "192.168.1.10");
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::MANAGER_PORT, "8000");
  }

  void TearDown() override {
    player.reset();
    row.reset();
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::MANAGER_ADDRESS, "");
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::MANAGER_PORT, "");
  }

  // Create the media player in NSPanelManager for a recorded Home Assistant media player.
  void create_player(const std::string &name, nlohmann::json entity_data = nlohmann::json::object()) {
    nlohmann::json data = django_media_player_data(recorded(name)["entity_id"].get<std::string>());
    data.update(entity_data);
    row = std::make_unique<ScopedEntity>("media_player", recorded(name)["attributes"]["friendly_name"].get<std::string>(), data);
    player = std::make_unique<HomeAssistantMediaPlayer>(row->id);
    nspm_test::mqtt_published_to(player->get_mqtt_state_topic()); // Drop the initial state.
  }

  // Deliver a recorded state the way the manager receives it from Home Assistant.
  void deliver(const std::string &name) {
    const nlohmann::json &state = recorded(name);
    auto event = home_assistant_state_changed(state["entity_id"].get<std::string>(), state["state"].get<std::string>(), state["attributes"]);
    HomeAssistantManager::test_process_event(event);
  }

  // The last state published for the NSPanels since the previous call.
  std::optional<NSPanelEntityState_MediaPlayer> last_published_state() {
    auto messages = nspm_test::mqtt_published_to(player->get_mqtt_state_topic());
    if (messages.empty()) {
      return std::nullopt;
    }
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

  // How the zones of the audio matrix are set up: the zone reports which media player feeds it and
  // that player's volume in its own attributes.
  static nlohmann::json matrix_zone_config() {
    return {{"source_volume_strategy", "player_attributes"}, {"source_entity_attribute", "active_source_entity_id"}, {"source_volume_attribute", "source_volume_level"}};
  }

  std::unique_ptr<ScopedEntity> row;
  std::unique_ptr<HomeAssistantMediaPlayer> player;
};

TEST_F(RecordedMediaPlayerTest, every_recorded_state_is_published_without_errors) {
  for (const auto &[name, state] : recorded_states().items()) {
    if (name.starts_with("_")) {
      continue;
    }
    SCOPED_TRACE(name);
    create_player(name);
    ScopedErrorLog log;

    deliver(name);

    EXPECT_TRUE(last_published_state().has_value());
    EXPECT_TRUE(log.errors().empty());
    player.reset();
    row.reset();
  }
}

TEST_F(RecordedMediaPlayerTest, matrix_zone_publishes_its_inputs_and_the_source_players_favorites) {
  create_player("matrix_zone", matrix_zone_config());

  deliver("matrix_zone");
  deliver("matrix_source_player");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING);
  EXPECT_EQ(state->media_title(), "Afire Love");
  EXPECT_EQ(state->media_artist(), "Ed Sheeran");
  EXPECT_TRUE(state->album_art_url().starts_with("http://192.168.1.10:8000/nextion-img/media_player/" + std::to_string(row->id) + "/album_art?v=")) << state->album_art_url();
  EXPECT_EQ(state->volume(), 45);
  EXPECT_TRUE(state->is_muted());
  ASSERT_TRUE(state->has_source_volume());
  EXPECT_EQ(state->source_volume(), 28);

  // The zone's inputs are the matrix inputs, one of which is the source player.
  EXPECT_EQ(sources_of(state->player_sources()), recorded_sources("matrix_zone"));
  EXPECT_EQ(state->player_sources().current_source(), "Upstairs Matrix");
  EXPECT_TRUE(state->player_sources().can_select());

  // The source player's sources are its favorites. It is playing a station that is not one of them,
  // so it reports no current source.
  ASSERT_TRUE(state->has_source_player_sources());
  EXPECT_EQ(sources_of(state->source_player_sources()), recorded_sources("matrix_source_player"));
  EXPECT_EQ(state->source_player_sources().current_source(), "");
  EXPECT_TRUE(state->source_player_sources().can_select());
}

TEST_F(RecordedMediaPlayerTest, matrix_zone_selects_inputs_on_the_zone_and_favorites_on_the_source_player) {
  create_player("matrix_zone", matrix_zone_config());
  deliver("matrix_zone");
  deliver("matrix_source_player");
  home_assistant_service_calls();

  send_panel_command([](auto *command) { command->set_select_player_source("Kitchen TV"); });
  send_panel_command([](auto *command) { command->set_select_source_player_source("Pat’s Station"); });

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 2);
  EXPECT_EQ(calls[0]["target"]["entity_id"], "media_player.acmax24_office");
  EXPECT_EQ(calls[0]["service_data"]["source"], "Kitchen TV");
  EXPECT_EQ(calls[1]["target"]["entity_id"], "media_player.upstairs_matrix");
  EXPECT_EQ(calls[1]["service_data"]["source"], "Pat’s Station");
}

TEST_F(RecordedMediaPlayerTest, apple_tv_sends_only_the_first_sources_to_the_panel) {
  create_player("apple_tv_off");
  ScopedErrorLog log(spdlog::level::warn);

  deliver("apple_tv_off");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_OFF);
  EXPECT_TRUE(state->player_sources().can_select());

  // The Apple TV lists its apps alphabetically, so the limit drops the end of the alphabet,
  // YouTube and TV included.
  std::vector<std::string> sources = recorded_sources("apple_tv_off");
  ASSERT_EQ(sources.size(), 41);
  sources.resize(MediaPlayerEntity::max_sources);
  EXPECT_EQ(sources_of(state->player_sources()), sources);
  EXPECT_EQ(state->player_sources().current_source(), "");
  auto warnings = log.errors();
  EXPECT_TRUE(std::any_of(warnings.begin(), warnings.end(), [](const std::string &warning) { return warning.find("has 41 sources, only the first 32") != std::string::npos; }));

  home_assistant_service_calls();
  send_panel_command([](auto *command) { command->set_select_player_source("YouTube"); });
  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(RecordedMediaPlayerTest, receiver_that_is_off_offers_its_inputs_without_a_current_one) {
  create_player("av_receiver_off");

  deliver("av_receiver_off");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_OFF);
  EXPECT_EQ(sources_of(state->player_sources()), recorded_sources("av_receiver_off"));
  EXPECT_EQ(state->player_sources().current_source(), "");
  EXPECT_TRUE(state->player_sources().can_select());
  EXPECT_TRUE(state->can_set_volume());
  EXPECT_FALSE(state->can_play());
}

TEST_F(RecordedMediaPlayerTest, music_assistant_player_with_its_queue_as_one_of_the_sources) {
  create_player("music_assistant_receiver");

  deliver("music_assistant_receiver");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_IDLE);
  EXPECT_EQ(sources_of(state->player_sources()), recorded_sources("music_assistant_receiver"));
  EXPECT_EQ(state->player_sources().current_source(), "Music Assistant Queue");
  EXPECT_TRUE(state->player_sources().can_select());
}

TEST_F(RecordedMediaPlayerTest, music_assistant_player_with_a_current_source_but_no_source_list) {
  create_player("music_assistant_speaker");

  deliver("music_assistant_speaker");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->player_sources().sources_size(), 0);
  EXPECT_EQ(state->player_sources().current_source(), "Music Assistant Queue");
  EXPECT_FALSE(state->player_sources().can_select());
}

TEST_F(RecordedMediaPlayerTest, matrix_controller_in_an_unknown_state_can_only_select_sources) {
  create_player("matrix_controller");

  deliver("matrix_controller");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN);
  EXPECT_EQ(sources_of(state->player_sources()), recorded_sources("matrix_controller"));
  EXPECT_TRUE(state->player_sources().can_select());
  EXPECT_FALSE(state->can_play());
  EXPECT_FALSE(state->can_set_volume());
}
