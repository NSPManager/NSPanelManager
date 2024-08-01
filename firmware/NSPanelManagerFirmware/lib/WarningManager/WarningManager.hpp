#include <string>
#include <vector>

#ifndef WARNING_MANAGER_HPP
#define WARNING_MANAGER_HPP

class NSPanelWarning {
public:
  std::string level;
  std::string text;
};

class WarningManager {
public:
  static void register_warning(std::string level, std::string text);
  static void register_warning(const char *level, const char *text);

  static void remove_warning(std::string level, std::string text);
  static void remove_warning(const char *level, const char *text);

  static std::vector<NSPanelWarning> get_warnings();

private:
  static inline std::vector<NSPanelWarning> _warnings;
};

#endif