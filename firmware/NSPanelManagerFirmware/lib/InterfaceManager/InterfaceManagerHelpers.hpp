#ifndef NSPM_ROOM_CONFIG_H
#define NSPM_ROOM_CONFIG_H

#include <list>
#include <string>
#include <vector>

/// @brief MQTT Messages container
class mqttMessage {
public:
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

#endif
