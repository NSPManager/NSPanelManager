#include "entity/entity.hpp"
#include "light/home_assistant_light.hpp"
#include "light/openhab_light.hpp"
#include "room/room.hpp"
#include "scenes/nspm_scene.hpp"
#include "scenes/scene.hpp"
#include <cstdint>
#include <entity_manager/entity_manager.hpp>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nspanel/nspanel.hpp>
#include <spdlog/spdlog.h>
#include <sys/types.h>

void EntityManager::init() {
  MQTT_Manager::attach_observer(EntityManager::mqtt_callback);
}

void EntityManager::init_entities() {
  SPDLOG_INFO("Initializing {} Rooms.", MqttManagerConfig::room_configs.size());
  for (nlohmann::json config : MqttManagerConfig::room_configs) {
    EntityManager::_entities.push_back(new Room(config));
  }

  SPDLOG_INFO("Initializing {} lights.", MqttManagerConfig::light_configs.size());
  for (nlohmann::json &config : MqttManagerConfig::light_configs) {
    std::string light_type = config["type"];
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
  }

  SPDLOG_INFO("Initializing {} scenes.", MqttManagerConfig::scenes_configs.size());
  for (nlohmann::json &config : MqttManagerConfig::scenes_configs) {
    std::string scene_type = config["type"];
    if (scene_type.compare("nspm_scene") == 0) {
      Scene *scene = new NSPMScene(config);
      EntityManager::add_entity(scene);
    }
    // TODO: Implement Home Assistant and Openhab scenes.
  }

  SPDLOG_INFO("Initializing {} NSPanels.", MqttManagerConfig::nspanel_configs.size());
  for (nlohmann::json config : MqttManagerConfig::nspanel_configs) {
    EntityManager::_nspanels.push_back(new NSPanel(config));
  }

  SPDLOG_INFO("Performing post init on {} entities.", EntityManager::_entities.size());
  for (MqttManagerEntity *entity : EntityManager::_entities) {
    entity->post_init();
  }
}

void EntityManager::add_entity(MqttManagerEntity *entity) {
  EntityManager::_entities.push_back(entity);
}

void EntityManager::remove_entity(MqttManagerEntity *entity) {
  EntityManager::_entities.remove(entity);
}

MqttManagerEntity *EntityManager::get_entity_by_type_and_id(MQTT_MANAGER_ENTITY_TYPE type, uint16_t id) {
  for (MqttManagerEntity *entity : EntityManager::_entities) {
    if (entity->get_type() == type && entity->get_id() == id) {
      return entity;
    }
  }
  return nullptr;
}

std::list<MqttManagerEntity *> EntityManager::get_all_entities_by_type(MQTT_MANAGER_ENTITY_TYPE type) {
  std::list<MqttManagerEntity *> return_entities;
  for (MqttManagerEntity *entity : EntityManager::_entities) {
    if (entity->get_type() == type) {
      return_entities.push_back(entity);
    }
  }
  return return_entities;
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
  if (topic.compare("nspanel/mqttmanager/command") == 0) {
    nlohmann::json data = nlohmann::json::parse(payload);
    std::string mac_origin = data["mac_origin"];
    NSPanel *panel = EntityManager::get_nspanel_by_mac(mac_origin);
    if (panel != nullptr) {
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
                light->set_brightness(new_brightness);
                light->turn_on();
              } else {
                light->turn_off();
              }
            }
          }
        } else if (command_set_attribute.compare("kelvin") == 0) {
          std::vector<uint> entity_ids = data["entity_ids"];
          uint new_kelvin = data["kelvin"];
          for (uint entity_id : entity_ids) {
            Light *light = EntityManager::get_light_by_id(entity_id);
            if (light != nullptr) {
              light->set_color_temperature(new_kelvin);
            }
          }
        } else if (command_set_attribute.compare("hue") == 0) {
          std::vector<uint> entity_ids = data["entity_ids"];
          uint new_hue = data["hue"];
          for (uint entity_id : entity_ids) {
            Light *light = EntityManager::get_light_by_id(entity_id);
            if (light != nullptr) {
              light->set_hue(new_hue);
            }
          }
        } else if (command_set_attribute.compare("saturation") == 0) {
          std::vector<uint> entity_ids = data["entity_ids"];
          uint new_saturation = data["saturation"];
          for (uint entity_id : entity_ids) {
            Light *light = EntityManager::get_light_by_id(entity_id);
            if (light != nullptr) {
              light->set_saturation(new_saturation);
            }
          }
        } else {
          SPDLOG_ERROR("Unknown attribute '{}' in set-command request.", command_set_attribute);
        }
      }
    } else {
      SPDLOG_WARN("Ignoring command from panel not known. Origin panel MAC: {}", mac_origin);
    }

    return true;
  }

  return false; // Message was not processed by us, keep looking.
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
  for (NSPanel *nspanel : EntityManager::_nspanels) {
    if (nspanel->get_id() == id) {
      return nspanel;
    }
  }
  return nullptr;
}

NSPanel *EntityManager::get_nspanel_by_mac(std::string mac) {
  for (NSPanel *nspanel : EntityManager::_nspanels) {
    if (nspanel->get_mac().compare(mac) == 0) {
      return nspanel;
    }
  }
  return nullptr;
}
