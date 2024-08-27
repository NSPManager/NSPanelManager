#include <MqttLog.hpp>
#include <WarningManager.hpp>

void WarningManager::register_warning(NSPanelWarningLevel level, std::string text) {
  for (NSPanelWarning &warning : WarningManager::_warnings) {
    if (std::string(warning.text).compare(text) == 0) {
      return;
    }
  }

  // We did not find any matching existing error from vector. Add to warnings.
  NSPanelWarning warning{
      .level = level,
      .text = (char *)text.c_str()};
  WarningManager::_warnings.push_back(warning);
}

void WarningManager::register_warning(NSPanelWarningLevel level, const char *text) {
  WarningManager::register_warning(level, std::string(text));
}

void WarningManager::remove_warning(NSPanelWarningLevel level, std::string text) {
  for (auto it = WarningManager::_warnings.cbegin(); it != WarningManager::_warnings.cend();) {
    if (std::string(it->text).compare(text) == 0) {
      LOG_TRACE("Adding warning text: ", text.c_str());
      WarningManager::_warnings.erase(it);
      break;
    }
  }
}

void WarningManager::remove_warning(NSPanelWarningLevel level, const char *text) {
  WarningManager::remove_warning(level, std::string(text));
}

std::vector<NSPanelWarning> WarningManager::get_warnings() {
  return WarningManager::_warnings;
}