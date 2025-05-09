#pragma once
#include <cstdint>
#include <esp_event.h>
#include <esp_timer.h>
#include <string>

class HomePage {
public:
  static void show();
  static void unshow();

  /**
   * Available "Affect" modes
   */
  enum HomePageAffectMode {
    ROOM,
    ALL
  };

  /**
   * Set the current affect mode for home page.
   */
  static void set_current_affect_mode(HomePageAffectMode mode);

  /**
   * Available "edit" modes
   */
  enum HomePageEditMode {
    ALL_LIGHTS,
    CEILING_LIGHTS,
    TABLE_LIGHTS,
  };

  /**
   * Set the current edit mode for home page.
   */
  static void set_current_edit_mode(HomePageEditMode mode);

private:
  /**
   * Update displayed values, modes and components given the current room status
   */
  static void _update_display();

  /**
   * Handle events from RoomManager, such as new room data arrived, update the display.
   */
  static void _handle_roommanager_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * Handle events from Nextion, such as touch events:
   */
  static void _handle_nextion_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * Handle "Master ceiling lights" button events
   * @param pressed: Is this a touch down or touch released event
   */
  static void _handle_nextion_event_master_ceiling_lights_button(bool pressed);

  /**
   * Handle "Master table lights" button events
   * @param pressed: Is this a touch down or touch released event
   */
  static void _handle_nextion_event_master_table_lights_button(bool pressed);

  /**
   * Handle touch event on "Ceiling master button"
   */
  static void _send_ceiling_master_button_command_to_manager();

  /**
   * Handle touch event on "Table master button"
   */
  static void _send_table_master_button_command_to_manager();

  /**
   * Handle brightness slider touch event
   */
  static void _handle_brightness_slider_event();

  /**
   * Handle color temperature slider touch event
   */
  static void _handle_color_temperature_slider_event();

  /**
   * Update the cached value for the "Brightness"-slider
   */
  static void _update_brightness_slider_cache();

  /**
   * Update the cached value for the "Color temperature"-slider
   */
  static void _update_color_temperature_slider_cache();

  /**
   * Function that will be called once the "Special mode" (ie. hold on table or ceiling lights buttons) timer triggers
   */
  static void _special_mode_timer_callback(void *arg);

  /**
   * Function that will be called once the "Special mode timeout" (ie. hold on table or ceiling lights buttons) timeout timer triggers
   */
  static void _special_mode_timeout_timer_callback(void *arg);

  // Vars
  // When pressing something, what are we supposed to affect?
  static inline HomePageAffectMode _current_affect_mode = HomePageAffectMode::ROOM;

  // What mode is the home page in?
  static inline HomePageEditMode _current_edit_mode = HomePageEditMode::ALL_LIGHTS;

  // The cached value for the "Brightness"-slider
  static inline int32_t _cache_brightness_slider;

  // The cached value for the "Color temperature"-slider
  static inline int32_t _cache_color_temperature_slider;

  // The cached value for the "Ceiling lights" brightness label
  static inline int32_t _cache_ceiling_light_brightness;

  // The cached value for the "Table lights" brightness label
  static inline int32_t _cache_table_light_brightness;

  // Should we ignore the next release of the ceiling button? This is used to
  // ignore the touch up that happens after special mode is activated
  static inline uint64_t _ignore_next_ceiling_button_touch_up = false;

  // Should we ignore the next release of the table button? This is used to
  // ignore the touch up that happens after special mode is activated
  static inline uint64_t _ignore_next_table_button_touch_up = false;

  // What mode shall be activated once the "Special mode" (ie. hold on table or ceiling lights buttons) timer triggers
  static inline HomePageEditMode _special_mode_timer_activation_mode;

  // Arguments that are used to create the special mode timer
  static inline esp_timer_create_args_t _special_mode_create_timer_args;

  // Arguments that are used to create the special mode timeout timer
  static inline esp_timer_create_args_t _special_mode_timeout_create_timer_args;

  // The actual handle to access the ESP timer that handles special mode activation
  static inline esp_timer_handle_t _special_mode_timer_handle = NULL;

  // The actual handle to access the ESP timer that handles special mode deactivation
  static inline esp_timer_handle_t _special_mode_timeout_timer_handle = NULL;
};