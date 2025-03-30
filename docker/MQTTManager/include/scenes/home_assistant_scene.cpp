#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include <curl/curl.h>
#include <entity/entity_icons.hpp>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <scenes/home_assistant_scene.hpp>
#include <spdlog/spdlog.h>
#include <string_view>
#include <system_error>

HomeAssistantScene::HomeAssistantScene(uint32_t id) {
  this->_id = id;
  this->reload_config();
}

void HomeAssistantScene::reload_config() {
  try {
    auto scene_config = database_manager::database.get<database_manager::Scene>(this->_id);
    this->_name = scene_config.friendly_name;
    this->_entity_id = scene_config.backend_name;
    this->_page_id = scene_config.entities_page_id;
    this->_page_slot = scene_config.room_view_position;
    if (scene_config.room_id == nullptr) {
      this->_is_global_scene = true;
    } else {
      this->_is_global_scene = false;
      this->_room_id = *scene_config.room_id;
    }
    SPDLOG_DEBUG("Loaded Home Assistant scene {}::{}.", this->_id, this->_name);
  } catch (std::system_error &ex) {
    SPDLOG_ERROR("Failed to reload config for scene {}::{}.", this->_id, this->_name);
  }
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
    std::shared_ptr<Room> room_entity = EntityManager::get_room(this->_room_id);
    if (room_entity != nullptr) {
      this->_room = room_entity;
    } else {
      SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
      return;
    }
  }
}

std::string HomeAssistantScene::get_name() {
  return this->_name;
}

bool HomeAssistantScene::can_save() {
  return false;
}

std::string_view HomeAssistantScene::get_icon() {
  return EntityIcons::home_assistant_icon;
}

uint16_t HomeAssistantScene::get_icon_color() {
  return GUI_Colors::icon_color_off;
}

uint16_t HomeAssistantScene::get_icon_active_color() {
  return GUI_Colors::icon_color_off;
}
