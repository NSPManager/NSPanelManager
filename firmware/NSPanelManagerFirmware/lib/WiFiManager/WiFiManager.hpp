#include "esp_wifi_types.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <string>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

class WiFiManager {
public:
  static void init();
  static void try_connect(std::string ssid, std::string psk);
  static bool is_connected();

private:
  static inline bool _is_running = false;
  static inline bool _connected = false;
  static inline uint8_t _wifi_connect_retries = 0;
  static inline EventGroupHandle_t _wifi_event_group;
  static inline wifi_init_config_t _wifi_init_config;
  static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
};
