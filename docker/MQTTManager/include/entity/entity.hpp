#ifndef MQTT_MANAGER_ENTITY
#define MQTT_MANAGER_ENTITY

#include <boost/signals2/signal.hpp>
enum MQTT_MANAGER_ENTITY_TYPE {
  LIGHT,
  SCENE,
  NSPANEL_RELAY_GROUP,
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

  /**
   * Register a entity_changed callback listener.
   */
  void attach_entity_changed_callback(void (*callback)(MqttManagerEntity *)) {
    this->_entity_changed_callbacks.connect(callback);
  }

  /**
   * Register a entity_changed callback listener.
   */
  template <typename CALLBACK_BIND>
  void attach_entity_changed_callback(CALLBACK_BIND callback) {
    this->_entity_changed_callbacks.connect(callback);
  }

  /**
   * Detach a entity_changed callback listener.
   */
  void register_entity_changed_callback(void (*callback)(MqttManagerEntity *)) {
    this->_entity_changed_callbacks.disconnect(callback);
  }

  /**
   * Register a entity_changed callback listener.
   */
  template <typename CALLBACK_BIND>
  void detach_entity_changed_callback(CALLBACK_BIND callback) {
    this->_entity_changed_callbacks.disconnect(callback);
  }

  /**
   * Register a callback listener to be called when entity is about to be destroyed.
   */
  template <typename CALLBACK_BIND>
  void attach_entity_destroy_callback(CALLBACK_BIND callback) {
    this->_entity_destroy_callback.connect(callback);
  }

  /**
   * Detach a callback listener to be called when entity is about to be destroyed.
   */
  template <typename CALLBACK_BIND>
  void detach_entity_destroy_callback(CALLBACK_BIND callback) {
    this->_entity_destroy_callback.disconnect(callback);
  }

  virtual ~MqttManagerEntity() {}

protected:
  boost::signals2::signal<void(MqttManagerEntity *)> _entity_changed_callbacks;
  boost::signals2::signal<void(MqttManagerEntity *)> _entity_destroy_callback;

  /**
   * Signal to all callbacks that this entity has changed.
   */
  void _signal_entity_changed();
  void _signal_entity_destroyed();
};

#endif // !MQTT_MANAGER_ENTITY
