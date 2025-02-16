#include <scenes/scene.hpp>

uint32_t Scene::get_entity_page_id() {
  return this->_page_id;
}

uint8_t Scene::get_entity_page_slot() {
  return this->_page_slot;
}

bool Scene::can_toggle() {
  return false;
}

void Scene::toggle() {
  // No-op
}
