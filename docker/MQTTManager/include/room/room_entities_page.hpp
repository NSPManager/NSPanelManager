#pragma once
#include "entity/entity.hpp"
#include "protobuf_nspanel.pb.h"
#include <mutex>
#include <protobuf/protobuf_general.pb.h>
#include <memory>

class RoomEntitiesPage {
public:
    /**
    * Create an instance that will pull it's own config from Djano on creation
    */
    RoomEntitiesPage(uint32_t page_id);

    /**
    * Get the ID of this RoomEntitiesPage
    */
    uint32_t get_id();

    /**
    * Get type of page: 4, 8 or 12 entities.
    */
    uint8_t get_type();

    /**
    * Perform post-init on this page. This will among other things look up and bind slots on the page to entities.
    */
    void post_init();

    /**
    * Get all entities registered to this page.
    */
    std::vector<std::shared_ptr<MqttManagerEntity>> get_entities();

    /**
    * Get an NSPanelRoomEntitiesPage status object for this page to send to NSPanel.
    */
    void populate_nspanel_room_entities_page_with_entities(NSPanelRoomEntitiesPage *page);

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

private:
    /**
    * Pull updated config from Django.
    */
    void _update();

    // Vars:
    // The ID of this "NSPanelRoomEntitiesPage" in the Django database
    int32_t _id;

    // Settings for this page
    RoomEntitiesPageSettings _page_settings;

    // Entities registered to this page
    std::mutex _entities_mutex;
    std::vector<std::shared_ptr<MqttManagerEntity>> _entities;
};
