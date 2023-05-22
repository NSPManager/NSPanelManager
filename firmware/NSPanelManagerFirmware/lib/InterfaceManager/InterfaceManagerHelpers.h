#ifndef NSPM_ROOM_CONFIG_H
#define NSPM_ROOM_CONFIG_H

#include <list>
#include <string>

/// @brief MQTT Messages container
struct mqttMessage {
  std::string topic;
  std::string payload;
};

/// @brief Edit mode (room or house)
enum roomMode {
  room,
  house,
  END // Keep END at end of enum
};

/// @brief Light mode (all lights, ceiling lights or table lights)
enum editLightMode {
  all_lights,
  ceiling_lights,
  table_lights,
  EXIT_SPECIAL_MODE // Used to exit special mode
};

/// @brief Object used to represent a light with all its configuration
struct lightConfig {
  uint16_t id = 0;
  uint8_t level = 0;
  uint8_t colorTemperature = 0;
  uint16_t colorHue = 0;
  uint16_t colorSat = 0;
  uint8_t roomViewPosition = 0;
  bool isCeiling = false;
  bool canDim = false;
  bool canTemperature = false;
  bool canRgb = false;
  std::string name;
};

struct sceneConfig {
  uint16_t id = 0;
  std::string name;
};

/// @brief Object/container used to represent a room
class roomConfig {
public:
  uint8_t id = 0;
  std::string name;
  std::list<lightConfig> ceilingLights;
  std::list<lightConfig> tableLights;
  std::list<sceneConfig> scenes;
  std::list<lightConfig *> getCeilingLightsThatAreOn();
  std::list<lightConfig *> getTableLightsThatAreOn();
  std::list<lightConfig *> getAllLightsThatAreOn();
  std::list<lightConfig *> getAllCeilingLights();
  std::list<lightConfig *> getAllTableLights();
  std::list<lightConfig *> getAllLights();
  std::list<lightConfig *> getAllRoomViewLights();
  lightConfig *getLightAtRoomViewPosition(int room_view_position);
  bool anyCeilingLightsOn();
  bool anyTableLightstOn();
  bool anyLightsOn();
};

/// @brief Configuration for the hole NSPanel
struct interfaceConfig {
  uint8_t homeScreen = 0;
  uint16_t colorTempMin = 2000;
  uint16_t colorTempMax = 6000;
  uint8_t raiseToMaxLightLevelAbove = 95;
  bool reverseColorTempSlider = false;
  /// @brief Mimimum time for a button to be pressed to register as a press
  uint16_t button_min_press_time;
  /// @brief Mimimum time for a button to be held to register as a long press
  uint16_t button_long_press_time;
  /// @brief Mimimum time to hold a button to start special mode
  uint16_t special_mode_trigger_time;
  /// @brief Time to exit special mode after no actions has been taken
  uint16_t special_mode_release_time;
  /// @brief Time to ignore incomming MQTT messages after a command is sent out
  uint16_t mqtt_ignore_time;
  std::list<roomConfig> rooms;
  std::list<roomConfig>::iterator currentRoom;
  std::list<lightConfig *> getCeilingLightsThatAreOn();
  std::list<lightConfig *> getTableLightsThatAreOn();
  std::list<lightConfig *> getAllLightsThatAreOn();
  std::list<lightConfig *> getAllCeilingLights();
  std::list<lightConfig *> getAllTableLights();
  std::list<lightConfig *> getAllLights();
  lightConfig *getLightById(uint16_t id);
  bool anyCeilingLightsOn();
  bool anyTableLightstOn();
  bool anyLightsOn();
};

#endif
