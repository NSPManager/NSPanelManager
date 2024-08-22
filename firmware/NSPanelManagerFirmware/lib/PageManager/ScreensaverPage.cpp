#include "ArduinoJson/Array/JsonArray.hpp"
#include "freertos/portmacro.h"
#include <ArduinoJson.h>
#include <InterfaceConfig.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <RoomManager.hpp>
#include <ScreensaverPage.hpp>
#include <TftDefines.h>

void ScreensaverPage::attachMqttCallback() {
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_screensaver_mode.c_str(), &ScreensaverPage::screensaverModeCallback);
  MqttManager::subscribeToTopic("nspanel/status/time", &ScreensaverPage::clockMqttCallback);
  MqttManager::subscribeToTopic("nspanel/status/date", &ScreensaverPage::dateMqttCallback);
  MqttManager::subscribeToTopic("nspanel/status/weather", &ScreensaverPage::weatherMqttCallback);

  if (InterfaceConfig::clock_us_style) {
    MqttManager::subscribeToTopic("nspanel/status/ampm", &ScreensaverPage::ampmMqttCallback);
  }
}

void ScreensaverPage::init() {
  this->_stopped = false;

  bool show_background = false;
  if (InterfaceConfig::screensaver_mode.compare("with_background") == 0) {
    this->_screensaver_page_name = SCREENSAVER_PAGE_NAME;
    this->_show_weather = true;
    show_background = true;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
  } else if (InterfaceConfig::screensaver_mode.compare("without_background") == 0) {
    this->_screensaver_page_name = SCREENSAVER_PAGE_NAME;
    this->_show_weather = true;
    show_background = false;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
  } else if (InterfaceConfig::screensaver_mode.compare("datetime_with_background") == 0) {
    this->_screensaver_page_name = SCREENSAVER_MINIMAL_PAGE_NAME;
    this->_show_weather = false;
    show_background = true;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
  } else if (InterfaceConfig::screensaver_mode.compare("datetime_without_background") == 0) {
    this->_screensaver_page_name = SCREENSAVER_MINIMAL_PAGE_NAME;
    this->_show_weather = false;
    show_background = false;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
  } else if (InterfaceConfig::screensaver_mode.compare("no_screensaver") == 0) {
    this->_screensaver_page_name = SCREENSAVER_PAGE_NAME;
    this->_show_weather = false;
    show_background = false;
    this->_screensaver_brightness = 0;
  } else {
    LOG_ERROR("Unknown screensaver mode '", InterfaceConfig::screensaver_mode.c_str(), "'!");
  }

  NSPanel::instance->setComponentVal(SCREENSAVER_PAGE_NAME "." SCREENSAVER_BACKGROUND_CHOICE_VARIABLE_NAME, show_background ? 1 : 0);
  NSPanel::instance->setComponentVal(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_BACKGROUND_CHOICE_VARIABLE_NAME, show_background ? 1 : 0);
}

void ScreensaverPage::stop() {
  LOG_DEBUG("Stopping ScreensaverPage.");
  vTaskDelay(50 / portTICK_PERIOD_MS);
  this->_stopped = true;
  LOG_INFO("ScreensaverPage stopped.");
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void ScreensaverPage::show() {
  NSPanel::instance->setDimLevel(this->_screensaver_brightness);
  NSPanel::instance->goToPage(this->_screensaver_page_name.c_str());

  if (PageManager::GetCurrentPage() != PageManager::GetScreensaverPage()) {
    PageManager::SetCurrentPage(this);
    MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "0");
    PageManager::GetHomePage()->setCurrentMode(roomMode::room);
    RoomManager::goToRoomId(InterfaceConfig::homeScreen);
  }

  if (InterfaceConfig::clock_us_style) {
    NSPanel::instance->setComponentVisible(SCREENSAVER_CURRENT_AMPM_TEXT_NAME, true);
  } else {
    NSPanel::instance->setComponentVisible(SCREENSAVER_CURRENT_AMPM_TEXT_NAME, false);
  }
}

void ScreensaverPage::update() {
  // Update is done though MQTT time callback
}

void ScreensaverPage::updateRoomTemp(std::string roomtemp_string) {
  if (ScreensaverPage::_stopped) {
    return;
  }
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_ROOMTEMP_TEXT_NAME, roomtemp_string.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_ROOMTEMP_TEXT_NAME, roomtemp_string.c_str());
}

void ScreensaverPage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  PageManager::GetHomePage()->show();
}

