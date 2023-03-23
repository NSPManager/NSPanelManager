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
    uint8_t colorTemperature = 0;
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
    std::list<lightConfig*> getAllLightsThatAreOn();
    std::list<lightConfig*> getAllCeilingLights();
    std::list<lightConfig*> getAllTableLights();
    std::list<lightConfig*> getAllLights();
    bool anyCeilingLightsOn();
    bool anyTableLightstOn();
    bool anyLightsOn();
};

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

enum editLightMode
{
    all_lights,
    ceiling_lights,
    table_lights
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
    static inline TaskHandle_t _taskHandleSpecialModeTimer;
    /// @brief Task to auto-stop special light mode after X seconds of no press on the display
    static void _taskSpecialModeTimer(void *param);
    /// @brief Task to auto-trigger special light mode without having to release finger from display.
    static void _taskSpecialModeTriggerTask(void *param);
    PubSubClient *_mqttClient;
    roomMode _currentRoomMode;
    editLightMode _currentEditMode;
    DynamicJsonDocument *_roomDataJson;
    interfaceConfig _cfg;
    unsigned long _lastMasterCeilingLightsButtonTouch;
    unsigned long _lastMasterCeilingLightsButtonRelease;
    unsigned long _lastMasterTableLightsButtonTouch;
    unsigned long _lastMasterTableLightsButtonRelease;
    unsigned long _lastSpecialModeEventMillis;
    editLightMode _triggerSpecialEditLightMode;
    bool _ignoreNextTouchRelease;
    void _goToNextRoom();
    void _changeRoom(uint8_t roomId);
    void _updatePanelWithNewRoomInfo();
    void _updatePanelLightStatus();
    void _goToNextMode();
    void _updateHomeValues();
    void _changeMode(roomMode mode);
    bool _getPanelConfig();
    void _processPanelConfig();
    /// @brief Set current light mode, handle starting/stopping special light mode
    /// @param mode The mode to set
    void _setEditLightMode(editLightMode mode);
    /// @brief Will start the special mode timer
    void _startSpecialModeTimer();
    /// @brief Will start the special mode trigger task
    /// @param triggerMode The new mode to trigger if sucessful
    void _startSpecialModeTriggerTask(editLightMode triggerMode);

    void _ceilingMasterButtonEvent();
    void _tableMasterButtonEvent();
    void _updateLightsThatAreOn(); // Set light level for all lights that are currently on
    void _updateAllLights(); // Send light level to all lights
    void _updateLightsColorTemp(); // Send new color temp to all lights

    void _subscribeToLightTopics(lightConfig *cfg);

    /// @brief Set internal light representation level
    /// @param light The light to set value for
    /// @param level The new value
    void _setLightLevel(std::string light, uint8_t level);

    /// @brief Set internal light representation color temperature
	/// @param light The light to set value for
	/// @param level The new value
	void _setLightColorTemperature(std::string light, uint8_t level);

    /// @brief Send out new value for a list of lights
    /// @param lights The lights to set value for
    /// @param level The new value
    void _changeLightsToLevel(std::list<lightConfig*> *lights, uint8_t level);
    void _changeLightsToKelvin(std::list<lightConfig*> *lights, uint16_t kelvin);
};

#endif
