#include "home_assistant_thermostat.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include "thermostat/thermostat.hpp"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <cstdint>
#include <gtest/gtest.h>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

HomeAssistantThermostat::HomeAssistantThermostat(uint32_t thermostat_id) : ThermostatEntity(thermostat_id) {
  // Process Home Assistant specific details. General thermostat data is loaded in the "ThermostatEntity" constructor.
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantThermostat has not been recognized as controlled by HOME_ASSISTANT. Will stop processing thermostat.");
    return;
  }

  nlohmann::json entity_data;
  try {
    auto thermostat = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = thermostat.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load thermostat {}: {}", this->_id, e.what());
    return;
  }

  if (entity_data.contains("home_assistant_name")) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No home assistant name defined for Thermostat {}::{}", this->_id, this->_name);
    return;
  }
  SPDLOG_DEBUG("Loaded thermostat {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantThermostat::home_assistant_event_callback, this, _1));

  if (this->_home_assistant_name.rfind("climate.", 0) != 0) {
    SPDLOG_ERROR("Unknown type of home assistant entity '{}'. Expected climate.", this->_home_assistant_name);
  }

  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

HomeAssistantThermostat::~HomeAssistantThermostat() {
  HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantThermostat::home_assistant_event_callback, this, _1));
}

void HomeAssistantThermostat::send_state_update_to_controller() {
  if (this->_requested_mode != this->_current_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_hvac_mode"},
        {"service_data", {{"hvac_mode", this->_requested_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_fan_mode != this->_current_fan_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_fan_mode"},
        {"service_data", {{"fan_mode", this->_requested_fan_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_preset != this->_current_preset) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_preset_mode"},
        {"service_data", {{"preset_mode", this->_requested_preset.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_swing_mode != this->_current_swing_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_swing_mode"},
        {"service_data", {{"swing_mode", this->_requested_swing_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_swingh_mode != this->_current_swingh_mode) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_swing_horizontal_mode"},
        {"service_data", {{"swing_horizontal_mode", this->_requested_swingh_mode.value}}}};
    HomeAssistantManager::send_json(command);
  }

  if (this->_requested_temperature != this->_current_temperature) {
    nlohmann::json command = {
        {"type", "call_service"},
        {"domain", "climate"},
        {"target", {{"entity_id", this->_home_assistant_name}}},
        {"service", "set_temperature"},
        {"service_data", {{"temperature", this->_requested_temperature}}}};
    HomeAssistantManager::send_json(command);
  }
}

void HomeAssistantThermostat::home_assistant_event_callback(nlohmann::json data) {
  if (!data.contains("event")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event' key.", this->_id, this->_name);
    return;
  } else if (!data["event"].contains("event_type")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event.event_type' key.", this->_id, this->_name);
    return;
  } else if (!data["event"].contains("data")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event.data' key.", this->_id, this->_name);
    return;
  } else if (!data["event"]["data"].contains("entity_id")) [[unlikely]] {
    SPDLOG_ERROR("Thermostat {}::{} received malformed event data from HA. Data contains no 'event.data.entity_id' key.", this->_id, this->_name);
    return;
  }

  if (std::string(data["event"]["event_type"]).compare("state_changed") == 0) {
    if (std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) == 0) {
      SPDLOG_DEBUG("Got event update for HA thermostat {}::{}.", this->_id, this->_name);
      nlohmann::json new_state_data = data["event"]["data"]["new_state"];
      nlohmann::json new_state_attributes = new_state_data["attributes"];
      bool changed_attribute = false;

      try {
        if (new_state_data.contains("state") && !new_state_data["state"].is_null()) {
          std::string new_state = new_state_data["state"].get<std::string>();
          if (new_state.compare(this->_current_mode.value) != 0) {
            auto new_mode = std::find_if(this->_supported_modes.begin(), this->_supported_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_state) == 0;
            });
            if (new_mode != this->_supported_modes.end()) {
              this->_current_mode = *new_mode;
              this->_requested_mode = *new_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new state: {}", this->_id, this->_name, new_mode->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new state '{}' from HA but that HVAC mode is not supported.", this->_id, this->_name, new_state_data.at("state").get<std::string>());
            }
          }
        }

        if (new_state_attributes.contains("fan_mode") && !new_state_attributes["fan_mode"].is_null()) {
          std::string new_fan_mode = new_state_attributes.at("fan_mode").get<std::string>();
          if (new_fan_mode.compare(this->_current_fan_mode.value) != 0) {
            auto new_mode = std::find_if(this->_supported_fan_modes.begin(), this->_supported_fan_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_fan_mode) == 0;
            });
            if (new_mode != this->_supported_fan_modes.end()) {
              this->_current_fan_mode = *new_mode;
              this->_requested_fan_mode = *new_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new fan mode: {}", this->_id, this->_name, new_mode->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new fan mode '{}' from HA but that fan mode is not supported.", this->_id, this->_name, new_fan_mode);
            }
          }
        }

        if (new_state_attributes.contains("preset_mode") && !new_state_attributes["preset_mode"].is_null()) {
          std::string new_preset_mode = new_state_attributes.at("preset_mode").get<std::string>();
          if (new_preset_mode.compare(this->_current_preset.value) != 0) {
            auto request_preset = std::find_if(this->_supported_presets.begin(), this->_supported_presets.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_preset_mode) == 0;
            });
            if (request_preset != this->_supported_presets.end()) {
              this->_current_preset = *request_preset;
              this->_requested_preset = *request_preset;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new preset mode: {}", this->_id, this->_name, request_preset->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new preset mode '{}' from HA but that preset mode is not supported.", this->_id, this->_name, new_preset_mode);
            }
          }
        }

        if (new_state_attributes.contains("swing_mode") && !new_state_attributes["swing_mode"].is_null()) {
          std::string new_swing_mode = new_state_attributes.at("swing_mode").get<std::string>();
          if (new_swing_mode.compare(this->_current_swing_mode.value) != 0) {
            auto request_swing_mode = std::find_if(this->_supported_swing_modes.begin(), this->_supported_swing_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_swing_mode) == 0;
            });
            if (request_swing_mode != this->_supported_swing_modes.end()) {
              this->_current_swing_mode = *request_swing_mode;
              this->_requested_swing_mode = *request_swing_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new swing mode: {}", this->_id, this->_name, request_swing_mode->value);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new swing mode '{}' from HA but that swing mode is not supported.", this->_id, this->_name, new_swing_mode);
            }
          }
        }

        if (new_state_attributes.contains("swing_horizontal_mode") && !new_state_attributes["swing_horizontal_mode"].is_null()) {
          std::string new_swingh_mode = new_state_attributes.at("swing_horizontal_mode").get<std::string>();
          if (new_swingh_mode.compare(this->_current_swingh_mode.value) != 0) {
            auto request_swingh_mode = std::find_if(this->_supported_swingh_modes.begin(), this->_supported_swingh_modes.end(), [&](const ThermostatOptionHolder &mode) {
              return mode.value.compare(new_swingh_mode) == 0;
            });
            if (request_swingh_mode != this->_supported_swingh_modes.end()) {
              this->_current_swingh_mode = *request_swingh_mode;
              this->_requested_swingh_mode = *request_swingh_mode;
              changed_attribute = true;
              SPDLOG_DEBUG("Thermostat {}::{} got new swing mode: {}", this->_id, this->_name, new_swingh_mode);
            } else {
              SPDLOG_WARN("Thermostat {}::{} got new swing mode '{}' from HA but that swing mode is not supported.", this->_id, this->_name, new_swingh_mode);
            }
          }
        }

        if (new_state_attributes.contains("temperature") && !new_state_attributes["temperature"].is_null()) {
          float temperature = new_state_attributes.at("temperature").get<float>();
          if (temperature != this->_current_temperature) {
            this->_current_temperature = temperature;
            changed_attribute = true;
            SPDLOG_DEBUG("Thermostat {}::{} got new temperature: {}", this->_id, this->_name, temperature);
          }
        }

      } catch (std::exception &e) {
        SPDLOG_ERROR("Caught exception when trying to update state for light {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
      }

      if (changed_attribute) {
        this->reset_requests();
        this->send_state_update_to_nspanel();
        this->_signal_entity_changed();
      }
    }
  }
}

