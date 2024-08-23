#include "entity/entity.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "protobuf/protobuf_general.pb.h"
#include "protobuf/protobuf_nspanel.pb.h"
#include <boost/bind.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <room/room.hpp>
#include <spdlog/spdlog.h>
#include <string>

Room::Room(RoomSettings &config) {
  this->update_config(config);
}

void Room::update_config(RoomSettings &config) {
  this->_id = config.id();
  this->_name = config.name();
  this->_mqtt_status_topic = fmt::format("nspanel/mqttmanager/room/{}/status", this->_id);

  SPDLOG_DEBUG("Room {}::{} initialized with status topic '{}'.", this->_id, this->_name, this->_mqtt_status_topic);

  this->_publish_protobuf_status();
}

MQTT_MANAGER_ENTITY_TYPE Room::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::ROOM;
}

MQTT_MANAGER_ENTITY_CONTROLLER Room::get_controller() {
  return MQTT_MANAGER_ENTITY_CONTROLLER::NSPM;
}

uint16_t Room::get_id() {
  return this->_id;
}

std::string Room::get_name() {
  return this->_name;
}

void Room::post_init() {
  // No post init tasks for room.
}

void Room::attach_entity(MqttManagerEntity *entity) {
  this->_entities.push_back(entity);
  entity->attach_entity_changed_callback(boost::bind(&Room::entity_changed_callback, this, _1));
}

void Room::detach_entity(MqttManagerEntity *entity) {
  this->_entities.remove(entity);
  entity->detach_entity_changed_callback(boost::bind(&Room::entity_changed_callback, this, _1));
}

void Room::entity_changed_callback(MqttManagerEntity *entity) {
  SPDLOG_TRACE("Entity with ID {} changed in room {}::{}, resending protobuf status.", entity->get_id(), this->_id, this->_name);
  switch (entity->get_type()) {

  case LIGHT:
    this->_publish_protobuf_status();
    break;
  case SCENE:
    this->_publish_protobuf_status();
    break;
  case NSPANEL_RELAY_GROUP:
  case ROOM:
    break;
  }
}

void Room::_publish_protobuf_status() {
  SPDLOG_TRACE("Publising protobuf status on topic {}", this->_mqtt_status_topic);

  NSPanelRoomStatus status;
  status.set_id(this->_id);
  status.set_name(this->_name);

  // Calculate average light level
  uint64_t total_light_level_all = 0;
  uint64_t total_light_level_ceiling = 0;
  uint64_t total_light_level_table = 0;
  uint16_t num_lights_total = 0;
  uint16_t num_lights_ceiling = 0;
  uint16_t num_lights_table = 0;
  SPDLOG_TRACE("Checking registered _entities to calculate average light levels.");
  for (auto entity : this->_entities) {
    if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT) {
      Light *light = (Light *)entity;
      total_light_level_all += light->get_brightness();
      num_lights_total++;
      switch (light->get_light_type()) {
      case MQTT_MANAGER_LIGHT_TYPE::TABLE:
        total_light_level_table += light->get_brightness();
        num_lights_table++;
        break;
      case MQTT_MANAGER_LIGHT_TYPE::CEILING:
        total_light_level_ceiling += light->get_brightness();
        num_lights_ceiling++;
        break;
      }

      // Build light status object
      NSPanelLightStatus *light_status = status.add_lights();
      light_status->set_id(light->get_id());
      light_status->set_name(light->get_name());
      light_status->set_can_dim(light->can_dim());
      light_status->set_can_color_temperature(light->can_color_temperature());
      light_status->set_can_rgb(light->can_rgb());
    }
  }

  if (num_lights_total > 0) {
    status.set_average_dim_level(total_light_level_all / num_lights_total);
  } else {
    status.set_average_dim_level(0);
  }
  if (num_lights_table > 0) {
    status.set_table_lights_dim_level(total_light_level_table / num_lights_table);
  } else {
    status.set_table_lights_dim_level(0);
  }
  if (num_lights_ceiling > 0) {
    status.set_ceiling_lights_dim_level(total_light_level_ceiling / num_lights_ceiling);
  } else {
    status.set_ceiling_lights_dim_level(0);
  }

  // Format and send
  std::string data;
  SPDLOG_TRACE("Formating...");
  status.SerializeToString(&data);
  SPDLOG_TRACE("Sending room status '{}' to status topic {}", data, this->_mqtt_status_topic);
  MQTT_Manager::publish(this->_mqtt_status_topic, data, true);
}
