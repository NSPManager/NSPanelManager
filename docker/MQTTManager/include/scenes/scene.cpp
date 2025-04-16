#include <scenes/scene.hpp>
#include <spdlog/spdlog.h>
#include <string>

uint32_t Scene::get_entity_page_id() {
  return this->_page_id;
}

uint8_t Scene::get_entity_page_slot() {
  return this->_page_slot;
}

bool Scene::can_toggle() {
  return true;
}

void Scene::toggle() {
  this->activate();
}

bool Scene::is_global() {
  return this->_is_global;
}

std::string Scene::get_mqtt_state_topic() {
  return "";
}
