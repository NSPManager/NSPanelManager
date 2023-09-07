#include "openhab_manager/openhab_manager.hpp"
#include <chrono>
#include <cstddef>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include <entity_manager/entity_manager.hpp>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mqtt_manager_config/mqtt_manager_config.hpp>

int main(void) {
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%t] %v");
  spdlog::set_level(spdlog::level::debug); // Set global log level to info

  EntityManager::init();
  MqttManagerConfig::load();
  EntityManager::init_entities();

  SPDLOG_INFO("Config loaded. Starting components.");
  std::thread mqtt_manager_thread(MQTT_Manager::connect);
  std::thread home_assistant_manager_thread;
  std::thread openhab_manager_thread;

  while (!MQTT_Manager::is_connected()) {
    SPDLOG_INFO("Waiting for MQTT to connect before proceeding.");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  if (!MqttManagerConfig::home_assistant_address.empty() && !MqttManagerConfig::home_assistant_access_token.empty()) {
    SPDLOG_INFO("Home Assistant address and access token configured. Starting Home Assistant component.");
    home_assistant_manager_thread = std::thread(HomeAssistantManager::connect);
  } else {
    SPDLOG_WARN("Home Assistant address and/or token missing. Won't start Home Assistant component.");
  }

  if (!MqttManagerConfig::openhab_address.empty() && !MqttManagerConfig::openhab_access_token.empty()) {
    SPDLOG_INFO("Openhab address and access token configured. Starting Openhab component.");
    openhab_manager_thread = std::thread(OpenhabManager::connect);
  } else {
    SPDLOG_WARN("OpenHAB address and/or token missing. Won't start OpenHAB component.");
  }

  // Wait for threads to exit
  mqtt_manager_thread.join();
  if (home_assistant_manager_thread.joinable()) {
    home_assistant_manager_thread.join();
  }
  if (openhab_manager_thread.joinable()) {
    home_assistant_manager_thread.join();
  }

  return EXIT_SUCCESS;
}
