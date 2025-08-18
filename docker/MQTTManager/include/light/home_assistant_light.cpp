#include "home_assistant_light.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <cstdint>
#include <gtest/gtest.h>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

HomeAssistantLight::HomeAssistantLight(uint32_t light_id) : Light(light_id) {
  // Process Home Assistant specific details. General light data is loaded in the "Light" constructor.

  this->_current_brightness = 0;
  this->_current_color_temperature = 0;
  this->_current_hue = 0;
  this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
  this->_current_state = false;
  this->_requested_brightness = 0;
  this->_requested_color_temperature = 0;
  this->_requested_hue = 0;
  this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
  this->_requested_state = false;

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantLight has not been rekognized as controlled by HOME_ASSISTANT. Will stop processing light.");
    return;
  }

  nlohmann::json entity_data;
  try {
    auto light = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = light.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load light {}: {}", this->_id, e.what());
    return;
  }

  if (entity_data.contains("home_assistant_name")) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {

    SPDLOG_ERROR("No home assistant name defined for Light {}::{}", this->_id, this->_name);
  }
  SPDLOG_DEBUG("Loaded light {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantLight::home_assistant_event_callback, this, _1));

  if (this->_home_assistant_name.rfind("light.", 0) == 0) {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT;
  } else if (this->_home_assistant_name.rfind("switch.", 0) == 0) {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_SWITCH;
  } else {
    this->_home_assistant_light_type = MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT;
    SPDLOG_ERROR("Unknown type of home assistant entity '{}'. Will assume light is a light and not a switch.", this->_home_assistant_name);
  }

  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

HomeAssistantLight::~HomeAssistantLight() {
  HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantLight::home_assistant_event_callback, this, _1));
}

void HomeAssistantLight::send_state_update_to_controller() {
  // SPDLOG_TRACE("Light {}::{} current mode  : {}", this->_id, this->_name, (int)this->_current_mode);
  // SPDLOG_TRACE("Light {}::{} requested mode: {}", this->_id, this->_name, (int)this->_requested_mode);
  // SPDLOG_TRACE("Light {}::{} current kelvin  : {}", this->_id, this->_name, this->_current_color_temperature);
  // SPDLOG_TRACE("Light {}::{} requested kelvin: {}", this->_id, this->_name, this->_requested_color_temperature);
  // SPDLOG_TRACE("Light {}::{} requested state: {}", this->_id, this->_name, this->_requested_state ? "on" : "off");
  // SPDLOG_TRACE("Light {}::{} current state: {}", this->_id, this->_name, this->_current_state ? "on" : "off");

  nlohmann::json service_data;
  service_data["type"] = "call_service";
  service_data["target"]["entity_id"] = this->_home_assistant_name;
  if (this->_home_assistant_light_type == MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_LIGHT) {
    service_data["domain"] = "light";
    if (this->_requested_state) {
      service_data["service"] = "turn_on";
      if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
        this->_current_state = true;
      }

      if (this->_requested_brightness != this->_current_brightness) {
        service_data["service_data"]["brightness_pct"] = this->_requested_brightness;
        if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
          this->_current_brightness = this->_requested_brightness;
        }
      }

      // This is a turn on event and it currently off. Send kelvin if turn on behavior is to use color temp.
      if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT || (!this->_current_state && MqttManagerConfig::get_light_turn_on_behaviour() == LightTurnOnBehaviour::COLOR_TEMPERATURE)) {
        service_data["service_data"]["kelvin"] = this->_requested_color_temperature;
        if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
          this->_current_color_temperature = this->_requested_color_temperature;
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
        }
      }

      if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT && this->_requested_color_temperature != this->_current_color_temperature) {
        service_data["service_data"]["kelvin"] = this->_requested_color_temperature;
        if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
          this->_current_color_temperature = this->_requested_color_temperature;
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
        }
      } else if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::RGB && this->_requested_hue != this->_current_hue || this->_requested_saturation != this->_current_saturation) {
        service_data["service_data"]["hs_color"] = {this->_requested_hue, this->_requested_saturation};
        if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
          this->_current_hue = this->_requested_hue;
          this->_current_saturation = this->_requested_saturation;
          this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
        }
      }
    } else {
      service_data["service"] = "turn_off";
      if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
        this->_current_state = false;
      }
    }
  } else if (this->_home_assistant_light_type == MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TYPE::TYPE_SWITCH) {
    service_data["domain"] = "switch";
    if (this->_requested_state) {
      service_data["service"] = "turn_on";
      if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
        this->_current_state = true;
      }
    } else {
      service_data["service"] = "turn_off";
      if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
        this->_current_state = false;
      }
    }
  }
  HomeAssistantManager::send_json(service_data);

  if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
    this->send_state_update_to_nspanel();
    this->_entity_changed_callbacks(this);
  }
}

