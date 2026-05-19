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
  // Satisfies MqttManagerEntity's pure virtual. In practice all scene activations go through
  // activate(triggering_room_id) directly, so this path should never be reached.
  this->activate(std::unexpected(EntityManager::EntityError::NOT_FOUND));
}

bool Scene::is_global() {
  return this->_is_global;
}

std::string Scene::get_mqtt_state_topic() {
  return "";
}
