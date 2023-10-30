#include "openhab_manager/openhab_manager.hpp"
#include "websocket_server/websocket_server.hpp"
#include <chrono>
#include <cstddef>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include <entity_manager/entity_manager.hpp>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mqtt_manager_config/mqtt_manager_config.hpp>

#define SIGUSR1 10

void sigusr1_handler(int signal) {
  if (signal == SIGUSR1) {
    SPDLOG_INFO("Reloading config from manager.");
  }

  MqttManagerConfig::load();
  EntityManager::init_entities();
}

int main(void) {
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%t] %v");
  spdlog::set_level(spdlog::level::debug); // Set global log level to info

  // When a config change is made in the web interface the Django application
  // will send a SIGUSR1 signal to the MQTTManager so that the manager
  // know that it needs to reload the config.
  struct sigaction sigUsr1Handler;
  sigUsr1Handler.sa_handler = sigusr1_handler;
  sigemptyset(&sigUsr1Handler.sa_mask);
  sigUsr1Handler.sa_flags = 0;
  sigaction(SIGUSR1, &sigUsr1Handler, NULL);

  EntityManager::init();
  MqttManagerConfig::load();

  if (MqttManagerConfig::mqtt_server.empty() || MqttManagerConfig::mqtt_port == 0) {
    SPDLOG_CRITICAL("No MQTT server or port configured! Will exit with code 1.");
    return 1;
  } else if (MqttManagerConfig::manager_address.empty()) {
    SPDLOG_CRITICAL("No manager address configured. Will exit with code 2.");
    return 2;
  } else if (MqttManagerConfig::manager_port == 0) {
    SPDLOG_CRITICAL("No manager port configured. Will exit with code 3.");
    return 3;
  }

  EntityManager::init_entities();

  SPDLOG_INFO("Config loaded. Starting components.");
  std::thread mqtt_manager_thread(MQTT_Manager::connect);
  std::thread home_assistant_manager_thread;
  std::thread openhab_manager_thread;
  std::thread websocket_server_thread;

  SPDLOG_INFO("Starting Websocket Server on port 8002.");
  websocket_server_thread = std::thread(WebsocketServer::start);

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
