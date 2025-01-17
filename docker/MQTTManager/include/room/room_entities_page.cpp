#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "light/light.hpp"
#include "protobuf_nspanel.pb.h"
#include <algorithm>
#include <exception>
#include <memory>
#include <mutex>
#include <room/room_entities_page.hpp>
#include <protobuf/protobuf_general.pb.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>
#include <web_helper/WebHelper.hpp>
#include <fmt/format.h>
#include <boost/stacktrace/stacktrace.hpp>

RoomEntitiesPage::RoomEntitiesPage(uint32_t page_id) {
    this->_id = page_id;
    this->_update();
    SPDLOG_DEBUG("Created RoomEntitiesPage with ID {}. Page will display {}", this->_id, this->_page_settings.is_scenes_page() ? "scenes." : "entities.");
}

uint32_t RoomEntitiesPage::get_id() {
    return this->_id;
}

uint8_t RoomEntitiesPage::get_type() {
    return this->_page_settings.page_type();
}

void RoomEntitiesPage::post_init() {
    this->_entities.resize(this->_page_settings.page_type());
    uint8_t entities_attached = 0;
    for(int i = 0; i < this->_page_settings.page_type(); i++) {
        this->_entities[i] = EntityManager::get_entity_by_page_id_and_slot(this->_id, i);
        if(this->_entities[i] != nullptr) {
            entities_attached++;
            SPDLOG_DEBUG("Attached entity type {} with ID {} to RoomEntitiesPage ID {} page slot {}.", (int)this->_entities[i]->get_type(), this->_entities[i]->get_id(), this->_id, i);
        }
    }
    SPDLOG_DEBUG("Attached {} entities to RoomEntitiesPage {}. this->_entities size: {}", entities_attached, this->_id, this->_entities.size());
}

std::vector<std::shared_ptr<MqttManagerEntity>> RoomEntitiesPage::get_entities() {
    std::vector<std::shared_ptr<MqttManagerEntity>> entities;
    // The _entities vector is built on index = page slot, that means that some
    // indicies are nullptr if no entity is assigned to that slot.
    // Filter out nullptrs from list of entities on this page:
    std::copy_if(this->_entities.begin(), this->_entities.end(), std::back_inserter(entities), [](std::shared_ptr<MqttManagerEntity> entity) {
        return entity != nullptr;
    });
    return entities;
}

void RoomEntitiesPage::populate_nspanel_room_entities_page_with_entities(NSPanelRoomEntitiesPage *page) {
    for(RoomEntityWrapper wrapper : RoomEntitiesPage::_page_settings.entities()) {
        NSPanelRoomEntitiesPage_EntitySlot *entity_slot = page->add_entities();
        if(wrapper.has_light()) {
            std::shared_ptr<Light> light = EntityManager::get_entity_by_id<Light>(MQTT_MANAGER_ENTITY_TYPE::LIGHT, wrapper.light().id());
            if(light != nullptr) {
                entity_slot->set_name(light->get_name());
                entity_slot->set_room_view_position(wrapper.light().room_view_position());
                // TODO: Move state icons for panel from GUI_DATA to manager and convert this!
                entity_slot->set_icon(light->get_state() ? "s" : "t");
                if(light->get_state()) {
                    entity_slot->set_pco(65024);
                    entity_slot->set_pco2(65024);
                } else {
                    entity_slot->set_pco(65535);
                    entity_slot->set_pco2(65535);
                }
                SPDLOG_DEBUG("Added Light {}::{} to RoomEntityWrapper {}.", light->get_id(), light->get_name(), this->_id);
            } else {
                SPDLOG_ERROR("Light entity at room view position {} was not found among loaded entities!", wrapper.light().id());
            }
        } else if (wrapper.has_switch_()) {
            SPDLOG_ERROR("Switch entity not implemented!");
        } else {
            SPDLOG_ERROR("Unknown entity type while processing EntityWrapper while building NSPanelRoomEntitiesPage protobuf object.");
        }
    }
}

void RoomEntitiesPage::_update() {
    std::string config_url = fmt::format("http://127.0.0.1:8000/protobuf/mqttmanager/get_room_entities_page/{}", this->_id);
    std::string data;
    SPDLOG_DEBUG("Fetching config for RoomEntitiesPage ID {}.", this->_id);
    while(!WebHelper::perform_get_request(&config_url, &data, nullptr)) {
        SPDLOG_ERROR("Failed to get config for RoomEntitiesPage with ID {}, will try again in 500ms", this->_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    try{
        SPDLOG_DEBUG("Got config data for RoomEntitiesPage with ID {}.", this->_id);
        if(!this->_page_settings.ParseFromString(data)) {
            SPDLOG_ERROR("Failed to parse RoomEntitiesPageSettings from Django while processing config for RoomEntitiesPage with ID {}.", this->_id);
            return;
        }
    }catch(std::exception &ex) {
        SPDLOG_ERROR("Error while processing settings for RoomEntitiesPage with ID {}. Stacktrace: {}", this->_id, boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
    }
}
