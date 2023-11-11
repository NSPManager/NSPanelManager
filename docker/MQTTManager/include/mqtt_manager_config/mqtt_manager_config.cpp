#include "mqtt_manager_config.hpp"
#include <cstddef>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

#define ITEM_IN_LIST(list, item) (std::find(list.begin(), list.end(), item) != list.end());

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void MqttManagerConfig::load() {
  // Load sensitive variables from environment and not via http get request to manager.
  char *ha_address = std::getenv("HOME_ASSISTANT_ADDRESS");
  char *ha_token = std::getenv("HOME_ASSISTANT_TOKEN");
  char *openhab_address = std::getenv("OPENHAB_ADDRESS");
  char *openhab_token = std::getenv("OPENHAB_TOKEN");
  char *mqtt_server = std::getenv("MQTT_SERVER");
  char *mqtt_port = std::getenv("MQTT_PORT");
  char *mqtt_username = std::getenv("MQTT_USERNAME");
  char *mqtt_password = std::getenv("MQTT_PASSWORD");

  // Load Home Assistant values
  if (ha_address != nullptr) {
    MqttManagerConfig::home_assistant_address = ha_address;
    if (ha_token != nullptr) {
      MqttManagerConfig::home_assistant_access_token = ha_token;
    } else {
      SPDLOG_ERROR("Configured to use Home Assistant at address '{}' but no token was given.", ha_address);
    }
  } else {
    SPDLOG_WARN("No Home Assistant address set. Will not use Home Assistant.");
  }

  // Load OpenHAB values
  if (openhab_address != nullptr) {
    MqttManagerConfig::openhab_address = openhab_address;
    if (ha_token != nullptr) {
      MqttManagerConfig::openhab_access_token = openhab_token;
    } else {
      SPDLOG_ERROR("Configured to use OpenHAB at address '{}' but no token was given.", openhab_address);
    }
  } else {
    SPDLOG_WARN("No Home Assistant address set. Will not use OpenHAB.");
  }

  // Load MQTT values
  if (mqtt_server != nullptr) {
    MqttManagerConfig::mqtt_server = mqtt_server;

    if (mqtt_port != nullptr) {
      MqttManagerConfig::mqtt_port = atoi(mqtt_port);
    } else {
      SPDLOG_WARN("No port configured, will use default MQTT port 1883.");
      MqttManagerConfig::mqtt_port = 1883;
    }

    if (mqtt_username != nullptr) {
      MqttManagerConfig::mqtt_username = mqtt_username;
    }

    if (mqtt_password != nullptr) {
      MqttManagerConfig::mqtt_password = mqtt_password;
    }
  } else {
    SPDLOG_ERROR("No MQTT server configured!");
  }

  // Load all other non-sensitive config via HTTP GET to manager.
  CURL *curl;
  CURLcode res;

  SPDLOG_INFO("Gathering config from web manager.");
  while (true) {
    curl = curl_easy_init();
    if (curl) {
      std::string response_data;
      SPDLOG_DEBUG("Requesting config from: http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/get_mqtt_manager_config");
      curl_easy_setopt(curl, CURLOPT_URL, "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/get_mqtt_manager_config");
      /* example.com is redirected, so we tell libcurl to follow redirection */
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      long http_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      /* Check for errors */
      if (res == CURLE_OK && !response_data.empty() && http_code == 200) {
        SPDLOG_DEBUG("Got config data. Processing config.");
        nlohmann::json data = nlohmann::json::parse(response_data);
        MqttManagerConfig::populate_settings_from_config(data);
        break; // Exit loop as we have gather and processed config.
      } else {
        SPDLOG_ERROR("curl_easy_perform() failed, got code: '{}' with status code: {}. Will retry.", curl_easy_strerror(res), http_code);
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
    } else {
      SPDLOG_ERROR("Failed to curl_easy_init(). Will try again.");
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
  }
}

void MqttManagerConfig::populate_settings_from_config(nlohmann::json &data) {
  SPDLOG_INFO("Got config from web manager, will process and load values.");
  MqttManagerConfig::manager_address = data["manager_address"];
  MqttManagerConfig::color_temp_min = data["color_temp_min"];
  MqttManagerConfig::color_temp_max = data["color_temp_max"];
  MqttManagerConfig::clock_us_style = std::string(data["clock_us_style"]).compare("True") == 0;
  MqttManagerConfig::use_farenheit = std::string(data["use_farenheit"]).compare("True") == 0;
  MqttManagerConfig::max_log_buffer_size = atoi(std::string(data["max_log_buffer_size"]).c_str());

  if (!std::string(data["manager_port"]).empty()) {
    MqttManagerConfig::manager_port = atoi(std::string(data["manager_port"]).c_str());
  } else {
    SPDLOG_ERROR("Manager port not configured! Setting 0.");
    MqttManagerConfig::manager_port = 0;
  }

  std::string turn_on_behavior = data["turn_on_behavior"];
  if (turn_on_behavior.compare("restore") == 0) {
    MqttManagerConfig::turn_on_behavior = LIGHT_TURN_ON_BEHAVIOR::RESTORE;
  } else if (turn_on_behavior.compare("color_temp") == 0) {
    MqttManagerConfig::turn_on_behavior = LIGHT_TURN_ON_BEHAVIOR::COLOR_TEMP;
  } else {
    MqttManagerConfig::turn_on_behavior = LIGHT_TURN_ON_BEHAVIOR::COLOR_TEMP; // Set default when error.
    SPDLOG_ERROR("Unknown turn on behavior for lights: {}. Will use COLOR_TEMP.", turn_on_behavior);
  }

  SPDLOG_DEBUG("Loading lights...");
  std::list<nlohmann::json> json_lights;
  for (nlohmann::json light_config : data["lights"]) {
    json_lights.push_back(light_config); // Build light list for next step.
    bool already_exists = ITEM_IN_LIST(MqttManagerConfig::light_configs, light_config);
    if (!already_exists) {
      MqttManagerConfig::light_configs.push_back(light_config);
      MqttManagerConfig::_config_added_listener(&MqttManagerConfig::light_configs.back());
    }
  }

  try {
    SPDLOG_DEBUG("Checking for removed lights.");
    auto it = MqttManagerConfig::light_configs.begin();
    while (it != MqttManagerConfig::light_configs.end()) {
      bool exists = ITEM_IN_LIST(json_lights, (*it));
      if (!exists) {
        SPDLOG_DEBUG("Removing light config as it doesn't exist in config anymore.");
        MqttManagerConfig::_config_removed_listener(&(*it));
        MqttManagerConfig::light_configs.erase(it++);
      } else {
        ++it;
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Chaught exception when checking for any removed lights. Exception: {}", e.what());
  }

  SPDLOG_DEBUG("Loading NSPanels...");
  std::list<nlohmann::json> json_nspanels;
  for (nlohmann::json nspanel_config : data["nspanels"]) {
    json_nspanels.push_back(nspanel_config); // Build light list for next step.
    bool already_exists = ITEM_IN_LIST(MqttManagerConfig::nspanel_configs, nspanel_config);
    if (!already_exists) {
      MqttManagerConfig::nspanel_configs.push_back(nspanel_config);
      MqttManagerConfig::_config_added_listener(&MqttManagerConfig::nspanel_configs.back());
    }
  }

  try {
    SPDLOG_DEBUG("Checking for removed NSPanels.");
    auto nit = MqttManagerConfig::nspanel_configs.begin();
    while (nit != MqttManagerConfig::nspanel_configs.end()) {
      bool exists = ITEM_IN_LIST(json_nspanels, (*nit));
      if (!exists) {
        SPDLOG_DEBUG("Removing NSPanel config as it doesn't exist in config anymore.");
        MqttManagerConfig::_config_removed_listener(&(*nit));
        MqttManagerConfig::nspanel_configs.erase(nit++);
        delete &nit;
      } else {
        ++nit;
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Chaught exception when checking for any removed NSPanels. Exception: {}", e.what());
  }

  SPDLOG_DEBUG("Loading Scenes...");
  std::list<nlohmann::json> json_scenes;
  for (nlohmann::json scene_config : data["scenes"]) {
    json_scenes.push_back(scene_config); // Build light list for next step.
    bool already_exists = ITEM_IN_LIST(MqttManagerConfig::scenes_configs, scene_config);
    if (!already_exists) {
      MqttManagerConfig::scenes_configs.push_back(scene_config);
      MqttManagerConfig::_config_added_listener(&MqttManagerConfig::scenes_configs.back());
    }
  }

  try {
    SPDLOG_DEBUG("Checking for removed scenes.");
    auto sit = MqttManagerConfig::scenes_configs.begin();
    while (sit != MqttManagerConfig::scenes_configs.end()) {
      bool exists = ITEM_IN_LIST(json_scenes, (*sit));
      if (!exists) {
        SPDLOG_DEBUG("Removing scene config as it doesn't exist in config anymore.");
        MqttManagerConfig::_config_removed_listener(&(*sit));
        MqttManagerConfig::scenes_configs.erase(sit++);
      } else {
        ++sit;
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Chaught exception when checking for any removed scenes. Exception: {}", e.what());
  }

  SPDLOG_DEBUG("Loading Rooms...");
  std::list<nlohmann::json> json_rooms;
  for (nlohmann::json room_config : data["rooms"]) {
    json_rooms.push_back(room_config); // Build light list for next step.
    bool already_exists = ITEM_IN_LIST(MqttManagerConfig::room_configs, room_config);
    if (!already_exists) {
      MqttManagerConfig::room_configs.push_back(room_config);
      MqttManagerConfig::_config_added_listener(&MqttManagerConfig::room_configs.back());
    }
  }

  try {
    SPDLOG_DEBUG("Checking for removed rooms.");
    auto rit = MqttManagerConfig::room_configs.begin();
    while (rit != MqttManagerConfig::room_configs.end()) {
      bool exists = ITEM_IN_LIST(json_rooms, (*rit));
      if (!exists) {
        SPDLOG_DEBUG("Removing room config as it doesn't exist in config anymore.");
        MqttManagerConfig::_config_removed_listener(&(*rit));
        MqttManagerConfig::room_configs.erase(rit++);
      } else {
        ++rit;
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Chaught exception when checking for any removed rooms. Exception: {}", e.what());
  }

  SPDLOG_DEBUG("Config loaded. Calling listeners.");
  MqttManagerConfig::_config_loaded_listeners();
}

void MqttManagerConfig::attach_config_added_listener(void (*callback)(nlohmann::json *config)) {
  MqttManagerConfig::_config_added_listener.connect(callback);
}

void MqttManagerConfig::dettach_config_added_listener(void (*callback)(nlohmann::json *config)) {
  MqttManagerConfig::_config_added_listener.disconnect(callback);
}

void MqttManagerConfig::attach_config_removed_listener(void (*callback)(nlohmann::json *config)) {
  MqttManagerConfig::_config_removed_listener.connect(callback);
}

void MqttManagerConfig::dettach_config_removed_listener(void (*callback)(nlohmann::json *config)) {
  MqttManagerConfig::_config_removed_listener.disconnect(callback);
}

void MqttManagerConfig::attach_config_loaded_listener(void (*callback)()) {
  MqttManagerConfig::_config_loaded_listeners.connect(callback);
}

void MqttManagerConfig::dettach_config_loaded_listener(void (*callback)()) {
  MqttManagerConfig::_config_loaded_listeners.disconnect(callback);
}
