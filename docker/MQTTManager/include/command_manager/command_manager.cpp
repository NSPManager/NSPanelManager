#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/bind/bind.hpp>
#include <command_manager/command_manager.hpp>
#include <mqtt_manager/mqtt_manager.hpp>
#include <spdlog/spdlog.h>

void CommandManager::init() {
  MQTT_Manager::subscribe(fmt::format("nspanel/mqttmanager_{}/command", MqttManagerConfig::get_settings().manager_address()), &CommandManager::process_command);
}

void CommandManager::process_command(const std::string &topic, const std::string &payload) {
  SPDLOG_DEBUG("Received command from MQTT.");
  SPDLOG_TRACE("Command: {}", payload);
}
