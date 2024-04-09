#include "mqtt_manager_config.hpp"
#include <cstddef>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <fstream>
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
  // Begin by loading timezone
  std::ifstream f("/etc/timezone", std::ios::in | std::ios::binary);
  const size_t file_size = std::filesystem::file_size("/etc/timezone");
  std::string timezone_str(file_size, '\0');
  f.read(timezone_str.data(), file_size);
  f.close();

  timezone_str.erase(std::find_if(timezone_str.rbegin(), timezone_str.rend(), [](unsigned char ch) {
                       return !std::isspace(ch) && ch != '\n' && ch != '\r';
                     }).base(),
                     timezone_str.end());
  MqttManagerConfig::timezone = timezone_str;

  SPDLOG_INFO("Read timezone {} from /etc/timezone.", timezone_str);

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
  MqttManagerConfig::date_format = std::string(data["date_format"]);
  MqttManagerConfig::outside_temp_sensor_provider = std::string(data["outside_temp_sensor_provider"]);
  MqttManagerConfig::outside_temp_sensor_entity_id = std::string(data["outside_temp_sensor_entity_id"]);
  MqttManagerConfig::weather_location_latitude = std::string(data["weather_location_latitude"]);
  MqttManagerConfig::weather_location_longitude = std::string(data["weather_location_longitude"]);
  MqttManagerConfig::weather_wind_speed_format = std::string(data["weather_wind_speed_format"]);
  MqttManagerConfig::weather_precipitation_format = std::string(data["weather_precipitation_format"]);
  MqttManagerConfig::icon_mapping = data["icon_mapping"];
  MqttManagerConfig::clock_us_style = data["clock_us_style"];
  MqttManagerConfig::use_farenheit = data["use_farenheit"];
  MqttManagerConfig::is_home_assistant_addon = data["is_home_assistant_addon"];
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
  MqttManagerConfig::light_configs.clear();
  for (nlohmann::json light_config : data["lights"]) {
    MqttManagerConfig::light_configs.push_back(light_config);
  }

  SPDLOG_DEBUG("Loading NSPanels...");
  MqttManagerConfig::nspanel_configs.clear();
  for (nlohmann::json nspanel_config : data["nspanels"]) {
    MqttManagerConfig::nspanel_configs.push_back(nspanel_config);
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

  SPDLOG_DEBUG("Loading relay groups...");
  std::list<nlohmann::json> json_rgs;
  for (nlohmann::json rg_config : data["nspanel_relay_groups"]) {
    json_rgs.push_back(rg_config); // Build light list for next step.
    bool already_exists = ITEM_IN_LIST(MqttManagerConfig::nspanel_relay_group_configs, rg_config);
    if (!already_exists) {
      MqttManagerConfig::nspanel_relay_group_configs.push_back(rg_config);
      MqttManagerConfig::_config_added_listener(&MqttManagerConfig::nspanel_relay_group_configs.back());
    }
  }

  try {
    SPDLOG_DEBUG("Checking for removed relay groups.");
    auto rit = MqttManagerConfig::nspanel_relay_group_configs.begin();
    while (rit != MqttManagerConfig::nspanel_relay_group_configs.end()) {
      bool exists = ITEM_IN_LIST(json_rgs, (*rit));
      if (!exists) {
        SPDLOG_DEBUG("Removing scene config as it doesn't exist in config anymore.");
        MqttManagerConfig::_config_removed_listener(&(*rit));
        MqttManagerConfig::nspanel_relay_group_configs.erase(rit++);
      } else {
        ++rit;
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Chaught exception when checking for any removed NSPanel relay groups. Exception: {}", e.what());
  }

  SPDLOG_DEBUG("Config loaded. Calling listeners.");
  MqttManagerConfig::_config_loaded_listeners();
}
