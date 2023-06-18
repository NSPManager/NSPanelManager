#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <ArduinoJson.h>
#include <InterfaceManagerHelpers.hpp>
// #include <Light.hpp>
class Light;
class NSPanel;
class RoomManager;
class Scene;
#include <list>
#include <string>

class InterfaceManager {
public:
  /// @brief Set default variables, initialize panel and start the user interface
  void init();
  /// @brief Stop and clear memory from InterfaceManager
  static void stop();
  /// @brief Callback for when a touch event occurs on the screen
  /// @param page Page
  /// @param component Component
  /// @param pressed Pressed
  static void processTouchEvent(uint8_t page, uint8_t component, bool pressed);
  /// @brief Callback for when the screen goes to sleep
  static void processSleepEvent();
  /// @brief Callback for when the screen wakes from sleep
  static void processWakeEvent();
  /// @brief Callback for when an entity update comes from MQTT
  /// @param topic The topic on which the payload was received
  /// @param payload The payload
  /// @param length Length of the payload
  static void mqttCallback(char *topic, byte *payload, unsigned int length);
  /// @brief This function is called whenevent MQTT is connected. Subscribe to all entity status topics
  static void subscribeToMqttTopics();
  /// @brief This variable will block the panel from communicating with panel before the panel has been registered.
  static inline bool hasRegisteredToManager = false;
  /// @brief The InfterfaceManager instance
  static inline InterfaceManager *instance;
  /// @brief If the light os ON, it will turn off. If the light is OFF, turn it ON
  /// @param light The light to change
  void _onOffLight(Light *light);
  /// @brief Update all relevant light values on the panel
  void _updatePanelLightStatus();

private:
  /// @brief The task that handles startup if InterfaceManager. It load the config from the server and processes it and
  /// @brief makes needed adjustments to make the panel ready for use.
  /// @param param Not used
  static void _taskLoadConfigAndInit(void *param);
  /// @brief MQTT Messages that has been received but not yet processed
  static inline std::list<mqttMessage> _mqttMessages;
  /// @brief The task handle used to notify task to start processing MQTT messages
  static inline TaskHandle_t _taskHandleProcessMqttMessages;
  /// @brief Task that processes all the MQTT messages in the queue
  /// @param param Not used
  static void _taskProcessMqttMessages(void *param);
  /// @brief Task handle used to indicate weather or not a special mode has already ben initialized
  static inline TaskHandle_t _taskHandleSpecialModeTimer;
  /// @brief Task to auto-stop special light mode after X seconds of no press on the display
  static void _taskSpecialModeTimer(void *param);
  /// @brief Task to auto-trigger special light mode without having to release finger from display.
  static void _taskSpecialModeTriggerTask(void *param);
  /// @brief What is the currently selected room mode
  roomMode _currentRoomMode;
  /// @brief What is the currently selected edit mode
  editLightMode _currentEditMode;
  /// @brief Used temporarely to hold JSON-data from the server while initializing the panel
  DynamicJsonDocument *_roomDataJson;
  /// @brief When was the last time someone pushed down on the master ceiling lights button
  unsigned long _lastMasterCeilingLightsButtonTouch;
  /// @brief When was the last time someone released the fringer from the panel for the master ceiling lights button
  unsigned long _lastMasterCeilingLightsButtonRelease;
  /// @brief When was the last time someone pushed down on the master table lights button
  unsigned long _lastMasterTableLightsButtonTouch;
  /// @brief When was the last time someone released the fringer from the panel for the master table lights button
  unsigned long _lastMasterTableLightsButtonRelease;
  /// @brief Used to indicate when the last event occured, this is used to determin when to exit "special mode"
  unsigned long _lastSpecialModeEventMillis;
  /// @brief Used to temporarely hold what typ of "special mode" to trigger when "hold time" has been reached
  editLightMode _triggerSpecialEditLightMode;
  /// @brief Used to indicate that a long press was used and to not register the next release
  bool _ignoreNextTouchRelease;
  /// @brief Flag of what state the last event was, is the finger still on the display?
  bool _isFingerOnDisplay;
  /// @brief When sending new values from the panel, the values might flucuate before setteling
  /// @brief this variable indicates for how long MQTT messages should be ignored
  unsigned long _ignoreMqttStatusUpdatesUntil;
  /// @brief Update panel with new values for the new room
  void _updatePanelWithNewRoomInfo();
  /// @brief Go to next edit mode (room, ceiling)
  void _goToNextMode();
  /// @brief Change the currently selected light mode (room, ceiling)
  void _changeMode(roomMode mode);
  /// @brief Try to download the configuration from the server
  /// @return True if sucessful, otherwise false
  bool _getPanelConfig();
  /// @brief Try to download the configuration for a specified room
  /// @param room_id The ID of the room
  /// @param buffer The buffer to put the JSON-data in to
  /// @return True if sucessful, otherwise false
  bool _getRoomConfig(int room_id, DynamicJsonDocument *buffer);
  /// @brief Set current light mode, handle starting/stopping special light mode
  /// @param mode The mode to set
  void _setEditLightMode(editLightMode mode);
  /// @brief Will start the special mode timer
  void _startSpecialModeTimer();
  /// @brief Will start the special mode trigger task
  /// @param triggerMode The new mode to trigger if sucessful
  void _startSpecialModeTriggerTask(editLightMode triggerMode);
  /// @brief Stop and exit special mode
  void _stopSpecialMode();

  /// @brief Called when the master ceiling lights button was pressed
  void _ceilingMasterButtonEvent();
  /// @brief Called when the master table lights button was pressed
  void _tableMasterButtonEvent();
  /// @brief Update all lights that are on to the new light level dictated by slider
  void _updateLightsThatAreOn();
  /// @brief Update all lights to the new light level dictated by slider
  void _updateAllLights();
  /// @brief Update color temperature to all lights dictated by slider
  void _updateLightsColorTemp();

  /// @brief Will populate the state, visibilty and names on lights in the Room page
  void _populateRoomPage();
  /// @brief Will update the state of all switches on the Room page
  void _updateRoomPageStates();
  /// @brief Handle a touch event that has occured on the "Room"-page
  /// @param component_id The component that was touched
  void _handleRoomPageComponentTouch(uint8_t component_id);
  /// @brief Handle a touch event that has occured on the "Light"-page
  /// @param component_id The component that was touched
  void _handleLightPageComponentTouch(uint8_t component_id);

  /// @brief Given a light, will subscribe to all relevant MQTT status topics for that light
  /// @param cfg The light to subscribe to
  void _subscribeToLightTopics(Light *cfg);

  /// @brief Set internal light representation level
  /// @param light_id The light ID to set value for
  /// @param level The new value
  void _setLightLevel(uint16_t light_id, uint8_t level);

  /// @brief Set internal light representation color temperature
  /// @param light_id The light ID to set value for
  /// @param level The new value
  void _setLightColorTemperature(uint16_t light_id, uint8_t level);
};

#endif