// TESTING
#if defined(TEST_MODE) && TEST_MODE == 1
#include <gtest/gtest.h>

class HomeAssistantLightTest : public ::testing::Test {
protected:
  HomeAssistantLightTest() {
    // Initialize any necessary resources or setup for the tests
  }

  void SetUp() override {

    // Initialize any necessary resources or setup for the tests
    database_manager::Entity light_entity;
    light_entity.entity_type = "light";
    light_entity.friendly_name = "Unit test HA light (light)";
    light_entity.room_id = 1;
    light_entity.room_view_position = 2;
    light_entity.set_entity_data_json({{"can_color_temperature", true},
                                       {"can_dim", true},
                                       {"can_rgb", true},
                                       {"controlled_by_nspanel_main_page", true},
                                       {"controller", "home_assistant"},
                                       {"home_assistant_name", "light.office_ceiling_light"},
                                       {"is_ceiling_light", true},
                                       {"openhab_control_mode", "dimmer"},
                                       {"openhab_item_color_temp", ""},
                                       {"openhab_item_dimmer", ""},
                                       {"openhab_item_rgb", ""},
                                       {"openhab_item_switch", ""},
                                       {"openhab_name", ""}});
    ha_light_id = database_manager::database.insert(light_entity);
    // light_entity = database_manager::database.get<database_manager::Entity>(ha_light_id);

    SPDLOG_INFO("HA Light created in DB. Creating instance of light object.");
    light = new HomeAssistantLight(ha_light_id);
  }

