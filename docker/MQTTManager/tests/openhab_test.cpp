#include "fake_http_server.hpp"
#include "test_helpers.hpp"
#include <cover/openhab_cover.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <media_player/openhab_media_player.hpp>
#include <protobuf_nspanel_entity.pb.h>
#include <scenes/openhab_scene.hpp>
#include <set>
#include <switch/openhab_switch.hpp>
#include <thermostat/openhab_thermostat.hpp>

using nspm_test::django_cover_data;
using nspm_test::django_openhab_media_player_data;
using nspm_test::django_switch_data;
using nspm_test::FakeHttpServer;
using nspm_test::ScopedEntity;
using nspm_test::ScopedErrorLog;
using nspm_test::ScopedScene;
using nspm_test::thermostat_mode;

// An ItemStateChangedEvent as OpenHAB sends it over the websocket (the payload is a JSON string).
static std::string item_state_changed(const std::string &item, const std::string &type, const std::string &value) {
  return nlohmann::json({
                            {"type", "ItemStateChangedEvent"},
                            {"topic", "openhab/items/" + item + "/statechanged"},
                            {"payload", nlohmann::json({{"type", type}, {"value", value}, {"oldType", type}, {"oldValue", "NULL"}}).dump()},
                        })
      .dump();
}

// GET /rest/items/<item> as OpenHAB answers it.
static std::string rest_item(const std::string &item, const std::string &type, const std::string &state) {
  return nlohmann::json({{"name", item}, {"type", type}, {"state", state}, {"link", "http://openhab/rest/items/" + item}}).dump();
}

// The ItemCommandEvents sent to OpenHAB, as (item, parsed payload).
static std::vector<std::pair<std::string, nlohmann::json>> item_commands() {
  std::vector<std::pair<std::string, nlohmann::json>> commands;
  for (auto &message : OpenhabManager::test_take_sent_messages()) {
    if (message.value("type", "") == "ItemCommandEvent") {
      std::string topic = message["topic"];                       // openhab/items/<item>/command
      std::string item = topic.substr(14, topic.size() - 14 - 8); // Strip "openhab/items/" and "/command".
      commands.emplace_back(item, nlohmann::json::parse(std::string(message["payload"])));
    }
  }
  return commands;
}

// Points the OpenHAB REST API at a fake server for the length of each test.
class OpenhabTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    nspm_test::SendCaptureTest::SetUp();
    OpenhabManager::test_set_rest_api(server.address(), "test-token");
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPENHAB_ADDRESS, server.address());
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPENHAB_TOKEN, "test-token");
  }

  void TearDown() override {
    OpenhabManager::test_set_rest_api("", "");
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPENHAB_ADDRESS, "");
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPENHAB_TOKEN, "");
  }

  FakeHttpServer server;
};

// ---------------------------------------------------------------------------------------------
// Switches

class OpenhabSwitchStateTest : public OpenhabTest {};

TEST_F(OpenhabSwitchStateTest, fetches_the_initial_state_over_rest) {
  server.respond("/rest/items/Fan_Switch", rest_item("Fan_Switch", "Switch", "ON"));
  ScopedEntity row("switch", "Fan", django_switch_data("openhab", "", "Fan_Switch"));

  OpenhabSwitch fan(row.id);

  EXPECT_TRUE(fan.get_state());
  auto requests = server.requests();
  ASSERT_EQ(requests.size(), 1);
  EXPECT_EQ(requests[0].method, "GET");
  EXPECT_EQ(requests[0].path, "/rest/items/Fan_Switch");
  EXPECT_EQ(requests[0].headers["authorization"], "Bearer test-token");
}

TEST_F(OpenhabSwitchStateTest, follows_state_changes_from_openhab) {
  ScopedEntity row("switch", "Fan", django_switch_data("openhab", "", "Fan_Switch"));
  OpenhabSwitch fan(row.id);

  OpenhabManager::test_process_websocket_message(item_state_changed("Other_Switch", "OnOff", "ON"));
  EXPECT_FALSE(fan.get_state());

  OpenhabManager::test_process_websocket_message(item_state_changed("Fan_Switch", "OnOff", "ON"));
  EXPECT_TRUE(fan.get_state());
}

