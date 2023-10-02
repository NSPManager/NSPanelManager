#include "mqtt_manager_config.hpp"
#include <cstddef>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

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
      /* Check for errors */
      if (res == CURLE_OK) {
        SPDLOG_DEBUG("Got config data. Processing config.");
        nlohmann::json data = nlohmann::json::parse(response_data);
        MqttManagerConfig::populate_settings_from_config(data);
        break; // Exit loop as we have gather and processed config.
      } else {
        SPDLOG_ERROR("curl_easy_perform() failed, got code: {}. Will retry.", curl_easy_strerror(res));
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
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
  MqttManagerConfig::color_temp_min = data["color_temp_min"];
  MqttManagerConfig::color_temp_max = data["color_temp_max"];
  MqttManagerConfig::clock_us_style = std::string(data["clock_us_style"]).compare("True") == 0;
  MqttManagerConfig::use_farenheit = std::string(data["use_farenheit"]).compare("True") == 0;
  MqttManagerConfig::max_log_buffer_size = atoi(std::string(data["max_log_buffer_size"]).c_str());

  std::string turn_on_behavior = data["turn_on_behavior"];
  if (turn_on_behavior.compare("restore") == 0) {
    MqttManagerConfig::turn_on_behavior = LIGHT_TURN_ON_BEHAVIOR::RESTORE;
  } else if (turn_on_behavior.compare("color_temp") == 0) {
    MqttManagerConfig::turn_on_behavior = LIGHT_TURN_ON_BEHAVIOR::COLOR_TEMP;
  } else {
    MqttManagerConfig::turn_on_behavior = LIGHT_TURN_ON_BEHAVIOR::COLOR_TEMP; // Set default when error.
    SPDLOG_ERROR("Unknown turn on behavior for lights: {}. Will use COLOR_TEMP.", turn_on_behavior);
  }

  for (nlohmann::json light_config : data["lights"]) {
    MqttManagerConfig::light_configs.push_back(light_config);
  }
  for (nlohmann::json nspanel_config : data["nspanels"]) {
    MqttManagerConfig::nspanel_configs.push_back(nspanel_config);
  }
  for (nlohmann::json scene_config : data["scenes"]) {
    MqttManagerConfig::scenes_configs.push_back(scene_config);
  }
  for (nlohmann::json room_config : data["rooms"]) {
    MqttManagerConfig::room_configs.push_back(room_config);
  }

  SPDLOG_DEBUG("Config loaded.");
}
