#ifndef MQTT_MANAGER_ROOM_H
#define MQTT_MANAGER_ROOM_H

#include "entity/entity.hpp"
#include <list>
#include <nlohmann/json_fwd.hpp>
#include <string>
class Room : public MqttManagerEntity {
public:
  /**
   * Initalize a room and load room settings from given JSON
   */
  Room(nlohmann::json &config);

  /**
   * Get the ID of the room.
   */
  uint16_t get_id();

  /**
   * Get the name of the room.
   */
  std::string get_name();

  /**
   * Get entity type.
   */
  MQTT_MANAGER_ENTITY_TYPE get_type();

  /**
   * Get controller for this entity.
   */
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();

  /**
   * Post init room.
   */
  void post_init();

  /**
   * Attach an entity to this room.
   */
  void attach_entity(MqttManagerEntity *entity);

  /**
   * Detach an entity from this room.
   */
  void detach_entity(MqttManagerEntity *entity);

private:
  uint16_t _id;
  std::string _name;
  std::list<MqttManagerEntity *> _entities;
};

#endif // !MQTT_MANAGER_ROOM_H
