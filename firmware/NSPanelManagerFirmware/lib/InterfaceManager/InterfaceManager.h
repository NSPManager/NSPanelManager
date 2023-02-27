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

struct roomConfig
{
    uint8_t id = 0;
    std::string name;
    std::list<lightConfig> ceilingLights;
    std::list<lightConfig> tableLights;
};

struct interfaceConfig
{
    uint8_t homeScreen = 0;
    std::list<roomConfig> rooms;
    std::list<roomConfig>::iterator currentRoom;
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

private:
    static inline InterfaceManager *_instance;
    static void _taskLoadConfigAndInit(void *param);
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

    void _setLightLevel(std::string light, uint8_t level);
};

#endif