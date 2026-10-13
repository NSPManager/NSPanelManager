#include "fake_http_server.hpp"
#include "test_helpers.hpp"
#include <gtest/gtest.h>
#include <protobuf_nspanel_entity.pb.h>
#include <scenes/openhab_scene.hpp>
#include <switch/openhab_switch.hpp>
#include <thermostat/openhab_thermostat.hpp>

using nspm_test::django_switch_data;
using nspm_test::FakeHttpServer;
using nspm_test::ScopedEntity;
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
      std::string topic = message["topic"];                                    // openhab/items/<item>/command
      std::string item = topic.substr(14, topic.size() - 14 - 8);              // Strip "openhab/items/" and "/command".
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

// KNOWN BUG: OpenhabScene::activate() builds the Authorization header with
// fmt::format(...).c_str() on a temporary, so the header list holds a dangling pointer and the
// header sent is whatever that memory holds by then. Remove DISABLED_ when fixing.
TEST_F(OpenhabSceneTest, DISABLED_activating_sends_the_token) {
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

// KNOWN BUG: the initial target temperature fetched over REST is rounded to a whole degree
// (std::round without the * 10 / 10 used for websocket events), so 21.5 shows as 22.
// Remove DISABLED_ when fixing.
TEST_F(OpenhabThermostatTest, DISABLED_fetches_the_initial_target_temperature_over_rest) {
  server.respond("/rest/items/HP_Target", rest_item("HP_Target", "Number", "21.5"));

  create_thermostat();

  EXPECT_FLOAT_EQ(thermostat->get_temperature(), 21.5);
}

// KNOWN BUG: OpenhabThermostat reads openhab_preset_item, openhab_swing_item and
// openhab_swingh_item, but Django stores openhab_preset_mode_item, openhab_swing_mode_item and
// openhab_swingh_mode_item. Presets and swing modes are commanded on "openhab/items//command"
// and never follow OpenHAB. Remove DISABLED_ when fixing.
TEST_F(OpenhabThermostatTest, DISABLED_set_preset_and_swing_command_their_items) {
  create_thermostat();

  thermostat->set_preset("away");
  thermostat->set_swing_mode("on");
  thermostat->set_swing_horizontal_mode("on");

  auto commands = item_commands();
  ASSERT_EQ(commands.size(), 3);
  EXPECT_EQ(commands[0].first, "HP_Preset");
  EXPECT_EQ(commands[1].first, "HP_Swing");
  EXPECT_EQ(commands[2].first, "HP_SwingH");
}

// KNOWN BUG: see DISABLED_set_preset_and_swing_command_their_items. Once the item names are
// read, the initial preset fetched over REST is also compared with the current HVAC mode
// instead of the current preset, and the horizontal swing fetch checks the vertical swing item.
TEST_F(OpenhabThermostatTest, DISABLED_fetches_the_initial_preset_and_swing_over_rest) {
  server.respond("/rest/items/HP_Preset", rest_item("HP_Preset", "String", "away"));
  server.respond("/rest/items/HP_Swing", rest_item("HP_Swing", "String", "on"));
  server.respond("/rest/items/HP_SwingH", rest_item("HP_SwingH", "String", "on"));

  create_thermostat();

  EXPECT_EQ(thermostat->get_preset().value, "away");
  EXPECT_EQ(thermostat->get_swing_mode().value, "on");
  EXPECT_EQ(thermostat->get_swing_horizontal_mode().value, "on");
}

// KNOWN BUG: the current temperature item is never observed (reload_config detaches it but
// never attaches it), and its callback compares events against the target temperature item
// and never marks the reading available. The panels never show the thermostat's own reading.
// Remove DISABLED_ when fixing.
TEST_F(OpenhabThermostatTest, DISABLED_publishes_the_current_temperature_from_openhab) {
  create_thermostat();
  last_published_state();

  OpenhabManager::test_process_websocket_message(item_state_changed("HP_Current", "Decimal", "19.5"));

  auto state = last_published_state();
  ASSERT_TRUE(state.has_value());
  EXPECT_TRUE(state->has_current_temperature());
  EXPECT_FLOAT_EQ(state->current_temperature(), 19.5);
}
