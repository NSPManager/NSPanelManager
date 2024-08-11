#ifndef MQTT_MANAGER_ROOM_H
#define MQTT_MANAGER_ROOM_H

#include "entity/entity.hpp"
#include <list>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/protobuf_general.pb.h>
#include <string>
class Room : public MqttManagerEntity {
public:
  /**
   * Initalize a room and load room settings from given JSON
   */
  Room(RoomSettings &config);

  /**
   * Update room with given config
   */
  void update_config(RoomSettings &config);

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

  /**
   * Get all entities matching the specified type that has the specified ID.
   * Return std::list of pointers to entities.
   */
  template <class EntityClass>
  std::list<EntityClass *> get_all_entities_by_type(MQTT_MANAGER_ENTITY_TYPE type) {
    std::list<EntityClass *> entities;
    for (MqttManagerEntity *entity : this->_entities) {
      if (entity->get_type() == type) {
        entities.push_back(static_cast<EntityClass *>(entity));
      }
    }
    return entities;
  }

  void entity_changed_callback(MqttManagerEntity *entity);

private:
  uint16_t _id;
  std::string _name;
  std::list<MqttManagerEntity *> _entities;
};

#endif // !MQTT_MANAGER_ROOM_H