void HomeAssistantLight::home_assistant_event_callback(nlohmann::json data) {
  if (std::string(data["event"]["event_type"]).compare("state_changed") == 0) {
    if (std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) == 0) {
      SPDLOG_DEBUG("Got event update for HA light {}::{}.", this->_id, this->_name);
      nlohmann::json new_state_data = data["event"]["data"]["new_state"];
      nlohmann::json new_state_attributes = new_state_data["attributes"];
      bool changed_attribute = false;

      if (new_state_data.contains("state") && !new_state_data["state"].is_null()) {
        try {
          std::string new_state = new_state_data["state"];
          if (new_state.compare("on") == 0) {
            if (!this->_current_state) {
              changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
            }
            this->_current_state = true;
            this->_requested_state = true;
            if (this->_can_dim) {
              if (new_state_attributes.contains("brightness")) {
                uint8_t new_brightness = 0;
                if (!new_state_attributes["brightness"].is_null()) {
                  new_brightness = new_state_attributes["brightness"];
                }
                new_brightness = ((float)new_brightness / 255.0) * 100; // Home assistant sends brightness as 0 to 255. We want percentage (0-100)
                if (new_brightness != this->_current_brightness) {
                  changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
                }
                this->_current_brightness = new_brightness;
                this->_requested_brightness = new_brightness;
              } else {
                // Light can dim but no brightness was given in update. Fallback to 100%.
                SPDLOG_ERROR("Got new event data for light {}::{}. Light is configured for brightness (dimmable) but no 'brightness' value was available in entity payload.", this->_id, this->_name);
                if (this->_current_brightness != 100) {
                  changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
                }
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
                this->_current_brightness = 100;
                this->_requested_brightness = 100;
              }
            } else {
              // We will never get a state_changed event from HA that the requested brightness has been set as this is a switch.
              if (this->_current_state) {
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
              }
              this->_current_brightness = 100;
              this->_requested_brightness = 100;
            }
          } else if (new_state.compare("off") == 0) {
            if (this->_current_state) {
              changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
            }
            this->_current_state = false;
            this->_requested_state = false;
            this->_requested_brightness = 0;
          } else {
            SPDLOG_ERROR("Unknown entity state: {}", new_state);
          }
        } catch (std::exception &e) {
          SPDLOG_ERROR("Caught exception when trying to update state for light {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
        }
      }

      if (new_state_attributes.contains("color_mode") && !new_state_attributes["color_mode"].is_null()) {
        try {
          std::string new_color_mode = new_state_attributes["color_mode"];
          if (this->_can_color_temperature && new_color_mode.compare("color_temp") == 0) {
            this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
            this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
            if (new_state_attributes.contains("color_temp_kelvin") && !new_state_attributes["color_temp_kelvin"].is_null()) {
              if (new_state_attributes["color_temp_kelvin"] != this->_current_color_temperature) {
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
              }
              this->_current_color_temperature = new_state_attributes["color_temp_kelvin"];
              this->_requested_color_temperature = this->_current_color_temperature;
            }
          } else if (new_color_mode.compare("onoff") == 0) {
            // Light does not support colors. Do nothing.
          } else if (this->_can_rgb) {
            this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
            this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
            if (new_state_attributes.contains("hs_color") && !new_state_attributes["hs_color"].is_null()) {
              std::vector<float> hs_color = new_state_attributes["hs_color"];

              if ((uint16_t)hs_color[0] != this->_current_hue || (uint8_t)hs_color[1]) {
                changed_attribute = true; // Something changed from what was assumed to be the current state. Flag to trigger "entity changed" callbacks.
              }

              this->_current_hue = (uint16_t)hs_color[0];
              this->_requested_hue = this->_current_hue;
              this->_current_saturation = (uint8_t)hs_color[1];
              this->_requested_saturation = this->_current_saturation;
            }
          }
        } catch (std::exception &e) {
          SPDLOG_ERROR("Caught exception when trying to update color mode for light {}::{} message: {}. Working data: {}", this->_id, this->_name, boost::diagnostic_information(e, true), new_state_attributes.dump());
        }
      }

      if (changed_attribute) {
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
  MqttManagerConfig::set_setting_value("optimistic_mode", "false");
  light->turn_on(false);
  light->set_brightness(50, true);
  EXPECT_EQ(light->get_state(), false);
  EXPECT_EQ(light->get_brightness(), 0);
}

TEST_F(HomeAssistantLightTest, is_on_after_turn_on_in_optimistic_mode) {
  MqttManagerConfig::set_setting_value("optimistic_mode", "true");
  light->turn_on(false);
  light->set_brightness(50, true);
  EXPECT_EQ(light->get_state(), true);
  EXPECT_EQ(light->get_brightness(), 50);
}

TEST_F(HomeAssistantLightTest, light_reacts_to_state_changes_from_home_assistant) {
  // Enable optimistic mode for this test
  MqttManagerConfig::set_setting_value("optimistic_mode", "true");

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
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153;     // 60%
  event_data["event"]["data"]["new_state"]["attributes"]["color_mode"] = "color"; // TODO: Update with correct color mode as sent from HA.
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
  event_data["event"]["data"]["new_state"]["attributes"]["brightness"] = 153;          // 60%
  event_data["event"]["data"]["new_state"]["attributes"]["color_mode"] = "color_temp"; // TODO: Update with correct color mode as sent from HA.
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
  MqttManagerConfig::set_setting_value("optimistic_mode", "true");
  uint32_t color_temp_min = MqttManagerConfig::get_setting_with_default<uint32_t>("color_temp_min");
  uint32_t color_temp_max = MqttManagerConfig::get_setting_with_default<uint32_t>("color_temp_max");
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
}

#endif // !MQTT_MANAGER_HOME_ASSISTANT_LIGHT_TEST
