#include "ArduinoJson/Array/JsonArray.hpp"
#include "freertos/portmacro.h"
#include <ArduinoJson.h>
#include <InterfaceConfig.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <ReadBufferFixedSize.h>
#include <RoomManager.hpp>
#include <ScreensaverPage.hpp>
#include <TftDefines.h>
#include <protobuf_nspanel.h>

void ScreensaverPage::attachMqttCallback() {
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_panel_screensaver_mode.c_str(), &ScreensaverPage::screensaverModeCallback);
  MqttManager::subscribeToTopic("nspanel/status/time", &ScreensaverPage::clockMqttCallback);
  MqttManager::subscribeToTopic("nspanel/status/date", &ScreensaverPage::dateMqttCallback);
  std::string weather_topic = "nspanel/status/mqttmanager_";
  weather_topic.append(NSPMConfig::instance->manager_address);
  weather_topic.append(":");
  weather_topic.append(std::to_string(NSPMConfig::instance->manager_port));
  weather_topic.append("/weather");
  MqttManager::subscribeToTopic(weather_topic.c_str(), &ScreensaverPage::weatherMqttCallback);

  if (InterfaceConfig::clock_us_style) {
    MqttManager::subscribeToTopic("nspanel/status/ampm", &ScreensaverPage::ampmMqttCallback);
  }
}

void ScreensaverPage::init() {
  this->_stopped = false;
  this->_weather_update = new PROTOBUF_NSPANEL_WEATHER_UPDATE;

  bool show_background = false;
  switch (InterfaceConfig::screensaver_mode) {
  case PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::WEATHER_WITH_BACKGROUND:
    this->_screensaver_page_name = SCREENSAVER_PAGE_NAME;
    this->_show_weather = true;
    show_background = true;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
    break;
  case PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::WEATHER_WITHOUT_BACKGROUND:
    this->_screensaver_page_name = SCREENSAVER_PAGE_NAME;
    this->_show_weather = true;
    show_background = false;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
    break;
  case PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::DATETIME_WITH_BACKGROUND:
    this->_screensaver_page_name = SCREENSAVER_MINIMAL_PAGE_NAME;
    this->_show_weather = false;
    show_background = true;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
    break;
  case PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::DATETIME_WITHOUT_BACKGROUND:
    this->_screensaver_page_name = SCREENSAVER_MINIMAL_PAGE_NAME;
    this->_show_weather = false;
    show_background = false;
    this->_screensaver_brightness = InterfaceConfig::screensaver_dim_level;
    break;
  case PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::NO_SCREENSAVER:
    this->_screensaver_page_name = SCREENSAVER_PAGE_NAME;
    this->_show_weather = false;
    show_background = false;
    this->_screensaver_brightness = 0;
    break;

  default:
    LOG_ERROR("Unknown screensaver mode '", (int32_t)InterfaceConfig::screensaver_mode, "'!");
    break;
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

void ScreensaverPage::screensaverModeCallback(MQTTMessage *message) {
  if (ScreensaverPage::_stopped) {
    return;
  }

  LOG_INFO("Received command to change screensaver mode to: ", message->data.c_str());
  if (message->data.compare("with_background")) {
    InterfaceConfig::screensaver_mode = PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::WEATHER_WITH_BACKGROUND;
  } else if (message->data.compare("without_background")) {
    InterfaceConfig::screensaver_mode = PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::WEATHER_WITHOUT_BACKGROUND;
  } else if (message->data.compare("datetime_with_background")) {
    InterfaceConfig::screensaver_mode = PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::DATETIME_WITH_BACKGROUND;
  } else if (message->data.compare("datetime_without_background")) {
    InterfaceConfig::screensaver_mode = PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::DATETIME_WITHOUT_BACKGROUND;
  } else if (message->data.compare("no_screensaver")) {
    InterfaceConfig::screensaver_mode = PROTOBUF_NSPANEL_CONFIG::NSPanelScreensaverMode::NO_SCREENSAVER;
  } else {
    LOG_ERROR("Received unknown screensaver mode: ", message->data.c_str());
  }

  PageManager::GetScreensaverPage()->init(); // Reload all internal variables

  if (PageManager::GetCurrentPage() == PageManager::GetScreensaverPage()) {
    PageManager::GetScreensaverPage()->show();
  }
}

void ScreensaverPage::clockMqttCallback(MQTTMessage *message) {
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_TIME_TEXT_NAME, message->data.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_TIME_TEXT_NAME, message->data.c_str());
}

void ScreensaverPage::ampmMqttCallback(MQTTMessage *message) {
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_AMPM_TEXT_NAME, message->data.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_AMPM_TEXT_NAME, message->data.c_str());
}

