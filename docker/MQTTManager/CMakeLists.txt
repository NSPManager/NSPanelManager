cmake_minimum_required(VERSION 3.23)
project(nspm_mqttmanager)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(TEST_MODE 0)
set(CMAKE_INCLUDE_SRC_DIRECTORY /MQTTManager/include)
set(PROTOBUF_SRC_DIRECTORY /MQTTManager/include/protobuf)

add_compile_options(-rdynamic -g)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE BOOST_STACKTRACE_USE_BACKTRACE BOOST_BIND_GLOBAL_PLACEHOLDERS BOOST_STACKTRACE_LIBCXX_RUNTIME_MAY_CAUSE_MEMORY_LEAK)
add_compile_definitions(TEST_MODE=0)

if(DEFINED ENV{STRIP})
    message("Will build as static binary!")
    add_link_options(-static)
endif()

# The following two lines are to check for address sanitazation.
# add_compile_options(-rdynamic -g -fsanitize=address)
# add_link_options(-fsanitize=address)

# Dependencies
find_package(PahoMqttCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(ixwebsocket REQUIRED)
find_package(tz REQUIRED)
find_package(protobuf REQUIRED)
find_package(SqliteOrm REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS signals2 stacktrace_backtrace)

include(GoogleTest)

# === Protobuf ===
file(GLOB PROTOBUF_PB_CC_FILES "${CMAKE_INCLUDE_SRC_DIRECTORY}/protobuf/*.pb.cc")
file(GLOB PROTOBUF_PB_H_FILES "${CMAKE_INCLUDE_SRC_DIRECTORY}/protobuf/*.pb.h")

add_library(Protobuf_MQTTManager STATIC ${PROTOBUF_PB_CC_FILES})
target_sources(Protobuf_MQTTManager
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_INCLUDE_SRC_DIRECTORY}/protobuf
        FILES ${PROTOBUF_PB_H_FILES}
)
target_include_directories(Protobuf_MQTTManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(Protobuf_MQTTManager protobuf::protobuf)

# === Helper Macro ===
function(add_mqtt_static_lib name base_dir sources headers)
    add_library(${name} STATIC ${sources})
    target_sources(${name}
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${base_dir}
            FILES ${headers}
    )
    target_include_directories(${name} PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
endfunction()

# === Individual Libraries ===

# MQTTManager_WebHelper
add_mqtt_static_lib(MQTTManager_WebHelper
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/web_helper
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/web_helper/WebHelper.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/web_helper/WebHelper.hpp"
)
target_include_directories(MQTTManager_WebHelper PUBLIC ${CURL_INCLUDE_DIR})
target_link_libraries(MQTTManager_WebHelper ${CURL_LIBRARIES} spdlog::spdlog Boost::boost gtest::gtest)

# MQTTManager_DatabaseManager
add_mqtt_static_lib(MQTTManager_DatabaseManager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/database_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/database_manager/database_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/database_manager/database_manager.hpp"
)
target_link_libraries(MQTTManager_DatabaseManager spdlog::spdlog sqlite_orm::sqlite_orm nlohmann_json::nlohmann_json gtest::gtest)

# MQTTManager_Entity
add_mqtt_static_lib(MQTTManager_Entity
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity_icons.hpp"
)
target_link_libraries(MQTTManager_Entity spdlog::spdlog Boost::boost nlohmann_json::nlohmann_json gtest::gtest)

# MQTT_Manager
add_mqtt_static_lib(MQTT_Manager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.hpp"
)
target_link_libraries(MQTT_Manager PahoMqttCpp::paho-mqttpp3-static MQTTManager_Config spdlog::spdlog Boost::boost protobuf::protobuf MQTTManager_WebsocketServer gtest::gtest)

# MQTTManager_CommandManager
add_mqtt_static_lib(MQTTManager_CommandManager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/command_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/command_manager/command_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/command_manager/command_manager.hpp"
)
target_link_libraries(MQTTManager_CommandManager MQTT_Manager spdlog::spdlog Boost::boost Protobuf_MQTTManager nlohmann_json::nlohmann_json gtest::gtest)

# MQTTManager_WebsocketServer
add_mqtt_static_lib(MQTTManager_WebsocketServer
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/websocket_server
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/websocket_server/websocket_server.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/websocket_server/websocket_server.hpp"
)
target_link_libraries(MQTTManager_WebsocketServer ixwebsocket::ixwebsocket spdlog::spdlog nlohmann_json::nlohmann_json Boost::boost gtest::gtest)

# MQTTManager_HomeAssistantManager
add_mqtt_static_lib(MQTTManager_HomeAssistantManager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.hpp"
)
target_link_libraries(MQTTManager_HomeAssistantManager Boost::stacktrace_backtrace spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket Boost::boost MQTTManager_Config MQTTManager_WebsocketServer gtest::gtest)

# MQTTManager_OpenhabManager
add_mqtt_static_lib(MQTTManager_OpenhabManager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.hpp"
)
target_link_libraries(MQTTManager_OpenhabManager Boost::stacktrace_backtrace spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket Boost::boost MQTTManager_Config MQTTManager_WebsocketServer gtest::gtest)

# MQTTManager_HomeyManager
add_mqtt_static_lib(MQTTManager_HomeyManager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/homey_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/homey_manager/homey_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/homey_manager/homey_manager.hpp"
)
target_link_libraries(MQTTManager_HomeyManager Boost::stacktrace_backtrace spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket Boost::boost MQTTManager_Config MQTTManager_WebsocketServer gtest::gtest)

# MQTTManager_Weather
add_mqtt_static_lib(MQTTManager_Weather
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/weather
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/weather/weather.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/weather/weather.hpp"
)
target_link_libraries(MQTTManager_Weather MQTTManager_WebHelper spdlog::spdlog nlohmann_json::nlohmann_json MQTTManager_Config MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager MQTT_Manager gtest::gtest)

# MQTTManager_Light
add_mqtt_static_lib(MQTTManager_Light
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/light
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/light/homey_light.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/light/homey_light.hpp"
)
target_link_libraries(MQTTManager_Light MQTTManager_HomeAssistantManager MQTTManager_HomeyManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager gtest::gtest)

# MQTTManager_Button
add_mqtt_static_lib(MQTTManager_Button
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/button
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/button/button.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/button/home_assistant_button.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/button/nspm_button.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/button/homey_button.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/button/button.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/button/home_assistant_button.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/button/nspm_button.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/button/homey_button.hpp"
)
target_link_libraries(MQTTManager_Button MQTTManager_HomeAssistantManager MQTTManager_HomeyManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager gtest::gtest)

# MQTTManager_Thermostat
add_mqtt_static_lib(MQTTManager_Thermostat
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat/thermostat.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat/home_assistant_thermostat.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat/homey_thermostat.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat/thermostat.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat/home_assistant_thermostat.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/thermostat/homey_thermostat.hpp"
)
target_link_libraries(MQTTManager_Thermostat MQTTManager_HomeAssistantManager MQTTManager_HomeyManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager MQTTManager_WebHelper gtest::gtest)

# MQTTManager_Switch
add_mqtt_static_lib(MQTTManager_Switch
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/switch.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/home_assistant_switch.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/openhab_switch.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/homey_switch.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/switch.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/home_assistant_switch.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/openhab_switch.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/homey_switch.hpp"
)
target_link_libraries(MQTTManager_Switch MQTTManager_HomeAssistantManager MQTTManager_HomeyManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager gtest::gtest)

# MQTTManager_NSPanel
add_mqtt_static_lib(MQTTManager_NSPanel
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.hpp"
)
target_link_libraries(MQTTManager_NSPanel spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager MQTTManager_Room MQTTManager_WebsocketServer tz::tz Boost::boost MQTTManager_Config Protobuf_MQTTManager MQTTManager_DatabaseManager gtest::gtest)

# MQTTManager_Scene
add_mqtt_static_lib(MQTTManager_Scene
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/scene.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/nspm_scene.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/home_assistant_scene.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/openhab_scene.cpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/homey_scene.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/scene.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/nspm_scene.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/home_assistant_scene.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/openhab_scene.hpp;${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/homey_scene.hpp"
)
target_link_libraries(MQTTManager_Scene spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager MQTTManager_Room MQTTManager_Entity MQTTManager_Light MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager MQTTManager_HomeyManager gtest::gtest)

# MQTTManager_RoomEntitiesPage
add_mqtt_static_lib(MQTTManager_RoomEntitiesPage
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/room
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room_entities_page.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room_entities_page.hpp"
)
target_link_libraries(MQTTManager_RoomEntitiesPage Boost::boost spdlog::spdlog nlohmann_json::nlohmann_json Protobuf_MQTTManager MQTTManager_WebHelper MQTTManager_Light MQTTManager_Switch MQTTManager_Scene MQTTManager_DatabaseManager gtest::gtest)

# MQTTManager_Room
add_mqtt_static_lib(MQTTManager_Room
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/room
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room.hpp"
)
target_link_libraries(MQTTManager_Room MQTTManager_RoomEntitiesPage spdlog::spdlog nlohmann_json::nlohmann_json MQTTManager_Entity MQTTManager_Light MQTTManager_Switch Protobuf_MQTTManager MQTT_Manager MQTTManager_DatabaseManager gtest::gtest)

# MQTTManager_EntityManager
add_mqtt_static_lib(MQTTManager_EntityManager
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.hpp"
)
target_link_libraries(MQTTManager_EntityManager MQTTManager_WebHelper spdlog::spdlog MQTT_Manager MQTTManager_NSPanel MQTTManager_Light MQTTManager_Button MQTTManager_Thermostat MQTTManager_Switch MQTTManager_WebsocketServer Boost::boost Boost::stacktrace_backtrace dl MQTTManager_Weather MQTTManager_HomeyManager gtest::gtest)

# MQTTManager_Config
add_mqtt_static_lib(MQTTManager_Config
    ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.cpp"
    "${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.hpp"
)
target_include_directories(MQTTManager_Config PUBLIC ${PROTOBUF_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Config spdlog::spdlog MQTTManager_WebHelper nlohmann_json::nlohmann_json Boost::boost Protobuf_MQTTManager MQTTManager_DatabaseManager MQTTManager_WebsocketServer gtest::gtest)

# === Executable ===
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(${PROJECT_NAME}
    MQTT_Manager
    MQTTManager_Config
    MQTTManager_HomeAssistantManager
    MQTTManager_OpenhabManager
    MQTTManager_HomeyManager
    MQTTManager_EntityManager
    MQTTManager_Scene
    MQTTManager_Room
    MQTTManager_WebsocketServer
    MQTTManager_CommandManager
    gtest::gtest
)

# === Tests ===
if (TEST_MODE EQUAL 1)
    enable_testing()
    gtest_discover_tests(${PROJECT_NAME})
endif()