void ScreensaverPage::unshow() {
  NSPanel::instance->setDimLevel(InterfaceConfig::screen_dim_level);
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1");
}

void ScreensaverPage::screensaverModeCallback(char *topic, byte *payload, unsigned int length) {
  if (ScreensaverPage::_stopped) {
    return;
  }

  std::string screensaver_mode = std::string((char *)payload, length);
  InterfaceConfig::screensaver_mode = screensaver_mode;
  LOG_INFO("Received command to change screensaver mode to: ", screensaver_mode.c_str());
  PageManager::GetScreensaverPage()->init(); // Reload all internal variables

  if (PageManager::GetCurrentPage() == PageManager::GetScreensaverPage()) {
    PageManager::GetScreensaverPage()->show();
  }
}

void ScreensaverPage::clockMqttCallback(char *topic, byte *payload, unsigned int length) {
  std::string clock_string = std::string((char *)payload, length);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_TIME_TEXT_NAME, clock_string.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_TIME_TEXT_NAME, clock_string.c_str());
}

void ScreensaverPage::ampmMqttCallback(char *topic, byte *payload, unsigned int length) {
  std::string ampm_string = std::string((char *)payload, length);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_AMPM_TEXT_NAME, ampm_string.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_AMPM_TEXT_NAME, ampm_string.c_str());
}

void ScreensaverPage::dateMqttCallback(char *topic, byte *payload, unsigned int length) {
  std::string date_string = std::string((char *)payload, length);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_DAY_TEXT_NAME, date_string.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_DAY_TEXT_NAME, date_string.c_str());
}

void ScreensaverPage::weatherMqttCallback(char *topic, byte *payload, unsigned int length) {
  LOG_TRACE("Received new weather data.");
  std::string payload_str = std::string((char *)payload, length);
  JsonDocument json;
  DeserializationError error = deserializeJson(json, payload_str);
  if (error) {
    LOG_ERROR("Failed to serialize weather data. Got code: ", error.code());
    return;
  }
  vTaskDelay(500 / portTICK_PERIOD_MS);
  JsonArray forecast = json["forecast"].as<JsonArray>();
  LOG_TRACE("Received forecast for ", forecast.size(), " days.");

  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_WEATHER_ICON_TEXT_NAME, json["icon"]);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_TEMP_TEXT_NAME, json["temp"]);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_WIND_TEXT_NAME, json["wind"]);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_SUNRISE_TEXT_NAME, json["sunrise"]);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_SUNSET_TEXT_NAME, json["sunset"]);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_MAXMIN_TEXT_NAME, json["maxmin"]);
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_RAIN_TEXT_NAME, json["prepro"]);

  if (forecast.size() >= 1) {
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY1_TEXT_NAME, forecast[0]["day"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON1_TEXT_NAME, forecast[0]["icon"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN1_TEXT_NAME, forecast[0]["maxmin"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN1_TEXT_NAME, forecast[0]["prepro"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND1_TEXT_NAME, forecast[0]["wind"]);
  }

  if (forecast.size() >= 2) {
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY2_TEXT_NAME, forecast[1]["day"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON2_TEXT_NAME, forecast[1]["icon"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN2_TEXT_NAME, forecast[1]["maxmin"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN2_TEXT_NAME, forecast[1]["prepro"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND2_TEXT_NAME, forecast[1]["wind"]);
  }

  if (forecast.size() >= 3) {
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY3_TEXT_NAME, forecast[2]["day"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON3_TEXT_NAME, forecast[2]["icon"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN3_TEXT_NAME, forecast[2]["maxmin"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN3_TEXT_NAME, forecast[2]["prepro"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND3_TEXT_NAME, forecast[2]["wind"]);
  }

  if (forecast.size() >= 4) {
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY4_TEXT_NAME, forecast[3]["day"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON4_TEXT_NAME, forecast[3]["icon"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN4_TEXT_NAME, forecast[3]["maxmin"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN4_TEXT_NAME, forecast[3]["prepro"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND4_TEXT_NAME, forecast[3]["wind"]);
  }

  if (forecast.size() >= 5) {
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY5_TEXT_NAME, forecast[4]["day"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON5_TEXT_NAME, forecast[4]["icon"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN5_TEXT_NAME, forecast[4]["maxmin"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN5_TEXT_NAME, forecast[4]["prepro"]);
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND5_TEXT_NAME, forecast[4]["wind"]);
  }
}
