#include <stdint.h>

#pragma once
// This file contains data that is needed to communicate between the Nextion display and the ESP32.
// This includes data such as page names, component IDs and so on

// BOOTSCREEN PAGE
class GUI_LOADING_PAGE {
public:
  static inline const char *page_name = "bootscreen";
  static inline const char *component_text_name = "t_loading";
  static inline const char *component_text_ip_text = "t_ip";
};

class GUI_HOME_PAGE {
public:
  static inline const char *page_name = "home";
  static inline const char *timer_screensaver_name = "home.sleep_display";

  static inline const char *dimmer_slider_name = "home.s_brightness";
  static inline const char *color_temperature_slider_name = "home.s_kelvin";

  static inline const uint8_t dimmer_slider_id = 2;
  static inline const uint8_t color_temperature_slider_id = 1;

  static inline const char *button_ceiling_name = "home.b_ceiling";
  static inline const char *button_table_name = "home.b_table";

  static inline const uint8_t button_ceiling_id = 22;
  static inline const uint8_t button_table_id = 21;

  static inline const char *button_scenes_name = "home.b_scenes";
  static inline const uint8_t button_scenes_room_pic = 59;
  static inline const uint8_t button_scenes_room_pic2 = 50;
  static inline const uint8_t button_scenes_all_rooms_pic = 61;
  static inline const uint8_t button_scenes_all_rooms_pic2 = 62;

  static inline const uint8_t button_room_entities_id = 5;
  static inline const uint8_t button_next_room_id = 10;

  static inline const char *label_ceiling_name = "home.n_ceiling";
  static inline const char *label_table_name = "home.n_table";

  static inline const char *highlight_ceiling_name = "p_lockceiling";
  static inline const char *highlight_table_name = "p_locktable";

  static inline const uint16_t slider_normal_color = 65535;
  static inline const uint16_t slider_highlight_color = 65024;

  static inline const char *mode_label_name = "home.mode";
  static inline const char *room_label_name = "home.room";
};