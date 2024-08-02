#include <MqttLog.hpp>
#include <WarningManager.hpp>

void WarningManager::register_warning(std::string level, std::string text) {
  LOG_TRACE("Adding warning level '", level.c_str(), "'. Text: ", text.c_str());

  for (NSPanelWarning &warning : WarningManager::_warnings) {
    if (warning.text.compare(text) == 0) {
      return;
    }
  }

  // We did not find any matching existing error from vector. Add to warnings.
  NSPanelWarning warning{
      .level = level,
      .text = text};
  WarningManager::_warnings.push_back(warning);
}

void WarningManager::register_warning(const char *level, const char *text) {
  WarningManager::register_warning(std::string(level), std::string(text));
}

void WarningManager::remove_warning(std::string level, std::string text) {
  for (auto it = WarningManager::_warnings.cbegin(); it != WarningManager::_warnings.cend();) {
    if (it->text.compare(text) == 0) {
      LOG_TRACE("Adding warning text: ", text.c_str());
      WarningManager::_warnings.erase(it);
      break;
    }
  }
}

void WarningManager::remove_warning(const char *level, const char *text) {
  WarningManager::remove_warning(std::string(level), std::string(text));
}

std::vector<NSPanelWarning> WarningManager::get_warnings() {
  return WarningManager::_warnings;
}