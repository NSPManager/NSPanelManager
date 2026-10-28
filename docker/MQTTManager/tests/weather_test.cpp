// What MQTTManagerWeather sends to the panels after processing an Open-Meteo forecast.

#include "test_helpers.hpp"
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <optional>
#include <protobuf_nspanel.pb.h>
#include <sstream>
#include <weather/weather.hpp>

class WeatherTest : public nspm_test::SendCaptureTest {
protected:
  void SetUp() override {
    SendCaptureTest::SetUp();
    // Processing a forecast sets TZ for the whole process. Put it back so other tests are unaffected.
    const char *tz = std::getenv("TZ");
    _saved_tz = tz ? std::optional<std::string>(tz) : std::nullopt;
  }

  void TearDown() override {
    if (_saved_tz) {
      setenv("TZ", _saved_tz->c_str(), 1);
    } else {
      unsetenv("TZ");
    }
    tzset();
  }

  // A recorded Open-Meteo response with distinct rain volumes (mm) and probabilities (%).
  static nlohmann::json forecast_fixture() {
    std::ifstream file(NSPM_TEST_FIXTURES_DIR "/open_meteo_forecast.json");
    std::stringstream contents;
    contents << file.rdbuf();
    return nlohmann::json::parse(contents.str());
  }

  // The weather update sent to the panels while processing the given forecast.
  static NSPanelWeatherUpdate process(const nlohmann::json &forecast) {
    MQTTManagerWeather::test_process_weather_data(forecast.dump());
    NSPanelWeatherUpdate update;
    for (auto &message : MQTT_Manager::test_take_published_messages()) {
      if (message.topic.ends_with("/status/weather")) {
        EXPECT_TRUE(update.ParseFromString(message.payload));
      }
    }
    return update;
  }

private:
  std::optional<std::string> _saved_tz;
};

// #203: the current rain chance was never read from the forecast, so the screensaver always showed 0%.
TEST_F(WeatherTest, current_precipitation_is_the_current_probability) {
  auto update = process(forecast_fixture());

  EXPECT_EQ(update.current_precipitation_string(), "35%");
}

// #203: the forecast days showed the rain volume in mm with a % sign (6.8 mm -> "7%") instead of the probability.
TEST_F(WeatherTest, forecast_precipitation_is_the_daily_probability) {
  auto update = process(forecast_fixture());

  ASSERT_EQ(update.forecast_items_size(), 5);
  std::vector<std::string> shown;
  for (const auto &item : update.forecast_items()) {
    shown.push_back(item.precipitation_string());
  }
  EXPECT_EQ(shown, (std::vector<std::string>{"40%", "85%", "5%", "95%", "20%"}));
}

// Not every weather model has a probability. A missing value shows as 0% rather than dropping the whole update.
TEST_F(WeatherTest, null_current_probability_still_sends_the_update) {
  auto forecast = forecast_fixture();
  forecast["current"]["precipitation_probability"] = nullptr;

  auto update = process(forecast);

  EXPECT_EQ(update.current_precipitation_string(), "0%");
  EXPECT_EQ(update.forecast_items_size(), 5);
}
