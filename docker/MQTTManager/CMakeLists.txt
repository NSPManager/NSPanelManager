cmake_minimum_required(VERSION 3.15)
project(nspm_mqttmanager)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
    set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++17")

elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
    set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++1z")
    set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++1z")
endif()

set(CMAKE_INCLUDE_SRC_DIRECTORY /MQTTManager/include)
set(PROTOBUF_SRC_DIRECTORY /MQTTManager/include/protobuf)

# IF(DEFINED ENV{STRIP})
#   MESSAGE("Will strip binaries!")
#   # set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#   # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
# ELSE()
#   MESSAGE("Will add debug symbols to binaries.")
# ENDIF()

add_compile_options(-rdynamic -g)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE BOOST_STACKTRACE_USE_BACKTRACE BOOST_BIND_GLOBAL_PLACEHOLDERS BOOST_STACKTRACE_LIBCXX_RUNTIME_MAY_CAUSE_MEMORY_LEAK)

# Statically link libraries
IF(DEFINED ENV{STRIP})
    MESSAGE("Will build as static binary!")
    ADD_LINK_OPTIONS(-static)
ENDIF()

# The following two lines are to check for address sanitazation.
# add_compile_options(-rdynamic -g -fsanitize=address)
# add_link_options(-fsanitize=address)

