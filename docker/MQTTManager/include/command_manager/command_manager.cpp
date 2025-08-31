#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include <boost/bind/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <command_manager/command_manager.hpp>
#include <exception>
#include <mqtt_manager/mqtt_manager.hpp>
#include <spdlog/spdlog.h>

void CommandManager::init() {
  MQTT_Manager::subscribe(fmt::format("nspanel/mqttmanager_{}/command", MqttManagerConfig::get_setting_with_default<std::string>("manager_address")), &CommandManager::process_command);
  SPDLOG_INFO("CommandManager initialized.");
}

void CommandManager::process_command(const std::string &topic, const std::string &payload) {
  SPDLOG_TRACE("Received command from MQTT: {}", payload);
  try {
    NSPanelMQTTManagerCommand command;
    if (command.ParseFromString(payload)) {
      SPDLOG_TRACE("Calling callbacks.");
      CommandManager::_callbacks(command);
      SPDLOG_TRACE("Callback completed.");
    } else {
      SPDLOG_ERROR("Failed to parse payload on MQTT command topic into NSPanelMQTTManagerCommand protobuf object. Will ignore it.");
    }
  } catch (std::exception &ex) {
    SPDLOG_ERROR("Caught exception during processing of command on MQTT command topic. Exception: {}", boost::diagnostic_information(ex));
  }
}
