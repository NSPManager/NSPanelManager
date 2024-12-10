#include <Arduino.h>
#include <HTTPClient.h>
#include <HttpLib.hpp>
#include <InterfaceManager.hpp>
#include <LittleFS.h>
#include <MqttLog.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <Update.h>
#include <WebManager.hpp>
#include <WiFi.h>
#include <list>

// Make space for variables in memory
WebManager *WebManager::instance;

void WebManager::init(const char *nspmFirmwareVersion) {
  if (this->_has_already_been_started) {
    LOG_ERROR("Trying to start WebManager while it has already been started.");
    return;
  }
  this->instance = this;
  this->_nspmFirmwareVersion = nspmFirmwareVersion;
  this->_state = WebManagerState::ONLINE;
  this->_update_progress = 0;

  LOG_TRACE("Setting up web server routes");
  this->_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(LittleFS, "/index.html", String(), false, WebManager::processIndexTemplate); });

  this->_server.on("/save_config", HTTP_POST, WebManager::saveConfigFromWeb);
  this->_server.on("/factory_reset", HTTP_GET, WebManager::factoryReset);
  this->_server.on("/do_reboot", HTTP_GET, WebManager::doRebootNow);
  this->_server.on("/available_wifi_networks", HTTP_GET, WebManager::respondAvailableWiFiNetworks);

  this->_server.onNotFound([](AsyncWebServerRequest *request) { request->send(404, "text/plain", "Path/File not found!"); });

  LOG_INFO("Starting web server.");

  this->_server.serveStatic("/static", LittleFS, "/static");
  this->_server.begin();
  this->_has_already_been_started = true;
}

String WebManager::processIndexTemplate(const String &templateVar) {
  if (templateVar == "version") {
    return WebManager::instance->_nspmFirmwareVersion.c_str();
  } else if (templateVar == "wifi_hostname") {
    return NSPMConfig::instance->wifi_hostname.c_str();
  } else if (templateVar == "wifi_ssid") {
    return NSPMConfig::instance->wifi_ssid.c_str();
  } else if (templateVar == "wifi_psk") {
    return NSPMConfig::instance->wifi_psk.c_str();
  } else if (templateVar == "mqtt_server") {
    return NSPMConfig::instance->mqtt_server.c_str();
  } else if (templateVar == "mqtt_port") {
    return String(NSPMConfig::instance->mqtt_port);
  } else if (templateVar == "mqtt_username") {
    return NSPMConfig::instance->mqtt_username.c_str();
  } else if (templateVar == "mqtt_psk") {
    return NSPMConfig::instance->mqtt_password.c_str();
  } else if (templateVar == "log_level") {
    return String(NSPMConfig::instance->logging_level);
  } else if (templateVar == "upload_proto_latest_checked") {
    return NSPMConfig::instance->use_new_upload_protocol ? "checked" : "";
  } else if (templateVar == "upload_proto_legacy_checked") {
    return NSPMConfig::instance->use_new_upload_protocol ? "" : "checked";
  } else if (templateVar == "tft_upload_baud") {
    return String(NSPMConfig::instance->tft_upload_baud);
  }

  return "-- UNKNOWN TEMPLATE --";
}

void WebManager::saveConfigFromWeb(AsyncWebServerRequest *request) {
  JsonDocument config_json;

  NSPMConfig::instance->wifi_hostname = request->arg("wifi_hostname").c_str();
  NSPMConfig::instance->wifi_ssid = request->arg("wifi_ssid").c_str();
  NSPMConfig::instance->wifi_psk = request->arg("wifi_psk").c_str();

  NSPMConfig::instance->logging_level = request->arg("log_level").toInt();

  NSPMConfig::instance->mqtt_server = request->arg("mqtt_server").c_str();
  NSPMConfig::instance->mqtt_port = request->arg("mqtt_port").toInt();
  NSPMConfig::instance->mqtt_username = request->arg("mqtt_username").c_str();
  NSPMConfig::instance->mqtt_password = request->arg("mqtt_psk").c_str();

  NSPMConfig::instance->tft_upload_baud = request->arg("upload_buad_rate").toInt();
  NSPMConfig::instance->use_new_upload_protocol = request->arg("upload_protocol") == "latest";

  if (!NSPMConfig::instance->saveToLittleFS(false)) {
    LOG_ERROR("Failed to save new configuration!");
  }

  request->redirect("/");

  // Wait for 2 seconds before restarting.
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  ESP.restart();
}

void WebManager::factoryReset(AsyncWebServerRequest *request) {
  LOG_INFO("Performing factory reset!");
  NSPMConfig::instance->factoryReset();
  ESP.restart();
  request->redirect("/");
}

void WebManager::doRebootNow(AsyncWebServerRequest *request) {
  request->send(200);
  ESP.restart();
}