  void TearDown() override {
    // Clean up any resources or teardown after the tests

    database_manager::database.remove<database_manager::Entity>(ha_light_id);
    // database_manager::database.remove<database_manager::Entity>(ha_light_switch_id);
  }

  HomeAssistantLight *light = nullptr;

  int32_t ha_light_id;
  int32_t ha_light_switch_id;
};

TEST_F(HomeAssistantLightTest, is_off_by_default) {
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
}

TEST_F(HomeAssistantLightTest, is_not_on_after_turn_on_in_nonoptimistic_mode) {
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "false");
  light->turn_on(false);
  light->set_brightness(50, true);
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
}

TEST_F(HomeAssistantLightTest, is_on_after_turn_on_in_optimistic_mode) {
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
  light->turn_on(false);
  light->set_brightness(50, true);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
}

TEST_F(HomeAssistantLightTest, light_reacts_to_state_changes_from_home_assistant) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");

  nlohmann::json event_data = nlohmann::json::parse(R"(
      {

          "event":{
              "context":{
                  "id":"01K2WNR8WT8VS6B5HTQ7ZEKSAP",
                  "parent_id":null,
                  "user_id":"d28bd745ad714c108c27c31a90406189"
              },
              "data":{
                  "entity_id":"light.office_ceiling_light",
                  "new_state":{
                      "attributes":{
                          "brightness":204,
                          "color_mode":"color_temp",
                          "color_temp":443,
                          "color_temp_kelvin":2257,
                          "effect":null,
                          "effect_list":[
                              "blink",
                              "breathe",
                              "okay",
                              "channel_change",
                              "finish_effect",
                              "stop_effect"
                          ],
                          "friendly_name":"office_ceiling_light",
                          "hs_color":[
                              20,
                              80
                          ],
                          "max_color_temp_kelvin":4000,
                          "max_mireds":454,
                          "min_color_temp_kelvin":2202,
                          "min_mireds":250,
                          "rgb_color":[
                              255,
                              149,
                              46
                          ],
                          "supported_color_modes":[
                              "color_temp"
                          ],
                          "supported_features":44,
                          "xy_color":[
                              0.572,
                              0.389
                          ]
                      },
                      "context":{
                          "id":"01K2WNR8WT8VS6B5HTQ7ZEKSAP",
                          "parent_id":null,
                          "user_id":"d28bd745ad714c108c27c31a90406189"
                      },
                      "entity_id":"light.office_ceiling_light",
                      "last_changed":"2025-08-17T18:45:35.203504+00:00",
                      "last_reported":"2025-08-17T18:48:00.253850+00:00",
                      "last_updated":"2025-08-17T18:48:00.253850+00:00",
                      "state":"on"
                  },
                  "old_state": "removed_as_it_is_not_used"
              },
              "event_type":"state_changed",
              "origin":"LOCAL",
              "time_fired":"2025-08-17T18:48:00.253850+00:00"
          },
          "id":3,
          "type":"event"

      }  )");

  light->home_assistant_event_callback(event_data);

  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 80); // We sent brightness 204, ie. 80%
  EXPECT_EQ(light->get_color_temperature(), 2257);
  EXPECT_EQ(light->get_color_temperature(), 2257);

  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153;
  event_data["event"]["data"]["new_state"]["attributes"]["color_temp_kelvin"] = 5000;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60); // We sent brightness 153, ie. 60%
  EXPECT_EQ(light->get_color_temperature(), 5000);

  event_data["event"]["data"]["new_state"]["state"] = "off";
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 0;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 60); // Remember last brightness value

  // Change to RGB mode
  event_data["event"]["data"]["new_state"]["state"] = "on";
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153;  // 60%
  event_data["event"]["data"]["new_state"]["attributes"]["color_mode"] = "xy"; // TODO: Update with correct color mode as sent from HA.
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60);
  EXPECT_EQ(light->get_hue(), 20);
  EXPECT_EQ(light->get_saturation(), 80);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that changes to RGB/HSB values as correctly interpreted
  event_data["event"]["data"]["new_state"]["attributes"]["hs_color"][0] = 30;
  event_data["event"]["data"]["new_state"]["attributes"]["hs_color"][1] = 60;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60);
  EXPECT_EQ(light->get_hue(), 30);
  EXPECT_EQ(light->get_saturation(), 60);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that we can revert back to color temp mode
  event_data["event"]["data"]["new_state"]["state"] = "on";
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153; // 60%
  event_data["event"]["data"]["new_state"]["attributes"]["color_mode"] = "color_temp";
  event_data["event"]["data"]["new_state"]["attributes"]["color_temp_kelvin"] = 4000;
  light->home_assistant_event_callback(event_data);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 60);
  EXPECT_EQ(light->get_color_temperature(), 4000);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);
}

