#ifndef INTERFACE_PAGE_HPP
#define INTERFACE_PAGE_HPP

#include <MqttLog.hpp>

class PageBase {
public:
  /// @brief Show the intended page and do everything need to get it working
  void show() {
    LOG_ERROR("Not implemented!");
  }
  /// @brief Update the page according to new values
  void update() {
    LOG_ERROR("Not implemented!");
  }
  /// @brief Remove and event handlers or simular
  void unshow() {
    LOG_ERROR("Not implemented!");
  }
};

#endif