TEST_F(OpenhabSwitchStateTest, optimistic_mode_shows_the_new_state_immediately) {
  set_optimistic_mode(true);
  ScopedEntity row("switch", "Fan", django_switch_data("openhab", "", "Fan_Switch"));
  OpenhabSwitch fan(row.id);

  fan.turn_on(true);

  EXPECT_TRUE(fan.get_state());
}

TEST_F(OpenhabSwitchStateTest, non_optimistic_mode_waits_for_openhab) {
  set_optimistic_mode(false);
  ScopedEntity row("switch", "Fan", django_switch_data("openhab", "", "Fan_Switch"));
  OpenhabSwitch fan(row.id);

  fan.turn_on(true);
  EXPECT_FALSE(fan.get_state());

  OpenhabManager::test_process_websocket_message(item_state_changed("Fan_Switch", "OnOff", "ON"));
  EXPECT_TRUE(fan.get_state());
}

// ---------------------------------------------------------------------------------------------
// Covers (OpenHAB Rollershutter items)

class OpenhabCoverTest : public OpenhabTest {
protected:
  void create_cover(const std::string &initial_state = "0") {
    server.respond("/rest/items/LivingRoom_Shutter", rest_item("LivingRoom_Shutter", "Rollershutter", initial_state));
    row = std::make_unique<ScopedEntity>("cover", "Living room shutter", django_cover_data("openhab", "", "LivingRoom_Shutter"));
    cover = std::make_unique<OpenhabCover>(row->id);
  }

  void TearDown() override {
    cover.reset();
    row.reset();
    OpenhabTest::TearDown();
  }

  std::optional<NSPanelEntityState_Cover> last_published_state() {
    auto messages = nspm_test::mqtt_published_to(cover->get_mqtt_state_topic());
    if (messages.empty()) {
      return std::nullopt;
    }
    NSPanelEntityState state;
    EXPECT_TRUE(state.ParseFromString(messages.back().payload));
    return state.cover();
  }

  std::unique_ptr<ScopedEntity> row;
  std::unique_ptr<OpenhabCover> cover;
};

TEST_F(OpenhabCoverTest, fetches_initial_state_over_rest_and_inverts_the_position) {
  create_cover("25");

  EXPECT_EQ(cover->get_type(), MQTT_MANAGER_ENTITY_TYPE::COVER);
  EXPECT_EQ(cover->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB);
  EXPECT_TRUE(cover->has_position());
  EXPECT_EQ(cover->get_position(), 75);
  EXPECT_TRUE(cover->can_open());
  EXPECT_TRUE(cover->can_close());
  EXPECT_TRUE(cover->can_stop());
  EXPECT_TRUE(cover->can_set_position());

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_Cover_CoverState_OPEN);
  EXPECT_EQ(state->position(), 75);
}

TEST_F(OpenhabCoverTest, follows_state_changes_from_openhab) {
  create_cover("0");
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("LivingRoom_Shutter", "Percent", "100"));

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_Cover_CoverState_CLOSED);
  EXPECT_EQ(state->position(), 0);
}

TEST_F(OpenhabCoverTest, null_state_becomes_unavailable) {
  create_cover("0");

  OpenhabManager::test_process_websocket_message(item_state_changed("LivingRoom_Shutter", "Percent", "NULL"));

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_Cover_CoverState_UNAVAILABLE);
  EXPECT_FALSE(state->has_position());
}

TEST_F(OpenhabCoverTest, commands_use_rollershutter_percent_and_stop) {
  create_cover("0");
  item_commands(); // Drop the initial REST request's side effects, if any.

  cover->open();
  cover->close();
  cover->stop();
  cover->set_position(25);

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 4);
  EXPECT_EQ(commands[0], std::make_pair(std::string("LivingRoom_Shutter"), nlohmann::json({{"type", "UpDown"}, {"value", "UP"}})));
  EXPECT_EQ(commands[1], std::make_pair(std::string("LivingRoom_Shutter"), nlohmann::json({{"type", "UpDown"}, {"value", "DOWN"}})));
  EXPECT_EQ(commands[2], std::make_pair(std::string("LivingRoom_Shutter"), nlohmann::json({{"type", "StopMove"}, {"value", "STOP"}})));
  EXPECT_EQ(commands[3], std::make_pair(std::string("LivingRoom_Shutter"), nlohmann::json({{"type", "Percent"}, {"value", "75"}})));
}

