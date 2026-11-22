#include "nspm_button.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_general.pb.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <cstdint>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <switch/switch.hpp>

NSPMButton::NSPMButton(uint32_t button_id) : ButtonEntity(button_id) {
  // Process Home Assistant specific details. General button data is loaded in the "Button" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantSwitch has not been recognized as controlled by HOME_ASSISTANT. Will stop processing switch.");
    return;
  }

  auto db_switch = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = db_switch.get_entity_data_json();
  if (entity_data.contains("home_assistant_name")) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No Home Assistant name defined for switch {}::{}", this->_id, this->_name);
  }
  SPDLOG_DEBUG("Loaded Switch {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
}

NSPMButton::~NSPMButton() {
}

void NSPMButton::send_state_update_to_controller() {
  auto db_switch = database_manager::database.get<database_manager::Entity>(this->_id);
  nlohmann::json entity_data = db_switch.get_entity_data_json();
  if (entity_data.contains("mqtt_topic")) {
    if (entity_data.contains("mqtt_payload")) {
      MQTT_Manager::publish(std::string(entity_data["mqtt_topic"]), std::string(entity_data["mqtt_payload"]));
    }
  }
}
