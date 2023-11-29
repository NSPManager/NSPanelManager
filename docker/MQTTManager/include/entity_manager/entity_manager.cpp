#include "entity/entity.hpp"
#include "light/home_assistant_light.hpp"
#include "light/openhab_light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "room/room.hpp"
#include "scenes/nspm_scene.hpp"
#include "scenes/scene.hpp"
#include "websocket_server/websocket_server.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/frame.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <cstdint>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <entity_manager/entity_manager.hpp>
#include <exception>
#include <iostream>
#include <ixwebsocket/IXWebSocket.h>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nspanel/nspanel.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/types.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void EntityManager::init() {
  // MQTT_Manager::attach_observer(EntityManager::mqtt_callback);
  WebsocketServer::attach_message_callback(EntityManager::websocket_callback);
  MqttManagerConfig::attach_config_added_listener(EntityManager::config_added);
  MqttManagerConfig::attach_config_removed_listener(EntityManager::config_removed);
  MqttManagerConfig::attach_config_loaded_listener(EntityManager::post_init_entities);
  MQTT_Manager::subscribe("nspanel/mqttmanager/command", &EntityManager::mqtt_topic_callback);
  MQTT_Manager::subscribe("nspanel/+/status", &EntityManager::mqtt_topic_callback);
  MQTT_Manager::subscribe("nspanel/+/status_report", &EntityManager::mqtt_topic_callback);
}

void EntityManager::config_added(nlohmann::json *config) {
  if ((*config).contains("type")) {
    std::string type = (*config)["type"];
    if (type.compare("light") == 0) {
      EntityManager::add_light((*config));
    } else if (type.compare("nspanel") == 0) {
      EntityManager::add_nspanel((*config));
    } else if (type.compare("scene") == 0) {
      EntityManager::add_scene((*config));
    } else if (type.compare("room") == 0) {
      EntityManager::add_room((*config));
    } else if (type.compare("nspanel_relay_group") == 0) {
      EntityManager::add_nspanel_relay_group((*config));
    } else {
      SPDLOG_ERROR("Unknown type for new config: {}", type);
    }
  } else {
    std::string config_str = config->dump();
    SPDLOG_ERROR("Config added without a type-specifier. Will ignore it! Config: {}", config_str);
  }
}

void EntityManager::config_removed(nlohmann::json *config) {
  if ((*config).contains("type")) {
    std::string type = (*config)["type"];
    if (type.compare("light") == 0) {
      std::lock_guard<std::mutex> mutex_guard(EntityManager::_lights_mutex);
      Light *entity = EntityManager::get_light_by_id((*config)["id"]);
      if (entity != nullptr) {
        EntityManager::_lights.remove(entity);
        delete entity;
      }
    } else if (type.compare("nspanel") == 0) {
      // Try to find NSPanel by ID or MAC
      NSPanel *nspanel = EntityManager::get_nspanel_by_id((*config)["id"]);
      if (nspanel == nullptr) {
        nspanel = EntityManager::get_nspanel_by_mac((*config)["mac"]);
      }

      if (nspanel != nullptr) {
        std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
        SPDLOG_INFO("Removing NSPanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        SPDLOG_DEBUG("NSPanels before remove: {}", EntityManager::_nspanels.size());
        EntityManager::_nspanels.remove(nspanel);
        SPDLOG_DEBUG("NSPanels after remove: {}", EntityManager::_nspanels.size());
        delete nspanel;
        SPDLOG_DEBUG("NSPanel deleted.");
      } else {
        SPDLOG_ERROR("Removing config for NSPanel with ID {} from ConfigManager but couldn't find an NSPanel with that ID to remove from EntityManager!", int((*config)["id"]));
      }
    } else if (type.compare("scene") == 0) {
      MqttManagerEntity *ptr = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, (*config)["id"]);
      if (ptr != nullptr) {
        EntityManager::remove_entity(ptr);
      }
    } else if (type.compare("room") == 0) {
      MqttManagerEntity *ptr = EntityManager::get_entity_by_id<Room>(MQTT_MANAGER_ENTITY_TYPE::ROOM, (*config)["id"]);
      if (ptr != nullptr) {
        EntityManager::remove_entity(ptr);
      }
    } else if (type.compare("nspanel_relay_group") == 0) {
      NSPanelRelayGroup *ptr = EntityManager::get_entity_by_id<NSPanelRelayGroup>(MQTT_MANAGER_ENTITY_TYPE::NSPANEL_RELAY_GROUP, (*config)["id"]);
      if (ptr != nullptr) {
        EntityManager::remove_entity(ptr);
      }
    } else {
      SPDLOG_ERROR("Unknown type for config: {}", type);
    }
  } else {
    std::string config_str = config->dump();
    SPDLOG_ERROR("Removing config without a type-specifier. Will ignore it! Config: {}", config_str);
  }
}

