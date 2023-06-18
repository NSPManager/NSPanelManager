#ifndef SCREENSAVER_PAGE_HPP
#define SCREENSAVER_PAGE_HPP

#include <Arduino.h>
#include <PageBase.hpp>

class ScreensaverPage : public PageBase {
public:
  void update();
  void show();
  void unshow();
  void attachMqttTimeCallback();
  static void clockMqttCallback(char *topic, byte *payload, unsigned int length);
};

#endif
