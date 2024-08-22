#include "openhab_manager/openhab_manager.hpp"
#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "websocket_server/websocket_server.hpp"
#include <boost/algorithm/minmax.hpp>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <ipc_handler/ipc_handler.hpp>
#include <memory>
#include <signal.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include <entity_manager/entity_manager.hpp>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <vector>

#define SIGUSR1 10
std::string last_time_published;
std::string last_date_published;

void sigusr1_handler(int signal) {
  if (signal == SIGUSR1) {
    SPDLOG_INFO("Reloading config from manager. Will wait 50ms for other processing to complete before proceeding with config reload.");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    MqttManagerConfig::load();
    MQTT_Manager::publish("nspanel/config/reload", "1");
    SPDLOG_INFO("Reload signal processing completed.");
  }
}

void publish_time_and_date() {
  // Set timezone
  setenv("TZ", MqttManagerConfig::timezone.c_str(), 1);
  tzset();

  // Timezone loaded, proceed to update MQTT
  for (;;) {
    char time_buffer[20];
    char ampm_buffer[20];
    std::string time_str;
    char date_buffer[100];
    std::string date_str;

    std::time_t time = std::time({});
    std::strftime(date_buffer, 100, MqttManagerConfig::get_settings().date_format().c_str(), std::localtime(&time));
    date_str = date_buffer;

    if (MqttManagerConfig::get_settings().clock_format() == time_format::AM_PM) {
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
      MQTT_Manager::publish("nspanel/status/ampm", "", true);
      last_date_published = date_buffer;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

int main(void) {
  SPDLOG_INFO("Starting MQTTManager.");

  std::filesystem::path log_partition_path = "/dev/shm/";
  std::filesystem::space_info log_partition_space_info = std::filesystem::space(log_partition_path);
  const uintmax_t free_space = log_partition_space_info.free;
  const uintmax_t free_space_10_percent = free_space * 0.10; // 10% of free space.
  const uintmax_t max_log_size = 1048576 * 30;

  SPDLOG_INFO("/dev/shm has {} bytes free.", log_partition_space_info.free);

  // Setup logging
  auto max_size = boost::minmax(free_space_10_percent, free_space).head; // Grow log file to 10% of free space or max 30MB and then rotate

  SPDLOG_INFO("Will log to /dev/shm/mqttmanager.log with a max size of {} bytes.", max_size);

  std::vector<spdlog::sink_ptr> spdlog_sinks;
  spdlog_sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>()); // Log to console
  if (max_size > 1024) {
    spdlog_sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>("/dev/shm/mqttmanager.log", max_size, 1)); // Log to rotating log file
  } else {
    SPDLOG_ERROR("Not enough storage/momory available on /dev/shm partition. Will not log to file!");
  }

  // Setup logging to console and file
  auto combined_logger = std::make_shared<spdlog::logger>("combined_logger", begin(spdlog_sinks), end(spdlog_sinks));
  spdlog::register_logger(combined_logger);
  spdlog::set_default_logger(combined_logger);

  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%t] %v");

  std::string log_level = std::getenv("LOG_LEVEL");
  if (log_level.size()) {
    if (log_level.compare("error") == 0) {
      spdlog::set_level(spdlog::level::err);
    } else if (log_level.compare("warning") == 0) {
      spdlog::set_level(spdlog::level::warn);
    } else if (log_level.compare("info") == 0) {
      spdlog::set_level(spdlog::level::info);
    } else if (log_level.compare("debug") == 0) {
      spdlog::set_level(spdlog::level::debug);
    } else if (log_level.compare("trace") == 0) {
      spdlog::set_level(spdlog::level::trace);
    } else {
      SPDLOG_INFO("No log level was set by 'LOG_LEVEL' environment variable. Will assume level debug.");
      spdlog::set_level(spdlog::level::debug);
    }
  } else {
    SPDLOG_INFO("No log level was set by 'LOG_LEVEL' environment variable. Will assume level debug.");
    spdlog::set_level(spdlog::level::debug);
  }

  // When a config change is made in the web interface the Django application
  // will send a SIGUSR1 signal to the MQTTManager so that the manager
  // know that it needs to reload the config.
  struct sigaction sigUsr1Handler;
  sigUsr1Handler.sa_handler = sigusr1_handler;
  sigemptyset(&sigUsr1Handler.sa_mask);
  sigUsr1Handler.sa_flags = 0;
  sigaction(SIGUSR1, &sigUsr1Handler, NULL);

  IPCHandler::start();

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

  if (MqttManagerConfig::get_private_settings().mqtt_server().empty() || MqttManagerConfig::get_private_settings().mqtt_server_port() == 0) {
    SPDLOG_CRITICAL("No MQTT server or port configured! Will exit with code 1.");
    return 1;
  } else if (MqttManagerConfig::get_settings().manager_address().empty()) {
    SPDLOG_CRITICAL("No manager address configured. Will exit with code 2.");
    return 2;
  } else if (MqttManagerConfig::get_settings().manager_port() == 0) {
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
  if (!MqttManagerConfig::get_private_settings().home_assistant_address().empty() && !MqttManagerConfig::get_private_settings().home_assistant_token().empty()) {
    SPDLOG_INFO("Home Assistant address and access token configured. Starting Home Assistant component.");
    home_assistant_manager_thread = std::thread(HomeAssistantManager::connect);
  } else {
    SPDLOG_WARN("Home Assistant address and/or token missing. Won't start Home Assistant component.");
  }

  if (!MqttManagerConfig::get_private_settings().openhab_address().empty() && !MqttManagerConfig::get_private_settings().openhab_token().empty()) {
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