TEST_F(OpenhabCoverTest, ignores_state_changes_for_other_items) {
  create_cover("0");
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("Other_Shutter", "Percent", "100"));

  EXPECT_FALSE(last_published_state().has_value());
}

// ---------------------------------------------------------------------------------------------
// Scenes (OpenHAB rules, run over REST)

class OpenhabSceneTest : public OpenhabTest {};

TEST_F(OpenhabSceneTest, loads_config_from_database) {
  ScopedScene row("openhab", "Movie time", "movie_rule", 3, 6, 2);

  OpenhabScene scene(row.id);

  EXPECT_EQ(scene.get_name(), "Movie time");
  EXPECT_EQ(scene.get_entity_page_id(), 6);
  EXPECT_EQ(scene.get_entity_page_slot(), 2);
  EXPECT_FALSE(scene.is_global());
  EXPECT_FALSE(scene.can_save());
  EXPECT_EQ(scene.get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB);
}

TEST_F(OpenhabSceneTest, activating_runs_the_rule) {
  server.respond("/rest/rules/movie_rule/runnow", "");
  ScopedScene row("openhab", "Movie time", "movie_rule", std::nullopt);
  OpenhabScene scene(row.id);

  scene.activate();

  auto requests = server.requests();
  ASSERT_EQ(requests.size(), 1);
  EXPECT_EQ(requests[0].method, "POST");
  EXPECT_EQ(requests[0].path, "/rest/rules/movie_rule/runnow");
  EXPECT_TRUE(OpenhabManager::test_take_sent_messages().empty());
}

// activate() used to build the Authorization header with fmt::format(...).c_str() on a temporary, so
// the header list held a dangling pointer and sent whatever that memory held by then.
TEST_F(OpenhabSceneTest, activating_sends_the_token) {
  server.respond("/rest/rules/movie_rule/runnow", "");
  ScopedScene row("openhab", "Movie time", "movie_rule", std::nullopt);
  OpenhabScene scene(row.id);

  scene.activate();

  auto requests = server.requests();
  ASSERT_EQ(requests.size(), 1);
  EXPECT_EQ(requests[0].headers["authorization"], "Bearer test-token");
}

// ---------------------------------------------------------------------------------------------
// Thermostats

// entity_data for an OpenHAB thermostat exactly as web/rest.py put_thermostat_entity() stores it.
static nlohmann::json django_openhab_thermostat_data() {
  return {
      {"controller", "openhab"},
      {"fan_modes", nlohmann::json::array({thermostat_mode("auto", "Auto"), thermostat_mode("high", "High")})},
      {"hvac_modes", nlohmann::json::array({thermostat_mode("off", "Off"), thermostat_mode("heat", "Heat")})},
      {"preset_modes", nlohmann::json::array({thermostat_mode("home", "Home"), thermostat_mode("away", "Away")})},
      {"swing_modes", nlohmann::json::array({thermostat_mode("off", "Off"), thermostat_mode("on", "On")})},
      {"swingh_modes", nlohmann::json::array({thermostat_mode("off", "Off"), thermostat_mode("on", "On")})},
      {"use_current_temperature", true},
      {"home_assistant_name", ""},
      {"openhab_fan_mode_item", "HP_Fan"},
      {"openhab_hvac_mode_item", "HP_Mode"},
      {"openhab_preset_mode_item", "HP_Preset"},
      {"openhab_swing_mode_item", "HP_Swing"},
      {"openhab_swingh_mode_item", "HP_SwingH"},
      {"openhab_temperature_item", "HP_Target"},
      {"openhab_current_temperature_item", "HP_Current"},
      {"step_size", 0.5},
  };
}

class OpenhabThermostatTest : public OpenhabTest {
protected:
  void create_thermostat() {
    row = std::make_unique<ScopedEntity>("thermostat", "Heat pump", django_openhab_thermostat_data(), 2, 8, 1);
    thermostat = std::make_unique<OpenhabThermostat>(row->id);
  }

