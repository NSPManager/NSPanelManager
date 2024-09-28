#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <ArduinoJson.h>
class Light;
class NSPanel;
class RoomManager;
#include <MqttManager.hpp>
#include <list>
#include <protobuf_nspanel.pb-c.h>
#include <string>

class InterfaceManager {
public:
  /// @brief Set default variables, initialize panel and start the user interface
  void init();
  /// @brief Stop and clear memory from InterfaceManager
  static void stop();
  /// @brief Callback for when the screen goes to sleep
  static void processSleepEvent();
  /// @brief Callback for when the screen wakes from sleep
  static void processWakeEvent();
  /// @brief This function is called whenevent MQTT is connected. Subscribe to all entity status topics
  static void subscribeToMqttTopics();
  /// @brief This variable will block the panel from communicating with panel before the panel has been registered.
  static inline bool hasRegisteredToManager = false;
  /// @brief The InfterfaceManager instance
  static inline InterfaceManager *instance;

  static inline void handleNSPanelCommand(MQTTMessage *message);
  static inline void handleNSPanelScreenBrightnessCommand(MQTTMessage *message);
  static inline void handleNSPanelScreensaverBrightnessCommand(MQTTMessage *message);
  static inline void handleNSPanelScreenCommand(MQTTMessage *message);
  static inline void handleNSPanelConfigUpdate(MQTTMessage *message);
  static void showDefaultPage();

private:
  /// @brief The task that handles startup if InterfaceManager. It load the config from the server and processes it and
  /// @brief makes needed adjustments to make the panel ready for use.
  /// @param param Not used
  static void _taskLoadConfigAndInit(void *param);
  /// @brief Handle data received by the handleNSPanelConfigUpdate callback
  static void _taskHandleConfigData(void *param);
  /// @brief Task handle used to indicate weather or not a special mode has already ben initialized
  static inline TaskHandle_t _taskHandleSpecialModeTimer;
  /// @brief Used temporarely to hold JSON-data from the server while initializing the panel
  JsonDocument *_roomDataJson;
  /// @brief Try to download the configuration from the server
  /// @return True if sucessful, otherwise false
  bool _getPanelConfig();
  /// @brief Try to download the configuration for a specified room
  /// @param room_id The ID of the room
  /// @param buffer The buffer to put the JSON-data in to
  /// @return True if sucessful, otherwise false
  bool _getRoomConfig(int room_id, JsonDocument *buffer);

  bool _processMqttMessages;
  bool _config_loaded;
  static inline NSPanelConfig _new_config;
};

#endif
