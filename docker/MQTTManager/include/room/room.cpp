#include "entity/entity.hpp"
#include <boost/bind.hpp>
#include <nlohmann/json.hpp>
#include <room/room.hpp>
#include <spdlog/spdlog.h>
#include <string>

Room::Room(nlohmann::json &config) {
  this->_id = config["id"];
  this->_name = config["name"];

  SPDLOG_DEBUG("Room {}::{} initialized.", this->_id, this->_name);
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
  SPDLOG_DEBUG("ENTITY CHANGED IN ROOM {}", this->_name);
}
