#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPanel.hpp>
#include <ScreensaverPage.hpp>
#include <TftDefines.h>

void ScreensaverPage::attachMqttTimeCallback() {
  MqttManager::subscribeToTopic("nspanel/status/time", &ScreensaverPage::clockMqttCallback);
}

void ScreensaverPage::show() {
  NSPanel::instance->goToPage(SCREENSAVE_PAGE_NAME);
}

void ScreensaverPage::update() {
  // Update is done though MQTT time callback
}

void ScreensaverPage::unshow() {
  // Nothing special needed
}

void ScreensaverPage::clockMqttCallback(char *topic, byte *payload, unsigned int length) {
  std::string clock_string = std::string((char *)payload, length);
  NSPanel::instance->setComponentText(SCREENSAVE_PAGE_TEXT_NAME, clock_string.c_str());
}
