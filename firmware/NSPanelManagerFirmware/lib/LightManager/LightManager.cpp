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
  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
  doc["method"] = "set";
  doc["attribute"] = "brightness";
  doc["brightness"] = level;
  JsonArray entity_ids = doc.createNestedArray("entity_ids");

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

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
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

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
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

  DynamicJsonDocument doc(1024);
  doc["mac_origin"] = WiFi.macAddress().c_str();
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
  return (*RoomManager::currentRoom)->getCeilingLightsThatAreOn();
}

std::list<Light *> LightManager::getTableLightsThatAreOn() {
  return (*RoomManager::currentRoom)->getTableLightsThatAreOn();
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
