#include "ButtonManager.hpp"
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>

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

  ButtonManager::_setRelayState(1, ButtonManager::_newButton1State);
  ButtonManager::_setRelayState(2, ButtonManager::_newButton2State);

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
    ButtonManager::_setRelayState(relay, (*payload) == '1');
  }
}

void ButtonManager::_setRelayState(uint8_t relay, bool state) {
  // a press has been detected.
  // TODO: Detect long press and make action(s) configurable from web interface
  if (relay == 1) {
    digitalWrite(BUTTON_MANAGER_RELAY1_PIN, state);
    MqttManager::publish(NSPMConfig::instance->mqtt_relay1_state_topic, state ? "1" : "0");
  } else {
    digitalWrite(BUTTON_MANAGER_RELAY2_PIN, state);
    MqttManager::publish(NSPMConfig::instance->mqtt_relay2_state_topic, state ? "1" : "0");
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
      if (ButtonManager::_newButton1State) {
        ButtonManager::_setRelayState(1, !digitalRead(BUTTON_MANAGER_RELAY1_PIN));
      }
    }

    if (ButtonManager::_newButton2State != ButtonManager::_lastButton2State) {
      ButtonManager::_lastButton2StateChange = millis();
      ButtonManager::_lastButton2State = ButtonManager::_newButton2State;
    } else if (ButtonManager::_newButton2State == ButtonManager::_lastButton2State && millis() - ButtonManager::_lastButton2StateChange <= BUTTON_MANAGER_BUTTON_DEBOUNCE_MS) {
      if (ButtonManager::_newButton2State) {
        ButtonManager::_setRelayState(2, !digitalRead(BUTTON_MANAGER_RELAY2_PIN));
      }
    }

    vTaskDelay(BUTTON_MANAGER_BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS);
  }
}
