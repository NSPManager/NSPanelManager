#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <NSPMConfig.h>
#include <WiFi.h>

// NSPMConfig
// Give somewhere in memory for instance to exist
NSPMConfig *NSPMConfig::instance;

bool NSPMConfig::init() {
  NSPMConfig::instance = this;
  this->manager_port = 8000;
  this->logging_level = 3;
  this->mqtt_port = 1883;
  this->use_new_upload_protocol = true;
  if (LittleFS.begin(false)) {
    Serial.println("LittleFS mounted.");
    this->littlefs_mount_successfull = true;
    return true;
  } else {
    Serial.println("Failed to mount LittleFS");
    this->littlefs_mount_successfull = false;
    return false;
  }
}

bool NSPMConfig::loadFromLittleFS() {
  Serial.println("Loading config from LittleFS");
  File configFile = LittleFS.open("/config.json");
  if (!configFile) {
    Serial.println("Failed to open config.json!");
    return false;
  }

  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();
  if (error) {
    Serial.println("Failed to deserialize config.json");
    return false;
  }

  // Load config data
  this->wifi_hostname = doc["wifi_hostname"] | "NSPMPanel";
  this->wifi_ssid = doc["wifi_ssid"] | "";
  this->wifi_psk = doc["wifi_psk"] | "";
  this->manager_address = doc["manager_address"] | "";
  this->manager_port = doc.containsKey("manager_port") ? doc["manager_port"].as<uint16_t>() : 8000;
  this->logging_level = doc.containsKey("log_level") ? doc["log_level"].as<uint8_t>() : 3; // Set logging to info if no level was read from file.

  this->mqtt_server = doc["mqtt_server"] | "";
  this->mqtt_port = doc.containsKey("mqtt_port") ? doc["mqtt_port"].as<uint16_t>() : 1883;
  this->mqtt_username = doc["mqtt_username"] | "";
  this->mqtt_password = doc["mqtt_password"] | "";

  this->tft_upload_baud = doc.containsKey("upload_baud") ? doc["upload_baud"].as<uint32_t>() : 115200;
  this->use_new_upload_protocol = doc.containsKey("use_new_upload_protocol") ? doc["use_new_upload_protocol"].as<String>() == "true" : true;

  this->relay1_default_mode = doc["relay1_default_mode"].as<String>() == "True";
  this->relay2_default_mode = doc["relay2_default_mode"].as<String>() == "True";

  // Load calculated values
  this->mqtt_availability_topic = "nspanel/";
  this->mqtt_availability_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_availability_topic.append("/status");

  this->mqtt_panel_cmd_topic = "nspanel/";
  this->mqtt_panel_cmd_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_panel_cmd_topic.append("/command");

  this->mqtt_panel_screen_brightness_topic = "nspanel/";
  this->mqtt_panel_screen_brightness_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_panel_screen_brightness_topic.append("/brightness");

  this->mqtt_panel_screensaver_brightness = "nspanel/";
  this->mqtt_panel_screensaver_brightness.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_panel_screensaver_brightness.append("/brightness_screensaver");

  this->mqtt_panel_status_topic = "nspanel/";
  this->mqtt_panel_status_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_panel_status_topic.append("/status_report");

  this->mqtt_panel_temperature_topic = "nspanel/";
  this->mqtt_panel_temperature_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_panel_temperature_topic.append("/temperature_state");

  this->mqtt_log_topic = "nspanel/";
  this->mqtt_log_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_log_topic.append("/log");

  this->mqtt_screen_state_topic = "nspanel/";
  this->mqtt_screen_state_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_screen_state_topic.append("/screen_state");

  this->mqtt_screen_cmd_topic = "nspanel/";
  this->mqtt_screen_cmd_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_screen_cmd_topic.append("/screen_cmd");

  this->mqtt_relay1_cmd_topic = "nspanel/";
  this->mqtt_relay1_cmd_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_relay1_cmd_topic.append("/r1_cmd");

  this->mqtt_relay1_state_topic = "nspanel/";
  this->mqtt_relay1_state_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_relay1_state_topic.append("/r1_state");

  this->mqtt_relay2_cmd_topic = "nspanel/";
  this->mqtt_relay2_cmd_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_relay2_cmd_topic.append("/r2_cmd");

  this->mqtt_relay2_state_topic = "nspanel/";
  this->mqtt_relay2_state_topic.append(NSPMConfig::instance->wifi_hostname);
  this->mqtt_relay2_state_topic.append("/r2_state");

  this->md5_firmware = doc["md5_firmware"] | "";
  this->md5_data_file = doc["md5_data_file"] | "";
  this->md5_tft_file = doc["md5_tft_file"] | "";

  Serial.println("Config data loaded.");
  return true;
}

bool NSPMConfig::saveToLittleFS() {
  StaticJsonDocument<2048> config_json;

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
  config_json["md5_tft_file"] = this->md5_tft_file;
  config_json["upload_baud"] = this->tft_upload_baud;
  config_json["use_new_upload_protocol"] = this->use_new_upload_protocol ? "true" : "false";

  File config_file = LittleFS.open("/config.json", "w");
  if (!config_file) {
    Serial.println("Failed to open 'config.json' for writing.");
  } else if (serializeJson(config_json, config_file) == 0) {
    Serial.println("Failed to save config file.");
  } else {
    Serial.println("Saved config file.");
  }
  config_file.close();

  return true;
}

bool NSPMConfig::factoryReset() {
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