void EntityManager::attach_entity_added_listener(void (*listener)(MqttManagerEntity *)) {
  EntityManager::_entity_added_signal.connect(listener);
}

void EntityManager::detach_entity_added_listener(void (*listener)(MqttManagerEntity *)) {
  EntityManager::_entity_added_signal.disconnect(listener);
}

void EntityManager::add_room(nlohmann::json &config) {
  if (EntityManager::get_entity_by_id<Room>(MQTT_MANAGER_ENTITY_TYPE::ROOM, config["id"]) == nullptr) {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    EntityManager::_entities.push_back(new Room(config));
  } else {
    int room_id = config["id"];
    SPDLOG_ERROR("A room with ID {} already exists!", room_id);
  }
}

void EntityManager::add_light(nlohmann::json &config) {
  try {
    if (EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, config["id"]) == nullptr) {
      std::string light_type = config["light_type"];
      if (light_type.compare("home_assistant") == 0) {
        HomeAssistantLight *light = new HomeAssistantLight(config);
        EntityManager::_lights.push_back(light);
        EntityManager::add_entity(light);
      } else if (light_type.compare("openhab") == 0) {
        OpenhabLight *light = new OpenhabLight(config);
        EntityManager::_lights.push_back(light);
        EntityManager::add_entity(light);
      } else {
        SPDLOG_ERROR("Unknown light type '{}'. Will ignore entity.", light_type);
      }
    } else {
      int light_id = config["id"];
      SPDLOG_ERROR("A light with ID {} already exists.", light_id);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::add_scene(nlohmann::json &config) {
  try {
    if (EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, config["id"]) == nullptr) {
      std::string scene_type = config["scene_type"];
      if (scene_type.compare("nspm_scene") == 0) {
        Scene *scene = new NSPMScene(config);
        EntityManager::add_entity(scene);
      }
      // TODO: Implement Home Assistant and Openhab scenes.
    } else {
      int scene_id = config["id"];
      SPDLOG_ERROR("A scene with ID {} already exists.", scene_id);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::add_nspanel_relay_group(nlohmann::json &config) {
  try {
    NSPanelRelayGroup *rg = EntityManager::get_entity_by_id<NSPanelRelayGroup>(MQTT_MANAGER_ENTITY_TYPE::NSPANEL_RELAY_GROUP, config["id"]);
    if (rg == nullptr) {
      rg = new NSPanelRelayGroup(config);
      EntityManager::add_entity(rg);
    } else {
      rg->update_config(config);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::add_nspanel(nlohmann::json &config) {
  try {
    int panel_id = config["id"];
    std::string panel_mac = config["mac"];
    for (nlohmann::json config : MqttManagerConfig::nspanel_configs) {
      bool already_exists = false;
      for (NSPanel *panel : EntityManager::_nspanels) {
        if ((panel->has_registered_to_manager() && panel->get_id()) == panel_id || panel->get_mac().compare(panel_mac) == 0) {
          already_exists = true;
          break;
        }
      }
      if (!already_exists) {
        NSPanel *panel = new NSPanel(config);
        panel->update_warnings_from_manager();
        panel->send_websocket_update();
        std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
        EntityManager::_nspanels.push_back(panel);
      } else {
        SPDLOG_ERROR("A NSPanel with ID {} already exists.", panel_id);
      }
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

void EntityManager::post_init_entities() {
  SPDLOG_INFO("Performing post init on {} entities.", EntityManager::_post_init_entities.size());
  for (MqttManagerEntity *entity : EntityManager::_post_init_entities) {
    entity->post_init();
  }
  EntityManager::_post_init_entities.clear();

  SPDLOG_INFO("Total loaded lights: {}", EntityManager::_lights.size());
  SPDLOG_INFO("Total loaded NSPanels: {}", EntityManager::_nspanels.size());
  SPDLOG_INFO("Total loaded Entities: {}", EntityManager::_entities.size());
}

void EntityManager::add_entity(MqttManagerEntity *entity) {
  {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    EntityManager::_entities.push_back(entity);
    EntityManager::_post_init_entities.push_back(entity);
  }
  EntityManager::_entity_added_signal(entity);
}

void EntityManager::remove_entity(MqttManagerEntity *entity) {
  SPDLOG_DEBUG("Removing entity with ID {}.", entity->get_id());
  {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    EntityManager::_entities.remove(entity);
  }
  EntityManager::_entity_removed_signal(entity);

  std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
  delete entity;
}

void EntityManager::mqtt_topic_callback(const std::string &topic, const std::string &payload) {
  SPDLOG_DEBUG("Got message on '{}'. Message: {}", topic, payload);
  EntityManager::_process_message(topic, payload);
}

bool EntityManager::mqtt_callback(const std::string &topic, const std::string &payload) {
  SPDLOG_TRACE("Processing message on topic: {}, payload: {}", topic, payload);
  try {
    return EntityManager::_process_message(topic, payload);
  } catch (const std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing message. Exception: ", ex.what());
  } catch (...) {
    SPDLOG_ERROR("Caught unknown exception while processing message.");
  }

  return false;
}

bool EntityManager::_process_message(const std::string &topic, const std::string &payload) {
  try {
    if (topic.compare("nspanel/mqttmanager/command") == 0) {
      SPDLOG_DEBUG("Received command payload: {}", payload);
      nlohmann::json data = nlohmann::json::parse(payload);
      if (!data.contains("mac_origin")) {
        SPDLOG_ERROR("Command payload did not contain a 'mac_origin' attribute. Will cancel processing.");
        return true;
      }
      std::string mac_origin = data["mac_origin"];
      if (data.contains("method")) {
        NSPanel *panel = EntityManager::get_nspanel_by_mac(mac_origin);
        std::string command_method = data["method"];
        if (command_method.compare("set") == 0) {
          std::string command_set_attribute = data["attribute"];
          if (command_set_attribute.compare("brightness") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint8_t new_brightness = data["brightness"];
            for (uint entity_id : entity_ids) {
              Light *light = EntityManager::get_light_by_id(entity_id);
              if (light != nullptr) {
                if (new_brightness != 0) {
                  light->set_brightness(new_brightness, false);
                  light->turn_on(true);
                } else {
                  light->turn_off(true);
                }
              }
            }
          } else if (command_set_attribute.compare("kelvin") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint new_kelvin = data["kelvin"];
            for (uint entity_id : entity_ids) {
              Light *light = EntityManager::get_light_by_id(entity_id);
              if (light != nullptr) {
                light->set_color_temperature(new_kelvin, true);
              }
            }
          } else if (command_set_attribute.compare("hue") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint new_hue = data["hue"];
            for (uint entity_id : entity_ids) {
              Light *light = EntityManager::get_light_by_id(entity_id);
              if (light != nullptr) {
                light->set_hue(new_hue, true);
              }
            }
          } else if (command_set_attribute.compare("saturation") == 0) {
            std::vector<uint> entity_ids = data["entity_ids"];
            uint new_saturation = data["saturation"];
            for (uint entity_id : entity_ids) {
              Light *light = EntityManager::get_light_by_id(entity_id);
              if (light != nullptr) {
                light->set_saturation(new_saturation, true);
              }
            }
          } else {
            SPDLOG_ERROR("Unknown attribute '{}' in set-command request.", command_set_attribute);
          }
        } else {
          SPDLOG_ERROR("Unknown method. Payload: {}", payload);
        }
      } else if (data.contains("command")) {
        std::string command = data["command"];
        if (command.compare("register_request") == 0) {
          EntityManager::_handle_register_request(data);
        } else if (command.compare("activate_scene") == 0) {
          int scene_id = data["scene_id"];
          Scene *scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, scene_id);
          if (scene != nullptr) {
            scene->activate();
          } else {
            SPDLOG_ERROR("No scene with ID {} exists.", scene_id);
          }
        } else if (command.compare("save_scene") == 0) {
          int scene_id = data["scene_id"];
          Scene *scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, scene_id);
          if (scene != nullptr) {
            scene->save();
          } else {
            SPDLOG_ERROR("No scene with ID {} exists.", scene_id);
          }

        } else {
          SPDLOG_ERROR("Got command but no handler for it exists. Command: {}", command);
        }
      } else {
        SPDLOG_ERROR("Received unknown message on command topic. Message: {}", payload);
      }

      return true;
    }

  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::diagnostic_information(e, true));
  }

  return false; // Message was not processed by us, keep looking.
}

void EntityManager::_handle_register_request(const nlohmann::json &data) {
  std::string mac_address = data["mac_origin"];
  std::string name = data["friendly_name"];
  SPDLOG_INFO("Got register request from NSPanel with name {} and MAC: {}", name, mac_address);
  NSPanel *panel = EntityManager::get_nspanel_by_mac(mac_address);
  if (panel != nullptr && (panel->get_state() != MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT || (!panel->has_registered_to_manager() && panel->get_state() == MQTT_MANAGER_NSPANEL_STATE::WAITING))) {
    SPDLOG_DEBUG("Has registered to manager? {}", panel->has_registered_to_manager() ? "TRUE" : "FALSE");
    if (panel->get_state() == MQTT_MANAGER_NSPANEL_STATE::WAITING) {
      SPDLOG_DEBUG("State: WAITING");
    } else if (panel->get_state() == MQTT_MANAGER_NSPANEL_STATE::AWAITING_ACCEPT) {
      SPDLOG_DEBUG("State: AWAITING_ACCEPT");
    } else {
      SPDLOG_DEBUG("State: something else.");
    }
    std::string nspanel_command_topic = "nspanel/";
    nspanel_command_topic.append(name);
    nspanel_command_topic.append("/command");
    if (panel->register_to_manager(data)) {
      nlohmann::json response;
      response["command"] = "register_accept";
      response["address"] = MqttManagerConfig::manager_address.c_str();
      response["port"] = MqttManagerConfig::manager_port;
      MQTT_Manager::publish(nspanel_command_topic, response.dump());
    }
    panel->send_websocket_update();
  } else {
    // TODO: Create pending NSPanels
    SPDLOG_INFO("Panel is not registered to manager, adding panel but as 'pending accept' status.");
    nlohmann::json init_data = data;
    NSPanel *new_nspanel = EntityManager::get_nspanel_by_mac(data["mac_origin"]);
    if (new_nspanel == nullptr) {
      new_nspanel = new NSPanel(init_data);
      EntityManager::_nspanels.push_back(new_nspanel);
      new_nspanel->send_websocket_update();
    }
  }
}

Light *EntityManager::get_light_by_id(uint id) {
  for (Light *light : EntityManager::_lights) {
    if (light->get_id() == id) {
      return light;
    }
  }
  return nullptr;
}

NSPanel *EntityManager::get_nspanel_by_id(uint id) {
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
  for (NSPanel *nspanel : EntityManager::_nspanels) {
    if (nspanel->get_id() == id) {
      return nspanel;
    }
  }
  return nullptr;
}

NSPanel *EntityManager::get_nspanel_by_mac(std::string mac) {
  std::lock_guard<std::mutex> mutex_guard(EntityManager::_nspanels_mutex);
  for (NSPanel *nspanel : EntityManager::_nspanels) {
    if (nspanel->get_mac().compare(mac) == 0) {
      return nspanel;
    }
  }
  return nullptr;
}

bool EntityManager::websocket_callback(std::string &message, std::string *response_buffer) {
  nlohmann::json data = nlohmann::json::parse(message);
  nlohmann::json args = data["args"];

  uint64_t command_id = data["cmd_id"];
  std::string command = data["command"];

  if (command.compare("get_nspanels_status") == 0) {
    SPDLOG_DEBUG("Processing request for NSPanels status.");
    std::vector<nlohmann::json> panel_responses;
    for (NSPanel *panel : EntityManager::_nspanels) {
      if (args.contains("nspanel_id")) {
        if (panel->get_id() == atoi(std::string(args["nspanel_id"]).c_str())) {
          panel->update_warnings_from_manager();
          panel_responses.push_back(panel->get_websocket_json_representation());
          break;
        }
      } else {
        // In case no ID was specified, send status for all panels.
        panel->update_warnings_from_manager();
        panel_responses.push_back(panel->get_websocket_json_representation());
      }
    }
    nlohmann::json response;
    response["nspanels"] = panel_responses;
    response["cmd_id"] = command_id;
    (*response_buffer) = response.dump();
    return true;
  } else if (command.compare("reboot_nspanels") == 0) {
    nlohmann::json args = data["args"];
    nlohmann::json nspanels = args["nspanels"];
    for (std::string nspanel_id_str : nspanels) {
      uint16_t nspanel_id = atoi(nspanel_id_str.c_str());
      NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
      if (nspanel != nullptr) {
        SPDLOG_INFO("Sending reboot command to nspanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        nlohmann::json cmd;
        cmd["command"] = "reboot";
        nspanel->send_command(cmd);
      } else {
        SPDLOG_ERROR("Received command to reboot NSPanel with ID {} but no panel with that ID is loaded.");
      }
    }
    return true;
  } else if (command.compare("firmware_update_nspanels") == 0) {
    nlohmann::json args = data["args"];
    nlohmann::json nspanels = args["nspanels"];
    for (std::string nspanel_id_str : nspanels) {
      uint16_t nspanel_id = atoi(nspanel_id_str.c_str());
      NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
      if (nspanel != nullptr) {
        SPDLOG_INFO("Sending firmware update command to nspanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        nlohmann::json cmd;
        cmd["command"] = "firmware_update";
        nspanel->send_command(cmd);
      } else {
        SPDLOG_ERROR("Received command to firmware update NSPanel with ID {} but no panel with that ID is loaded.");
      }
    }
    return true;
  } else if (command.compare("tft_update_nspanels") == 0) {
    nlohmann::json args = data["args"];
    nlohmann::json nspanels = args["nspanels"];
    for (std::string nspanel_id_str : nspanels) {
      uint16_t nspanel_id = atoi(nspanel_id_str.c_str());
      NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
      if (nspanel != nullptr) {
        SPDLOG_INFO("Sending TFT update command to nspanel {}::{}.", nspanel->get_id(), nspanel->get_name());
        nlohmann::json cmd;
        cmd["command"] = "tft_update";
        nspanel->send_command(cmd);
      } else {
        SPDLOG_ERROR("Received command to TFT update NSPanel with ID {} but no panel with that ID is loaded.");
      }
    }
    return true;
  } else if (command.compare("get_nspanel_logs") == 0) {
    SPDLOG_DEBUG("Processing request for NSPanel logs.");
    nlohmann::json args = data["args"];
    uint16_t nspanel_id = atoi(std::string(args["nspanel_id"]).c_str());
    NSPanel *nspanel = EntityManager::get_nspanel_by_id(nspanel_id);
    if (nspanel != nullptr) {
      nlohmann::json response = nspanel->get_websocket_json_logs();
      response["cmd_id"] = command_id;
      (*response_buffer) = response.dump();
    } else {
      SPDLOG_ERROR("Received request for logs from NSPanel with ID {} but no panel with that ID is loaded.");
    }
    return true;
  } else if (command.compare("nspanel_accept") == 0) {
    nlohmann::json args = data["args"];
    std::string mac = args["mac_address"];
    NSPanel *panel = EntityManager::get_nspanel_by_mac(mac);
    if (panel != nullptr) {
      SPDLOG_INFO("Accepting reqister request for NSPanel with MAC {} as per user request from websocket.", mac);
      panel->accept_register_request();
      nlohmann::json response;
      response["cmd_id"] = command_id;
      response["success"] = true;
      response["mac"] = mac;
      (*response_buffer) = response.dump();
      panel->send_websocket_update();
      return true;
    } else {
      SPDLOG_DEBUG("Received NSPanel accept request for a panel we could not find. Ignoring request.");
    }
  } else if (command.compare("nspanel_delete") == 0) {
    nlohmann::json args = data["args"];
    std::string mac = args["mac_address"];
    NSPanel *panel = EntityManager::get_nspanel_by_mac(mac);
    if (panel != nullptr) {
      CURL *curl;
      CURLcode res;
      curl = curl_easy_init();
      if (curl) {
        std::string response_data;
        SPDLOG_DEBUG("Requesting config from: http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/delete_nspanel/{}", panel->get_id());
        curl_easy_setopt(curl, CURLOPT_URL, fmt::format("http://" MANAGER_ADDRESS ":" MANAGER_PORT "/api/delete_nspanel/{}", panel->get_id()).c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        long http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        /* Check for errors */
        if (res == CURLE_OK && !response_data.empty() && http_code == 200) {
          nlohmann::json response;
          response["cmd_id"] = command_id;
          response["success"] = true;
          response["mac"] = mac;
          (*response_buffer) = response.dump();
          curl_easy_cleanup(curl);
          SPDLOG_DEBUG("Panel with MAC {} delete call completed.", mac);
          return true;
        } else {
          SPDLOG_ERROR("curl_easy_perform() failed, got code: '{}' with status code: {}. Will retry.", curl_easy_strerror(res), http_code);
          curl_easy_cleanup(curl);
        }
      } else {
        SPDLOG_ERROR("Failed to curl_easy_init().");
      }
    } else {
      SPDLOG_ERROR("Received request to delete NSPanel but no NSPanel with MAC {} is register to this manager.", mac);
    }
  }

  return false;
}
