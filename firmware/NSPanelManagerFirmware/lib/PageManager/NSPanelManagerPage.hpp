#ifndef NSPANEL_MANAGER_PAGE
#define NSPANEL_MANAGER_PAGE

#include <Arduino.h>
#include <PageBase.hpp>

class NSpanelManagerPage : public PageBase {
public:
  void show();
  void update();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);

  void setText(const char *text);
  void setText(std::string &text);
  void setSecondaryText(const char *text);
  void setSecondaryText(std::string &text);
};

#endif
