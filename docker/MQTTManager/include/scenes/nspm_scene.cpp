#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "light/light.hpp"
#include "room/room.hpp"
#include "web_helper/WebHelper.hpp"
#include <curl/curl.h>
#include <memory>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <scenes/nspm_scene.hpp>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <system_error>

NSPMScene::NSPMScene(uint32_t id) {
  this->_id = id;
  this->reload_config();
}

void NSPMScene::reload_config() {
  try {
    auto scene_config = database_manager::database.get<database_manager::Scene>(this->_id);
    this->_name = scene_config.friendly_name;
    if (scene_config.room_id == nullptr) {
      this->_is_global_scene = true;
    } else {
      this->_is_global_scene = false;
      this->_room_id = *scene_config.room_id;
    }
    SPDLOG_DEBUG("Loading NSPM scene {}::{}.", this->_id, this->_name);
  } catch (std::system_error &ex) {
    SPDLOG_DEBUG("Failed to update config for scene {}::{}.", this->_id, this->_name);
  }
}

void NSPMScene::activate() {
  SPDLOG_INFO("Activating scene {}::{}.", this->_id, this->_name);
  try {
    auto light_states = database_manager::database.get_all<database_manager::SceneLightState>(sqlite_orm::where(sqlite_orm::c(&database_manager::SceneLightState::scene_id) == this->_id));
    for (auto &light_state : light_states) {
      auto light_entity = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, light_state.light_id);
      if (light_entity != nullptr) [[likely]] {
        if (light_state.light_level == 0) {
          light_entity->turn_off(true);
          continue;
        }

        if (light_state.color_mode.compare("dimmer") == 0) {
          light_entity->turn_on(false);
          light_entity->set_color_temperature(light_state.color_temperature, false);
          light_entity->set_brightness(light_state.light_level, true);
        } else if (light_state.color_mode.compare("color") == 0) {
          light_entity->turn_on(false);
          light_entity->set_hue(light_state.hue, false);
          light_entity->set_saturation(light_state.saturation, false);
          light_entity->set_brightness(light_state.light_level, true);
        } else {
          SPDLOG_ERROR("Tried to apply light state from scene with {}::{} for light with ID {} but could not interpret color mode '{}'.", this->get_id(), this->_name, light_state.light_id, light_state.color_mode);
        }
      } else {
        SPDLOG_WARN("Tried to activate setting from scene {}::{} to light with ID {} but not such light was found.", this->_id, this->_name, light_state.light_id);
      }
    }
  } catch (std::system_error &ex) {
    SPDLOG_ERROR("Tried to activate scene {}::{} but something went wrong.", this->_id, this->_name);
  }
}

void NSPMScene::save() {
  // Update all light_states to current values.
  SPDLOG_DEBUG("Saving scene {}::{}.", this->_id, this->_name);

  // Remove any exinsting light states for this scene:
  try {
    database_manager::database.remove_all<database_manager::SceneLightState>(sqlite_orm::where(sqlite_orm::c(&database_manager::SceneLightState::scene_id) == this->_id));
  } catch (std::system_error &ex) {
    SPDLOG_ERROR("Error while deleting existing light states for scene {}::{}. Error: {}", this->_id, this->_name, ex.what());
  }

  try {
    std::vector<std::shared_ptr<Light>> lights;
    // TODO: Only add light to scene if it is "controllable from main page"
    if (this->_is_global_scene) {
      for (std::shared_ptr<Room> room : EntityManager::get_all_rooms()) {
        std::vector<std::shared_ptr<Light>> room_lights = room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
        lights.insert(lights.end(), room_lights.begin(), room_lights.end());
      }
    } else {
      auto room = EntityManager::get_room(this->_room_id);
      if (room != nullptr) [[likely]] {
        lights = room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
      } else {
        SPDLOG_ERROR("Failed to get room for scene {}::{} when saving light states.", this->_id, this->_name);
      }
    }

    // Remove lights not controlled from main page.
    lights.erase(std::remove_if(lights.begin(), lights.end(), [](std::shared_ptr<Light> light) {
                   return !light->get_controlled_from_main_page();
                 }),
                 lights.end());

    for (auto &light : lights) {
      database_manager::SceneLightState new_light_state;
      new_light_state.light_id = light->get_id();
      new_light_state.light_level = light->get_state() ? light->get_brightness() : 0;
      switch (light->get_mode()) {
      case MQTT_MANAGER_LIGHT_MODE::DEFAULT:
        new_light_state.color_mode = "dimmer";
        new_light_state.saturation = 0;
        new_light_state.hue = 0;
        new_light_state.color_temperature = light->get_color_temperature();
        database_manager::database.insert(new_light_state);
        break;
      case MQTT_MANAGER_LIGHT_MODE::RGB:
        new_light_state.color_mode = "color";
        new_light_state.saturation = light->get_saturation();
        new_light_state.hue = light->get_hue();
        new_light_state.color_temperature = 0;
        database_manager::database.insert(new_light_state);
        break;
      default:
        SPDLOG_ERROR("Unknown light mode {} of light {}::{}!", (int)light->get_mode(), light->get_id(), light->get_name());
        break;
      }
    }

  } catch (std::system_error &ex) {
    SPDLOG_ERROR("Error while saving new scene light states for scene {}::{}. Error: {}", this->_id, this->_name, ex.what());
  }
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

std::string NSPMScene::get_name() {
  return this->_name;
}

bool NSPMScene::can_save() {
  return true;
}
