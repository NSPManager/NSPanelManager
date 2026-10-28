// What HomeAssistantLight asks Home Assistant to do when it is turned on or changed.

#include "test_helpers.hpp"
#include <gtest/gtest.h>
#include <light/home_assistant_light.hpp>

using nspm_test::django_light_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::home_assistant_state_changed;
using nspm_test::ScopedEntity;

class HomeAssistantLightTurnOnTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    SendCaptureTest::SetUp();
    set_optimistic_mode(false);
  }

  // The one call_service message the action under test sent.
  static nlohmann::json only_call() {
    auto calls = home_assistant_service_calls();
    EXPECT_EQ(calls.size(), 1);
    return calls.empty() ? nlohmann::json::object() : calls[0];
  }

  static bool sends_kelvin(const nlohmann::json &call) {
    return call.contains("service_data") && call["service_data"].contains("color_temp_kelvin");
  }
};

// #343: Home Assistant reports no colour temperature for a light that is off, so the manager doesn't know it yet.
// Turning the light on used to send color_temp_kelvin = COLOR_TEMP_MIN, i.e. the warmest white, every time.
TEST_F(HomeAssistantLightTurnOnTest, turning_on_a_light_with_unknown_kelvin_leaves_the_kelvin_alone) {
  ScopedEntity row("light", "Ceiling", django_light_data());
  HomeAssistantLight light(row.id);
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "off", {{"supported_color_modes", {"color_temp"}}, {"color_mode", nullptr}, {"color_temp_kelvin", nullptr}}));
  home_assistant_service_calls();

  light.turn_on(true);

  auto call = only_call();
  SCOPED_TRACE(call.dump());
  EXPECT_EQ(call["service"], "turn_on");
  EXPECT_FALSE(sends_kelvin(call));
}

// #343: a light configured without colour temperature support must never be sent one.
TEST_F(HomeAssistantLightTurnOnTest, on_off_light_is_not_sent_a_colour_temperature) {
  auto data = django_light_data();
  data["can_dim"] = false;
  data["can_color_temperature"] = false;
  ScopedEntity row("light", "Ceiling", data);
  HomeAssistantLight light(row.id);
  home_assistant_service_calls();

  light.turn_on(true);

  auto call = only_call();
  SCOPED_TRACE(call.dump());
  EXPECT_EQ(call["service"], "turn_on");
  EXPECT_FALSE(sends_kelvin(call));
}

// With the default "color temperature" turn on behaviour, a light that was off is turned on at its last known kelvin.
TEST_F(HomeAssistantLightTurnOnTest, turning_on_a_light_that_was_off_restores_its_last_known_kelvin) {
  ScopedEntity row("light", "Ceiling", django_light_data());
  HomeAssistantLight light(row.id);
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "on", {{"brightness", 255}, {"color_mode", "color_temp"}, {"color_temp_kelvin", 3500}}));
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "off", {{"color_mode", nullptr}, {"color_temp_kelvin", nullptr}}));
  home_assistant_service_calls();

  light.turn_on(true);

  auto call = only_call();
  SCOPED_TRACE(call.dump());
  EXPECT_EQ(call["service_data"]["color_temp_kelvin"], 3500);
}

// Dimming a light that is already on only changes the brightness.
TEST_F(HomeAssistantLightTurnOnTest, changing_brightness_of_a_light_that_is_on_does_not_send_a_kelvin) {
  ScopedEntity row("light", "Ceiling", django_light_data());
  HomeAssistantLight light(row.id);
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "on", {{"brightness", 255}, {"color_mode", "color_temp"}, {"color_temp_kelvin", 3500}}));
  home_assistant_service_calls();

  light.set_brightness(40, true);

  auto call = only_call();
  SCOPED_TRACE(call.dump());
  EXPECT_EQ(call["service_data"]["brightness_pct"], 40);
  EXPECT_FALSE(sends_kelvin(call));
}

// Picking a colour for a light that is off turns it on in that colour, rather than forcing colour temperature mode.
// Sending both would also be rejected by Home Assistant.
TEST_F(HomeAssistantLightTurnOnTest, turning_on_with_a_new_colour_sends_the_colour_and_not_a_kelvin) {
  auto data = django_light_data();
  data["can_rgb"] = true;
  ScopedEntity row("light", "Ceiling", data);
  HomeAssistantLight light(row.id);
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "on", {{"brightness", 255}, {"color_mode", "color_temp"}, {"color_temp_kelvin", 3500}}));
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "off", {{"color_mode", nullptr}}));
  home_assistant_service_calls();

  light.set_hue(120, false);
  light.set_saturation(80, false);
  light.turn_on(true);

  auto call = only_call();
  SCOPED_TRACE(call.dump());
  EXPECT_EQ(call["service_data"]["hs_color"], nlohmann::json({120, 80}));
  EXPECT_FALSE(sends_kelvin(call));
}

// The hue/saturation check used to be `RGB && hue changed || saturation changed`, so a saturation difference sent hs_color
// to a light in colour temperature mode.
TEST_F(HomeAssistantLightTurnOnTest, light_in_colour_temperature_mode_is_not_sent_a_colour) {
  auto data = django_light_data();
  data["can_rgb"] = true;
  ScopedEntity row("light", "Ceiling", data);
  HomeAssistantLight light(row.id);
  light.home_assistant_event_callback(home_assistant_state_changed("light.kitchen_ceiling", "on", {{"brightness", 255}, {"color_mode", "color_temp"}, {"color_temp_kelvin", 3500}}));
  light.set_saturation(80, false);
  light.set_color_temperature(3500, false); // Back to colour temperature mode, same kelvin.
  home_assistant_service_calls();

  light.set_brightness(40, true);

  auto call = only_call();
  SCOPED_TRACE(call.dump());
  EXPECT_FALSE(call["service_data"].contains("hs_color"));
}
