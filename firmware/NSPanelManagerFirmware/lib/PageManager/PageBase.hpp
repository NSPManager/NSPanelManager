#ifndef INTERFACE_PAGE_HPP
#define INTERFACE_PAGE_HPP

#include <MqttLog.hpp>

class PageBase {
public:
  /// @brief Show the intended page and do everything need to get it working
  virtual void show() = 0;
  /// @brief Update the page according to new values
  virtual void update() = 0;
  /// @brief Remove and event handlers or simular
  virtual void unshow() = 0;
  /// @brief Handle events from panel
  virtual void processTouchEvent(uint8_t page, uint8_t component, bool pressed) = 0;
};

#endif
