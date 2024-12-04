#pragma once
#include <esp_event.h>

class InterfaceManager {
public:
  /**
   * @brief Initialize the InterfaceManager and the Nextion display. Start loading config and enter main interface.
   */
  static void init();

private:
  /**
   * @brief Handle any event trigger from the Nextion display
   */
  static void _nextion_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * @brief Handle any event trigger from the RoomManager
   */
  static void _room_manager_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
};