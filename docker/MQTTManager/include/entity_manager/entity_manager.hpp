#ifndef MQTT_MANAGER_ENTITY_MANAGER
#define MQTT_MANAGER_ENTITY_MANAGER

#include <entity/entity.hpp>
#include <light/light.hpp>
#include <list>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nspanel/nspanel.hpp>

class EntityManager {
public:
  /**
   * Setup function callbacks and other prerequisites for the EntityManager
   */
  static void init();

  /**
   * Create and initialize entities from config.
   */
  static void init_entities();

  /**
   * Add an entity to the list of managed entities.
   */
  static void add_entity(MqttManagerEntity *entity);

  /**
   * Remove an entity from the list of managed entities.
   */
  static void remove_entity(MqttManagerEntity *entity);

  /**
   * Get an item by the specified type that has the specified ID.
   * Return pointer to entity if found, otherwise a nullptr
   */
  static MqttManagerEntity *get_entity_by_type_and_id(MQTT_MANAGER_ENTITY_TYPE type, uint16_t id);

  /**
   * Get all entities matching the specified type that has the specified ID.
   * Return std::list of MqttManagerEntity pointers.
   */
  static std::list<MqttManagerEntity *> get_all_entities_by_type(MQTT_MANAGER_ENTITY_TYPE type);

  /**
   * Process MQTT Message. Return true if message was handled.
   */
  static bool mqtt_callback(const std::string &topic, const std::string &payload);

  /**
   * Process any incomming message from the websocket. Return true if message was handled.
   */
  static bool websocket_callback(std::string &message, std::string *response_buffer);

  static Light *get_light_by_id(uint id);
  static NSPanel *get_nspanel_by_id(uint id);
  static NSPanel *get_nspanel_by_mac(std::string mac);

private:
  static inline std::list<MqttManagerEntity *> _entities;

  static bool _process_message(const std::string &topic, const std::string &payload);
  static inline std::list<Light *> _lights;
  static inline std::list<NSPanel *> _nspanels;
};

#endif // !MQTT_MANAGER_ENTITY_MANAGER