void ScreensaverPage::dateMqttCallback(MQTTMessage *message) {
  NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_DAY_TEXT_NAME, message->data.c_str());
  NSPanel::instance->setComponentText(SCREENSAVER_MINIMAL_PAGE_NAME "." SCREENSAVER_CURRENT_DAY_TEXT_NAME, message->data.c_str());
}

void ScreensaverPage::weatherMqttCallback(MQTTMessage *message) {
  LOG_DEBUG("Received new weather data.");

  try {
    if (message->get_protobuf_obj<PROTOBUF_NSPANEL_WEATHER_UPDATE>(ScreensaverPage::_weather_update)) {
      message->clear(); // We no longer need the data in the message.
    } else {
      LOG_ERROR("Failed to deserialize weather update. Will cancel operation!");
      return;
    }

    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_WEATHER_ICON_TEXT_NAME, _weather_update->current_weather_icon());
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_TEMP_TEXT_NAME, _weather_update->current_temperature_string());
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_WIND_TEXT_NAME, _weather_update->current_wind_string());
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_SUNRISE_TEXT_NAME, _weather_update->sunrise_string());
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_SUNSET_TEXT_NAME, _weather_update->sunset_string());
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_MAXMIN_TEXT_NAME, _weather_update->current_maxmin_temperature());
    NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_CURRENT_RAIN_TEXT_NAME, _weather_update->current_precipitation_string());

    if (_weather_update->forecast_items().get_size() >= 1) {
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY1_TEXT_NAME, _weather_update->forecast_items()[0].display_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON1_TEXT_NAME, _weather_update->forecast_items()[0].weather_icon());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN1_TEXT_NAME, _weather_update->forecast_items()[0].temperature_maxmin_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN1_TEXT_NAME, _weather_update->forecast_items()[0].precipitation_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND1_TEXT_NAME, _weather_update->forecast_items()[0].wind_string());
    }

    if (_weather_update->forecast_items().get_size() >= 2) {
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY2_TEXT_NAME, _weather_update->forecast_items()[1].display_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON2_TEXT_NAME, _weather_update->forecast_items()[1].weather_icon());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN2_TEXT_NAME, _weather_update->forecast_items()[1].temperature_maxmin_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN2_TEXT_NAME, _weather_update->forecast_items()[1].precipitation_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND2_TEXT_NAME, _weather_update->forecast_items()[1].wind_string());
    }

    if (_weather_update->forecast_items().get_size() >= 3) {
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY3_TEXT_NAME, _weather_update->forecast_items()[2].display_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON3_TEXT_NAME, _weather_update->forecast_items()[2].weather_icon());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN3_TEXT_NAME, _weather_update->forecast_items()[2].temperature_maxmin_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN3_TEXT_NAME, _weather_update->forecast_items()[2].precipitation_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND3_TEXT_NAME, _weather_update->forecast_items()[2].wind_string());
    }

    if (_weather_update->forecast_items().get_size() >= 4) {
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY4_TEXT_NAME, _weather_update->forecast_items()[3].display_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON4_TEXT_NAME, _weather_update->forecast_items()[3].weather_icon());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN4_TEXT_NAME, _weather_update->forecast_items()[3].temperature_maxmin_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN4_TEXT_NAME, _weather_update->forecast_items()[3].precipitation_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND4_TEXT_NAME, _weather_update->forecast_items()[3].wind_string());
    }

    if (_weather_update->forecast_items().get_size() >= 5) {
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_DAY5_TEXT_NAME, _weather_update->forecast_items()[4].display_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_ICON5_TEXT_NAME, _weather_update->forecast_items()[4].weather_icon());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_MAXMIN5_TEXT_NAME, _weather_update->forecast_items()[4].temperature_maxmin_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_RAIN5_TEXT_NAME, _weather_update->forecast_items()[4].precipitation_string());
      NSPanel::instance->setComponentText(SCREENSAVER_PAGE_NAME "." SCREENSAVER_FORECAST_WIND5_TEXT_NAME, _weather_update->forecast_items()[4].wind_string());
    }

  } catch (...) {
    LOG_ERROR("Something went wrong while processing weather data protobuf.");
  }
}
