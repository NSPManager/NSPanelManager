#include <LightPage.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <TftDefines.h>
#include <WriteBufferFixedSize.h>
#include <protobuf_defines.h>

void LightPage::show() {
  this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
  PageManager::SetCurrentPage(this);
  NSPanel::instance->goToPage(LIGHT_PAGE_NAME);

  if (this->selectedLight != nullptr) {
    if (this->selectedLight->can_color_temperature()) {
      this->_currentMode = LIGHT_PAGE_MODE::COLOR_TEMP;
    } else if (this->selectedLight->can_rgb()) {
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
  if (PageManager::GetLightPage()->selectedLight != nullptr) {
    switch (component) {
    case LIGHT_PAGE_BACK_BUTTON_ID: {
      PageManager::GoBack();
      break;
    }
    case LIGHT_PAGE_BRIGHTNESS_SLIDER_ID: {
      if (PageManager::GetLightPage()->selectedLight != nullptr) {
        PROTOBUF_MQTTMANAGER_CMD_LIGHT_COMMAND light_command;
        light_command.add_light_ids(PageManager::GetLightPage()->selectedLight->id());
        light_command.set_has_brightness(true);
        light_command.set_brightness(PageManager::GetLightPage()->getBrightnessValue());

        PROTOBUF_MQTTMANAGER_CMD command;
        command.set_light_command(light_command);

        EmbeddedProto::WriteBufferFixedSize<PROTOBUF_MQTTMANAGER_CMD_MAX_SIZE> write_buffer;
        if (command.serialize(write_buffer) != EmbeddedProto::Error::NO_ERRORS) {
          LOG_ERROR("Failed to serialize MQTTManager command. Will cancel command.");
          return;
        }
        MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, (char *)write_buffer.get_data(), true);
      }
      break;
    }
    case LIGHT_PAGE_KELVIN_SLIDER_ID: {
      if (PageManager::GetLightPage()->selectedLight != nullptr) {
        PROTOBUF_MQTTMANAGER_CMD_LIGHT_COMMAND light_command;
        light_command.add_light_ids(PageManager::GetLightPage()->selectedLight->id());
        if (PageManager::GetLightPage()->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
          light_command.set_has_color_temperature(true);
          light_command.set_brightness(PageManager::GetLightPage()->getBrightnessValue());
        } else if (PageManager::GetLightPage()->_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
          light_command.set_has_saturation(true);
          light_command.set_saturation(PageManager::GetLightPage()->getKelvinSatValue());
        }

        PROTOBUF_MQTTMANAGER_CMD command;
        command.set_light_command(light_command);

        EmbeddedProto::WriteBufferFixedSize<PROTOBUF_MQTTMANAGER_CMD_MAX_SIZE> write_buffer;
        if (command.serialize(write_buffer) != EmbeddedProto::Error::NO_ERRORS) {
          LOG_ERROR("Failed to serialize MQTTManager command. Will cancel command.");
          return;
        }
        MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, (char *)write_buffer.get_data(), true);
      }
      break;
    }
    case LIGHT_PAGE_HUE_SLIDER_ID: {
      if (PageManager::GetLightPage()->selectedLight != nullptr) {
        PROTOBUF_MQTTMANAGER_CMD_LIGHT_COMMAND light_command;
        light_command.add_light_ids(PageManager::GetLightPage()->selectedLight->id());
        light_command.set_has_hue(true);
        light_command.set_hue(PageManager::GetLightPage()->getHueValue());

        PROTOBUF_MQTTMANAGER_CMD command;
        command.set_light_command(light_command);

        EmbeddedProto::WriteBufferFixedSize<PROTOBUF_MQTTMANAGER_CMD_MAX_SIZE> write_buffer;
        if (command.serialize(write_buffer) != EmbeddedProto::Error::NO_ERRORS) {
          LOG_ERROR("Failed to serialize MQTTManager command. Will cancel command.");
          return;
        }
        MqttManager::publish(NSPMConfig::instance->mqttmanager_command_topic, (char *)write_buffer.get_data(), true);
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
}

void LightPage::unshow() {
  this->selectedLight = nullptr;
}

void LightPage::updateValues() {
  if (this->selectedLight != nullptr) {
    NSPanel::instance->setComponentText(LIGHT_PAGE_LIGHT_LABEL_NAME, this->selectedLight->name());
    if (this->selectedLight->light_level() != this->_last_brightness) {
      NSPanel::instance->setComponentVal(LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME, this->selectedLight->light_level());
      this->_last_brightness = this->selectedLight->light_level();
    }

    if (this->selectedLight->can_color_temperature() && this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
      if (this->_last_kelvin_saturation != this->selectedLight->color_temp()) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, this->selectedLight->color_temp());
        this->_last_kelvin_saturation = this->selectedLight->color_temp();
      }
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_KELVIN_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_RGB_MODE_PIC);
    } else if (this->selectedLight->can_rgb() && this->_currentMode == LIGHT_PAGE_MODE::COLOR_RGB) {
      if (this->_last_hue != this->selectedLight->hue()) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_HUE_SLIDER_NAME, this->selectedLight->hue());
        this->_last_hue = this->selectedLight->hue();
      }
      if (this->_last_kelvin_saturation != this->selectedLight->saturation()) {
        NSPanel::instance->setComponentVal(LIGHT_PAGE_KELVIN_SLIDER_NAME, this->selectedLight->saturation());
        this->_last_kelvin_saturation = this->selectedLight->saturation();
      }
      NSPanel::instance->setComponentPic(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC);
      NSPanel::instance->setComponentPic1(LIGHT_PAGE_KELVIN_SLIDER_NAME, LIGHT_PAGE_SAT_SLIDER_PIC1);
      NSPanel::instance->setComponentPic(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, LIGHT_PAGE_COLOR_TEMP_MODE_PIC);
    }

    if (this->selectedLight->can_color_temperature() && LightPage::selectedLight->can_rgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      if (this->_currentMode == LIGHT_PAGE_MODE::COLOR_TEMP) {
        NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      } else {
        NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      }
    } else if (this->selectedLight->can_color_temperature() && !LightPage::selectedLight->can_rgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, false);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!this->selectedLight->can_color_temperature() && LightPage::selectedLight->can_rgb()) {
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_KELVIN_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_HUE_SLIDER_NAME, true);
      NSPanel::instance->setComponentVisible(LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME, false);
    } else if (!this->selectedLight->can_color_temperature() && !LightPage::selectedLight->can_rgb()) {
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
