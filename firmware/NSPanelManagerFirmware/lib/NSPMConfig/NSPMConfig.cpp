#include <NSPMConfig.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// NSPMConfig
// Give somewhere in memory for instance to exist
NSPMConfig *NSPMConfig::instance;

bool NSPMConfig::init()
{
    NSPMConfig::instance = this;
    if (LittleFS.begin(false))
    {
        Serial.println("LittleFS mounted.");
        return true;
    }
    else
    {
        Serial.println("Failed to mount LittleFS");
        return false;
    }
}

bool NSPMConfig::loadFromLittleFS()
{
    Serial.println("Loading config from LittleFS");
    File configFile = LittleFS.open("/config.json");
    if (!configFile)
    {
        Serial.println("Failed to load config.json!");
        return false;
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();
    if (error)
    {
        Serial.println("Failed to deserialize config.json");
        return false;
    }

    // Load config data
    this->wifi_hostname = doc["wifi_hostname"] | "NSPanelManager-ESP";
    this->wifi_ssid = doc["wifi_ssid"] | "";
    this->wifi_psk = doc["wifi_psk"] | "";
    this->manager_address = doc["manager_address"] | "";
    this->manager_port = doc["manager_port"] | 8000;
    this->logging_level = doc["log_level"] | 4; // Set logging to debug if no level was read from file.

    this->mqtt_server = doc["mqtt_server"] | "";
    this->mqtt_port = doc["mqtt_port"].as<uint8_t>() | 1883;
    this->mqtt_username = doc["mqtt_username"] | "";
    this->mqtt_password = doc["mqtt_password"] | "";

    // Load calculated values
    this->mqtt_availability_topic = "nspanel/";
    this->mqtt_availability_topic.append(NSPMConfig::instance->wifi_hostname);
    this->mqtt_availability_topic.append("/status");

    // Load calculated values
    this->mqtt_panel_status_topic = "nspanel/";
    this->mqtt_panel_status_topic.append(NSPMConfig::instance->wifi_hostname);
    this->mqtt_panel_status_topic.append("/status_report");

    this->mqtt_log_topic = "nspanel/";
    this->mqtt_log_topic.append(NSPMConfig::instance->wifi_hostname);
    this->mqtt_log_topic.append("/log");

    this->mqtt_screen_state_topic = "nspanel/";
    this->mqtt_screen_state_topic.append(NSPMConfig::instance->wifi_hostname);
    this->mqtt_screen_state_topic.append("/screen_state");

    this->mqtt_screen_cmd_topic = "nspanel/";
    this->mqtt_screen_cmd_topic.append(NSPMConfig::instance->wifi_hostname);
    this->mqtt_screen_cmd_topic.append("/screen_cmd");

    this->md5_firmware = doc["md5_firmware"] | "";
    this->md5_data_file = doc["md5_data_file"] | "";

    Serial.println("Config data loaded.");
    return true;
}

bool NSPMConfig::saveToLittleFS()
{
    StaticJsonDocument<1024> config_json;

    config_json["wifi_hostname"] = this->wifi_hostname.c_str();
    config_json["wifi_ssid"] = this->wifi_ssid.c_str();
    config_json["wifi_psk"] = this->wifi_psk.c_str();
    config_json["manager_address"] = this->manager_address.c_str();
    config_json["manager_port"] = this->manager_port;
    config_json["mqtt_server"] = this->mqtt_server.c_str();
    config_json["mqtt_port"] = this->mqtt_port;
    config_json["mqtt_username"] = this->mqtt_username;
    config_json["mqtt_password"] = this->mqtt_password;
    config_json["log_level"] = this->logging_level;
    config_json["md5_firmware"] = this->md5_firmware;
    config_json["md5_data_file"] = this->md5_data_file;

    File config_file = LittleFS.open("/config.json", "w");
    if (!config_file)
    {
        Serial.println("Failed to open 'config.json' for writing.");
    }
    else if (serializeJson(config_json, config_file) == 0)
    {
        Serial.println("Failed to save config file.");
    }
    else
    {
        Serial.println("Saved config file.");
    }
    config_file.close();

    return true;
}

bool NSPMConfig::factoryReset()
{
    this->wifi_hostname = "NSPMPanel";
    this->wifi_ssid = "";
    this->wifi_psk = "";
    this->logging_level = 0;

    this->manager_address = "";
    this->manager_port = 8000;

    this->mqtt_server = "";
    this->mqtt_port = 1883;
    this->mqtt_username = "";
    this->mqtt_password = "";
    return this->saveToLittleFS();
}