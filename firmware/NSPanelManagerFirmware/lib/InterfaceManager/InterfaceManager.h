#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <NSPanel.h>
#include <list>
#include <string>
#include <PubSubClient.h>
#include <ArduinoJson.h>

struct lightConfig
{
    uint8_t id = 0;
    uint8_t level = 0;
    bool isCeiling = false;
    bool canDim = false;
    bool canTemperature = false;
    bool canRgb = false;
    std::string name;
};

class roomConfig
{
public:
    uint8_t id = 0;
    std::string name;
    std::list<lightConfig> ceilingLights;
    std::list<lightConfig> tableLights;
    std::list<lightConfig*> getCeilingLightsThatAreOn();
    std::list<lightConfig*> getTableLightsThatAreOn();
    std::list<lightConfig*> getAllCeilingLights();
    std::list<lightConfig*> getAllTableLights();
    bool anyCeilingLighstOn();
    bool anyTableLighstOn();
    bool anyLightsOn();
};

struct interfaceConfig
{
    uint8_t homeScreen = 0;
    std::list<roomConfig> rooms;
    std::list<roomConfig>::iterator currentRoom;
};

struct mqttMessage
{
    std::string topic;
    std::string payload;
};

enum roomMode
{
    room,
    house,
    END // Keep END at end of enum
};

class InterfaceManager
{
public:
    void init(PubSubClient *mqttClient);
    static void processTouchEvent(uint8_t, uint8_t, bool);
    static void mqttCallback(char *topic, byte *payload, unsigned int length);
    static uint8_t roundToNearest(uint8_t original, uint8_t step);
    static void subscribeToMqttTopics();

private:
    static inline InterfaceManager *_instance;
    static void _taskLoadConfigAndInit(void *param);
    static inline std::list<mqttMessage> _mqttMessages;
    static inline TaskHandle_t _taskHandleProcessMqttMessages;
    static void _taskProcessMqttMessages(void *param);
    PubSubClient *_mqttClient;
    roomMode _currentRoomMode;
    DynamicJsonDocument *_roomDataJson;
    interfaceConfig _cfg;
    void _goToNextRoom();
    void _changeRoom(uint8_t roomId);
    void _updatePanelWithNewRoomInfo();
    void _updatePanelLightStatus();
    void _goToNextMode();
    void _updateHomeValues();
    void _changeMode(roomMode mode);
    bool _getPanelConfig();
    void _processPanelConfig();

    void _ceilingMasterButtonEvent();
    void _tableMasterButtonEvent();
    void _updateLightsThatAreOn(); // Set light level for all lights that are currently on
    void _updateAllLights(); // Send light level to all lights
    void _updateLightsColorTemp(); // Send new color temp to all lights

    /// @brief Set internal light representation level
    /// @param light The light to set value for
    /// @param level The new value
    void _setLightLevel(std::string light, uint8_t level);

    /// @brief Send out new value for a list of lights
    /// @param lights The lights to set value for
    /// @param level The new value
    void _changeLightsToLevel(std::list<lightConfig*> *lights, uint8_t level);
    void _changeLightsToKelvin(std::list<lightConfig*> *lights, uint16_t kelvin);
};

#endif