find_package(PahoMqttCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(ixwebsocket REQUIRED)
find_package(tz REQUIRED)
find_package(protobuf REQUIRED)
find_package(SqliteOrm REQUIRED)
# find_package(inja REQUIRED)
find_package(Boost REQUIRED COMPONENTS signals2 stacktrace_backtrace)

file(GLOB PROTOBUF_PB_CC_FILES "${CMAKE_INCLUDE_SRC_DIRECTORY}/protobuf/*.pb.cc")
file(GLOB PROTOBUF_PB_H_FILES "${CMAKE_INCLUDE_SRC_DIRECTORY}/protobuf/*.pb.h")
add_library(Protobuf_MQTTManager STATIC ${PROTOBUF_PB_CC_FILES} ${PROTOBUF_PB_H_FILES})
#set_target_properties(Protobuf_MQTTManager PROPERTIES PUBLIC_HEADER ${PROTOBUF_PB_H_FILES})
target_include_directories(Protobuf_MQTTManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(Protobuf_MQTTManager protobuf::protobuf)

add_library(MQTTManager_WebHelper STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/web_helper/WebHelper.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/web_helper/WebHelper.hpp)
set_target_properties(MQTTManager_WebHelper PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/WebHelper/web_helper.hpp)
target_include_directories(MQTTManager_WebHelper PUBLIC ${CURL_INCLUDE_DIR} ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_WebHelper ${CURL_LIBRARIES} spdlog::spdlog Boost::boost)

add_library(MQTTManager_DatabaseManager STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/database_manager/database_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/database_manager/database_manager.hpp)
# set_target_properties(MQTTManager_DatabaseManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/database_manager/database_manager.hpp)
target_include_directories(MQTTManager_DatabaseManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_DatabaseManager PUBLIC spdlog::spdlog sqlite_orm::sqlite_orm nlohmann_json::nlohmann_json)

add_library(MQTTManager_Entity STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity_icons.hpp)
set_target_properties(MQTTManager_Entity PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity.hpp PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity/entity_icons.hpp)
target_include_directories(MQTTManager_Entity PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Entity spdlog::spdlog Boost::boost nlohmann_json::nlohmann_json)

add_library(MQTT_Manager STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.hpp)
set_target_properties(MQTT_Manager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager/mqtt_manager.hpp)
target_include_directories(MQTT_Manager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTT_Manager PahoMqttCpp::paho-mqttpp3-static MQTTManager_Config spdlog::spdlog Boost::boost protobuf::protobuf MQTTManager_WebsocketServer)

add_library(MQTTManager_CommandManager STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/command_manager/command_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/command_manager/command_manager.hpp)
set_target_properties(MQTTManager_CommandManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/command_manager/command_manager.hpp)
target_include_directories(MQTTManager_CommandManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_CommandManager MQTT_Manager spdlog::spdlog Boost::boost Protobuf_MQTTManager nlohmann_json::nlohmann_json)

add_library(MQTTManager_WebsocketServer STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/websocket_server/websocket_server.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/websocket_server/websocket_server.hpp)
set_target_properties(MQTTManager_WebsocketServer PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/websocket_server/websocket_server.hpp)
target_include_directories(MQTTManager_WebsocketServer PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_WebsocketServer ixwebsocket::ixwebsocket spdlog::spdlog nlohmann_json::nlohmann_json Boost::boost)

add_library(MQTTManager_HomeAssistantManager STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.hpp)
set_target_properties(MQTTManager_HomeAssistantManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/home_assistant_manager/home_assistant_manager.hpp)
target_include_directories(MQTTManager_HomeAssistantManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_HomeAssistantManager Boost::stacktrace_backtrace spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket Boost::boost Boost::stacktrace_backtrace MQTTManager_Config MQTTManager_WebsocketServer)

add_library(MQTTManager_OpenhabManager STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.hpp)
set_target_properties(MQTTManager_OpenhabManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/openhab_manager/openhab_manager.hpp)
target_include_directories(MQTTManager_OpenhabManager PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_OpenhabManager Boost::stacktrace_backtrace spdlog::spdlog nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket Boost::boost MQTTManager_Config MQTTManager_WebsocketServer)

add_library(MQTTManager_Weather STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/weather/weather.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/weather/weather.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/weather/weather_icons.hpp)
set_target_properties(MQTTManager_Weather PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/weather/weather.hpp)
target_include_directories(MQTTManager_Weather PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Weather MQTTManager_WebHelper spdlog::spdlog nlohmann_json::nlohmann_json MQTTManager_Config MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager MQTT_Manager)

add_library(MQTTManager_Light STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.hpp)
set_target_properties(MQTTManager_Light PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/home_assistant_light.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/light/openhab_light.hpp)
target_include_directories(MQTTManager_Light PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Light MQTTManager_HomeAssistantManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager)

add_library(MQTTManager_Button STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/button.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/button.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/home_assistant_button.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/home_assistant_button.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/nspm_button.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/nspm_button.hpp)
set_target_properties(MQTTManager_Button PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/button/button.hpp)
target_include_directories(MQTTManager_Button PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Button MQTTManager_HomeAssistantManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager)

add_library(MQTTManager_Switch STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/switch.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/switch.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/home_assistant_switch.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/home_assistant_switch.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/openhab_switch.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/openhab_switch.hpp)
set_target_properties(MQTTManager_Switch PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/switch.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/home_assistant_switch.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/switch/openhab_switch.hpp)
target_include_directories(MQTTManager_Switch PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Switch MQTTManager_HomeAssistantManager spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager Boost::boost MQTTManager_Entity Protobuf_MQTTManager MQTTManager_CommandManager)

add_library(MQTTManager_NSPanel STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.hpp)
set_target_properties(MQTTManager_NSPanel PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/nspanel/nspanel.hpp)
target_include_directories(MQTTManager_NSPanel PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_NSPanel spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager MQTTManager_Room MQTTManager_WebsocketServer tz::tz Boost::boost MQTTManager_Config Protobuf_MQTTManager MQTTManager_DatabaseManager)

add_library(MQTTManager_Scene STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/scene.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/scene.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/nspm_scene.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/nspm_scene.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/home_assistant_scene.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/home_assistant_scene.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/openhab_scene.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scenes/openhab_scene.cpp)
# set_target_properties(MQTTManager_Scene PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/scene/scene.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/scene/nspm_scene.hpp)
target_include_directories(MQTTManager_Scene PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Scene spdlog::spdlog nlohmann_json::nlohmann_json MQTT_Manager MQTTManager_Room MQTTManager_Entity MQTTManager_Light MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager)

add_library(MQTTManager_RoomEntitiesPage STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room_entities_page.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room_entities_page.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room_entities_page.cpp)
target_include_directories(MQTTManager_RoomEntitiesPage PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_RoomEntitiesPage Boost::boost spdlog::spdlog nlohmann_json::nlohmann_json Protobuf_MQTTManager MQTTManager_WebHelper MQTTManager_Light MQTTManager_Switch MQTTManager_Scene MQTTManager_DatabaseManager)

add_library(MQTTManager_Room STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room.hpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/room/room.cpp)
target_include_directories(MQTTManager_Room PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Room MQTTManager_RoomEntitiesPage spdlog::spdlog nlohmann_json::nlohmann_json MQTTManager_Entity MQTTManager_Light MQTTManager_Switch Protobuf_MQTTManager MQTT_Manager MQTTManager_DatabaseManager)

add_library(MQTTManager_EntityManager STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.hpp)
set_target_properties(MQTTManager_EntityManager PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/entity_manager/entity_manager.hpp)
target_include_directories(MQTTManager_EntityManager PUBLIC MQTTManager_WebHelper ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(MQTTManager_EntityManager MQTTManager_WebHelper spdlog::spdlog MQTT_Manager MQTTManager_NSPanel MQTTManager_Light MQTTManager_Button MQTTManager_Switch MQTTManager_WebsocketServer Boost::boost Boost::stacktrace_backtrace dl MQTTManager_Weather)

add_library(MQTTManager_Config STATIC ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.cpp ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.hpp)
# set_target_properties(MQTTManager_Config PROPERTIES PUBLIC_HEADER ${CMAKE_INCLUDE_SRC_DIRECTORY}/mqtt_manager_config/mqtt_manager_config.hpp)
target_include_directories(MQTTManager_Config PUBLIC MQTTManager_WebHelper ${CMAKE_INCLUDE_SRC_DIRECTORY} ${PROTOBUF_SRC_DIRECTORY})
target_link_libraries(MQTTManager_Config spdlog::spdlog MQTTManager_WebHelper nlohmann_json::nlohmann_json Boost::boost Protobuf_MQTTManager MQTTManager_DatabaseManager)


add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_INCLUDE_SRC_DIRECTORY})
target_link_libraries(${PROJECT_NAME} MQTT_Manager MQTTManager_Config MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager MQTTManager_EntityManager MQTTManager_Scene MQTTManager_Room MQTTManager_WebsocketServer MQTTManager_CommandManager)
