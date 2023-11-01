#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "light/light.hpp"
#include <cstddef>
#include <nlohmann/json_fwd.hpp>
#include <scenes/nspm_scene.hpp>
#include <spdlog/spdlog.h>

NSPMScene::NSPMScene(nlohmann::json &data) {
  this->_id = data["id"];
  this->_name = data["scene_name"];
  if (!data["room_id"].is_null()) {
    this->_is_global_scene = false;
    this->_room_id = data["room_id"];
  } else {
    this->_is_global_scene = true;
  }
  SPDLOG_DEBUG("Loading NSPM scene {}::{}.", this->_id, this->_name);
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
  if (!this->_is_global_scene) {
    MqttManagerEntity *room_entity = EntityManager::get_entity_by_type_and_id(MQTT_MANAGER_ENTITY_TYPE::ROOM, this->_room_id);
    if (room_entity != nullptr) {
      this->_room = dynamic_cast<Room *>(room_entity);
    } else {
      SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
      return;
    }

    for (LightState state : this->_light_states) {
      Light *light = EntityManager::get_light_by_id(state.light_id);
      if (light != nullptr) {
        SPDLOG_DEBUG("Attached light {}::{} to light state attached to scene {}::{}.", light->get_id(), light->get_name(), this->_id, this->_name);
        state._light = light;
        light->attach_delete_callback(this->light_destroyed_callback);
      } else {
        SPDLOG_ERROR("Did not find any light matching a light state for scene {}::{}.", this->_id, this->_name);
      }
    }
  }
}

void NSPMScene::remove_light(Light *light) {
  auto it = this->_light_states.begin();
  while (it != this->_light_states.end()) {
    if (it->light_id == light->get_id()) {
      SPDLOG_INFO("Removing light state for light {}::{}", light->get_id(), light->get_name());
      this->_light_states.erase(it++);
    } else {
      it++;
    }
  }
}

void NSPMScene::light_destroyed_callback(Light *light) {
  std::list<MqttManagerEntity *> all_nspm_scenes = EntityManager::get_all_entities_by_type(MQTT_MANAGER_ENTITY_TYPE::SCENE);
  for (MqttManagerEntity *entity : all_nspm_scenes) {
    NSPMScene *scene = dynamic_cast<NSPMScene *>(entity);
    if (scene != nullptr) {
      scene->remove_light(light);
    }
  }
}
