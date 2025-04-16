#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include "light/light.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include "scenes/scene.hpp"
#include "switch/switch.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <database_manager/database_manager.hpp>
#include <entity/entity_icons.hpp>
#include <exception>
#include <fmt/format.h>
#include <google/protobuf/util/message_differencer.h>
#include <memory>
#include <mutex>
#include <protobuf/protobuf_general.pb.h>
#include <room/room.hpp>
#include <room/room_entities_page.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <web_helper/WebHelper.hpp>

RoomEntitiesPage::RoomEntitiesPage(uint32_t page_id, Room *room) {
  this->_id = page_id;
  this->_room = room;
  this->reload_config(false);
  SPDLOG_DEBUG("Created RoomEntitiesPage with ID {}. Page will display {}", this->_id, this->_page_settings.is_scenes_page ? "scenes." : "entities.");

  // As this is object initialization we need to send out an MQTT status topic so that data exists that represents this page
  this->_send_mqtt_state_update();
}

RoomEntitiesPage::~RoomEntitiesPage() {
  MQTT_Manager::clear_retain(this->_mqtt_state_topic); // Clear old state topic.
  for (int i = 0; i < this->_entities.size(); i++) {
    if (this->_entities[i] != nullptr) {
      this->_entities[i]->detach_entity_changed_callback(boost::bind(&RoomEntitiesPage::_entity_changed_callback, this, _1));
    }
  }
  SPDLOG_DEBUG("Destroyed RoomEntitiesPage with ID {}.", this->_id);
}

