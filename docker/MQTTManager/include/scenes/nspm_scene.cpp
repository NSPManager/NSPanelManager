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

NSPMScene::NSPMScene(nlohmann::json &data) {
  this->_id = data["scene_id"];
  this->update_config(data);
}

void NSPMScene::update_config(nlohmann::json &data) {
  this->_name = data["scene_name"];
  if (!data["room_id"].is_null()) {
    this->_is_global_scene = false;
    this->_room_id = data["room_id"];
  } else {
    this->_is_global_scene = true;
  }
  SPDLOG_DEBUG("Loading NSPM scene {}::{}.", this->_id, this->_name);
  this->_light_states.clear();
  for (nlohmann::json light_data : data["light_states"]) {
    LightState state;
    state._light = nullptr;
    state.light_id = light_data["light_id"];
    state.color_mode = light_data["color_mode"];
    state.brightness = light_data["light_level"];
    state.color_temperature = light_data["color_temp"];
    state.hue = light_data["hue"];
    state.saturation = light_data["saturation"];
    this->_light_states.push_back(state);
  }
  SPDLOG_DEBUG("Loaded scene {}::{}.", this->_id, this->_name);
}

void NSPMScene::activate() {
  SPDLOG_INFO("Activating scene {}::{}.", this->_id, this->_name);
  for (LightState state : this->_light_states) {
    if (state._light != nullptr) {
      SPDLOG_DEBUG("Activating light state for light {}::{}.", state._light->get_id(), state._light->get_name());
      if (state.brightness == 0) {
        state._light->turn_off(true);
        continue;
      }

      if (state.color_mode.compare("dimmer") == 0) {
        state._light->turn_on(false);
        state._light->set_color_temperature(state.color_temperature, false);
        state._light->set_brightness(state.brightness, true);
      } else if (state.color_mode.compare("color") == 0) {
        state._light->turn_on(false);
        state._light->set_hue(state.hue, false);
        state._light->set_saturation(state.saturation, false);
        state._light->set_brightness(state.brightness, true);
      } else {
        SPDLOG_ERROR("Trying to apply light state from scene with {}::{} for light with ID {} but could not interpret color mode '{}'.", this->get_id(), this->_name, state.light_id, state.color_mode);
      }
    } else {
      SPDLOG_ERROR("Trying to apply light state from scene with ID {}::{} for light with ID {} but could not find such a light.", this->get_id(), this->_name, state.light_id);
    }
  }
}

void NSPMScene::save() {
  // Update all light_states to current values.
  SPDLOG_DEBUG("Saving scene {}::{}.", this->_id, this->_name);
  this->_light_states.clear(); // Clear current light states
  std::list<nlohmann::json> json_light_states;
  std::vector<std::shared_ptr<Light>> lights;
  // TODO: Only add light to scene if it is "controllable from main page"
  if (this->_is_global_scene) {
    for(std::shared_ptr<Room> room : EntityManager::get_all_rooms()) {
        std::vector<std::shared_ptr<Light>> room_lights = room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
        lights.insert(lights.end(), room_lights.begin(), room_lights.end());
    }
  } else {
    lights = this->_room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT);
  }
  for (std::shared_ptr<Light> light : lights) {
    SPDLOG_TRACE("Saving light {}::{}", light->get_id(), light->get_name());
    nlohmann::json light_state_json;
    LightState new_light_state;
    switch (light->get_mode()) {
    case MQTT_MANAGER_LIGHT_MODE::DEFAULT:
      light_state_json["mode"] = "dimmer";
      light_state_json["saturation"] = 0;
      light_state_json["hue"] = 0;
      light_state_json["color_temp"] = light->get_color_temperature();

      new_light_state.color_mode = "dimmer";
      new_light_state.saturation = 0;
      new_light_state.hue = 0;
      new_light_state.color_temperature = light->get_color_temperature();
      break;
    case MQTT_MANAGER_LIGHT_MODE::RGB:
      light_state_json["mode"] = "color";
      light_state_json["saturation"] = light->get_saturation();
      light_state_json["hue"] = light->get_hue();
      light_state_json["color_temp"] = 0;

      new_light_state.color_mode = "color";
      new_light_state.saturation = light->get_saturation();
      new_light_state.hue = light->get_hue();
      new_light_state.color_temperature = 0;
      break;
    default:
      SPDLOG_ERROR("Unknown light mode!");
      break;
    }
    light_state_json["light_id"] = light->get_id();
    light_state_json["level"] = light->get_state() ? light->get_brightness() : 0;
    json_light_states.push_back(light_state_json);

    new_light_state.light_id = light->get_id();
    new_light_state._light = light;
    new_light_state.brightness = light->get_state() ? light->get_brightness() : 0;
    this->_light_states.push_back(new_light_state);
  }

  // Update all light_states for lights.
  nlohmann::json save_scene_data;
  save_scene_data["scene_id"] = this->get_id();
  save_scene_data["light_states"] = json_light_states;
  std::string json_payload = save_scene_data.dump();

  std::string url = "http://127.0.0.1:8000/api/save_scene";
  if (WebHelper::perform_post_request(&url, nullptr, nullptr, &json_payload)) {
    SPDLOG_INFO("Saved scene {}::{}.", this->_id, this->_name);
  } else {
    SPDLOG_INFO("Failed to save scene {}::{}.", this->_id, this->_name);
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

void NSPMScene::post_init() {
  if (!this->_is_global_scene) {
    std::shared_ptr<Room> room_entity = EntityManager::get_room(this->_room_id);
    if (room_entity != nullptr) {
      this->_room = room_entity;
    } else {
      SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
      return;
    }

    for (LightState &state : this->_light_states) {
        bool found_light = false;
        for(std::shared_ptr<Room> room : EntityManager::get_all_rooms()) {
            for(std::shared_ptr<Light> light : room->get_all_entities_by_type<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT)) {
                if(light->get_id() == state.light_id) {
                    SPDLOG_DEBUG("Attaching light {}::{} to light state attached to scene {}::{}.", light->get_id(), light->get_name(), this->_id, this->_name);
                    state._light = light;
                    light->attach_delete_callback(this->light_destroyed_callback);
                    found_light = true;
                    break;
                }
            }
            if(found_light) {
                break;
            }
        }
        if(!found_light) {
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
  std::vector<std::shared_ptr<Scene>> all_scene_entities = EntityManager::get_all_entities_by_type<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE);
  for (auto entity : all_scene_entities) {
      if(entity->get_controller() == MQTT_MANAGER_ENTITY_CONTROLLER::NSPM) {
          std::shared_ptr<NSPMScene> scene = std::static_pointer_cast<NSPMScene>(entity);
          scene->remove_light(light);
      }
  }
}

std::string NSPMScene::get_name() {
  return this->_name;
}

bool NSPMScene::can_save() {
  return true;
}
