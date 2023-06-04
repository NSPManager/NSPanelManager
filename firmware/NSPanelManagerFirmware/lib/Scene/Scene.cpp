#include <MqttLog.hpp>
#include <Room.hpp>
#include <Scene.hpp>

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
