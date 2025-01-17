#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "web_helper/WebHelper.hpp"
#include <curl/curl.h>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <openhab_manager/openhab_manager.hpp>
#include <scenes/openhab_scene.hpp>
#include <spdlog/spdlog.h>

OpenhabScene::OpenhabScene(nlohmann::json &data) {
  this->_id = data["scene_id"];
  this->update_config(data);
}

void OpenhabScene::update_config(nlohmann::json &data) {
  this->_name = data["scene_name"];
  if (!data["room_id"].is_null()) {
    this->_is_global_scene = false;
    this->_room_id = data["room_id"];
  } else {
    this->_is_global_scene = true;
  }
  this->_entity_id = data["entity_name"];
  SPDLOG_DEBUG("Loaded OpenHAB scene {}::{}.", this->_id, this->_name);
}

void OpenhabScene::activate() {
  SPDLOG_INFO("Activating scene {}::{}.", this->_id, this->_name);

  std::string openhab_trigger_scene_url = fmt::format("{}/rest/rules/{}/runnow", MqttManagerConfig::get_private_settings().openhab_address(), this->_entity_id);
  std::list<const char *> headers = {
      fmt::format("Authorization: Bearer {}", MqttManagerConfig::get_private_settings().openhab_token()).c_str(),
      "Content-type: application/json"};
  std::string response_data;
  std::string post_data = "";
  SPDLOG_DEBUG("Triggering OpenHAB scene {}::{} via POST request to {}", this->_id, this->_name, openhab_trigger_scene_url);
  if (WebHelper::perform_post_request(&openhab_trigger_scene_url, &response_data, &headers, &post_data)) {
    SPDLOG_DEBUG("Successfully activated OpenHAB scene {}::{}", this->_id, this->_name);
  } else {
    SPDLOG_ERROR("Failed to activate scene.");
  }
}

void OpenhabScene::save() {
  SPDLOG_ERROR("Save is not a possible action for OpenHAB scenes.");
}

void OpenhabScene::remove() {
}

uint16_t OpenhabScene::get_id() {
  return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE OpenhabScene::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::SCENE;
}

MQTT_MANAGER_ENTITY_CONTROLLER OpenhabScene::get_controller() {
  return MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
}

void OpenhabScene::post_init() {
  if (!this->_is_global_scene) {
    std::shared_ptr<Room> room_entity = EntityManager::get_room(this->_room_id);
    if (room_entity != nullptr) {
      this->_room = room_entity;
    } else {
      SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
      return;
    }
  }
}

std::string OpenhabScene::get_name() {
  return this->_name;
}

bool OpenhabScene::can_save() {
  return false;
}
