#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPanel.hpp>
#include <ScreensaverPage.hpp>
#include <TftDefines.h>

void ScreensaverPage::attachMqttTimeCallback() {
  LOG_DEBUG("Attaching MQTT callback for screensaver clock.");
  MqttManager::subscribeToTopic("nspanel/status/time", &ScreensaverPage::clockMqttCallback);
}

void ScreensaverPage::show() {
}

void ScreensaverPage::update() {
}

void ScreensaverPage::unshow() {
}

void ScreensaverPage::clockMqttCallback(char *topic, byte *payload, unsigned int length) {
  std::string clock_string = std::string((char *)payload, length);
  LOG_DEBUG("Setting screensaver clock to: ", clock_string.c_str());
  NSPanel::instance->setComponentText(SCREENSAVE_PAGE_TEXT_NAME, clock_string.c_str());
}
