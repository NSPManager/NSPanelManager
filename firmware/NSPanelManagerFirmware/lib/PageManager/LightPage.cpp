#include <Light.hpp>
#include <LightManager.hpp>
#include <LightPage.hpp>
#include <MqttLog.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <TftDefines.h>

void LightPage::show() {
  this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
  PageManager::SetCurrentPage(this);
  NSPanel::instance->goToPage(LIGHT_PAGE_NAME);

  if (this->selectedLight != nullptr) {
    if (this->selectedLight->canTemperature()) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
    } else if (this->selectedLight->canRgb()) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_RGB;
    } else {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP; // Default to color temp although this wont be shown.
    }
    this->updateValues();
  }

  if (this->selectedLight != nullptr) {
    this->selectedLight->attachDeconstructCallback(this);
    this->selectedLight->attachUpdateCallback(this);
  }
}

void LightPage::update() {
  this->updateValues();
}

void LightPage::entityDeconstructCallback(DeviceEntity *entity) {
  entity->detachUpdateCallback(this);
  entity->detachDeconstructCallback(this);
  this->selectedLight = nullptr;
  PageManager::GoBack();
}

void LightPage::entityUpdateCallback(DeviceEntity *entity) {
  this->update();
}

void LightPage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  LOG_DEBUG("Got touch event, component ", page, ".", component, " ", pressed ? "pressed" : "released");

  switch (component) {
  case LIGHT_PAGE_BACK_BUTTON_ID: {
    // NSPanel::instance->goToPage(ROOM_PAGE_NAME);
    // InterfaceManager::instance->_populateRoomPage();
    PageManager::GoBack();
    break;
  }
  case LIGHT_PAGE_BRIGHTNESS_SLIDER_ID: {
    if (PageManager::GetLightPage()->selectedLight != nullptr) {
      std::list<Light *> lights;
      lights.push_back(PageManager::GetLightPage()->selectedLight);
      LightManager::ChangeLightsToLevel(&lights, PageManager::GetLightPage()->getBrightnessValue());
      // PageManager::GetLightPage()->updateValues(); Not needed as slider changes directly
    }
    break;
  }
  case LIGHT_PAGE_KELVIN_SLIDER_ID: {
    if (PageManager::GetLightPage()->selectedLight != nullptr) {
      std::list<Light *> lights;
      lights.push_back(PageManager::GetLightPage()->selectedLight);
      if (PageManager::GetLightPage()->getCurrentMode() == LIGHT_PAGE_MODE::COLOR_TEMP) {
        LightManager::ChangeLightToColorTemperature(&lights, PageManager::GetLightPage()->getKelvinSatValue());
      } else if (PageManager::GetLightPage()->getCurrentMode() == LIGHT_PAGE_MODE::COLOR_RGB) {
        LightManager::ChangeLightsToColorSaturation(&lights, PageManager::GetLightPage()->getKelvinSatValue());
      }
      // PageManager::GetLightPage()->updateValues(); Not needed as slider changes directly
    }
    break;
  }
  case LIGHT_PAGE_HUE_SLIDER_ID: {
    if (PageManager::GetLightPage()->selectedLight != nullptr) {
      std::list<Light *> lights;
      lights.push_back(PageManager::GetLightPage()->selectedLight);
      LightManager::ChangeLightsToColorHue(&lights, PageManager::GetLightPage()->getHueValue());
    }
    break;
  }
  case LIGHT_PAGE_SWITCH_MODE_BUTTON_ID: {
    PageManager::GetLightPage()->switchMode();
    break;
  }
  default:
    break;
  }
}

void LightPage::unshow() {
  if (this->selectedLight != nullptr) {
    this->selectedLight->detachDeconstructCallback(this);
    this->selectedLight->detachUpdateCallback(this);
    this->selectedLight = nullptr;
  }
}

void LightPage::updateValues() {
  if (this->selectedLight != nullptr) {
    NSPanel::instance->setComponentText(LIGHT_PAGE_LIGHT_LABEL_NAME, this->selectedLight->getName().c_str());
    if (this->selectedLight->getLightLevel() != this->_last_brightness) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME, this->selectedLight->getLightLevel());
      this->_last_brightness = this->selectedLight->getLightLevel();
    }

    if (this->selectedLight->canTemperature() && this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
      if (this->_last_kelvin_saturation != this->selectedLight->getColorTemperature()) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, this->selectedLight->getColorTemperature());
        this->_last_kelvin_saturation = this->selectedLight->getColorTemperature();
      }
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_RGB_MODE_PIC);
    } else if (this->selectedLight->canRgb() && this->_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
      if (this->_last_hue != this->selectedLight->getHue()) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_HUE_SLIDER_NAME, this->selectedLight->getHue());
        this->_last_hue = this->selectedLight->getHue();
      }
      if (this->_last_kelvin_saturation != this->selectedLight->getSaturation()) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, this->selectedLight->getSaturation());
        this->_last_kelvin_saturation = this->selectedLight->getSaturation();
      }
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_TEMP_MODE_PIC);
    }

    if (this->selectedLight->canTemperature() && LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      if (this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
        NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      } else {
        NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      }
    } else if (this->selectedLight->canTemperature() && !LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!this->selectedLight->canTemperature() && LightPage::selectedLight->canRgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!this->selectedLight->canTemperature() && !LightPage::selectedLight->canRgb()) {
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