  void TearDown() override {
    thermostat.reset();
    row.reset();
    OpenhabTest::TearDown();
  }

  // The last state published for the NSPanels since the previous call.
  std::optional<NSPanelEntityState_Thermostat> last_published_state() {
    auto messages = nspm_test::mqtt_published_to(thermostat->get_mqtt_state_topic());
    if (messages.empty()) {
      return std::nullopt;
    }
    NSPanelEntityState state;
    EXPECT_TRUE(state.ParseFromString(messages.back().payload));
    return state.thermostat();
  }

  std::unique_ptr<ScopedEntity> row;
  std::unique_ptr<OpenhabThermostat> thermostat;
};

TEST_F(OpenhabThermostatTest, set_temperature_commands_the_target_item) {
  create_thermostat();

  thermostat->set_temperature(21.5);

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 1);
  EXPECT_EQ(commands[0].first, "HP_Target");
  EXPECT_EQ(commands[0].second, nlohmann::json({{"type", "Decimal"}, {"value", 21.5}}));
}

TEST_F(OpenhabThermostatTest, set_fan_mode_commands_the_fan_item) {
  create_thermostat();

  thermostat->set_fan_mode("high");

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 1);
  EXPECT_EQ(commands[0].first, "HP_Fan");
  EXPECT_EQ(commands[0].second, nlohmann::json({{"type", "String"}, {"value", "high"}}));
}

TEST_F(OpenhabThermostatTest, set_mode_commands_the_hvac_item) {
  create_thermostat();

  thermostat->set_mode("heat");

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 1);
  EXPECT_EQ(commands[0].first, "HP_Mode");
  EXPECT_EQ(commands[0].second, nlohmann::json({{"type", "String"}, {"value", "heat"}}));
}

TEST_F(OpenhabThermostatTest, follows_target_temperature_and_mode_from_openhab) {
  create_thermostat();

  OpenhabManager::test_process_websocket_message(item_state_changed("HP_Target", "Decimal", "22.5"));
  OpenhabManager::test_process_websocket_message(item_state_changed("HP_Mode", "String", "heat"));
  OpenhabManager::test_process_websocket_message(item_state_changed("HP_Fan", "String", "high"));

  EXPECT_FLOAT_EQ(thermostat->get_temperature(), 22.5);
  EXPECT_EQ(thermostat->get_mode().value, "heat");
  EXPECT_EQ(thermostat->get_fan_mode().value, "high");
}

TEST_F(OpenhabThermostatTest, fetches_the_initial_mode_over_rest) {
  server.respond("/rest/items/HP_Mode", rest_item("HP_Mode", "String", "heat"));

  create_thermostat();

  EXPECT_EQ(thermostat->get_mode().value, "heat");
}

// #386: the initial target temperature fetched over REST was rounded to a whole degree, so 21.5 showed as 22.
TEST_F(OpenhabThermostatTest, fetches_the_initial_target_temperature_over_rest) {
  server.respond("/rest/items/HP_Target", rest_item("HP_Target", "Number", "21.5"));

  create_thermostat();

  EXPECT_FLOAT_EQ(thermostat->get_temperature(), 21.5);
}

// The React UI stores openhab_preset_mode_item, openhab_swing_mode_item and openhab_swingh_mode_item, but only the
// names the older settings form used were read, so presets and swing modes were commanded on "openhab/items//command".
TEST_F(OpenhabThermostatTest, set_preset_and_swing_command_their_items) {
  create_thermostat();

  thermostat->set_preset("away");
  thermostat->set_swing_mode("on");
  thermostat->set_swing_horizontal_mode("on");

  // Without optimistic mode each call also re-sends the earlier changes OpenHAB hasn't confirmed yet, so check which
  // items were commanded rather than how often.
  std::set<std::string> items;
  for (const auto &command : item_commands()) {
    items.insert(command.first);
  }
  EXPECT_EQ(items, (std::set<std::string>{"HP_Preset", "HP_Swing", "HP_SwingH"}));
}

