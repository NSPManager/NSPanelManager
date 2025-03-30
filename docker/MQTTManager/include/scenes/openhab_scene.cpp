#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "web_helper/WebHelper.hpp"
#include <curl/curl.h>
#include <entity/entity_icons.hpp>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <openhab_manager/openhab_manager.hpp>
#include <scenes/openhab_scene.hpp>
#include <spdlog/spdlog.h>
#include <system_error>

OpenhabScene::OpenhabScene(uint32_t id) {
  this->_id = id;
  this->reload_config();
}

void OpenhabScene::reload_config() {
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
    SPDLOG_DEBUG("Loaded OpenHAB scene {}::{}.", this->_id, this->_name);
  } catch (std::system_error &ex) {
    SPDLOG_ERROR("Failed to update config for scene {}::{}.", this->_id, this->_name);
  }
}

void OpenhabScene::activate() {
  SPDLOG_INFO("Activating scene {}::{}.", this->_id, this->_name);

  std::string openhab_trigger_scene_url = fmt::format("{}/rest/rules/{}/runnow", MqttManagerConfig::get_setting_with_default("openhab_address", ""), this->_entity_id);
  std::list<const char *> headers = {
      fmt::format("Authorization: Bearer {}", MqttManagerConfig::get_setting_with_default("openhab_token", "")).c_str(),
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
  return MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB;
}

std::string OpenhabScene::get_name() {
  return this->_name;
}

bool OpenhabScene::can_save() {
  return false;
}

std::string_view OpenhabScene::get_icon() {
  return EntityIcons::openhab_icon;
}

uint16_t OpenhabScene::get_icon_color() {
  return GUI_Colors::icon_color_off;
}

uint16_t OpenhabScene::get_icon_active_color() {
  return GUI_Colors::icon_color_off;
}
