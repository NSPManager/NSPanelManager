#include <NSPMConfig.h>
#include <ArduLog.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// LMANConfig
// Give somewhere in memory for instance to exist
NSPMConfig *NSPMConfig::instance;

bool NSPMConfig::init()
{
    NSPMConfig::instance = this;
    if (LittleFS.begin(false))
    {
        LOG_INFO("LittleFS mounted.");
        return true;
    }
    else
    {
        LOG_ERROR("Failed to mount LittleFS");
        return false;
    }
}

bool NSPMConfig::loadFromLittleFS()
{
    LOG_INFO("Loading config from LittleFS");
    File configFile = LittleFS.open("/config.json");
    if (!configFile)
    {
        LOG_ERROR("Failed to load config.json!");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();
    if (error)
    {
        LOG_ERROR("Failed to deserialize config.json");
        return false;
    }

    // Load config data
    this->wifi_hostname = doc["wifi_hostname"] | "NSPanelManager-ESP";
    this->wifi_ssid = doc["wifi_ssid"] | "";
    this->wifi_psk = doc["wifi_psk"] | "";
    this->manager_address = doc["manager_address"] | "";
    this->manager_port = doc["manager_port"] | 8000;
    this->logging_level = doc["log_level"] | 4; // Set logging to debug if no level was read from file.
    LOG_INFO("Setting logging level to ", LOG_BOLD, this->logging_level);
    ArduLog::getInstance()->SetLogLevel(static_cast<ArduLogLevel>(this->logging_level));

    this->mqtt_server = doc["mqtt_server"] | "";
    this->mqtt_port = doc["mqtt_port"].as<uint8_t>() | 1883;
    this->mqtt_username = doc["mqtt_username"] | "";
    this->mqtt_password = doc["mqtt_password"] | "";

    LOG_INFO("Config data loaded.");
    return true;
}

bool NSPMConfig::saveToLittleFS()
{
    StaticJsonDocument<512> config_json;

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

    File config_file = LittleFS.open("/config.json", "w");
    if (!config_file)
    {
        LOG_ERROR("Failed to open 'config.json' for writing.");
    }
    else if (serializeJson(config_json, config_file) == 0)
    {
        LOG_ERROR("Failed to save config file.");
    }
    else
    {
        LOG_INFO("Saved config file.");
    }
    config_file.close();

    return true;
}

bool NSPMConfig::factoryReset()
{
    this->wifi_hostname = "lman";
    this->wifi_ssid = "";
    this->wifi_psk = "";
    this->logging_level = 0;

    this->mqtt_server = "";
    this->mqtt_port = 1883;
    this->mqtt_username = "";
    this->mqtt_password = "";

    return this->saveToLittleFS();
}