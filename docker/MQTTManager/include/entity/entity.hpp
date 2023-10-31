#ifndef MQTT_MANAGER_ENTITY
#define MQTT_MANAGER_ENTITY

#include <cstdint>
enum MQTT_MANAGER_ENTITY_TYPE {
  LIGHT,
  SCENE,
  ROOM
};

enum MQTT_MANAGER_ENTITY_CONTROLLER {
  NSPM,
  HOME_ASSISTANT,
  OPENHAB
};

class MqttManagerEntity {
public:
  /**
   * Get the ID of the entity.
   */
  virtual uint16_t get_id() = 0;

  /**
   * Get the type of entity.
   */
  virtual MQTT_MANAGER_ENTITY_TYPE get_type() = 0;

  /**
   * Get the controller of this entity.
   */
  virtual MQTT_MANAGER_ENTITY_CONTROLLER get_controller() = 0;

  /**
   * All entities and config has been loaded, do post-processing tasks.
   */
  virtual void post_init() = 0;

  virtual ~MqttManagerEntity() {}
};

#endif // !MQTT_MANAGER_ENTITY
