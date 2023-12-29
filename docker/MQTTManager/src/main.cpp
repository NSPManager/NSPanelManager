#include "openhab_manager/openhab_manager.hpp"
#include "websocket_server/websocket_server.hpp"
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
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
std::string last_time_published;
std::string last_date_published;

void sigusr1_handler(int signal) {
  if (signal == SIGUSR1) {
    SPDLOG_INFO("Reloading config from manager.");
    MqttManagerConfig::load();
    MQTT_Manager::publish("nspanel/config/reload", "1");
    SPDLOG_INFO("Reload signal processing completed.");
  }
}

void publish_time_and_date() {
  // Read and set timezone from /etc/timezone
  std::ifstream f("/etc/timezone", std::ios::in | std::ios::binary);
  const size_t file_size = std::filesystem::file_size("/etc/timezone");
  std::string timezone_str(file_size, '\0');
  f.read(timezone_str.data(), file_size);
  f.close();

  timezone_str.erase(std::find_if(timezone_str.rbegin(), timezone_str.rend(), [](unsigned char ch) {
                       return !std::isspace(ch) && ch != '\n' && ch != '\r';
                     }).base(),
                     timezone_str.end());

  SPDLOG_INFO("Read timezone {} from /etc/timezone.", timezone_str);
  setenv("TZ", timezone_str.c_str(), 1);

  // Timezone loaded, proceed to update MQTT
  for (;;) {
    char time_buffer[20];
    char ampm_buffer[20];
    std::string time_str;
    char date_buffer[100];
    std::string date_str;

    std::time_t time = std::time({});
    std::strftime(date_buffer, 100, MqttManagerConfig::date_format.c_str(), std::localtime(&time));
    date_str = date_buffer;

    if (MqttManagerConfig::clock_us_style) {
      std::strftime(time_buffer, 20, "%I:%M", std::localtime(&time));
      std::strftime(ampm_buffer, 20, "%p", std::localtime(&time));
      time_str = time_buffer;
    } else {
      std::strftime(time_buffer, 20, "%H:%M", std::localtime(&time));
      time_str = time_buffer;
    }

    if (time_str.compare(last_time_published) != 0) {
      MQTT_Manager::publish("nspanel/status/time", time_buffer, true);
      MQTT_Manager::publish("nspanel/status/ampm", ampm_buffer, true);
      last_time_published = time_buffer;
    }

    if (date_str.compare(last_date_published) != 0) {
      MQTT_Manager::publish("nspanel/status/date", date_buffer, true);
      last_date_published = date_buffer;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
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

  std::thread mqtt_manager_thread;
  std::thread home_assistant_manager_thread;
  std::thread openhab_manager_thread;
  std::thread websocket_server_thread;
  std::thread time_and_date_thread;

  // Load config from environment/manager
  EntityManager::init();
  MqttManagerConfig::load();

  SPDLOG_INFO("Starting Websocket Server on port 8002.");
  websocket_server_thread = std::thread(WebsocketServer::start);

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

  SPDLOG_INFO("Config loaded. Starting components.");
  mqtt_manager_thread = std::thread(MQTT_Manager::connect);

  while (!MQTT_Manager::is_connected()) {
    SPDLOG_INFO("Waiting for MQTT to connect before proceeding.");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  time_and_date_thread = std::thread(publish_time_and_date);
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
