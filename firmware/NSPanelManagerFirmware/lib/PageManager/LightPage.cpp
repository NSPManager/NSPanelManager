#include <Light.hpp>
#include <LightPage.hpp>
#include <MqttLog.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <TftDefines.h>

void LightPage::show() {
  this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
  NSPanel::instance->goToPage(LIGHT_PAGE_NAME);
  PageManager::SetCurrentPage(this);

  if (LightPage::selectedLight != nullptr) {
    if (LightPage::selectedLight->canTemperature()) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
    } else if (LightPage::selectedLight->canRgb()) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_RGB;
    } else {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP; // Default to color temp although this wont be shown.
    }

    this->selectedLight->attachDeconstructCallback(this);
    this->selectedLight->attachUpdateCallback(this);
  }
}

void LightPage::update() {
  LightPage::updateValues();
}

void LightPage::entityDeconstructCallback(DeviceEntity *entity) {
  PageManager::GoBack();
}

void LightPage::entityUpdateCallback(DeviceEntity *entity) {
  this->update();
}

void LightPage::unshow() {
  LightPage::selectedLight->detachDeconstructCallback(this);
  LightPage::selectedLight->detachUpdateCallback(this);
  LightPage::selectedLight = nullptr;
}

void LightPage::updateValues() {
  if (LightPage::selectedLight != nullptr) {
    NSPanel::instance->setComponentText(LIGHT_PAGE_LIGHT_LABEL_NAME, LightPage::selectedLight->getName().c_str());
    NSPanel::instance->setComponentVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME, LightPage::selectedLight->getLightLevel());

    if (LightPage::selectedLight->canTemperature() && this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, LightPage::selectedLight->getColorTemperature());
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_TEMP_MODE_PIC);
    } else if (LightPage::selectedLight->canRgb() && this->_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_HUE_SLIDER_NAME, LightPage::selectedLight->getHue());
      NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, LightPage::selectedLight->getSaturation());
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_RGB_MODE_PIC);
    }

    if (LightPage::selectedLight->canTemperature() && LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
    } else if (LightPage::selectedLight->canTemperature() && !LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!LightPage::selectedLight->canTemperature() && LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!LightPage::selectedLight->canTemperature() && !LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    }
  }
}

LIGHT_PAGE_MODE LightPage::getCurrentMode() {
  return this->_currentMode;
}

void LightPage::switchMode() {
  if (this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
    this->_currentMode = LIGHT_PAGE_MODE::COLOR_RGB;
  } else if (this->_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
    this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
  }
  LightPage::updateValues();
}

uint8_t LightPage::getBrightnessValue() {
  return NSPanel::instance->getComponentIntVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME);
}

uint8_t LightPage::getKelvinSatValue() {
  return NSPanel::instance->getComponentIntVal(LIGHT_PAGE_KELVIN_SLIDER_NAME);
}

uint16_t LightPage::getHueValue() {
  return NSPanel::instance->getComponentIntVal(LIGHT_PAGE_HUE_SLIDER_NAME);
}
