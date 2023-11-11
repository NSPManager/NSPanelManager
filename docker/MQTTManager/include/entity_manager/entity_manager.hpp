#ifndef MQTT_MANAGER_ENTITY_MANAGER
#define MQTT_MANAGER_ENTITY_MANAGER

#include <boost/signals2.hpp>
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

  static void config_added(nlohmann::json *config);
  static void config_removed(nlohmann::json *config);

  /**
   * Call post-initialized on all entities.
   */
  static void post_init_entities();

  /**
   * Attach callback for when a new entity is added
   */
  static void attach_entity_added_listener(void (*listener)(MqttManagerEntity *));

  /**
   * Detach an existing callback for when an entity is added
   */
  static void detach_entity_added_listener(void (*listener)(MqttManagerEntity *));

  /**
   * Add an entity to the list of managed entities.
   */
  static void add_entity(MqttManagerEntity *entity);

  /**
   * Remove an entity from the list of managed entities.
   */
  static void remove_entity(MqttManagerEntity *entity);

  /**
   * Create and add a light to the manager
   */
  static void add_light(nlohmann::json &config);

  /**
   * Create and add a nspanel to the manager
   */
  static void add_nspanel(nlohmann::json &config);

  /**
   * Create and add a room to the manager
   */
  static void add_room(nlohmann::json &config);

  /**
   * Create and add a scene to the manager
   */
  static void add_scene(nlohmann::json &config);

  /**
   * Get an item by the specified type that has the specified ID.
   * Return pointer to entity if found, otherwise a nullptr
   */
  template <class EntityClass>
  static EntityClass *get_entity_by_id(MQTT_MANAGER_ENTITY_TYPE type, int id) {
    for (MqttManagerEntity *entity : EntityManager::_entities) {
      if (entity->get_type() == type && entity->get_id() == id) {
        return static_cast<EntityClass *>(entity);
      }
    }
    return nullptr;
  }

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
   * Callback from MQTT Manager for when a message was received on a subscribed topic.
   */
  static void mqtt_topic_callback(const std::string &topic, const std::string &payload);

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
  static inline std::list<MqttManagerEntity *> _post_init_entities; // The entities to post init when called next time.

  static inline boost::signals2::signal<void(MqttManagerEntity *)> _entity_added_signal;
  static inline boost::signals2::signal<void(MqttManagerEntity *)> _entity_removed_signal;
};

#endif // !MQTT_MANAGER_ENTITY_MANAGER