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
  std::string mqtt_activation_topic = "nspanel/scenes/";
  if (this->room != nullptr) {
    mqtt_activation_topic.append("room/");
    mqtt_activation_topic.append(this->room->name);
    mqtt_activation_topic.append("/");
  } else {
    mqtt_activation_topic.append("global/");
  }
  mqtt_activation_topic.append(this->name);
  mqtt_activation_topic.append("/activate");
  LOG_INFO("Activating scene: ", this->id, "::", this->name.c_str());

  MqttManager::publish(mqtt_activation_topic, "1");
}

void Scene::save() {
  std::string mqtt_activation_topic = "nspanel/scenes/";
  if (this->room != nullptr) {
    mqtt_activation_topic.append("room/");
    mqtt_activation_topic.append(this->room->name);
    mqtt_activation_topic.append("/");
  } else {
    mqtt_activation_topic.append("global/");
  }
  mqtt_activation_topic.append(this->name);
  mqtt_activation_topic.append("/save");
  LOG_INFO("Saving scene: ", this->id, "::", this->name.c_str());

  MqttManager::publish(mqtt_activation_topic, "1");
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
