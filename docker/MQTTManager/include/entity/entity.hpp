#ifndef MQTT_MANAGER_ENTITY
#define MQTT_MANAGER_ENTITY

#include <boost/signals2/signal.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string_view>
enum MQTT_MANAGER_ENTITY_TYPE {
  LIGHT,
  SWITCH_ENTITY,
  BUTTON,
  THERMOSTAT,
  SCENE,
};

enum MQTT_MANAGER_ENTITY_CONTROLLER
{
  NONE,           // None is only used to indicate that an entity is not set.
  NSPM,           // We control the entity using the NSPanel Manager.
  HOME_ASSISTANT, // Home assistant is the owner of this entity.
  OPENHAB,        // OpenHAB is the owner of this entity.
  HOMEY,          // Homey is the owner of this entity.
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
   * Get the ID of the entity page this entity is located on.
   */
  virtual uint32_t get_entity_page_id() = 0;

  /**
   * Get the viewed slot (room view position) of this entity as to where on the page it should be shown.
   */
  virtual uint8_t get_entity_page_slot() = 0;

  /*
   * Can this entity be toggled?
   */
  virtual bool can_toggle() = 0;

  /*
   * Toggle the entity state.
   */
  virtual void toggle() = 0;

  /*
   * Get the icon of this entity as to how it should be displayed in the NSPanel itself.
   */
  virtual std::string_view get_icon() = 0;

  /*
   * Get the icon color of this entity as to how it should be displayed in the NSPanel itself.
   */
  virtual uint16_t get_icon_color() = 0;

  /*
   * Get the icon active (pressed) color of this entity as to how it should be displayed in the NSPanel itself.
   */
  virtual uint16_t get_icon_active_color() = 0;

  /*
   * Get the MQTT state topic of this entity where the NSPanel can subscribe to state changes for this entity.
   */
  virtual std::string get_mqtt_state_topic() = 0;

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
