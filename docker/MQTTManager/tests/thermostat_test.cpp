#include "test_helpers.hpp"
#include <gtest/gtest.h>
#include <thermostat/home_assistant_thermostat.hpp>

using nspm_test::django_thermostat_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::home_assistant_state_changed;
using nspm_test::ScopedEntity;

class HomeAssistantThermostatTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    nspm_test::SendCaptureTest::SetUp();
    row = std::make_unique<ScopedEntity>("thermostat", "Hallway heat pump", django_thermostat_data("climate.hallway"), 2, 8, 1);
    thermostat = std::make_unique<HomeAssistantThermostat>(row->id);
    home_assistant_service_calls(); // Drop anything sent while loading.
  }

  void TearDown() override {
    thermostat.reset();
    row.reset();
  }

  void receive_state(const std::string &hvac_mode, const nlohmann::json &attributes) {
    auto event = home_assistant_state_changed("climate.hallway", hvac_mode, attributes);
    HomeAssistantManager::test_process_event(event);
  }

  std::unique_ptr<ScopedEntity> row;
  std::unique_ptr<HomeAssistantThermostat> thermostat;
};

static std::vector<std::string> values(const std::vector<ThermostatOptionHolder> &options) {
  std::vector<std::string> result;
  for (auto &option : options) {
    result.push_back(option.value);
  }
  return result;
}

TEST_F(HomeAssistantThermostatTest, loads_config_from_database) {
  EXPECT_EQ(thermostat->get_name(), "Hallway heat pump");
  EXPECT_EQ(thermostat->get_room_id(), 2);
  EXPECT_EQ(thermostat->get_entity_page_id(), 8);
  EXPECT_EQ(thermostat->get_entity_page_slot(), 1);
  EXPECT_EQ(thermostat->get_type(), MQTT_MANAGER_ENTITY_TYPE::THERMOSTAT);
  EXPECT_EQ(values(thermostat->get_supported_modes()), std::vector<std::string>({"off", "heat", "cool"}));
  EXPECT_EQ(values(thermostat->get_supported_fan_modes()), std::vector<std::string>({"auto", "high"}));
  EXPECT_TRUE(thermostat->get_supported_presets().empty());
  EXPECT_EQ(thermostat->get_supported_modes()[1].label, "Heat");
}

TEST_F(HomeAssistantThermostatTest, set_temperature_calls_climate_set_temperature) {
  thermostat->set_temperature(21.5);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({
                          {"type", "call_service"},
                          {"domain", "climate"},
                          {"service", "set_temperature"},
                          {"target", {{"entity_id", "climate.hallway"}}},
                          {"service_data", {{"temperature", 21.5}}},
                      }));
}

TEST_F(HomeAssistantThermostatTest, set_mode_calls_climate_set_hvac_mode) {
  receive_state("off", {{"temperature", 20}});

  thermostat->set_mode("heat");

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service"], "set_hvac_mode");
  EXPECT_EQ(calls[0]["service_data"], nlohmann::json({{"hvac_mode", "heat"}}));
}

TEST_F(HomeAssistantThermostatTest, set_fan_mode_calls_climate_set_fan_mode) {
  receive_state("off", {{"temperature", 20}, {"fan_mode", "auto"}});

  thermostat->set_fan_mode("high");

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service"], "set_fan_mode");
  EXPECT_EQ(calls[0]["service_data"], nlohmann::json({{"fan_mode", "high"}}));
}

TEST_F(HomeAssistantThermostatTest, unsupported_mode_is_ignored) {
  receive_state("off", {{"temperature", 20}});

  thermostat->set_mode("dry");

  EXPECT_TRUE(home_assistant_service_calls().empty());
  EXPECT_EQ(thermostat->get_mode().value, "off");
}

TEST_F(HomeAssistantThermostatTest, follows_state_changes_from_home_assistant) {
  receive_state("cool", {{"temperature", 23.5}, {"fan_mode", "high"}, {"current_temperature", 25}});

  EXPECT_EQ(thermostat->get_mode().value, "cool");
  EXPECT_EQ(thermostat->get_fan_mode().value, "high");
  EXPECT_FLOAT_EQ(thermostat->get_temperature(), 23.5);
}

TEST_F(HomeAssistantThermostatTest, unsupported_mode_from_home_assistant_is_ignored) {
  receive_state("heat", {{"temperature", 20}});
  receive_state("dry", {{"temperature", 20}});

  EXPECT_EQ(thermostat->get_mode().value, "heat");
}

TEST_F(HomeAssistantThermostatTest, only_changed_settings_are_sent) {
  receive_state("heat", {{"temperature", 20}, {"fan_mode", "auto"}});

  thermostat->set_temperature(20);
  EXPECT_TRUE(home_assistant_service_calls().empty());

  thermostat->set_temperature(22);
  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service"], "set_temperature");
}