void RoomEntitiesPage::reload_config(bool send_state_update) {
  try {
    // Update the page settings from the database
    {
      SPDLOG_DEBUG("Loading RoomEntitiesPage {}, trying to get settings from DB.", this->_id);
      auto db_room = database_manager::database.get<database_manager::RoomEntitiesPage>(this->_id);

      std::lock_guard<std::mutex> mutex_guard(this->_page_settings_mutex);
      this->_page_settings = db_room;

      this->_mqtt_state_topic = fmt::format("nspanel/mqttmanager_{}/entity_pages/{}/state", MqttManagerConfig::get_settings().manager_address, this->_id);
    }

    SPDLOG_DEBUG("RoomEntitiesPage {} loaded successfully.", this->_id);
    if (send_state_update) {
      this->_send_mqtt_state_update();
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception: {}", e.what());
    SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
  }
}

uint32_t RoomEntitiesPage::get_id() {
  return this->_id;
}

uint8_t RoomEntitiesPage::get_type() {
  std::lock_guard<std::mutex> mutex_guard(this->_page_settings_mutex);
  return this->_page_settings.page_type;
}

uint16_t RoomEntitiesPage::get_display_order() {
  std::lock_guard<std::mutex> mutex_guard(this->_page_settings_mutex);
  return this->_page_settings.display_order;
}

void RoomEntitiesPage::post_init(bool send_state_update) {
  {
    std::lock_guard<std::mutex> mutex_guard(this->_page_settings_mutex);
    this->_entities.resize(this->_page_settings.page_type);
    uint8_t entities_attached = 0;
    for (int i = 0; i < this->_page_settings.page_type; i++) {
      this->_entities[i] = EntityManager::get_entity_by_page_id_and_slot(this->_id, i);
      if (this->_entities[i] != nullptr) {
        entities_attached++;
        SPDLOG_DEBUG("Attached entity type {} with ID {} to RoomEntitiesPage ID {} page slot {}.", (int)this->_entities[i]->get_type(), this->_entities[i]->get_id(), this->_id, i);

        this->_entities[i]->attach_entity_changed_callback(boost::bind(&RoomEntitiesPage::_entity_changed_callback, this, _1));
      }
    }
    SPDLOG_DEBUG("Attached {} entities to RoomEntitiesPage {}.", entities_attached, this->_id);
  }

  if (send_state_update) {
    this->_send_mqtt_state_update();
  }
}

std::vector<std::shared_ptr<MqttManagerEntity>> RoomEntitiesPage::get_entities() {
  std::vector<std::shared_ptr<MqttManagerEntity>> entities;
  // The _entities vector is built on index = page slot logic, that means that some
  // indicies are nullptr if no entity is assigned to that slot.
  // Filter out nullptrs from list of entities on this page:
  std::copy_if(this->_entities.begin(), this->_entities.end(), std::back_inserter(entities), [](std::shared_ptr<MqttManagerEntity> entity) {
    return entity != nullptr;
  });
  return entities;
}

void RoomEntitiesPage::_entity_changed_callback(MqttManagerEntity *entity) {
  this->_send_mqtt_state_update();
  this->_state_changed_callbacks(this);
}

void RoomEntitiesPage::_send_mqtt_state_update() {
  SPDLOG_DEBUG("Entity page {} sending state update to MQTT topic: {}", this->_id, this->_mqtt_state_topic);
  std::lock_guard<std::mutex> mutex_guard(this->_page_settings_mutex);

  NSPanelRoomEntitiesPage proto_state;
  proto_state.set_id(this->_id);
  proto_state.set_page_type(this->_page_settings.page_type);
  if (this->_page_settings.is_scenes_page) {
    if (this->_room != nullptr) {
      proto_state.set_header_text(fmt::format("{} {}/{}", this->_room->get_name(), this->_page_settings.display_order + 1, this->_room->get_number_of_scene_pages()));
    } else {
      proto_state.set_header_text(fmt::format("{}/{}", this->_page_settings.display_order + 1, EntityManager::get_number_of_global_room_entities_pages()));
    }
  } else {
    if (this->_room != nullptr) {
      proto_state.set_header_text(fmt::format("{} {}/{}", this->_room->get_name(), this->_page_settings.display_order + 1, this->_room->get_number_of_entity_pages()));
    } else {
      proto_state.set_header_text(fmt::format("{}/{}", this->_page_settings.display_order + 1, EntityManager::get_number_of_global_room_entities_pages()));
    }
  }

  std::lock_guard<std::mutex> lock_guard(this->_entities_mutex);
  for (int i = 0; i < this->_page_settings.page_type; i++) {
    std::shared_ptr<MqttManagerEntity> entity = EntityManager::get_entity_by_page_id_and_slot(this->_id, i);
    if (entity == nullptr) {
      continue; // No entity assigned in slot, move to next slot.
    }

    if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::LIGHT) {
      std::shared_ptr<Light> light = std::static_pointer_cast<Light>(entity);
      NSPanelRoomEntitiesPage_EntitySlot *entity_slot = proto_state.add_entities();
      entity_slot->set_name(light->get_name());
      entity_slot->set_room_view_position(i);
      // TODO: Move state icons for panel from GUI_DATA to manager and convert this!
      entity_slot->set_icon(light->get_icon());
      entity_slot->set_pco(light->get_icon_color());
      entity_slot->set_pco2(light->get_icon_active_color());
      entity_slot->set_can_save_scene(false); // Entity is not a scene.
      entity_slot->set_mqtt_state_topic(light->get_mqtt_state_topic());
    } else if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY) {
      std::shared_ptr<SwitchEntity> switch_entity = std::static_pointer_cast<SwitchEntity>(entity);
      NSPanelRoomEntitiesPage_EntitySlot *entity_slot = proto_state.add_entities();
      entity_slot->set_name(switch_entity->get_name());
      entity_slot->set_room_view_position(i);
      // TODO: Move state icons for panel from GUI_DATA to manager and convert this!
      entity_slot->set_icon(switch_entity->get_icon());
      entity_slot->set_pco(switch_entity->get_icon_color());
      entity_slot->set_pco2(switch_entity->get_icon_active_color());
      entity_slot->set_mqtt_state_topic(switch_entity->get_mqtt_state_topic());
    } else if (entity->get_type() == MQTT_MANAGER_ENTITY_TYPE::SCENE) {
      std::shared_ptr<Scene> scene = std::static_pointer_cast<Scene>(entity);
      NSPanelRoomEntitiesPage_EntitySlot *entity_slot = proto_state.add_entities();
      entity_slot->set_name(scene->get_name());
      entity_slot->set_room_view_position(i);
      // TODO: Move state icons for panel from GUI_DATA to manager and convert this!
      entity_slot->set_icon(scene->get_icon());
      entity_slot->set_pco(scene->get_icon_color());
      entity_slot->set_pco2(scene->get_icon_active_color());
      entity_slot->set_mqtt_state_topic(scene->get_mqtt_state_topic());
      if (scene->get_controller() == MQTT_MANAGER_ENTITY_CONTROLLER::NSPM) {
        entity_slot->set_can_save_scene(true);
      } else {
        entity_slot->set_can_save_scene(false);
      }
    } else {
      SPDLOG_ERROR("Unknown entity type {} while processing EntityWrapper while building NSPanelRoomEntitiesPage protobuf object.", (int)entity->get_type());
    }
  }

  google::protobuf::util::MessageDifferencer differencier;
  if (!differencier.Equals(proto_state, this->_last_entities_page_protobuf_state)) {
    std::string protobuf_buffer;
    if (proto_state.SerializeToString(&protobuf_buffer)) {
      SPDLOG_DEBUG("Publishing state update to {}", this->_mqtt_state_topic);
      MQTT_Manager::publish(this->_mqtt_state_topic, protobuf_buffer, true);
      this->_last_entities_page_protobuf_state = proto_state;
    } else {
      SPDLOG_ERROR("Failed to serialize protobuf for entity page {} state.", this->_id);
    }
  } else {
    SPDLOG_DEBUG("Did not send update, no difference.");
  }
}
