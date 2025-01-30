#ifndef MQTT_MANAGER_ENTITY_MANAGER
#define MQTT_MANAGER_ENTITY_MANAGER

#include "protobuf_general.pb.h"
#include <algorithm>
#include <boost/signals2.hpp>
#include <entity/entity.hpp>
#include <list>
#include <memory>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nspanel/nspanel.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <weather/weather.hpp>

// Forward declare entities:
class Room;
class Light;

class EntityManager {
public:
  /**
   * Setup function callbacks and other prerequisites for the EntityManager
   */
  static void init();

  /**
   * Call post-initialized on all entities.
   */
  static void post_init_entities();

  /**
   * Attach callback for when a new entity is added
   */
  static void attach_entity_added_listener(void (*listener)(std::shared_ptr<MqttManagerEntity>));

  /**
   * Detach an existing callback for when an entity is added
   */
  static void detach_entity_added_listener(void (*listener)(std::shared_ptr<MqttManagerEntity>));

  /**
   * Remove an entity from the list of managed entities.
   */
  static void remove_entity(std::shared_ptr<MqttManagerEntity> entity);

  /*
   * Load all rooms from the DB and remove any existing room that no longer exist.
   */
  static void load_rooms();

  /*
   * Load all NSPanels from the DB and remove any existing panel that no longer exist.
   */
  static void load_nspanels();

  /**
   * Get a room with the given ID if it existing. Otherwise returns nullptr.
   */
  static std::shared_ptr<Room> get_room(uint32_t room_id);

  /**
   * Get all currently registered rooms
   */
  static std::vector<std::shared_ptr<Room>> get_all_rooms();

  /**
   * Remove a room with the given ID if found.
   */
  static void remove_room(uint32_t room_id);

  /**
   * Create and add a light to the manager
   */
  static void add_light(LightSettings &config);

  /**
  Create light protobuf from JSON and send to add_light, simply a callback for IPCHandler for partial updates.
  */
  static bool ipc_callback_add_light(nlohmann::json data, nlohmann::json *response);

  /**
   * Create and add a scene to the manager
   */
  static void add_scene(nlohmann::json &config);

  /**
   * Create and add a NSPanel relay group to the manager
   */
  static void add_nspanel_relay_group(nlohmann::json &config);

  /**
   * Get an NSPanel Relay Group if a group with the given id exists. Otherwise, return nullptr.
   */
  static std::shared_ptr<NSPanelRelayGroup> get_relay_group(uint32_t relay_group_id);

  /**
   * Get all currently register NSPanel relay groups.
   */
  static std::vector<std::shared_ptr<NSPanelRelayGroup>> get_all_relay_groups();

  /**
   * Get an item by the specified type that has the specified ID.
   * Return pointer to entity if found, otherwise a nullptr
   */
  template <class EntityClass>
  static std::shared_ptr<EntityClass> get_entity_by_id(MQTT_MANAGER_ENTITY_TYPE type, int id) {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    auto rit = EntityManager::_entities.cbegin();
    while (rit != EntityManager::_entities.cend()) {
      if ((*rit)->get_type() == type && (*rit)->get_id() == id) {
        return std::static_pointer_cast<EntityClass>(*rit);
      } else {
        ++rit;
      }
    }
    return nullptr;
  }

  /**
   * Get an MqttManagerEntity that is placed in the given slot on the given page.
   * Returns shared_ptr to MqttManagerEntity object if found, otherwise nullptr.
   */
  static std::shared_ptr<MqttManagerEntity> get_entity_by_page_id_and_slot(uint32_t page_id, uint8_t page_slot) {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    auto rit = EntityManager::_entities.cbegin();
    while (rit != EntityManager::_entities.cend()) {
      if ((*rit)->get_entity_page_id() == page_id && (*rit)->get_entity_page_slot() == page_slot) {
        return (*rit);
      } else {
        ++rit;
      }
    }
    return nullptr;
  }

  /**
   * Get all entities matching the specified type that has the specified ID.
   * Return std::list of pointers to entities.
   */
  template <class EntityClass>
  static std::vector<std::shared_ptr<EntityClass>> get_all_entities_by_type(MQTT_MANAGER_ENTITY_TYPE type) {
    std::lock_guard<std::mutex> mutex_guard(EntityManager::_entities_mutex);
    std::vector<std::shared_ptr<EntityClass>> entities;

    for (auto entity : EntityManager::_entities) {
      if (entity->get_type() == type) {
        entities.push_back(std::static_pointer_cast<EntityClass>(entity));
      }
    }

    return entities;
  }

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

  static std::shared_ptr<NSPanel> get_nspanel_by_id(uint id);
  static std::shared_ptr<NSPanel> get_nspanel_by_mac(std::string mac);

private:
  static inline std::vector<std::shared_ptr<MqttManagerEntity>> _entities;
  static inline std::mutex _entities_mutex;

  static inline std::vector<std::shared_ptr<Room>> _rooms;
  static inline std::mutex _rooms_mutex;

  static inline std::vector<std::shared_ptr<NSPanel>> _nspanels;
  static inline std::mutex _nspanels_mutex;

  static inline std::vector<std::shared_ptr<NSPanelRelayGroup>> _nspanel_relay_groups;
  static inline std::mutex _nspanel_relay_groups_mutex;

  static bool _process_message(const std::string &topic, const std::string &payload);
  static void _handle_register_request(const nlohmann::json &data);
  static inline MQTTManagerWeather _weather_manager;

  static inline boost::signals2::signal<void(std::shared_ptr<MqttManagerEntity>)> _entity_added_signal;
  static inline boost::signals2::signal<void(std::shared_ptr<MqttManagerEntity>)> _entity_removed_signal;
};

#endif // !MQTT_MANAGER_ENTITY_MANAGER
