#pragma once
#include <esp_wifi.h>
#include <string>
#include <vector>

class WiFiManager {
public:
  /**
   * @brief Start the WiFi client in STA-mode and try to connect to given SSID.
   * @param ssid: The name of the WiFi to connect to.
   * @param psk: The password to connect to the WiFi.
   * @param hostname: The hostname of this device
   */
  static void start_client(std::string *ssid, std::string *psk, std::string *hostname);

  /**
   * @brief Start the WiFi access point for initial configuration.
   * @param ssid: The name of the WiFi to broadcast.
   * @param psk: The password of the WiFi.
   */
  static void start_ap(std::string *ssid);

  // TODO: void start_ap();

  /**
   * @brief Get a list of available networks
   * @return A std::vector with entities of type wifi_ap_record_t that contains information about the network
   */
  static std::vector<wifi_ap_record_t> search_available_networks();

  /**
   * @brief Check if WiFi is connected.
   * @return True if connected, otherwise false.
   */
  static bool connected();

  /**
   * @brief Get the last esp_netif_ip_info_t that was handed to the ESP32
   * @return Latest network IP information handed to the ESP32.
   */
  static esp_netif_ip_info_t ip_info();

private:
  static void _event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  // Vars
  // WiFi initialization config
  static inline wifi_init_config_t _init_config;

  // WiFi connection config
  static inline wifi_config_t _config;

  // Is the WiFi connected?
  static inline bool _connected;

  // Current IP address
  static inline esp_netif_ip_info_t _ip_info;
};