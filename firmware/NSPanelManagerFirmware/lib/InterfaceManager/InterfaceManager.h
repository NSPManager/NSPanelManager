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

class InterfaceManager
{
public:
    void init(PubSubClient *mqttClient);
    static void processTouchEvent(uint8_t, uint8_t, bool);

private:
    static inline InterfaceManager *_instance;
    PubSubClient *_mqttClient;
    DynamicJsonDocument *_roomDataJson;
    interfaceConfig _cfg;
    void _goToNextRoom();
    void _changeRoom(uint8_t roomId);
    void _updatePanelWithNewRoomInfo();
    bool _getPanelConfig();
    void _processPanelConfig();
};

#endif