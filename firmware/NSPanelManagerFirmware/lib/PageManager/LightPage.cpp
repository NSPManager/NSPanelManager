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
    if (this->selectedLight->can_color_temperature) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
    } else if (this->selectedLight->can_rgb) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_RGB;
    } else {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP; // Default to color temp although this wont be shown.
    }
    this->updateValues();
  }
}

void LightPage::update() {
  this->updateValues();
}

void LightPage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  LOG_DEBUG("Got touch event, component ", page, ".", component, " ", pressed ? "pressed" : "released");

  switch (component) {
  case LIGHT_PAGE_BACK_BUTTON_ID: {
    PageManager::GoBack();
    break;
  }
  case LIGHT_PAGE_BRIGHTNESS_SLIDER_ID: {
    if (PageManager::GetLightPage()->selectedLight != nullptr) {
      // TODO: Implement with protobuf
      // NSPanelMQTTManagerCommand command = NSPANEL_MQTTMANAGER_COMMAND__INIT;
      // command.command_data_case = NSPanelMQTTManagerCommand__CommandDataCase::NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
      // command.first_page_turn_on->affect_lights = _NSPanelMQTTManagerCommand__AffectLightsOptions::NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__ALL;
      // command.first_page_turn_on->brightness_slider_value = PageManager::GetLightPage()->getBrightnessValue();
      // command.first_page_turn_on->kelvin_slider_value = PageManager::GetLightPage()->selectedLight->color_temp;

      // size_t pack_length = nspanel_mqttmanager_command__get_packed_size(&command);
      // uint8_t buffer[pack_length];
      // size_t pack_size = nspanel_mqttmanager_command__pack(&command, buffer);

      // std::string full_buffer = std::string(buffer, buffer + pack_size);
      // MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, full_buffer, true);
    }
    break;
  }
  case LIGHT_PAGE_KELVIN_SLIDER_ID: {
    // TODO: Implement with protobuf
    if (PageManager::GetLightPage()->selectedLight != nullptr) {
      // std::list<Light *> lights;
      // lights.push_back(PageManager::GetLightPage()->selectedLight);
      // if (PageManager::GetLightPage()->getCurrentMode() == LIGHT_PAGE_MODE::COLOR_TEMP) {
      //   LightManager::ChangeLightToColorTemperature(&lights, PageManager::GetLightPage()->getKelvinSatValue());
      // } else if (PageManager::GetLightPage()->getCurrentMode() == LIGHT_PAGE_MODE::COLOR_RGB) {
      //   LightManager::ChangeLightsToColorSaturation(&lights, PageManager::GetLightPage()->getKelvinSatValue());
      // }
      // PageManager::GetLightPage()->updateValues(); Not needed as slider changes directly
    }
    break;
  }
  case LIGHT_PAGE_HUE_SLIDER_ID: {
    // TODO: Implement with protobuf
    // if (PageManager::GetLightPage()->selectedLight != nullptr) {
    //   std::list<Light *> lights;
    //   lights.push_back(PageManager::GetLightPage()->selectedLight);
    //   LightManager::ChangeLightsToColorHue(&lights, PageManager::GetLightPage()->getHueValue());
    // }
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
  this->selectedLight = nullptr;
}

void LightPage::updateValues() {
  if (this->selectedLight != nullptr) {
    NSPanel::instance->setComponentText(LIGHT_PAGE_LIGHT_LABEL_NAME, this->selectedLight->name);
    if (this->selectedLight->light_level != this->_last_brightness) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME, this->selectedLight->light_level);
      this->_last_brightness = this->selectedLight->light_level;
    }

    if (this->selectedLight->can_color_temperature && this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
      if (this->_last_kelvin_saturation != this->selectedLight->color_temp) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, this->selectedLight->color_temp);
        this->_last_kelvin_saturation = this->selectedLight->color_temp;
      }
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_RGB_MODE_PIC);
    } else if (this->selectedLight->can_rgb && this->_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
      if (this->_last_hue != this->selectedLight->hue) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_HUE_SLIDER_NAME, this->selectedLight->hue);
        this->_last_hue = this->selectedLight->hue;
      }
      if (this->_last_kelvin_saturation != this->selectedLight->saturation) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, this->selectedLight->saturation);
        this->_last_kelvin_saturation = this->selectedLight->saturation;
      }
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_TEMP_MODE_PIC);
    }

    if (this->selectedLight->can_color_temperature && LightPage::selectedLight->can_rgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      if (this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
        NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      } else {
        NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      }
    } else if (this->selectedLight->can_color_temperature && !LightPage::selectedLight->can_rgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!this->selectedLight->can_color_temperature && LightPage::selectedLight->can_rgb) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!this->selectedLight->can_color_temperature && !LightPage::selectedLight->can_rgb) {
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
