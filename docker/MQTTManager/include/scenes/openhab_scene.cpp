#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <curl/curl.h>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <openhab_manager/openhab_manager.hpp>
#include <scenes/openhab_scene.hpp>
#include <spdlog/spdlog.h>

OpenhabScene::OpenhabScene(nlohmann::json &data) {
  this->_id = data["id"];
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

  try {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
      std::string bearer_token = "Authorization: Bearer ";
      bearer_token.append(MqttManagerConfig::openhab_access_token);

      struct curl_slist *headers = NULL;
      headers = curl_slist_append(headers, bearer_token.c_str());
      if (headers == NULL) {
        SPDLOG_ERROR("Failed to set bearer token header for OpenHAB light rest request.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
      }
      headers = curl_slist_append(headers, "Content-type: application/json");
      if (headers == NULL) {
        SPDLOG_ERROR("Failed to set content-type header for OpenHAB light rest request.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
      }

      std::string openhab_trigger_scene_url = MqttManagerConfig::openhab_address;
      openhab_trigger_scene_url.append("/rest/rules/");
      openhab_trigger_scene_url.append(this->_entity_id);
      openhab_trigger_scene_url.append("/runnow");
      curl_easy_setopt(curl, CURLOPT_URL, openhab_trigger_scene_url.c_str());
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

      SPDLOG_DEBUG("Triggering OpenHAB scene {}::{} via POST request to {}", this->_id, this->_name, openhab_trigger_scene_url);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      long http_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      /* Check for errors */
      if (res == CURLE_OK && http_code == 200) {
        SPDLOG_DEBUG("Successfully activated OpenHAB scene {}::{}", this->_id, this->_name);
      } else {
        SPDLOG_ERROR("curl_easy_perform() when triggering OpenHAB scene failed, got code: {}.", curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
    }
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to register NSPanel: {}", boost::diagnostic_information(e, true));
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
    Room *room_entity = EntityManager::get_entity_by_id<Room>(MQTT_MANAGER_ENTITY_TYPE::ROOM, this->_room_id);
    if (room_entity != nullptr) {
      this->_room = room_entity;
    } else {
      SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
      return;
    }
  }
}
