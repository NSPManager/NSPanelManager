#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include <curl/curl.h>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <scenes/home_assistant_scene.hpp>
#include <spdlog/spdlog.h>

HomeAssistantScene::HomeAssistantScene(nlohmann::json &data) {
  this->_id = data["id"];
  this->update_config(data);
}

void HomeAssistantScene::update_config(nlohmann::json &data) {
  this->_name = data["scene_name"];
  if (!data["room_id"].is_null()) {
    this->_is_global_scene = false;
    this->_room_id = data["room_id"];
  } else {
    this->_is_global_scene = true;
  }
  this->_entity_id = data["entity_name"];
  SPDLOG_DEBUG("Loaded Home Assistant scene {}::{}.", this->_id, this->_name);
}

void HomeAssistantScene::activate() {
  SPDLOG_INFO("Activating scene {}::{}.", this->_id, this->_name);
  nlohmann::json service_data;
  service_data["type"] = "call_service";
  service_data["domain"] = "scene";
  service_data["service"] = "turn_on";
  service_data["target"]["entity_id"] = this->_entity_id;
  HomeAssistantManager::send_json(service_data);
}

void HomeAssistantScene::save() {
  SPDLOG_ERROR("Save is not a possible action for Home Assistant scenes.");
}

void HomeAssistantScene::remove() {
}

uint16_t HomeAssistantScene::get_id() {
  return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE HomeAssistantScene::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::SCENE;
}

MQTT_MANAGER_ENTITY_CONTROLLER HomeAssistantScene::get_controller() {
  return MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
}

void HomeAssistantScene::post_init() {
  if (!this->_is_global_scene) {
    Room *room_entity = EntityManager::get_entity_by_id<Room>(MQTT_MANAGER_ENTITY_TYPE::ROOM, this->_room_id);
    if (room_entity != nullptr) {
      this->_room = room_entity;
    } else {
      SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
      return;
    }
  }
}
