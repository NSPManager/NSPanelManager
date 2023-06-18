#ifndef NSPANEL_MANAGER_PAGE
#define NSPANEL_MANAGER_PAGE

#include <Arduino.h>
#include <PageBase.hpp>

class NSpanelManagerPage : public PageBase {
public:
  void show();
  void setText(const char *text);
  void setText(std::string &text);
};

#endif
