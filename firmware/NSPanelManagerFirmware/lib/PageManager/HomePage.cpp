#include <HomePage.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceManager.hpp>
#include <Light.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <TftDefines.h>

void HomePage::show() {
  NSPanel::instance->goToPage(HOME_PAGE_NAME);
  PageManager::SetCurrentPage(this);
}

void HomePage::update() {
  InterfaceManager::instance->_updatePanelLightStatus();
}

void HomePage::unshow() {
}

void HomePage::entityUpdateCallback(DeviceEntity *entity) {
}

void HomePage::entityDeconstructCallback(DeviceEntity *entity) {
}

void HomePage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  LOG_DEBUG("Got touch event, component ", page, ".", component, " ", pressed ? "pressed" : "released");
}

int HomePage::getDimmingValue() {
  return this->_dimmerValue;
}

void HomePage::setDimmingValue(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_DIMMER_SLIDER_NAME, value);
  this->_dimmerValue = value;
}

void HomePage::updateDimmerValueCache() {
  int newValue = NSPanel::instance->getComponentIntVal(HOME_DIMMER_SLIDER_NAME);
  if (newValue > InterfaceConfig::raiseToMaxLightLevelAbove) {
    this->_dimmerValue = 100;
  } else {
    this->_dimmerValue = newValue;
  }
}

int HomePage::getColorTempValue() {
  return this->_colorTemp;
}

void HomePage::setColorTempValue(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_LIGHT_COLOR_SLIDER_NAME, value);
  this->_colorTemp = value;
}

void HomePage::updateColorTempValueCache() {
  this->_colorTemp = NSPanel::instance->getComponentIntVal(HOME_LIGHT_COLOR_SLIDER_NAME);
}

void HomePage::setCeilingBrightnessLabelText(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_LABEL_CEILING_BRIGHTNESS, value);
}

void HomePage::setTableBrightnessLabelText(uint8_t value) {
  NSPanel::instance->setComponentVal(HOME_LABEL_TABLE_BRIGHTNESS, value);
}

void HomePage::setCeilingLightsState(bool state) {
  NSPanel::instance->setComponentVal(HOME_BUTTON_CEILING_NAME, state ? 1 : 0);
}

void HomePage::setTableLightsState(bool state) {
  NSPanel::instance->setComponentVal(HOME_BUTTON_TABLE_NAME, state ? 1 : 0);
}

void HomePage::setSliderLightLevelColor(uint color) {
  NSPanel::instance->setComponentForegroundColor(HOME_DIMMER_SLIDER_NAME, color);
}

void HomePage::setSliderColorTempColor(uint color) {
  NSPanel::instance->setComponentForegroundColor(HOME_LIGHT_COLOR_SLIDER_NAME, color);
}

void HomePage::setHighlightCeilingVisibility(bool visibility) {
  NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_CEILING_NAME, visibility);
}

void HomePage::setHighlightTableVisibility(bool visibility) {
  NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_TABLE_NAME, visibility);
}

void HomePage::setScreensaverTimeout(uint16_t timeout) {
  NSPanel::instance->setTimerTimeout(HOME_PAGE_SCREENSAVER_TIMER_NAME, timeout);
}

void HomePage::setRoomText(const char *text) {
  NSPanel::instance->setComponentText(HOME_PAGE_ROOM_LABEL_NAME, text);
}

void HomePage::setModeText(const char *text) {
  NSPanel::instance->setComponentText(HOME_PAGE_MODE_LABEL_NAME, text);
}
