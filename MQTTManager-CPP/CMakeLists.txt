cmake_minimum_required(VERSION 3.15)
project(nspm_mqttmanager)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /usr/src/app/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /usr/src/app/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /usr/src/app/bin)
set(CMAKE_INCLUDE_SRC_DIRECTORY /usr/src/app/include)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(PahoMqttCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(ixwebsocket REQUIRED)


add_library(MQTT_Manager SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.hpp)
set_target_properties(MQTT_Manager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.hpp)
target_include_directories(MQTT_Manager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTT_Manager PahoMqttCpp::paho-mqttpp3-static MQTTManager_Config spdlog::spdlog)

add_library(MQTTManager_HomeAssistantManager SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.hpp)
set_target_properties(MQTTManager_HomeAssistantManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.hpp)
target_include_directories(MQTTManager_HomeAssistantManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_HomeAssistantManager spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket)

add_library(MQTTManager_OpenhabManager SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.hpp)
set_target_properties(MQTTManager_OpenhabManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.hpp)
target_include_directories(MQTTManager_OpenhabManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_OpenhabManager spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket)

add_library(MQTTManager_Light SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.hpp)
set_target_properties(MQTTManager_Light PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.hpp)
target_include_directories(MQTTManager_Light PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Light MQTTManager_HomeAssistantManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager)

#include "nspanel/nspanel.hpp"
add_library(MQTTManager_NSPanel SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.hpp)
set_target_properties(MQTTManager_NSPanel PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.hpp)
target_include_directories(MQTTManager_NSPanel PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_NSPanel spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager)

add_library(MQTTManager_EntityManager SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.hpp)
set_target_properties(MQTTManager_EntityManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.hpp)
target_include_directories(MQTTManager_EntityManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_EntityManager spdlog::spdlog MQTT_Manager MQTTManager_NSPanel MQTTManager_Light)

add_library(MQTTManager_Config SHARED ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.hpp)
set_target_properties(MQTTManager_Config PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.hpp)
target_include_directories(MQTTManager_Config PUBLIC ${CURL_INCLUDE_DIR} ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Config spdlog::spdlog ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)


add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(${PROJECT_NAME} MQTT_Manager MQTTManager_Config MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager MQTTManager_EntityManager)
