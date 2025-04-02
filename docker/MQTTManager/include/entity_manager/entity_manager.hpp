#ifndef MQTT_MANAGER_ENTITY_MANAGER
#define MQTT_MANAGER_ENTITY_MANAGER

#include "protobuf_general.pb.h"
#include "protobuf_nspanel.pb.h"
#include <algorithm>
#include <boost/signals2.hpp>
#include <condition_variable>
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

  /*
   * Load entities from database, update any already loaded entities.
   */
  static void load_entities();

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

  /*
   * Load all lights from the DB and remove any existing light that no longer exist.
   */
  static void load_lights();

  /*
   * Load all switches from the DB and remove any existing switch that no longer exist.
   */
  static void load_switches();

  /*
   * Load all scenes from the DB and remove any existing scene that no longer exist.
   */
  static void load_scenes();

  /**
   * Get a room with the given ID if it existing. Otherwise returns nullptr.
   */
  static std::shared_ptr<Room> get_room(uint32_t room_id);

  /**
   * Get all currently registered rooms
   */
  static std::vector<std::shared_ptr<Room>> get_all_rooms();

  /*
   * When a room is updated, update the "All rooms" status for when the panel is in the "All rooms" mode.
   */
  static void update_all_rooms_status();

  /*
   * Handle commands that are sent to the MQTTManager but are not
   * neccesarily handled by other entities directly.
   */
  static void _command_callback(NSPanelMQTTManagerCommand &command);

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

  static inline std::atomic<bool> _all_rooms_status_updated;
  static inline void _room_updated_callback(Room *room);
  static inline std::thread _update_all_rooms_status_thread;
  static inline std::atomic<std::chrono::time_point<std::chrono::system_clock>> _last_room_update_time;
  static inline std::condition_variable _room_update_condition_variable; // Notify thread that handles "All rooms" status updates

  static bool _process_message(const std::string &topic, const std::string &payload);
  static void _handle_register_request(const nlohmann::json &data);
  static inline MQTTManagerWeather _weather_manager;

  static inline boost::signals2::signal<void(std::shared_ptr<MqttManagerEntity>)> _entity_added_signal;
  static inline boost::signals2::signal<void(std::shared_ptr<MqttManagerEntity>)> _entity_removed_signal;
};

#endif // !MQTT_MANAGER_ENTITY_MANAGER
