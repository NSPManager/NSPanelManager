#ifndef WARNING_MANAGER_HPP
#define WARNING_MANAGER_HPP

#include <protobuf_nspanel.pb-c.h>
#include <string>
#include <vector>

class WarningManager {
public:
  static void register_warning(NSPanelWarningLevel level, std::string text);
  static void register_warning(NSPanelWarningLevel level, const char *text);

  static void remove_warning(NSPanelWarningLevel level, std::string text);
  static void remove_warning(NSPanelWarningLevel level, const char *text);

  static std::vector<NSPanelWarning> get_warnings();

private:
  static inline std::vector<NSPanelWarning> _warnings;
};

#endif