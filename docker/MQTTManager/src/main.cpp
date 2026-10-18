#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "manager_clock/manager_clock.hpp"
#include "openhab_manager/openhab_manager.hpp"
#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "websocket_server/websocket_server.hpp"
#include <boost/algorithm/minmax.hpp>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <entity_manager/entity_manager.hpp>
#include <filesystem>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <memory>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <nextion_image_server/nextion_image_server.hpp>
#include <signal.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <vector>

#if defined(TEST_MODE) && TEST_MODE == 1
#include <gtest/gtest.h>
#endif

#define SIGUSR1 10

void sigusr1_handler(int signal) {
  if (signal == SIGUSR1) {
    SPDLOG_INFO("Reloading config from manager. Will wait 50ms for other processing to complete before proceeding with config reload.");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    MqttManagerConfig::load();
    MQTT_Manager::reload_config();
    HomeAssistantManager::reload_config();
    OpenhabManager::reload_config();
    EntityManager::load_entities();

    MQTT_Manager::publish("nspanel/config/reload", "1");
    SPDLOG_INFO("Reload signal processing completed.");
  }
}

int main(int argc, char *argv[]) {
// If we are in test mode, don't start the MQTTManager. Simply run all test then exit.
#if defined(TEST_MODE) && TEST_MODE == 1
  database_manager::init();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#endif

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

  char *log_level = std::getenv("LOG_LEVEL");
  if (log_level != NULL) {
    std::string log_level_str = log_level;
    if (log_level_str.compare("error") == 0) {
      spdlog::set_level(spdlog::level::err);
    } else if (log_level_str.compare("warning") == 0) {
      spdlog::set_level(spdlog::level::warn);
    } else if (log_level_str.compare("info") == 0) {
      spdlog::set_level(spdlog::level::info);
    } else if (log_level_str.compare("debug") == 0) {
      spdlog::set_level(spdlog::level::debug);
    } else if (log_level_str.compare("trace") == 0) {
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

  // Ignore SIGPIPE signals to prevent crashes when writing/reading to a closed socket.
  signal(SIGPIPE, SIG_IGN);

  if (sqlite3_threadsafe() == 0) {
    SPDLOG_WARN("SQLite3 compiled NOT threadsafe, ie. without mutexes!");
  } else {
    SPDLOG_INFO("SQLite3 seems to be compiled with threadsafe mutexes. Setting: {}", sqlite3_threadsafe());
  }
  database_manager::init();

  // Load config from environment/manager
  MqttManagerConfig::load();   // Load all entities, rooms and panels.
  MQTTManagerWeather::start(); // Start thread to handle weather updates

  std::thread home_assistant_manager_thread;
  std::thread openhab_manager_thread;
  std::thread websocket_server_thread;
  std::thread nextion_image_server_thread;

  SPDLOG_INFO("Starting Websocket Server on port 8002.");
  websocket_server_thread = std::thread(WebsocketServer::start);

  SPDLOG_INFO("Starting Nextion image server on port 8003.");
  nextion_image_server_thread = std::thread(NextionImageServer::start);

  SPDLOG_INFO("Config loaded. Starting components.");

  ManagerClock::start(); // Send the time and date to NSPanels every minute and each time MQTT connects
  home_assistant_manager_thread = std::thread(HomeAssistantManager::connect);
  openhab_manager_thread = std::thread(OpenhabManager::connect);

  SPDLOG_INFO("Home Assistant and/or OpenHAB initialized. Will load entities.");
  EntityManager::init();

  SPDLOG_INFO("Entities loaded, connect to MQTT and start listening for commands.");
  MQTT_Manager::init(); // Initialize MQTT manager to handle everything to do with MQTT
  CommandManager::init();

  // Wait for threads to exit
  if (home_assistant_manager_thread.joinable()) {
    home_assistant_manager_thread.join();
  }
  if (openhab_manager_thread.joinable()) {
    openhab_manager_thread.join();
  }

  // Sleep forever in main thread as to keep manager alive.
  // Config changes may come from web interface that will restart threads that has exited as they have no
  // settings to load.
  while (true) {
    std::this_thread::sleep_for(std::chrono::minutes(1));
  }

  return EXIT_FAILURE;
}
