#ifndef INTERFACE_CONFIG_ENUMS_HPP
#define INTERFACE_CONFIG_ENUMS_HPP

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
