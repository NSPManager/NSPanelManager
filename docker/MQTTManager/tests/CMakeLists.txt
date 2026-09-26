# Unit tests for MQTTManager. Built when NSPM_BUILD_TESTS is ON.
#
# Tests live in this directory (*_test.cpp). The older inline tests inside the library
# sources (wrapped in `#if TEST_MODE == 1`) are linked in too, so this one executable
# runs everything.

file(GLOB NSPM_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

add_executable(nspm_mqttmanager_tests main.cpp ${NSPM_TEST_SOURCES})
target_include_directories(nspm_mqttmanager_tests PRIVATE ${CMAKE_INCLUDE_SRC_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})

# Libraries containing inline tests. They are static libraries, and the linker only pulls in
# object files something references, so they are linked whole to make sure every inline test
# registers with GoogleTest. (CMake's WHOLE_ARCHIVE link feature cannot be used because the
# same libraries are also linked transitively. This is a single call because
# target_link_options drops repeated options, which would unwrap every library after the first.)
set(NSPM_LIBRARIES_WITH_INLINE_TESTS
    MQTTManager_Config
    MQTTManager_Light
)
set(NSPM_INLINE_TEST_ARCHIVES)
foreach(library ${NSPM_LIBRARIES_WITH_INLINE_TESTS})
    list(APPEND NSPM_INLINE_TEST_ARCHIVES "$<TARGET_FILE:${library}>")
endforeach()
target_link_options(nspm_mqttmanager_tests PRIVATE "LINKER:--whole-archive" ${NSPM_INLINE_TEST_ARCHIVES} "LINKER:--no-whole-archive")

target_link_libraries(nspm_mqttmanager_tests PRIVATE
    ${NSPM_LIBRARIES_WITH_INLINE_TESTS}
    MQTTManager_DatabaseManager MQTTManager_Entity
    MQTT_Manager MQTTManager_HomeAssistantManager MQTTManager_OpenhabManager MQTTManager_EntityManager
    MQTTManager_Scene MQTTManager_Room MQTTManager_WebsocketServer MQTTManager_CommandManager
    gtest::gtest
)

gtest_discover_tests(nspm_mqttmanager_tests DISCOVERY_TIMEOUT 60)
