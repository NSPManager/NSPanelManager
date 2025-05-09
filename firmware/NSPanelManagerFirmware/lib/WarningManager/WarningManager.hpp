#ifndef WARNING_MANAGER_HPP
#define WARNING_MANAGER_HPP

#include <protobuf_defines.h>
#include <string>
#include <vector>

class WarningManager {
public:
  static void register_warning(NSPanelWarningLevel level, std::string text);
  static void register_warning(NSPanelWarningLevel level, const char *text);

  static void remove_warning(NSPanelWarningLevel level, std::string text);
  static void remove_warning(NSPanelWarningLevel level, const char *text);

  static std::vector<PROTOBUF_NSPANEL_WARNING> get_warnings();

private:
  static inline std::vector<PROTOBUF_NSPANEL_WARNING> _warnings;
};

#endif