#include "entity/entity.hpp"
#include <nlohmann/json_fwd.hpp>
#include <scenes/nspm_scene.hpp>
#include <spdlog/spdlog.h>

NSPMScene::NSPMScene(nlohmann::json &data) {
  this->_id = data["scene_id"];
  this->_name = data["scene_name"];
  SPDLOG_DEBUG("Loading NSPM scene {}::{}.", this->_id, this->_name);
  // TODO: Find and link pointer to room
  for (nlohmann::json light_data : data["light_states"]) {
    LightState state;
    state.light_id = light_data["light_id"];
    state.color_mode = light_data["color_mode"];
    state.brightness = light_data["light_level"];
    state.color_temperature = light_data["color_temperature"];
    state.hue = light_data["hue"];
    state.saturation = light_data["saturation"];
    this->_light_states.push_back(state);
  }
}

void NSPMScene::activate() {
  SPDLOG_ERROR("Activate scene not implemented.");
}

void NSPMScene::save() {
  SPDLOG_ERROR("Save scene not implemented.");
}

void NSPMScene::remove() {
  SPDLOG_ERROR("Remove scene not implemented.");
}

uint16_t NSPMScene::get_id() {
  return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE NSPMScene::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::SCENE;
}

MQTT_MANAGER_ENTITY_CONTROLLER NSPMScene::get_controller() {
  return MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
}

void NSPMScene::post_init() {
  // No post_init tasks
}