#define COLOR_TEMP_PERCENT_TO_KELVIN(min, max, kelvin_pct) (((max - min) / 100) * kelvin_pct + min)
TEST_F(HomeAssistantLightTest, verify_nspanel_command_compliance) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MIN);
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MAX);
  // spdlog::set_level(spdlog::level::trace);

  NSPanelMQTTManagerCommand cmd;
  cmd.set_nspanel_id(0);
  NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
  std::vector<uint32_t> light_ids = {light->get_id()};
  light_cmd->add_light_ids(light->get_id());
  light_cmd->set_brightness(50);
  light_cmd->set_has_brightness(true);
  light_cmd->set_color_temperature(50);
  light_cmd->set_has_color_temperature(true);
  light_cmd->set_has_hue(false);
  light_cmd->set_has_saturation(false);

  light->command_callback(cmd);

  // Check that light turns on, sets correct brightness and color temperature.
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 50));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  // Verify that only color temperature can be change.
  light_cmd->set_has_brightness(false);
  light_cmd->set_color_temperature(70);
  light_cmd->set_has_color_temperature(true);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  // Verify that only brightness can be change.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(true);
  light_cmd->set_brightness(70);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  // Verify that hue and saturation can be changed and the lights goes into RGB mode.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(false);
  light_cmd->set_hue(30);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(60);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);                                                                      // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 30);
  EXPECT_EQ(light->get_saturation(), 60);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that only hue can be changed.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(false);
  light_cmd->set_hue(40);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(60);
  light_cmd->set_has_saturation(false);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);                                                                      // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 40);
  EXPECT_EQ(light->get_saturation(), 60); // Verify that value is unchanged.
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that only saturation can be changed.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_has_brightness(false);
  light_cmd->set_hue(40);
  light_cmd->set_has_hue(false);
  light_cmd->set_saturation(70);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 70);                                                                      // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 40);                                                                             // Verify that value is unchanged.
  EXPECT_EQ(light->get_saturation(), 70);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  // Verify that brightness can be changed and RGB is kept.
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_brightness(100);
  light_cmd->set_has_brightness(true);
  light_cmd->set_hue(40);
  light_cmd->set_has_hue(false);
  light_cmd->set_saturation(70);
  light_cmd->set_has_saturation(false);
  light->command_callback(cmd);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 100);                                                                     // Verify that value is unchanged.
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 70)); // Verify that value is unchanged.
  EXPECT_EQ(light->get_hue(), 40);                                                                             // Verify that value is unchanged.
  EXPECT_EQ(light->get_saturation(), 70);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB);

  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
}