// The initial preset fetched over REST was also compared with the current HVAC mode instead of the current preset,
// and the horizontal swing fetch checked the vertical swing item's name.
TEST_F(OpenhabThermostatTest, fetches_the_initial_preset_and_swing_over_rest) {
  server.respond("/rest/items/HP_Preset", rest_item("HP_Preset", "String", "away"));
  server.respond("/rest/items/HP_Swing", rest_item("HP_Swing", "String", "on"));
  server.respond("/rest/items/HP_SwingH", rest_item("HP_SwingH", "String", "on"));

  create_thermostat();

  EXPECT_EQ(thermostat->get_preset().value, "away");
  EXPECT_EQ(thermostat->get_swing_mode().value, "on");
  EXPECT_EQ(thermostat->get_swing_horizontal_mode().value, "on");
}

// #386: the current temperature item was never observed, its callback compared events against the target temperature
// item, and the reading was never marked available, so the panels showed 0.0°.
TEST_F(OpenhabThermostatTest, publishes_the_current_temperature_from_openhab) {
  create_thermostat();
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("HP_Current", "Decimal", "19.5"));

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_TRUE(state->has_current_temperature());
  EXPECT_FLOAT_EQ(state->current_temperature(), 19.5);
}

// #386: the current temperature is also fetched over REST when the thermostat loads. That fetch read the wrong field.
TEST_F(OpenhabThermostatTest, fetches_the_initial_current_temperature_over_rest) {
  server.respond("/rest/items/HP_Current", rest_item("HP_Current", "Number", "19.5"));

  create_thermostat();

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_TRUE(state->has_current_temperature());
  EXPECT_FLOAT_EQ(state->current_temperature(), 19.5);
}

// Thermostats saved by the older settings form have the original item names, without "_mode".
TEST_F(OpenhabThermostatTest, reads_preset_and_swing_items_saved_by_the_older_settings_form) {
  auto data = django_openhab_thermostat_data();
  for (const std::string kind : {"preset", "swing", "swingh"}) {
    data.erase("openhab_" + kind + "_mode_item");
  }
  data["openhab_preset_item"] = "Old_Preset";
  data["openhab_swing_item"] = "Old_Swing";
  data["openhab_swingh_item"] = "Old_SwingH";
  row = std::make_unique<ScopedEntity>("thermostat", "Heat pump", data, 2, 8, 1);
  thermostat = std::make_unique<OpenhabThermostat>(row->id);

  thermostat->set_preset("away");
  thermostat->set_swing_mode("on");
  thermostat->set_swing_horizontal_mode("on");

  // Without optimistic mode each call also re-sends the earlier changes OpenHAB hasn't confirmed yet, so check which
  // items were commanded rather than how often.
  std::set<std::string> items;
  for (const auto &command : item_commands()) {
    items.insert(command.first);
  }
  EXPECT_EQ(items, (std::set<std::string>{"Old_Preset", "Old_Swing", "Old_SwingH"}));
}

// The callbacks read the item from the topic before checking it had one. OpenhabManager already drops such topics, so
// this only guards the callbacks themselves.
TEST_F(OpenhabThermostatTest, callbacks_ignore_a_topic_without_an_item) {
  create_thermostat();
  float temperature_before = thermostat->get_temperature();
  auto event = nlohmann::json::parse(item_state_changed("HP_Target", "Decimal", "25"));
  event["topic"] = "openhab/items";

  thermostat->openhab_target_temperature_event_callback(event);
  thermostat->openhab_current_temperature_event_callback(event);
  thermostat->openhab_fan_mode_event_callback(event);
  thermostat->openhab_mode_event_callback(event);
  thermostat->openhab_preset_event_callback(event);
  thermostat->openhab_swing_event_callback(event);
  thermostat->openhab_swingh_event_callback(event);

  EXPECT_FLOAT_EQ(thermostat->get_temperature(), temperature_before);
}

// ---------------------------------------------------------------------------------------------
// Media players (an OpenHAB Player item for transport and a Dimmer/Number item for volume)