void WebManager::respondAvailableWiFiNetworks(AsyncWebServerRequest *request) {
  String json = "[";
  int n = WiFi.scanComplete();
  if (n == -2) {
    WiFi.scanNetworks(true);
  } else if (n) {
    for (int i = 0; i < n; ++i) {
      if (!WiFi.SSID(i).isEmpty()) {
        LOG_DEBUG("Found WiFi ", WiFi.SSID(i).c_str());
        if (i)
          json += ",";
        json += "{";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
        json += ",\"channel\":" + String(WiFi.channel(i));

        switch (WiFi.encryptionType(i)) {
        case wifi_auth_mode_t::WIFI_AUTH_OPEN:
          json += ",\"security\": \"OPEN\"";
          break;
        case wifi_auth_mode_t::WIFI_AUTH_WPA2_PSK:
          json += ",\"security\": \"WPA2 PSK\"";
          break;
        case wifi_auth_mode_t::WIFI_AUTH_WPA3_PSK:
          json += ",\"security\": \"WPA3 PSK\"";
          break;
        case wifi_auth_mode_t::WIFI_AUTH_WPA_PSK:
          json += ",\"security\": \"WPA PSK\"";
          break;
        case wifi_auth_mode_t::WIFI_AUTH_WEP:
          json += ",\"security\": \"WEP\"";
          break;

        default:
          json += ",\"security\": \"UNKNOWN\"";
          break;
        }
        json += "}";
      }
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }
  json += "]";
  request->send(200, "application/json", json);
  json = String();
}

void WebManager::startOTAUpdate() {
  // TODO: Move function to InterfaceManager
  BaseType_t result = xTaskCreatePinnedToCore(WebManager::_taskPerformOTAUpdate, "taskPerformOTAUpdate", 10000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
  if (result != pdPASS) {
    LOG_ERROR("Failed to create task to perform OTA update. Error: ", result);
  }
}

void WebManager::_taskPerformOTAUpdate(void *param) {
  PageManager::GetNSPanelManagerPage()->setText("Updating...");
  PageManager::GetNSPanelManagerPage()->show();

  char checksum_holder[33];
  while (true) {
    std::string checksumUrl = "http://";
    checksumUrl.append(NSPMConfig::instance->manager_address);
    checksumUrl.append(":");
    checksumUrl.append(std::to_string(NSPMConfig::instance->manager_port));
    checksumUrl.append("/checksum_firmware");
    if (HttpLib::GetMD5sum(checksumUrl.c_str(), checksum_holder)) {
      break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  LOG_DEBUG("Got firmware MD5 ", checksum_holder);
  LOG_DEBUG("Stored firmware MD5 ", NSPMConfig::instance->md5_firmware.c_str());

  bool hasAnythingUpdated = false;
  bool firmwareUpdateSuccessful = true;
  yield();
  vTaskDelay(250 / portTICK_PERIOD_MS); // Wait for other tasks.
  if (NSPMConfig::instance->md5_firmware.compare(checksum_holder) != 0) {
    do {
      firmwareUpdateSuccessful = WebManager::_update(U_FLASH, "/download_firmware");
      if (!firmwareUpdateSuccessful) {
        LOG_ERROR("Failed to run OTA. Will try again in 5 seconds.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
      }
    } while (!firmwareUpdateSuccessful);
    if (firmwareUpdateSuccessful) {
      LOG_INFO("Successfully updated firmware.");
      // Save new firmware checksum
      NSPMConfig::instance->md5_firmware = checksum_holder;
      hasAnythingUpdated = true;
    } else {
      LOG_ERROR("Something went wrong during firmware upgrade.");
      PageManager::GetNSPanelManagerPage()->setText("Update failed!");
    }
  } else {
    LOG_INFO("Firmware is the same, will not update.");
  }

  if (firmwareUpdateSuccessful) {
    LOG_INFO("Firmware update done without errors, will check LittleFS.");
    while (true) {
      std::string checksumUrl = "http://";
      checksumUrl.append(NSPMConfig::instance->manager_address);
      checksumUrl.append(":");
      checksumUrl.append(std::to_string(NSPMConfig::instance->manager_port));
      checksumUrl.append("/checksum_data_file");
      if (HttpLib::GetMD5sum(checksumUrl.c_str(), checksum_holder)) {
        break;
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    LOG_DEBUG("Got LittleFS MD5 ", checksum_holder);
    LOG_DEBUG("Stored LittleFS MD5 ", NSPMConfig::instance->md5_data_file.c_str());
    vTaskDelay(250 / portTICK_PERIOD_MS); // Wait for other tasks.
    if (NSPMConfig::instance->md5_data_file.compare(checksum_holder) != 0) {
      LOG_INFO("Will update LittleFS.");
      vTaskDelay(250 / portTICK_PERIOD_MS);
      bool littleFSUpdateSuccessful = false;
      do {
        littleFSUpdateSuccessful = WebManager::_update(U_SPIFFS, "/download_data_file");
        if (!littleFSUpdateSuccessful) {
          LOG_ERROR("Failed to run OTA. Will try again in 5 seconds.");
          vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
      } while (!littleFSUpdateSuccessful);
      if (littleFSUpdateSuccessful) {
        LOG_INFO("Successfully updated LittleFS.");
        // Save new LittleFS checksum
        NSPMConfig::instance->md5_data_file = checksum_holder;
        hasAnythingUpdated = true;
      } else {
        LOG_ERROR("Something went wrong during LittleFS upgrade.");
        PageManager::GetNSPanelManagerPage()->setText("Update failed!");
      }
    } else {
      LOG_INFO("LittleFS is the same, will not update.");
    }
  }

  PageManager::GetNSPanelManagerPage()->setText("Restarting...");
  LOG_INFO("Will restart in 5 seconds.");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  if (hasAnythingUpdated) {
    bool successfully_saved = false;
    do {
      successfully_saved = NSPMConfig::instance->saveToLittleFS(true);
      if (!successfully_saved) {
        LOG_ERROR("Failed to save config, will try again in 1 second.");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
    } while (!successfully_saved);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  ESP.restart();

  vTaskDelete(NULL); // Task complete. Delete FreeRTOS task
}

WebManagerState WebManager::getState() {
  return WebManager::_state;
}

uint8_t WebManager::getUpdateProgress() {
  return WebManager::_update_progress;
}

bool WebManager::_update(uint8_t type, const char *url) {
  LOG_INFO("Starting ", type == U_FLASH ? "Firmware" : "LittleFS", " OTA update...");
  vTaskDelay(250 / portTICK_PERIOD_MS);
  InterfaceManager::stop();
  PageManager::GetScreensaverPage()->stop();
  WebManager::_update_progress = 0;
  std::string downloadUrl = "http://";
  downloadUrl.append(NSPMConfig::instance->manager_address);
  downloadUrl.append(":");
  downloadUrl.append(std::to_string(NSPMConfig::instance->manager_port));
  downloadUrl.append(url);
  if (type == U_FLASH) {
    WebManager::_state = WebManagerState::UPDATING_FIRMWARE;
  } else {
    WebManager::_state = WebManagerState::UPDATING_LITTLEFS;
  }

  LOG_DEBUG("Checking size of '", downloadUrl.c_str(), "'.");
  vTaskDelay(500 / portTICK_PERIOD_MS);
  size_t totalSize = 0;
  while (totalSize == 0) {
    totalSize = HttpLib::GetFileSize(downloadUrl.c_str());
  }
  LOG_INFO("Size of remote file '", downloadUrl.c_str(), "' is ", totalSize, " bytes.");

  LOG_INFO("Starting update.");
  Update.onProgress(WebManager::_onProgress);
  bool canBegin = Update.begin(totalSize, type);
  if (!canBegin) {
    LOG_ERROR("Could not start update!");
    return false;
  }

  HTTPClient http;
  http.begin(downloadUrl.c_str());
  int http_code = http.GET();
  if (http_code == 200) {
    LOG_INFO("Established download stream for firmware update.");
    Update.writeStream(http.getStream());
  }

  if (!Update.end()) {
    LOG_ERROR("Update failed #:", Update.getError());
    return false;
  }

  WebManager::_update_progress = 100;
  LOG_INFO("OTA Successful!");
  if (Update.isFinished()) {
    LOG_WARNING("OTA Done, will reboot!");
    return true;
  } else {
    LOG_ERROR("OTA Not finished. Something went wrong!");
    return false;
  }
}

void WebManager::_onProgress(size_t progress, size_t total) {
  WebManager::_update_progress = (progress / (total / 100));
  if (WebManager::_state == WebManagerState::UPDATING_FIRMWARE && WebManager::_last_displayed_update_progress != WebManager::_update_progress) {
    std::string update_string = "Updating FW ";
    update_string.append(std::to_string(WebManager::_update_progress));
    update_string.append("%");
    PageManager::GetNSPanelManagerPage()->setText(update_string);
  } else if (WebManager::_state == WebManagerState::UPDATING_LITTLEFS && WebManager::_last_displayed_update_progress != WebManager::_update_progress) {
    std::string update_string = "Updating FS ";
    update_string.append(std::to_string(WebManager::_update_progress));
    update_string.append("%");
    PageManager::GetNSPanelManagerPage()->setText(update_string);
  }
  WebManager::_last_displayed_update_progress = progress;
}
