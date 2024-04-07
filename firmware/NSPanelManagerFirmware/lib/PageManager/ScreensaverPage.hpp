#ifndef SCREENSAVER_PAGE_HPP
#define SCREENSAVER_PAGE_HPP

#include <Arduino.h>
#include <PageBase.hpp>

class ScreensaverPage : public PageBase {
public:
  void init();
  void update();
  void show();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);

  void attachMqttCallback();
  static void clockMqttCallback(char *topic, byte *payload, unsigned int length);
  static void dateMqttCallback(char *topic, byte *payload, unsigned int length);
  static void weatherMqttCallback(char *topic, byte *payload, unsigned int length);
  static void ampmMqttCallback(char *topic, byte *payload, unsigned int length);
  static void screensaverModeCallback(char *topic, byte *payload, unsigned int length);
  static void updateRoomTemp(std::string temp_string);

private:
  std::string _screensaver_page_name;
  std::string _screensaver_time_name;
  std::string _screensaver_ampm_name;
  std::string _screensaver_date_name;
  std::string _screensaver_temperature_name;
  uint8_t _screensaver_brightness;
  bool _show_weather;
};

#endif
