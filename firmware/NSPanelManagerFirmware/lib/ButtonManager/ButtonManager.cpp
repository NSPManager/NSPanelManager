#include "ButtonManager.hpp"
#include <InterfaceManager.hpp>
#include <Light.hpp>
#include <LightManager.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <PageManager.hpp>

void ButtonManager::init() {
  // Setup pins for input/output
  pinMode(BUTTON_MANAGER_BUTTON1_PIN, INPUT);
  pinMode(BUTTON_MANAGER_BUTTON2_PIN, INPUT);
  pinMode(BUTTON_MANAGER_RELAY1_PIN, OUTPUT);
  pinMode(BUTTON_MANAGER_RELAY2_PIN, OUTPUT);

  ButtonManager::_lastButton1State = !digitalRead(BUTTON_MANAGER_BUTTON1_PIN);
  ButtonManager::_newButton1State = ButtonManager::_lastButton1State;
  ButtonManager::_lastButton2State = !digitalRead(BUTTON_MANAGER_BUTTON2_PIN);
  ButtonManager::_newButton2State = ButtonManager::_lastButton2State;

  LOG_DEBUG("Setting relay 1 to default state: ", NSPMConfig::instance->relay1_default_mode ? "ON" : "OFF");
  ButtonManager::setRelayState(1, NSPMConfig::instance->relay1_default_mode);
  LOG_DEBUG("Setting relay 2 to default state: ", NSPMConfig::instance->relay2_default_mode ? "ON" : "OFF");
  ButtonManager::setRelayState(2, NSPMConfig::instance->relay2_default_mode);

  xTaskCreatePinnedToCore(ButtonManager::_loop, "_taskButtonManagerLoop", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);

  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_relay1_cmd_topic.c_str(), &ButtonManager::mqttCallback);
  MqttManager::subscribeToTopic(NSPMConfig::instance->mqtt_relay2_cmd_topic.c_str(), &ButtonManager::mqttCallback);
}

void ButtonManager::mqttCallback(char *topic, byte *payload, unsigned int length) {
  uint8_t relay = 0;
  if (std::string(topic).compare(NSPMConfig::instance->mqtt_relay1_cmd_topic) == 0) {
    relay = 1;
  } else if (std::string(topic).compare(NSPMConfig::instance->mqtt_relay2_cmd_topic) == 0) {
    relay = 2;
  }

  if (relay > 0) {
    ButtonManager::setRelayState(relay, (*payload) == '1');
  }
}

void ButtonManager::setRelayState(uint8_t relay, bool state) {
  if (NSPMConfig::instance->reverse_relays) {
    if (relay == 1) {
      relay = 2;
    } else if (relay == 2) {
      relay = 1;
    }
  }
  if (relay == 1) {
    digitalWrite(BUTTON_MANAGER_RELAY1_PIN, state);
    MqttManager::publish(NSPMConfig::instance->mqtt_relay1_state_topic, state ? "1" : "0");
  } else {
    digitalWrite(BUTTON_MANAGER_RELAY2_PIN, state);
    MqttManager::publish(NSPMConfig::instance->mqtt_relay2_state_topic, state ? "1" : "0");
  }
}

