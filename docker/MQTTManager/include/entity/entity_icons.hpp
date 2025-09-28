#include <cstdint>

class EntityIcons {
public:
  // Entity Icons
  static constexpr const char *entity_icon_switch_on = "s";
  static constexpr const char *entity_icon_switch_off = "t";
  static constexpr const char *entity_icon_button = "z";

  // Misc Icons
  static constexpr const char *save_icon = "w";
  static constexpr const char *home_assistant_icon = "x";
  static constexpr const char *openhab_icon = "y";

  // Thermostat Icons
  static constexpr const char *heating = "!";
  static constexpr const char *cooling = "\"";
  static constexpr const char *hot_cold = "#";
  static constexpr const char *thermostat_auto = "$";
  static constexpr const char *dry = "%";
  static constexpr const char *eco = "&";
  static constexpr const char *away = "'";
  static constexpr const char *home = "(";
  static constexpr const char *sleep = ")";
  static constexpr const char *boost = "*";
  static constexpr const char *comfort = "+";
  static constexpr const char *activity = ",";
  static constexpr const char *defrosting = "-";
  static constexpr const char *swing_both = ".";
  static constexpr const char *swing_horizontal = "/";
  static constexpr const char *swing_vertical = "0";
  static constexpr const char *fan_off = "1";
  static constexpr const char *fan_auto = "2";
  static constexpr const char *fan = "3";
  static constexpr const char *fan1 = "4";
  static constexpr const char *fan2 = "5";
  static constexpr const char *fan3 = "6";
};

class GUI_Colors {
public:
  static constexpr const uint16_t icon_color_off = 65535;
  static constexpr const uint16_t icon_color_on = 65024;
};
