#include <cstdint>
class EntityIcons {
public:
  // Entity Icons
  static constexpr const char *entity_icon_switch_on = "s";
  static constexpr const char *entity_icon_switch_off = "t";

  // Misc Icons
  static constexpr const char *save_icon = "w";
  static constexpr const char *home_assistant_icon = "x";
  static constexpr const char *openhab_icon = "y";
};

class GUI_Colors {
public:
  static constexpr const uint16_t icon_color_off = 65535;
  static constexpr const uint16_t icon_color_on = 65024;
};
