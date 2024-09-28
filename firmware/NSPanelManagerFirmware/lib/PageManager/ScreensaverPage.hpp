#ifndef SCREENSAVER_PAGE_HPP
#define SCREENSAVER_PAGE_HPP

#include <Arduino.h>
#include <PageBase.hpp>
// #include <pb.h>

class ScreensaverPage : public PageBase {
public:
  void init();
  void update();
  void show();
  void unshow();
  void stop();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);

  void attachMqttCallback();
  static void clockMqttCallback(MQTTMessage *message);
  static void dateMqttCallback(MQTTMessage *message);
  static void weatherMqttCallback(MQTTMessage *message);
  static void ampmMqttCallback(MQTTMessage *message);
  static void screensaverModeCallback(MQTTMessage *message);
  static void updateRoomTemp(std::string temp_string);

  // static bool decodeForecastItemProtobuf(pb_istream_t *stream, const pb_field_t *field, void **arg);

private:
  static inline std::string _screensaver_page_name;
  static inline uint8_t _screensaver_brightness;
  static inline bool _show_weather;
  static inline bool _stopped;
};

#endif