class OpenhabMediaPlayerTest : public OpenhabTest {
protected:
  void create_player(const std::string &control_state = "PAUSE", const std::string &volume_state = "40") {
    server.respond("/rest/items/Lounge_Control", rest_item("Lounge_Control", "Player", control_state));
    server.respond("/rest/items/Lounge_Volume", rest_item("Lounge_Volume", "Dimmer", volume_state));
    row = std::make_unique<ScopedEntity>("media_player", "Lounge speaker", django_openhab_media_player_data("Lounge_Control", "Lounge_Volume", 10), 5, 11, 4);
    player = std::make_unique<OpenhabMediaPlayer>(row->id);
  }

  void TearDown() override {
    player.reset();
    row.reset();
    OpenhabTest::TearDown();
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
  std::unique_ptr<OpenhabMediaPlayer> player;
};

TEST_F(OpenhabMediaPlayerTest, loads_config_and_fetches_the_initial_state_over_rest) {
  create_player("PLAY", "40");

  EXPECT_EQ(player->get_name(), "Lounge speaker");
  EXPECT_EQ(player->get_room_id(), 5);
  EXPECT_EQ(player->get_entity_page_id(), 11);
  EXPECT_EQ(player->get_entity_page_slot(), 4);
  EXPECT_EQ(player->get_type(), MQTT_MANAGER_ENTITY_TYPE::MEDIA_PLAYER);
  EXPECT_EQ(player->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB);

  std::set<std::string> paths;
  for (auto &request : server.requests()) {
    EXPECT_EQ(request.method, "GET");
    EXPECT_EQ(request.headers["authorization"], "Bearer test-token");
    paths.insert(request.path);
  }
  EXPECT_EQ(paths, std::set<std::string>({"/rest/items/Lounge_Control", "/rest/items/Lounge_Volume"}));

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->media_player_id(), row->id);
  EXPECT_EQ(state->name(), "Lounge speaker");
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING);
  EXPECT_EQ(state->volume(), 40);
  EXPECT_EQ(state->volume_step(), 10);
}

TEST_F(OpenhabMediaPlayerTest, offers_only_what_the_two_items_can_do) {
  create_player();

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_TRUE(state->can_play());
  EXPECT_TRUE(state->can_pause());
  EXPECT_TRUE(state->can_next_track());
  EXPECT_TRUE(state->can_previous_track());
  EXPECT_TRUE(state->can_set_volume());
  EXPECT_FALSE(state->can_mute());
  EXPECT_FALSE(state->has_source_volume());
  EXPECT_FALSE(state->has_media_position());
  EXPECT_EQ(state->player_sources().sources_size(), 0);
  EXPECT_FALSE(state->has_source_player_sources());
}

TEST_F(OpenhabMediaPlayerTest, follows_control_item_state_changes) {
  create_player("PAUSE");
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", "PLAY"));
  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING);

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", "PAUSE"));
  state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PAUSED);
}

TEST_F(OpenhabMediaPlayerTest, control_states_without_a_playback_state_become_unknown) {
  create_player("PLAY");
  for (const std::string &control_state : {"NULL", "UNDEF", "REWIND", "FASTFORWARD"}) {
    SCOPED_TRACE(control_state);
    OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", "PLAY"));
    last_published_state();

    OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", control_state));

    auto state = last_published_state();
    ASSERT_TRUE(state.has_value());
    EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN);
  }
}

TEST_F(OpenhabMediaPlayerTest, follows_volume_item_state_changes) {
  create_player("PLAY", "40");
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "Percent", "55"));

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->volume(), 55);
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING);
}

TEST_F(OpenhabMediaPlayerTest, volume_is_rounded_and_clamped) {
  create_player("PLAY", "40");

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "Decimal", "33.6"));
  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->volume(), 34);

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "Decimal", "150"));
  state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->volume(), 100);

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "Decimal", "-5"));
  state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->volume(), 0);
}

TEST_F(OpenhabMediaPlayerTest, volume_that_is_not_a_number_keeps_the_last_volume) {
  create_player("PLAY", "40");
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "UnDef", "NULL"));

  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(OpenhabMediaPlayerTest, initial_volume_that_is_not_a_number_is_ignored) {
  create_player("PLAY", "NULL");

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING);
  EXPECT_EQ(state->volume(), 0);
}

