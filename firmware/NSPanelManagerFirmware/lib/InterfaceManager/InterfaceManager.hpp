#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <ArduinoJson.h>
class Light;
class NSPanel;
class RoomManager;
class Scene;
class mqttMessage;
#include <list>
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

  static inline void handleNSPanelCommand(char *topic, byte *payload, unsigned int length);
  static inline void handleNSPanelScreenBrightnessCommand(char *topic, byte *payload, unsigned int length);
  static inline void handleNSPanelScreensaverBrightnessCommand(char *topic, byte *payload, unsigned int length);
  static inline void handleNSPanelConfigUpdate(char *topic, byte *payload, unsigned int length);
  static inline void handleNSPanelRoomConfigUpdate(char *topic, byte *payload, unsigned int length);
  static void showDefaultPage();

private:
  /// @brief The task that handles startup if InterfaceManager. It load the config from the server and processes it and
  /// @brief makes needed adjustments to make the panel ready for use.
  /// @param param Not used
  static void _taskLoadConfigAndInit(void *param);
  /// @brief Will load base NSPanel config from MQTT
  static void _mqtt_config_callback();
  /// @brief MQTT Messages that has been received but not yet processed
  static inline std::list<mqttMessage> _mqttMessages;
  /// @brief The task handle used to notify task to start processing MQTT messages
  static inline TaskHandle_t _taskHandleProcessMqttMessages;
  /// @brief Task that processes all the MQTT messages in the queue
  /// @param param Not used
  static void _taskProcessMqttMessages(void *param);
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
};

#endif