TEST_F(HomeAssistantLightTest, verify_optimistic_mode_compliance) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "true");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MIN);
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MAX);
  // spdlog::set_level(spdlog::level::trace);

  NSPanelMQTTManagerCommand cmd;
  cmd.set_nspanel_id(0);
  NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
  std::vector<uint32_t> light_ids = {light->get_id()};
  light_cmd->add_light_ids(light->get_id());
  light_cmd->set_brightness(50);
  light_cmd->set_has_brightness(true);
  light_cmd->set_color_temperature(50);
  light_cmd->set_has_color_temperature(true);
  light_cmd->set_has_hue(false);
  light_cmd->set_has_saturation(false);

  light->command_callback(cmd);

  // Check that light turns on, sets correct brightness and color temperature.
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 50));
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT);

  light_cmd->set_has_brightness(false);
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_hue(30);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(50);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);

  EXPECT_EQ(light->get_state(), true);                                                                         // Verify light is still on
  EXPECT_EQ(light->get_brightness(), 50);                                                                      // Verify light is still set to 50% brightness
  EXPECT_EQ(light->get_color_temperature(), COLOR_TEMP_PERCENT_TO_KELVIN(color_temp_min, color_temp_max, 50)); // Verify color temp has not changed.
  EXPECT_EQ(light->get_hue(), 30);
  EXPECT_EQ(light->get_saturation(), 50);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::RGB); // Light should have switched to RGB mode.
}

TEST_F(HomeAssistantLightTest, verify_non_optimistic_mode_compliance) {
  // Turn off optimistic mode and verify that no values change.
  MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, "false");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MIN);
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::COLOR_TEMP_MAX);
  // spdlog::set_level(spdlog::level::trace);

  NSPanelMQTTManagerCommand cmd;
  cmd.set_nspanel_id(0);
  NSPanelMQTTManagerCommand_LightCommand *light_cmd = cmd.mutable_light_command();
  std::vector<uint32_t> light_ids = {light->get_id()};
  light_cmd->add_light_ids(light->get_id());
  light_cmd->set_brightness(50);
  light_cmd->set_has_brightness(true);
  light_cmd->set_color_temperature(50);
  light_cmd->set_has_color_temperature(true);
  light_cmd->set_has_hue(false);
  light_cmd->set_has_saturation(false);
  light->command_callback(cmd);

  // Verify light is still off, brightness is still 0 and hue and saturation did not change.
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
  EXPECT_EQ(light->get_color_temperature(), 0);
  EXPECT_EQ(light->get_hue(), 0);
  EXPECT_EQ(light->get_saturation(), 0);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT); // Light should have switched to RGB mode.

  light_cmd->set_has_brightness(false);
  light_cmd->set_has_color_temperature(false);
  light_cmd->set_hue(30);
  light_cmd->set_has_hue(true);
  light_cmd->set_saturation(30);
  light_cmd->set_has_saturation(true);
  light->command_callback(cmd);

  // Verify light is still off, brightness is still 0 and hue and saturation did not change.
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
  EXPECT_EQ(light->get_color_temperature(), 0);
  EXPECT_EQ(light->get_hue(), 0);
  EXPECT_EQ(light->get_saturation(), 0);
  EXPECT_EQ(light->get_mode(), MQTT_MANAGER_LIGHT_MODE::DEFAULT); // Light should have switched to RGB mode.
}

#endif // !MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TEST