TEST_F(OpenhabMediaPlayerTest, ignores_state_changes_for_other_items) {
  create_player();
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("Kitchen_Control", "PlayPause", "PLAY"));
  OpenhabManager::test_process_websocket_message(item_state_changed("Kitchen_Volume", "Percent", "90"));

  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(OpenhabMediaPlayerTest, unchanged_state_is_not_published_again) {
  create_player("PLAY", "40");
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", "PLAY"));
  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "Percent", "40"));

  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(OpenhabMediaPlayerTest, malformed_events_are_ignored) {
  create_player();
  last_published_state();

  // No payload, a payload that is not JSON, a payload without a value and a topic without an item.
  OpenhabManager::test_process_websocket_message(nlohmann::json({{"type", "ItemStateChangedEvent"}, {"topic", "openhab/items/Lounge_Control/statechanged"}}).dump());
  OpenhabManager::test_process_websocket_message(nlohmann::json({{"type", "ItemStateChangedEvent"}, {"topic", "openhab/items/Lounge_Control/statechanged"}, {"payload", "not json"}}).dump());
  OpenhabManager::test_process_websocket_message(nlohmann::json({{"type", "ItemStateChangedEvent"}, {"topic", "openhab/items/Lounge_Control/statechanged"}, {"payload", nlohmann::json({{"type", "PlayPause"}}).dump()}}).dump());
  player->openhab_event_callback({{"type", "ItemStateChangedEvent"}, {"topic", "openhab"}, {"payload", nlohmann::json({{"type", "PlayPause"}, {"value", "PLAY"}}).dump()}});
  player->openhab_event_callback({{"topic", "openhab/items/Lounge_Control/statechanged"}});

  EXPECT_FALSE(last_published_state().has_value());
}

TEST_F(OpenhabMediaPlayerTest, playback_and_volume_commands_go_to_their_items) {
  create_player();
  item_commands();

  player->play();
  player->pause();
  player->next_track();
  player->previous_track();
  player->set_volume(35);

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 5);
  EXPECT_EQ(commands[0], std::make_pair(std::string("Lounge_Control"), nlohmann::json({{"type", "PlayPause"}, {"value", "PLAY"}})));
  EXPECT_EQ(commands[1], std::make_pair(std::string("Lounge_Control"), nlohmann::json({{"type", "PlayPause"}, {"value", "PAUSE"}})));
  EXPECT_EQ(commands[2], std::make_pair(std::string("Lounge_Control"), nlohmann::json({{"type", "NextPrevious"}, {"value", "NEXT"}})));
  EXPECT_EQ(commands[3], std::make_pair(std::string("Lounge_Control"), nlohmann::json({{"type", "NextPrevious"}, {"value", "PREVIOUS"}})));
  EXPECT_EQ(commands[4], std::make_pair(std::string("Lounge_Volume"), nlohmann::json({{"type", "Percent"}, {"value", 35}})));
}

TEST_F(OpenhabMediaPlayerTest, panel_commands_are_sent_to_openhab) {
  create_player();
  item_commands();

  send_panel_command([](auto *command) { command->set_playback_action(NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PLAY); });
  send_panel_command([](auto *command) { command->set_playback_action(NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_NEXT_TRACK); });
  send_panel_command([](auto *command) {
    command->set_has_volume(true);
    command->set_volume(150);
  });

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 3);
  EXPECT_EQ(commands[0], std::make_pair(std::string("Lounge_Control"), nlohmann::json({{"type", "PlayPause"}, {"value", "PLAY"}})));
  EXPECT_EQ(commands[1], std::make_pair(std::string("Lounge_Control"), nlohmann::json({{"type", "NextPrevious"}, {"value", "NEXT"}})));
  EXPECT_EQ(commands[2], std::make_pair(std::string("Lounge_Volume"), nlohmann::json({{"type", "Percent"}, {"value", 100}})));
}

