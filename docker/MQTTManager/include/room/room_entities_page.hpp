#pragma once
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "protobuf_nspanel.pb.h"
#include <memory>
#include <mutex>
#include <protobuf/protobuf_general.pb.h>

class Room; // Forward declare room as to not create circular dependencies

class RoomEntitiesPage {
public:
  /**
   * Create an instance that will pull it's own config from Djano on creation
   */
  RoomEntitiesPage(uint32_t page_id, Room *room);
  ~RoomEntitiesPage();

  /*
   * Redownload config from manager and reload this page.
   * @param send_state_update: Whether to send a state update to the MQTT broker after reloading the config.
   */
  void reload_config(bool send_state_update);

  /**
   * Get the ID of this RoomEntitiesPage
   */
  uint32_t get_id();

  /**
   * Get type of page: 4, 8 or 12 entities.
   */
  uint8_t get_type();

  /*
   * Get display order
   */
  uint16_t get_display_order();

  /**
   * Perform post-init on this page. This will among other things look up and bind slots on the page to entities.
   */
  void post_init();

  /**
   * Get all entities registered to this page.
   */
  std::vector<std::shared_ptr<MqttManagerEntity>> get_entities();

  /**
   * Get all entities registered to this page with a specific type.
   */
  template <class EntityClass>
  std::vector<std::shared_ptr<EntityClass>> get_entities_by_type(MQTT_MANAGER_ENTITY_TYPE type) {
    std::lock_guard<std::mutex> mutex_guard(this->_entities_mutex);
    std::vector<std::shared_ptr<EntityClass>> entities;
    for (int i = 0; i < this->_entities.size(); i++) {
      if (this->_entities[i] != nullptr && this->_entities[i]->get_type() == type) {
        entities.push_back(std::static_pointer_cast<EntityClass>(this->_entities[i]));
      }
    }
    return entities;
  }

  /**
   * Register a callback for when the state of this entity page changes.
   */
  template <typename CALLBACK_BIND>
  void attach_state_change_callback(CALLBACK_BIND callback) {
    this->_state_changed_callbacks.connect(callback);
  }

  /**
   * Unregister a callback for when the state of this entity page changes.
   */
  template <typename CALLBACK_BIND>
  void detach_state_change_callback(CALLBACK_BIND callback) {
    this->_state_changed_callbacks.disconnect(callback);
  }

private:
  /*
   * When a state changes in one of the underlaying entities, recalculate
   * entity page state.
   */
  void _entity_changed_callback(MqttManagerEntity *entity);

  /*
   * Will create a new protobuf state object and publish to _mqtt_state_topic.
   */
  void _send_mqtt_state_update();

  // MQTT topic where the state for this entities page is sent.
  std::string _mqtt_state_topic;

  // Vars:
  // The ID of this "NSPanelRoomEntitiesPage" in the Django database
  int32_t _id;

  // The room object that this entity page is attached to.
  Room *_room;

  // Settings for this page
  std::mutex _page_settings_mutex;
  database_manager::RoomEntitiesPage _page_settings;

  // Callbacks for when the state of this entity page changes.
  boost::signals2::signal<void(RoomEntitiesPage *)> _state_changed_callbacks;

  // Entities registered to this page
  std::mutex _entities_mutex;
  std::vector<std::shared_ptr<MqttManagerEntity>> _entities;
};
