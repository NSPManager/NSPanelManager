#include "home_assistant_button.hpp"
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

HomeAssistantButton::HomeAssistantButton(uint32_t button_id) : ButtonEntity(button_id) {
  // Process Home Assistant specific details. General button data is loaded in the "Button" constructor.

  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantSwitch has not been recognized as controlled by HOME_ASSISTANT. Will stop processing switch.");
    return;
  }
  SPDLOG_DEBUG("Loaded Switch {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_entity_data.value("home_assistant_name", "NOT DEFINED!"));
}

HomeAssistantButton::~HomeAssistantButton() {
}

void HomeAssistantButton::send_state_update_to_controller() {
  std::lock_guard<std::mutex> lock_guard(this->_entity_data_mutex);
  if (!this->_entity_data.contains("home_assistant_name")) {
    SPDLOG_ERROR("Home Assistant button {}::{} has no HA item defined!", this->_id, this->_name);
    return;
  }
  std::string home_assistant_name = this->_entity_data["home_assistant_name"];

  nlohmann::json service_data;
  service_data["type"] = "call_service";
  service_data["target"]["entity_id"] = home_assistant_name;
  service_data["service"] = "press";
  if (boost::starts_with(home_assistant_name, "button.")) {
    service_data["domain"] = "button";
  } else if (boost::starts_with(home_assistant_name, "input_button.")) {
    service_data["domain"] = "input_button";
  }
  HomeAssistantManager::send_json(service_data);

  if (MqttManagerConfig::get_setting_with_default<bool>("optimistic_mode")) {
    this->_entity_changed_callbacks(this);
  }
}