void ButtonManager::_processButtonStateChange(uint8_t button, bool new_state) {
  if (button == 1) {
    if (NSPMConfig::instance->button1_mode == BUTTON_MODE::DIRECT && new_state) {
      if (NSPMConfig::instance->reverse_relays) {
        ButtonManager::setRelayState(1, !digitalRead(BUTTON_MANAGER_RELAY2_PIN));
      } else {
        ButtonManager::setRelayState(1, !digitalRead(BUTTON_MANAGER_RELAY1_PIN));
      }
    } else if (NSPMConfig::instance->button1_mode == BUTTON_MODE::FOLLOW) {
      ButtonManager::setRelayState(1, new_state);
    } else if (NSPMConfig::instance->button1_mode == BUTTON_MODE::DETACHED && ButtonManager::button1_detached_mode_light != nullptr && new_state) {
      LOG_DEBUG("Button 1 pressed, detached light: ", ButtonManager::button1_detached_mode_light->getName().c_str());
      std::list<Light *> lightsToChange;
      lightsToChange.push_back(ButtonManager::button1_detached_mode_light);
      if (ButtonManager::button1_detached_mode_light->getLightLevel() == 0) {
        LightManager::ChangeLightsToLevel(&lightsToChange, PageManager::GetHomePage()->getDimmingValue());
      } else {
        LightManager::ChangeLightsToLevel(&lightsToChange, 0);
      }
    } else if (NSPMConfig::instance->button1_mode == BUTTON_MODE::CUSTOM_MQTT && new_state) {
      MqttManager::publish(NSPMConfig::instance->button1_mqtt_topic, NSPMConfig::instance->button1_mqtt_payload);
    }
  } else if (button == 2) {
    if (NSPMConfig::instance->button2_mode == BUTTON_MODE::DIRECT && new_state) {
      if (NSPMConfig::instance->reverse_relays) {
        ButtonManager::setRelayState(2, !digitalRead(BUTTON_MANAGER_RELAY1_PIN));
      } else {
        ButtonManager::setRelayState(2, !digitalRead(BUTTON_MANAGER_RELAY2_PIN));
      }
    } else if (NSPMConfig::instance->button2_mode == BUTTON_MODE::FOLLOW) {
      ButtonManager::setRelayState(2, new_state);
    } else if (NSPMConfig::instance->button2_mode == BUTTON_MODE::DETACHED && ButtonManager::button2_detached_mode_light != nullptr && new_state) {
      LOG_DEBUG("Button 2 pressed, detached light: ", ButtonManager::button2_detached_mode_light->getName().c_str());
      std::list<Light *> lightsToChange;
      lightsToChange.push_back(ButtonManager::button2_detached_mode_light);
      if (ButtonManager::button2_detached_mode_light->getLightLevel() == 0) {
        LightManager::ChangeLightsToLevel(&lightsToChange, PageManager::GetHomePage()->getDimmingValue());
      } else {
        LightManager::ChangeLightsToLevel(&lightsToChange, 0);
      }
    } else if (NSPMConfig::instance->button2_mode == BUTTON_MODE::CUSTOM_MQTT && new_state) {
      MqttManager::publish(NSPMConfig::instance->button2_mqtt_topic, NSPMConfig::instance->button2_mqtt_payload);
    }
  }
}

void ButtonManager::_loop(void *param) {
  LOG_DEBUG("Started ButtonManager _loop.");

  for (;;) {
    ButtonManager::_newButton1State = !digitalRead(BUTTON_MANAGER_BUTTON1_PIN);
    ButtonManager::_newButton2State = !digitalRead(BUTTON_MANAGER_BUTTON2_PIN);

    if (ButtonManager::_newButton1State != ButtonManager::_lastButton1State) {
      ButtonManager::_lastButton1StateChange = millis();
      ButtonManager::_lastButton1State = ButtonManager::_newButton1State;
    } else if (ButtonManager::_newButton1State == ButtonManager::_lastButton1State && millis() - ButtonManager::_lastButton1StateChange <= BUTTON_MANAGER_BUTTON_DEBOUNCE_MS) {
      ButtonManager::_processButtonStateChange(1, ButtonManager::_newButton1State);
    }

    if (ButtonManager::_newButton2State != ButtonManager::_lastButton2State) {
      ButtonManager::_lastButton2StateChange = millis();
      ButtonManager::_lastButton2State = ButtonManager::_newButton2State;
    } else if (ButtonManager::_newButton2State == ButtonManager::_lastButton2State && millis() - ButtonManager::_lastButton2StateChange <= BUTTON_MANAGER_BUTTON_DEBOUNCE_MS) {
      ButtonManager::_processButtonStateChange(2, ButtonManager::_newButton2State);
    }

    vTaskDelay(BUTTON_MANAGER_BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS);
  }
}
