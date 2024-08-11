#include "mqtt_manager_config.hpp"
#include "web_helper/WebHelper.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <fmt/core.h>
#include <fstream>
#include <mutex>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <nlohmann/detail/exceptions.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/protobuf_mqttmanager.pb.h>
#include <protobuf_general.pb.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

#define ITEM_IN_LIST(list, item) (std::find(list.begin(), list.end(), item) != list.end());

MQTTManagerSettings MqttManagerConfig::get_settings() {
  return MqttManagerConfig::_settings;
}

MQTTManagerPrivateSettings MqttManagerConfig::get_private_settings() {
  return MqttManagerConfig::_private_settings;
}

void MqttManagerConfig::load() {
  {
    std::lock_guard<std::mutex> mutex_guard(MqttManagerConfig::_config_load_mutex);

    SPDLOG_TRACE("Loading timezone from /etc/timezone.");
    // Begin by loading timezone
    std::ifstream f("/etc/timezone", std::ios::in | std::ios::binary);
    const size_t file_size = std::filesystem::file_size("/etc/timezone");
    std::string timezone_str(file_size, '\0');
    f.read(timezone_str.data(), file_size);
    f.close();
    boost::algorithm::trim(timezone_str);
    MqttManagerConfig::timezone = timezone_str;
    SPDLOG_INFO("Read timezone {} from /etc/timezone.", timezone_str);

    // Load sensitive variables from environment and not via http get request to manager.
    SPDLOG_TRACE("Loading sensitive settings from environment variables.");
    const char *settings = std::getenv("SETTINGS");
    MqttManagerConfig::_private_settings.ParseFromString(settings);

    // Load icon mapping
    std::ifstream icon_mapping_stream("/usr/src/app/nspanelmanager/icon_mapping.json");
    MqttManagerConfig::icon_mapping = nlohmann::json::parse(icon_mapping_stream);
    icon_mapping_stream.close();

    // Load all other non-sensitive config via HTTP GET to manager.
    // TODO: Load via protobuf instead and simplify the checking of removed entities.
    CURL *curl;
    CURLcode res;

    SPDLOG_INFO("Gathering config from web manager.");
    while (true) {
      std::string url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/protobuf/mqttmanager/all_settings";
      std::string response_data;
      if (WebHelper::perform_post_request(&url, &response_data, nullptr, nullptr)) {
        SPDLOG_DEBUG("Got config data. Processing config.");
        MqttManagerConfig::_settings.ParseFromString(response_data);
        if (MqttManagerConfig::_settings.manager_address().empty()) {
          SPDLOG_CRITICAL("No manager address is configured!");
        }
        if (MqttManagerConfig::_settings.manager_port() == 0) {
          SPDLOG_CRITICAL("No manager port is configured!");
        }
        break; // We successfully gather settings from DB. Exit loop.
      } else {
        SPDLOG_ERROR("Failed to get config. Will try again.");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
    }

    SPDLOG_DEBUG("Loading NSPanels...");
    std::string nspanels_url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/protobuf/mqttmanager/all_nspanels";
    std::string nspanels_string;
    if (WebHelper::perform_get_request(&nspanels_url, &nspanels_string, nullptr)) {
      MqttManagerConfig::nspanel_configs.clear();
      MultipleNSPanelsSettings panels_settings;
      panels_settings.ParseFromString(nspanels_string);
      for (NSPanelSettings nspanel_config : panels_settings.nspanels()) {
        MqttManagerConfig::nspanel_configs.push_back(nspanel_config);
      }
    } else {
      SPDLOG_ERROR("Failed to get NSPanel configs!");
    }
  }

  // Notify all listeners that the config has been loaded
  MqttManagerConfig::_config_loaded_listeners();
}

void MqttManagerConfig::populate_settings_from_config(nlohmann::json &data) {
  try {
    SPDLOG_INFO("Got config from web manager, will process and load values.");
    SPDLOG_DEBUG("Loading lights...");
    std::string lights_url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/rest/lights";
    std::string lights_string;
    if (WebHelper::perform_get_request(&lights_url, &lights_string, nullptr)) {
      SPDLOG_TRACE("Got lights config: {}", lights_string);
      nlohmann::json lights_json = nlohmann::json::parse(lights_string);
      MqttManagerConfig::light_configs.clear();
      for (nlohmann::json light_config : lights_json.at("lights")) {
        MqttManagerConfig::light_configs.push_back(light_config);
      }
    } else {
      SPDLOG_ERROR("Failed to get lights config!");
    }

    SPDLOG_DEBUG("Loading Scenes...");
    std::list<nlohmann::json> json_scenes;
    std::string scenes_url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/rest/scenes";
    std::string scenes_string;
    if (WebHelper::perform_get_request(&scenes_url, &scenes_string, nullptr)) {
      SPDLOG_TRACE("Got Scene configs: {}", scenes_string);
      nlohmann::json scenes_json = nlohmann::json::parse(scenes_string);
      for (nlohmann::json scene_config : scenes_json.at("scenes")) {
        json_scenes.push_back(scene_config); // Build light list for next step.
        bool already_exists = ITEM_IN_LIST(MqttManagerConfig::scenes_configs, scene_config);
        if (!already_exists) {
          MqttManagerConfig::scenes_configs.push_back(scene_config);
          MqttManagerConfig::_config_added_listener(&MqttManagerConfig::scenes_configs.back());
        }
      }
    } else {
      SPDLOG_ERROR("Failed to get Scene configs!");
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
    std::string rooms_url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/rest/rooms";
    std::string rooms_string;
    if (WebHelper::perform_get_request(&rooms_url, &rooms_string, nullptr)) {
      nlohmann::json rooms_json = nlohmann::json::parse(rooms_string);
      SPDLOG_TRACE("Got Room configs: {}", rooms_string);
      for (nlohmann::json room_config : rooms_json.at("rooms")) {
        json_rooms.push_back(room_config); // Build light list for next step.
        bool already_exists = ITEM_IN_LIST(MqttManagerConfig::room_configs, room_config);
        if (!already_exists) {
          MqttManagerConfig::room_configs.push_back(room_config);
          MqttManagerConfig::_config_added_listener(&MqttManagerConfig::room_configs.back());
        }
      }
    } else {
      SPDLOG_ERROR("Failed to get room configs!");
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

    std::list<nlohmann::json> json_rgs;
    try {
      SPDLOG_DEBUG("Loading relay groups...");
      std::string relay_groups_url = "http://" MANAGER_ADDRESS ":" MANAGER_PORT "/rest/relay_groups";
      std::string relay_groups_string;
      if (WebHelper::perform_get_request(&relay_groups_url, &relay_groups_string, nullptr)) {
        nlohmann::json relay_groups_json = nlohmann::json::parse(relay_groups_string);
        SPDLOG_TRACE("Got Relay Group configs: {}", rooms_string);
        for (nlohmann::json rg_config : relay_groups_json.at("relay_groups")) {
          json_rgs.push_back(rg_config); // Build light list for next step.
          bool already_exists = ITEM_IN_LIST(MqttManagerConfig::nspanel_relay_group_configs, rg_config);
          if (!already_exists) {
            MqttManagerConfig::nspanel_relay_group_configs.push_back(rg_config);
            MqttManagerConfig::_config_added_listener(&MqttManagerConfig::nspanel_relay_group_configs.back());
          }
        }
      } else {
        SPDLOG_ERROR("Failed to get Relay Group configs!");
      }
    } catch (std::exception &e) {
      SPDLOG_ERROR("Chaught exception when loading relay groups. Exception: {}", e.what());
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
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to process config Django: {}", boost::diagnostic_information(e, true));
    exit(1);
  }
}