TEST_F(OpenhabMediaPlayerTest, unsupported_panel_commands_send_nothing) {
  create_player();
  item_commands();

  send_panel_command([](auto *command) {
    command->set_has_muted(true);
    command->set_muted(true);
  });
  send_panel_command([](auto *command) {
    command->set_has_source_volume(true);
    command->set_source_volume(20);
  });
  send_panel_command([](auto *command) { command->set_select_player_source("Radio"); });
  send_panel_command([](auto *command) { command->set_select_source_player_source("Spotify"); });

  EXPECT_TRUE(item_commands().empty());
}

TEST_F(OpenhabMediaPlayerTest, commands_for_other_media_players_are_ignored) {
  create_player();
  item_commands();

  NSPanelMQTTManagerCommand command;
  command.mutable_media_player_command()->set_media_player_id(row->id + 1000);
  command.mutable_media_player_command()->set_playback_action(NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PLAY);
  player->command_callback(command);

  EXPECT_TRUE(item_commands().empty());
}

TEST_F(OpenhabMediaPlayerTest, reload_with_new_items_follows_the_new_items) {
  create_player("PLAY", "40");
  server.respond("/rest/items/Kitchen_Control", rest_item("Kitchen_Control", "Player", "PAUSE"));
  server.respond("/rest/items/Kitchen_Volume", rest_item("Kitchen_Volume", "Dimmer", "20"));
  row->update_entity_data(django_openhab_media_player_data("Kitchen_Control", "Kitchen_Volume"));
  last_published_state();

  player->reload_config();

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state->state(), NSPanelEntityState_MediaPlayer_PlaybackState_PAUSED);
  EXPECT_EQ(state->volume(), 20);

  // The previous items are no longer observed.
  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", "PLAY"));
  EXPECT_FALSE(last_published_state().has_value());

  item_commands();
  player->play();
  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 1);
  EXPECT_EQ(commands[0].first, "Kitchen_Control");
}

TEST_F(OpenhabMediaPlayerTest, reload_with_the_same_items_keeps_the_state) {
  create_player("PLAY", "40");
  last_published_state();
  size_t requests_before_reload = server.requests().size();

  player->reload_config();

  // Not reattached, so nothing is fetched again and the known state is kept.
  EXPECT_EQ(server.requests().size(), requests_before_reload);
  EXPECT_FALSE(last_published_state().has_value());
}

static bool any_line_contains(const std::vector<std::string> &lines, const std::string &text) {
  return std::any_of(lines.begin(), lines.end(), [&text](const std::string &line) { return line.find(text) != std::string::npos; });
}

TEST_F(OpenhabMediaPlayerTest, missing_items_are_logged_and_nothing_is_sent) {
  // rest.py requires both items, but the row may have been saved without them by an older version.
  nlohmann::json entity_data = django_openhab_media_player_data("", "");
  entity_data.erase("openhab_item_control");
  entity_data.erase("openhab_item_volume");
  ScopedErrorLog errors;
  row = std::make_unique<ScopedEntity>("media_player", "Lounge speaker", entity_data);
  player = std::make_unique<OpenhabMediaPlayer>(row->id);

  EXPECT_TRUE(any_line_contains(errors.errors(), "No openhab control item"));
  EXPECT_TRUE(any_line_contains(errors.errors(), "No openhab volume item"));
  EXPECT_TRUE(server.requests().empty());

  player->play();
  player->set_volume(50);
  EXPECT_TRUE(item_commands().empty());
  EXPECT_TRUE(any_line_contains(errors.errors(), "no item configured"));
}

TEST_F(OpenhabMediaPlayerTest, empty_items_are_not_observed_and_nothing_is_sent) {
  row = std::make_unique<ScopedEntity>("media_player", "Lounge speaker", django_openhab_media_player_data("", ""));
  player = std::make_unique<OpenhabMediaPlayer>(row->id);

  EXPECT_TRUE(server.requests().empty());
  player->play();
  player->set_volume(50);
  EXPECT_TRUE(item_commands().empty());
}

TEST_F(OpenhabMediaPlayerTest, destroyed_player_stops_observing_openhab) {
  create_player();
  player.reset();

  // Would call into the destroyed player if its observers were still attached.
  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Control", "PlayPause", "PLAY"));
  OpenhabManager::test_process_websocket_message(item_state_changed("Lounge_Volume", "Percent", "10"));
}
