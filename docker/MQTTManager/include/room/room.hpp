#ifndef MQTT_MANAGER_ROOM_H
#define MQTT_MANAGER_ROOM_H

#include "entity/entity.hpp"
#include "protobuf_nspanel.pb.h"
#include "room/room_entities_page.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <protobuf/protobuf_general.pb.h>
#include <string>
#include <vector>

class Room {
public:
  /**
   * Initalize a room and load room settings from given JSON
   */
  Room(uint32_t room_id);
  ~Room();

  /**
   * Reload config from database
   */
  void reload_config();

  /**
   * Get the ID of the room.
   */
  uint16_t get_id();

  /**
   * Get the name of the room.
   */
  std::string get_name();

  /**
   * Get the display order of the room.
   */
  uint32_t get_display_order();

  /**
   * Post init room.
   */
  void post_init();

  /**
   * Get all entities in the room.
   * Return a std::vector of std::shared_ptr to all entities in room.
   */
  std::vector<std::shared_ptr<MqttManagerEntity>> get_all_entities();

  /**
   * Get all entities matching the specified type that has the specified ID.
   * Return std::vector of pointers to entities.
   */
  template <class EntityClass>
  std::vector<std::shared_ptr<EntityClass>> get_all_entities_by_type(MQTT_MANAGER_ENTITY_TYPE type) {
    std::lock_guard<std::mutex> mutex_guard(this->_entities_pages_mutex);
    std::vector<std::shared_ptr<EntityClass>> entities;
    for (std::shared_ptr<RoomEntitiesPage> &page : this->_entity_pages) {
      std::vector<std::shared_ptr<EntityClass>> page_entities = page->get_entities_by_type<EntityClass>(type);
      entities.insert(entities.end(), page_entities.begin(), page_entities.end());
    }
    return entities;
  }

  /**
   * Get the total number of entity pages in this room.
   */
  uint16_t get_number_of_entity_pages();

  /**
   * Get the total number of entity pages used for scenes in this room.
   */
  uint16_t get_number_of_scene_pages();

  /*
   * Get all RoomEntitiesPages attached to this room.
   */
  std::vector<std::shared_ptr<RoomEntitiesPage>> get_all_entities_pages();

  /*
   * Get all RoomEntitiesPages attached to this room for presenting scenes.
   */
  std::vector<std::shared_ptr<RoomEntitiesPage>> get_all_scenes_pages();

  /**
   * Callback when that gets run when an entitiy has changed state
   */
  void page_changed_callback(RoomEntitiesPage *page);

  /**
   * Callback for NSPanelMQTTManagerCommands sent over MQTT
   */
  void command_callback(NSPanelMQTTManagerCommand &command);

  /**
   * Register a room changed callback listener.
   */
  template <typename CALLBACK_BIND>
  void attach_room_changed_callback(CALLBACK_BIND callback) {
    this->_room_changed_callbacks.connect(callback);
  }

  /**
   * Unregister a room changed callback listener.
   */
  template <typename CALLBACK_BIND>
  void detach_room_changed_callback(CALLBACK_BIND callback) {
    this->_room_changed_callbacks.disconnect(callback);
  }

private:
  /*
   * Create a protobuf room state object and send out to _mqtt_state_topic.
   */
  void _send_room_state_update();

  uint16_t _id;
  std::string _name;
  std::string _mqtt_state_topic;

  // In what place in the display order is this room shown?
  uint32_t _display_order;

  // All pages with entities for this room
  std::vector<std::shared_ptr<RoomEntitiesPage>> _entity_pages;

  // Mutex to only allow one task at the time to access the entity_pages
  std::mutex _entities_pages_mutex;

  // All scene pages
  std::vector<std::shared_ptr<RoomEntitiesPage>> _scene_pages;

  // Mutex to only allow one task at the time to access the entity_pages
  std::mutex _scene_entities_pages_mutex;

  // Mutex to only allow one task at the time to send updates to the panel regarding room status
  std::mutex _send_room_status_update_mutex;

  // If set to true, automatic status updates over MQTT when entities changes are disabled.
  bool _send_status_updates;

  // Last room status sent to the panel. Used to avoid sending duplicate status updates.
  NSPanelRoomStatus _last_room_status;

  // When was the last status update sent.
  std::chrono::time_point<std::chrono::system_clock> _last_status_update;

  // List of callback to call when this room is changed.
  boost::signals2::signal<void(Room *)> _room_changed_callbacks;
};

#endif // !MQTT_MANAGER_ROOM_H
