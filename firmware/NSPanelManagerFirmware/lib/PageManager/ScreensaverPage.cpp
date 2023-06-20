#include <InterfaceConfig.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <ScreensaverPage.hpp>
#include <TftDefines.h>

void ScreensaverPage::attachMqttTimeCallback() {
  MqttManager::subscribeToTopic("nspanel/status/time", &ScreensaverPage::clockMqttCallback);
}

void ScreensaverPage::show() {
  PageManager::SetCurrentPage(this);
  NSPanel::instance->setDimLevel(InterfaceConfig::screensaver_dim_level);
  NSPanel::instance->goToPage(SCREENSAVE_PAGE_NAME);
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "0");
}

void ScreensaverPage::update() {
  // Update is done though MQTT time callback
}

void ScreensaverPage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  LOG_DEBUG("Got touch event, component ", page, ".", component, " ", pressed ? "pressed" : "released");
  PageManager::GetHomePage()->show();
}

void ScreensaverPage::unshow() {
  NSPanel::instance->setDimLevel(InterfaceConfig::screen_dim_level);
  MqttManager::publish(NSPMConfig::instance->mqtt_screen_state_topic, "1");
}

void ScreensaverPage::clockMqttCallback(char *topic, byte *payload, unsigned int length) {
  std::string clock_string = std::string((char *)payload, length);
  NSPanel::instance->setComponentText(SCREENSAVE_PAGE_TEXT_NAME, clock_string.c_str());
}
