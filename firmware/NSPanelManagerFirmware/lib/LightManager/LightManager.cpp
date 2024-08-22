#include <ArduinoJson.h>
#include <InterfaceConfig.hpp>
#include <Light.hpp>
#include <LightManager.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <WiFi.h>
#include <hash_map>

#define ELEMENT_IN_LIST(element, list) (std::find(list.begin(), list.end(), element) != list.end())

void LightManager::ChangeLightsToLevel(std::list<Light *> *lights, uint8_t level) {
  JsonDocument doc;
  doc["mac_origin"] = WiFi.macAddress();
  doc["method"] = "set";
  doc["attribute"] = "brightness";
  doc["brightness"] = level;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  std::list<Light *> send_color_temp_update;

  for (Light *light : (*lights)) {
    entity_ids.add(light->getId());
  }

  char buffer[1024];
  size_t json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (Light *light : (*lights)) {
      light->setLightLevel(level);
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void LightManager::ChangeLightToColorTemperature(std::list<Light *> *lights, uint16_t kelvin) {
  InterfaceConfig::ignore_mqtt_state_events_until = millis() + InterfaceConfig::mqtt_ignore_time;

  uint16_t sendKelvin = kelvin * ((InterfaceConfig::colorTempMax - InterfaceConfig::colorTempMin) / 100);
  if (InterfaceConfig::reverseColorTempSlider) {
    sendKelvin = InterfaceConfig::colorTempMax - sendKelvin;
  } else {
    sendKelvin = InterfaceConfig::colorTempMin + sendKelvin;
  }

  JsonDocument doc;
  doc["mac_origin"] = WiFi.macAddress();
  doc["method"] = "set";
  doc["attribute"] = "kelvin";
  doc["kelvin"] = sendKelvin;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (Light *light : (*lights)) {
    entity_ids.add(light->getId());
  }

  char buffer[1024];
  uint json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (Light *light : (*lights)) {
      light->setColorTemperature(kelvin);
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void LightManager::ChangeLightsToColorSaturation(std::list<Light *> *lights, uint16_t saturation) {
  InterfaceConfig::ignore_mqtt_state_events_until = millis() + InterfaceConfig::mqtt_ignore_time;

  JsonDocument doc;
  doc["mac_origin"] = WiFi.macAddress();
  doc["method"] = "set";
  doc["attribute"] = "saturation";
  doc["saturation"] = saturation;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (Light *light : (*lights)) {
    entity_ids.add(light->getId());
  }

  char buffer[1024];
  uint json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (Light *light : (*lights)) {
      light->setSaturation(saturation);
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

void LightManager::ChangeLightsToColorHue(std::list<Light *> *lights, uint16_t hue) {
  InterfaceConfig::ignore_mqtt_state_events_until = millis() + InterfaceConfig::mqtt_ignore_time;

  JsonDocument doc;
  doc["mac_origin"] = WiFi.macAddress();
  doc["method"] = "set";
  doc["attribute"] = "hue";
  doc["hue"] = hue;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

  for (Light *light : (*lights)) {
    entity_ids.add(light->getId());
  }

  char buffer[1024];
  uint json_length = serializeJson(doc, buffer);
  if (json_length > 0 && MqttManager::publish("nspanel/mqttmanager/command", buffer)) {
    for (Light *light : (*lights)) {
      light->setHue(hue);
    }
  } else {
    LOG_ERROR("Failed to send MQTT brightness update.");
  }
}

std::list<Light *> LightManager::getCeilingLightsThatAreOn() {
  std::list<Light *> lightsThatAreOn;
  for (auto room : RoomManager::rooms) {
    for (auto ceilingLight : room->ceilingLights) {
      if (ceilingLight.second->getLightLevel() > 0) {
        lightsThatAreOn.push_back(ceilingLight.second);
      }
    }
  }
  return lightsThatAreOn;
}

std::list<Light *> LightManager::getTableLightsThatAreOn() {
  std::list<Light *> lightsThatAreOn;
  for (auto room : RoomManager::rooms) {
    for (auto tableLight : room->tableLights) {
      if (tableLight.second->getLightLevel() > 0) {
        lightsThatAreOn.push_back(tableLight.second);
      }
    }
  }
  return lightsThatAreOn;
}

std::list<Light *> LightManager::getAllLightsThatAreOn() {
  std::list<Light *> return_lights;
  for (Room *room : RoomManager::rooms) {
    for (std::pair<uint16_t, Light *> pair : room->ceilingLights) {
      if (pair.second->getLightLevel() > 0) {
        return_lights.push_back(pair.second);
      }
    }
    for (std::pair<uint16_t, Light *> pair : room->tableLights) {
      if (pair.second->getLightLevel() > 0) {
        return_lights.push_back(pair.second);
      }
    }
  }
  return return_lights;
}

std::list<Light *> LightManager::getAllCeilingLightsThatAreOn() {
  std::list<Light *> return_lights;
  for (Room *room : RoomManager::rooms) {
    for (std::pair<uint16_t, Light *> pair : room->ceilingLights) {
      if (pair.second->getLightLevel() > 0) {
        return_lights.push_back(pair.second);
      }
    }
  }
  return return_lights;
}

std::list<Light *> LightManager::getAllTableLightsThatAreOn() {
  std::list<Light *> return_lights;
  for (Room *room : RoomManager::rooms) {
    for (std::pair<uint16_t, Light *> pair : room->tableLights) {
      if (pair.second->getLightLevel() > 0) {
        return_lights.push_back(pair.second);
      }
    }
  }
  return return_lights;
}

std::list<Light *> LightManager::getAllCeilingLights() {
  std::list<Light *> return_lights;
  for (Room *room : RoomManager::rooms) {
    for (std::pair<uint16_t, Light *> pair : room->ceilingLights) {
      return_lights.push_back(pair.second);
    }
  }
  return return_lights;
}

std::list<Light *> LightManager::getAllTableLights() {
  std::list<Light *> return_lights;
  for (Room *room : RoomManager::rooms) {
    for (std::pair<uint16_t, Light *> pair : room->tableLights) {
      return_lights.push_back(pair.second);
    }
  }
  return return_lights;
}

std::list<Light *> LightManager::getAllLights() {
  std::list<Light *> return_lights;
  for (Room *room : RoomManager::rooms) {
    for (std::pair<uint16_t, Light *> pair : room->ceilingLights) {
      return_lights.push_back(pair.second);
    }
    for (std::pair<uint16_t, Light *> pair : room->tableLights) {
      return_lights.push_back(pair.second);
    }
  }
  return return_lights;
}

Light *LightManager::getLightById(uint16_t id) {
  for (Room *room : RoomManager::rooms) {
    std::unordered_map<uint16_t, Light *>::iterator light = room->ceilingLights.find(id);
    if (light != room->ceilingLights.end()) {
      return light->second;
    } else {
      light = room->tableLights.find(id);
      if (light != room->tableLights.end()) {
        return light->second;
      }
    }
  }
  return nullptr;
}

bool LightManager::anyCeilingLightsOn() {
  for (Room *room : RoomManager::rooms) {
    for (auto lightPair : room->ceilingLights) {
      if (lightPair.second->getLightLevel() > 0) {
        return true;
      }
    }
  }
  return false;
}

bool LightManager::anyTableLightsOn() {
  for (Room *room : RoomManager::rooms) {
    for (auto lightPair : room->tableLights) {
      if (lightPair.second->getLightLevel() > 0) {
        return true;
      }
    }
  }
  return false;
}

bool LightManager::anyLightsOn() {
  return LightManager::anyCeilingLightsOn() || LightManager::anyTableLightsOn();
}

void LightManager::subscribeToMqttLightUpdates() {
  if (LightManager::_mqttMessageQueue == NULL) {
    LightManager::_mqttMessageQueue = xQueueCreate(8, sizeof(mqttMessage *));
    if (LightManager::_taskHandleProcessMqttMessage == NULL) {
      xTaskCreatePinnedToCore(_taskProcessMqttMessages, "taskProcessMqttMessages", 5000, NULL, 1, &LightManager::_taskHandleProcessMqttMessage, CONFIG_ARDUINO_RUNNING_CORE);
    }
  }

  for (Light *light : LightManager::getAllLights()) {
    MqttManager::subscribeToTopic(light->getLevelStateTopic().c_str(), &LightManager::mqttCallback);

    if (light->canTemperature()) {
      MqttManager::subscribeToTopic(light->getColorTemperatureStateTopic().c_str(), &LightManager::mqttCallback);
    }

    if (light->canRgb()) {
      MqttManager::subscribeToTopic(light->getSaturationStateTopic().c_str(), &LightManager::mqttCallback);
      MqttManager::subscribeToTopic(light->getHueStateTopic().c_str(), &LightManager::mqttCallback);
    }
  }
}

void LightManager::stop() {
  LOG_DEBUG("Stopping LightManager.");
  vTaskDelay(50 / portTICK_PERIOD_MS);
  if (LightManager::_taskHandleProcessMqttMessage != NULL) {
    vTaskDelete(LightManager::_taskHandleProcessMqttMessage);
    LightManager::_taskHandleProcessMqttMessage = NULL;
    LightManager::_mqttMessageQueue = NULL;
  }

  LOG_INFO("LightManager stopped.");
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void LightManager::mqttCallback(char *topic, byte *payload, unsigned int length) {
  if (LightManager::_mqttMessageQueue != NULL) {
    // TODO: Ignore MQTT light updates for X millis
    mqttMessage *msg = new mqttMessage;
    msg->topic = topic;
    msg->payload = std::string((char *)payload, length);
    if (xQueueSendToBack(LightManager::_mqttMessageQueue, (void *)&msg, 20 / portTICK_PERIOD_MS) != pdTRUE) {
      delete msg;
    }
  }
}

void LightManager::_taskProcessMqttMessages(void *param) {
  LOG_DEBUG("Started task to process MQTT messages.");
  mqttMessage *msg;
  for (;;) {
    if (xQueueReceive(LightManager::_mqttMessageQueue, &msg, portMAX_DELAY) == pdTRUE) {
      // Delete any message that does not contain any payload
      if (msg->payload.size() <= 0) {
        delete msg;
        continue;
      }
      if (msg->topic.find("nspanel/entities/") == 0) { // If topic begins with nspanel/entities/
        std::string domain = msg->topic;
        domain = domain.erase(0, strlen("nspanel/entities/"));
        domain = domain.substr(0, domain.find('/'));

        std::string entity = msg->topic;
        entity = entity.erase(0, strlen("nspanel/entities/"));
        entity = entity.erase(0, entity.find('/') + 1);
        entity = entity.substr(0, entity.find('/'));

        std::string attribute = msg->topic;
        attribute = attribute.erase(0, attribute.find_last_of('/') + 1);

        if (domain.compare("light") == 0 && attribute.compare("state_brightness_pct") == 0) {
          Light *light = LightManager::getLightById(atoi(entity.c_str()));
          if (light != nullptr) {
            light->setLightLevel(atoi(msg->payload.c_str()));
            light->callUpdateCallbacks();
          }
        } else if (domain.compare("light") == 0 && attribute.compare("state_kelvin") == 0) {
          unsigned long colorTemp = atoi(msg->payload.c_str());
          if (colorTemp > InterfaceConfig::colorTempMax) {
            LOG_WARNING("Received kelvin value that was outside kelvin range. Received: ", colorTemp, ", max allowed: ", InterfaceConfig::colorTempMax, ". Clamping.");
            colorTemp = InterfaceConfig::colorTempMax;
          } else if (colorTemp < InterfaceConfig::colorTempMin) {
            LOG_WARNING("Received kelvin value that was outside kelvin range. Received: ", colorTemp, ", min allowed: ", InterfaceConfig::colorTempMin, ". Clamping.");
            colorTemp = InterfaceConfig::colorTempMin;
          }
          colorTemp = ((colorTemp - InterfaceConfig::colorTempMin) * 100) / (InterfaceConfig::colorTempMax - InterfaceConfig::colorTempMin);

          if (InterfaceConfig::reverseColorTempSlider) {
            colorTemp = 100 - colorTemp;
          }

          Light *light = LightManager::getLightById(atoi(entity.c_str()));
          if (light != nullptr) {
            // LOG_DEBUG("Setting light ", light->getId(), "::", light->getName().c_str(), " color temp %: ", std::to_string(colorTemp).c_str());
            light->setColorTemperature(colorTemp);
            light->callUpdateCallbacks();
          } else {
            LOG_ERROR("Got kelvin update for unknown light ID: ", entity.c_str());
          }
        } else if (domain.compare("light") == 0 && attribute.compare("state_hue") == 0) {
          uint16_t hue = atoi(msg->payload.c_str());

          Light *light = LightManager::getLightById(atoi(entity.c_str()));
          if (light != nullptr) {
            light->setHue(hue);
            light->callUpdateCallbacks();
          } else {
            LOG_ERROR("Got hue update for unknown light ID: ", entity.c_str());
          }
        } else if (domain.compare("light") == 0 && attribute.compare("state_sat") == 0) {
          uint8_t sat = atoi(msg->payload.c_str());

          Light *light = LightManager::getLightById(atoi(entity.c_str()));
          if (light != nullptr) {
            light->setSaturation(sat);
            light->callUpdateCallbacks();
          } else {
            LOG_ERROR("Got saturation update for unknown light ID: ", entity.c_str());
          }
        } else {
          LOG_ERROR("Got state update for unknown attribute: ", attribute.c_str());
        }
      }
      delete msg;
    }
  }
}
