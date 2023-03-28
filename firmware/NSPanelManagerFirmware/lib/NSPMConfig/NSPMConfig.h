#ifndef NSPMConfig_H
#define NSPMConfig_H

#include <string>

class NSPMConfig
{
public:
    /// @brief Will try to initialize and load LittleFS
    /// @return True if successful
    bool init();
    /// @brief Load config file from LittleFS
    /// @return True if successful
    bool loadFromLittleFS();
    /// @brief Save current config file to LittleFS
    /// @return True if successful
    bool saveToLittleFS();
    /// @brief Reset all values to default
    /// @return True if successfuly saved to LittleFS
    bool factoryReset();
    /// @brief The instance of the config manager
    static NSPMConfig *instance;

    /// @brief The address (hostname or IP) to the manager server
    std::string manager_address;
    /// @brief The port to access to manager at
    uint16_t manager_port;

    /// @brief The logging level set for runtime
    uint8_t logging_level;

    /// @brief The hostname of this device
    std::string wifi_hostname;
    /// @brief The WiFi name to connect to
    std::string wifi_ssid;
    /// @brief The pre-shared key for the WiFi
    std::string wifi_psk;

    /// @brief Address to MQTT server
    std::string mqtt_server;
    /// @brief The port to connect to MQTT with
    uint16_t mqtt_port;
    /// @brief MQTT Username
    std::string mqtt_username;
    /// @brief MQTT Password
    std::string mqtt_password;
    /// @brief MQTT Availability topic
    std::string mqtt_availability_topic;
    /// @brief MQTT panel status topic
    std::string mqtt_panel_status_topic;
    /// @brief MQTT topic to send logs to
    std::string mqtt_log_topic;

    std::string md5_firmware;
    std::string md5_data_file;
};

#endif