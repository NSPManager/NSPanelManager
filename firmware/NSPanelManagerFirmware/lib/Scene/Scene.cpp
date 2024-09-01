#include <ArduinoJson.h>
#include <MqttLog.hpp>
#include <Room.hpp>
#include <Scene.hpp>

uint16_t Scene::getId() {
  return this->id;
}

std::string Scene::getName() {
  return this->name;
}

void Scene::activate() {
  LOG_INFO("Activating scene: ", this->id, "::", this->name.c_str());
  JsonDocument doc;
  doc["command"] = "activate_scene";
  doc["mac_origin"] = WiFi.macAddress().c_str();
  doc["scene_id"] = this->getId();

  char buffer[512];
  serializeJson(doc, buffer);
  MqttManager::publish("nspanel/mqttmanager/command", buffer);
}

void Scene::save() {
  if (this->canSave) {
    LOG_INFO("Saving scene: ", this->id, "::", this->name.c_str());
    JsonDocument doc;
    doc["command"] = "save_scene";
    doc["mac_origin"] = WiFi.macAddress().c_str();
    doc["scene_id"] = this->getId();

    char buffer[512];
    serializeJson(doc, buffer);
    MqttManager::publish("nspanel/mqttmanager/command", buffer);
  } else {
    LOG_ERROR("Tried saving a scene that does not support saving.");
  }
}

void Scene::attachDeconstructCallback(DeviceEntityObserver *observer) {
  // Do not add a subsriber twice
  for (DeviceEntityObserver *obs : this->_updateObservers) {
    if (observer == obs) {
      break;
    }
  }
  this->_deconstructObservers.push_back(observer);
}

void Scene::detachDeconstructCallback(DeviceEntityObserver *observer) {
  this->_deconstructObservers.remove(observer);
}

void Scene::callDeconstructCallbacks() {
  for (DeviceEntityObserver *observer : this->_deconstructObservers) {
    observer->entityDeconstructCallback(this);
  }
}

void Scene::attachUpdateCallback(DeviceEntityObserver *observer) {
  // Do not add a subsriber twice
  for (DeviceEntityObserver *obs : this->_updateObservers) {
    if (observer == obs) {
      break;
    }
  }
  this->_updateObservers.push_back(observer);
}

void Scene::detachUpdateCallback(DeviceEntityObserver *observer) {
  this->_updateObservers.remove(observer);
}

void Scene::callUpdateCallbacks() {
  for (DeviceEntityObserver *observer : this->_updateObservers) {
    observer->entityUpdateCallback(this);
  }
}

DeviceEntityType Scene::getType() {
  return DeviceEntityType::SCENE;
}
