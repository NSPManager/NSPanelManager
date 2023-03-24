#ifndef NSPM_ROOM_CONFIG_H
#define NSPM_ROOM_CONFIG_H

#include <string>
#include <list>

/// @brief MQTT Messages container
struct mqttMessage
{
    std::string topic;
    std::string payload;
};

/// @brief Edit mode (room or house)
enum roomMode
{
    room,
    house,
    END // Keep END at end of enum
};

/// @brief Light mode (all lights, ceiling lights or table lights)
enum editLightMode
{
    all_lights,
    ceiling_lights,
    table_lights
};

/// @brief Object used to represent a light with all its configuration
struct lightConfig
{
    uint8_t id = 0;
    uint8_t level = 0;
    uint8_t colorTemperature = 0;
    bool isCeiling = false;
    bool canDim = false;
    bool canTemperature = false;
    bool canRgb = false;
    std::string name;
};

/// @brief Object/container used to represent a room
class roomConfig
{
public:
    uint8_t id = 0;
    std::string name;
    std::list<lightConfig> ceilingLights;
    std::list<lightConfig> tableLights;
    std::list<lightConfig*> getCeilingLightsThatAreOn();
    std::list<lightConfig*> getTableLightsThatAreOn();
    std::list<lightConfig*> getAllLightsThatAreOn();
    std::list<lightConfig*> getAllCeilingLights();
    std::list<lightConfig*> getAllTableLights();
    std::list<lightConfig*> getAllLights();
    bool anyCeilingLightsOn();
    bool anyTableLightstOn();
    bool anyLightsOn();
};

/// @brief Configuration for the hole NSPanel
struct interfaceConfig
{
    uint8_t homeScreen = 0;
    std::list<roomConfig> rooms;
    std::list<roomConfig>::iterator currentRoom;
    std::list<lightConfig*> getCeilingLightsThatAreOn();
    std::list<lightConfig*> getTableLightsThatAreOn();
    std::list<lightConfig*> getAllLightsThatAreOn();
    std::list<lightConfig*> getAllCeilingLights();
    std::list<lightConfig*> getAllTableLights();
    std::list<lightConfig*> getAllLights();
    bool anyCeilingLightsOn();
    bool anyTableLightstOn();
    bool anyLightsOn();
};

#endif