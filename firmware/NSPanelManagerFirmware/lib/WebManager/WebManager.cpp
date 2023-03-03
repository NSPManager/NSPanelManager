#include <WebManager.h>
#include <LittleFS.h>
#include <NSPMConfig.h>
#include <list>
#include <WiFi.h>
#include <Arduino.h>
#include <Update.h>
#include <MqttLog.h>
#include <HTTPClient.h>
#include <NSPanel.h>

// Make space for variables in memory
WebManager *WebManager::instance;

void WebManager::init(const char *nspmFirmwareVersion)
{
    this->instance = this;
    this->_nspmFirmwareVersion = nspmFirmwareVersion;

    LOG_DEBUG("Setting up web server routes");
    this->_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(LittleFS, "/index.html", String(), false, WebManager::processIndexTemplate); });

    this->_server.on("/save_config", HTTP_POST, WebManager::saveConfigFromWeb);
    this->_server.on("/start_ota_update", HTTP_POST, WebManager::startOTAUpdate);
    this->_server.on("/start_tft_ota_update", HTTP_POST, WebManager::startTFTOTAUpdate);

    this->_server.onNotFound([](AsyncWebServerRequest *request)
                             { request->send(404, "text/plain", "Path/File not found!"); });

    LOG_INFO("Starting web server.");

    this->_server.serveStatic("/static", LittleFS, "/static");
    this->_server.begin();
}

String WebManager::processIndexTemplate(const String &templateVar)
{
    if (templateVar == "version")
    {
        return WebManager::instance->_nspmFirmwareVersion.c_str();
    }
    else if (templateVar == "wifi_hostname")
    {
        return NSPMConfig::instance->wifi_hostname.c_str();
    }
    else if (templateVar == "wifi_ssid")
    {
        return NSPMConfig::instance->wifi_ssid.c_str();
    }
    else if (templateVar == "wifi_psk")
    {
        return NSPMConfig::instance->wifi_psk.c_str();
    }
    else if (templateVar == "manager_address")
    {
        return NSPMConfig::instance->manager_address.c_str();
    }
    else if (templateVar == "manager_port")
    {
        return String(NSPMConfig::instance->manager_port);
    }
    else if (templateVar == "mqtt_server")
    {
        return NSPMConfig::instance->mqtt_server.c_str();
    }
    else if (templateVar == "mqtt_port")
    {
        return String(NSPMConfig::instance->mqtt_port);
    }
    else if (templateVar == "mqtt_username")
    {
        return NSPMConfig::instance->mqtt_username.c_str();
    }
    else if (templateVar == "mqtt_psk")
    {
        return NSPMConfig::instance->mqtt_password.c_str();
    }
    else if (templateVar == "log_level")
	{
		return String(NSPMConfig::instance->logging_level);
	}

    return "-- UNKNOWN TEMPLATE --";
}

void WebManager::saveConfigFromWeb(AsyncWebServerRequest *request)
{
    StaticJsonDocument<2048> config_json;

    NSPMConfig::instance->wifi_hostname = request->arg("wifi_hostname").c_str();
    NSPMConfig::instance->wifi_ssid = request->arg("wifi_ssid").c_str();
    NSPMConfig::instance->wifi_psk = request->arg("wifi_psk").c_str();

    NSPMConfig::instance->manager_address = request->arg("manager_address").c_str();
    NSPMConfig::instance->manager_port = request->arg("manager_port").toInt();

    NSPMConfig::instance->logging_level = request->arg("log_level").toInt();

    NSPMConfig::instance->mqtt_server = request->arg("mqtt_server").c_str();
    NSPMConfig::instance->mqtt_port = request->arg("mqtt_port").toInt();
    NSPMConfig::instance->mqtt_username = request->arg("mqtt_username").c_str();
    NSPMConfig::instance->mqtt_password = request->arg("mqtt_password").c_str();

    if (!NSPMConfig::instance->saveToLittleFS())
    {
        LOG_ERROR("Failed to save new configuration!");
    }

    // TODO: Restart after settings saved
    request->redirect("/");
}

//void WebManager::respondAvailableWiFiNetworks(AsyncWebServerRequest *request)
//{
//    String json = "[";
//    int n = WiFi.scanComplete();
//    if (n == -2)
//    {
//        WiFi.scanNetworks(true);
//    }
//    else if (n)
//    {
//        for (int i = 0; i < n; ++i)
//        {
//            if (!WiFi.SSID(i).isEmpty())
//            {
//                LOG_DEBUG("Found WiFi ", WiFi.SSID(i).c_str());
//                if (i)
//                    json += ",";
//                json += "{";
//                json += "\"rssi\":" + String(WiFi.RSSI(i));
//                json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
//                json += ",\"channel\":" + String(WiFi.channel(i));
//
//                switch (WiFi.encryptionType(i))
//                {
//                case wifi_auth_mode_t::WIFI_AUTH_OPEN:
//                    json += ",\"security\": \"OPEN\"";
//                    break;
//                case wifi_auth_mode_t::WIFI_AUTH_WPA2_PSK:
//                    json += ",\"security\": \"WPA2 PSK\"";
//                    break;
//                case wifi_auth_mode_t::WIFI_AUTH_WPA3_PSK:
//                    json += ",\"security\": \"WPA3 PSK\"";
//                    break;
//                case wifi_auth_mode_t::WIFI_AUTH_WPA_PSK:
//                    json += ",\"security\": \"WPA PSK\"";
//                    break;
//                case wifi_auth_mode_t::WIFI_AUTH_WEP:
//                    json += ",\"security\": \"WEP\"";
//                    break;
//
//                default:
//                    json += ",\"security\": \"UNKNOWN\"";
//                    break;
//                }
//                json += "}";
//            }
//        }
//        WiFi.scanDelete();
//        if (WiFi.scanComplete() == -2)
//        {
//            WiFi.scanNetworks(true);
//        }
//    }
//    json += "]";
//    request->send(200, "application/json", json);
//    json = String();
//}

void WebManager::startTFTOTAUpdate(AsyncWebServerRequest *request)
{
    NSPanel::instance->startOTAUpdate();
    request->send(200);
}

void WebManager::startOTAUpdate(AsyncWebServerRequest *request)
{
    xTaskCreatePinnedToCore(WebManager::_taskPerformOTAUpdate, "taskPerformOTAUpdate", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    request->send(200);
}

void WebManager::_taskPerformOTAUpdate(void *param)
{
    char checksum_holder[32];
    WebManager::_httpGetMD5("/checksum_firmware", checksum_holder);
    LOG_DEBUG("Got firmware MD5 ", checksum_holder);
    LOG_DEBUG("Stored firmware MD5 ", NSPMConfig::instance->md5_firmware.c_str());
    bool hasAnythingUpdated = false;
    bool firmwareUpdateSuccessful = true;
    vTaskDelay(250 / portTICK_PERIOD_MS); // Wait for other tasks.
    if (NSPMConfig::instance->md5_firmware.compare(checksum_holder) != 0)
    {
        int tries = 0;
        do
        {
            firmwareUpdateSuccessful = WebManager::_update(U_FLASH, "/download_firmware");
            tries++;
            if (!firmwareUpdateSuccessful && tries <= 3)
            {
                LOG_ERROR("Failed to run OTA. Will try again in 2 seconds.");
                vTaskDelay(2000);
            }
        } while (!firmwareUpdateSuccessful && tries <= 3);
        if (firmwareUpdateSuccessful)
        {
            LOG_INFO("Successfully updated firmware.");
            // Save new firmware checksum
            NSPMConfig::instance->md5_firmware = checksum_holder;
            NSPMConfig::instance->saveToLittleFS();
            hasAnythingUpdated = true;
        }
        else
        {
            LOG_ERROR("Something went wrong during firmware upgrade.");
        }
    }
    else
    {
        LOG_INFO("Firmware is the same, will not update.");
    }

    if (firmwareUpdateSuccessful)
    {
        LOG_INFO("Firmware update done without errors, will check LittleFS.");
        WebManager::_httpGetMD5("/checksum_data_file", checksum_holder);
        LOG_DEBUG("Got LittleFS MD5 ", checksum_holder);
        LOG_DEBUG("Stored LittleFS MD5 ", NSPMConfig::instance->md5_data_file.c_str());
        vTaskDelay(250 / portTICK_PERIOD_MS); // Wait for other tasks.
        if (NSPMConfig::instance->md5_data_file.compare(checksum_holder) != 0)
        {
            bool littleFSUpdateSuccessful = false;
            int tries = 0;
            do
            {
                littleFSUpdateSuccessful = WebManager::_update(U_SPIFFS, "/download_data_file");
                tries++;
                if (!firmwareUpdateSuccessful && tries <= 3)
                {
                    LOG_ERROR("Failed to run OTA. Will try again in 2 seconds.");
                    vTaskDelay(2000);
                }
            } while (!littleFSUpdateSuccessful && tries <= 3);
            if (littleFSUpdateSuccessful)
            {
                LOG_INFO("Successfully updated LittleFS.");
                // Save new LittleFS checksum
                NSPMConfig::instance->md5_data_file = checksum_holder;
                NSPMConfig::instance->saveToLittleFS();
                hasAnythingUpdated = true;
            }
            else
            {
                LOG_ERROR("Something went wrong during LittleFS upgrade.");
            }
        }
        else
        {
            LOG_INFO("LittleFS is the same, will not update.");
        }
    }

    if (hasAnythingUpdated)
    {
        LOG_INFO("OTA Done, will reboot.");
        ESP.restart();
    }
    else
    {
        LOG_INFO("No OTA was performed, will not reboot.");
    }

    vTaskDelete(NULL); // Task complete. Delete FreeRTOS task
}

bool WebManager::_httpGetMD5(const char *path, char *buffer)
{
    std::string serverUrl = "http://";
    serverUrl.append(NSPMConfig::instance->manager_address);
    serverUrl.append(":");
    serverUrl.append(std::to_string(NSPMConfig::instance->manager_port));
    serverUrl.append(path);

    HTTPClient http;
    http.begin(serverUrl.c_str());
    int responseCode = http.GET();

    if (responseCode == 200)
    {
        http.getString().toCharArray(buffer, 32);
        http.end();
        return true;
    }
    http.end();
    return false;
}

bool WebManager::_update(uint8_t type, const char *url)
{
    LOG_INFO("Starting ", type == U_FLASH ? "FLASH" : "LittleFS", " OTA update...");
    WiFiClient client;
    unsigned long contentLength = 0;
    bool isValidContentType = false;
    if (client.connect(NSPMConfig::instance->manager_address.c_str(), NSPMConfig::instance->manager_port))
    {
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + NSPMConfig::instance->manager_address.c_str() + "\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

        // Wait for response
        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 5000)
            {
                LOG_ERROR("Timeout while downloading firmware!");
                client.stop();
                return false;
            }
        }

        while (client.available())
        {
            String line = client.readStringUntil('\n');
            // remove space, to check if the line is end of headers
            line.trim();

            if (!line.length())
            {
                // headers ended
                break; // and get the OTA started
            }

            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1"))
            {
                if (line.indexOf("200") < 0)
                {
                    LOG_ERROR("Got a non 200 status code from server. Exiting OTA Update.");
                    break;
                }
            }

            // extract headers here
            // Start with content length
            if (line.startsWith("Content-Length: "))
            {
                contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
                LOG_DEBUG("Got ", contentLength, " bytes from server");
            }

            // Next, the content type
            if (line.startsWith("Content-Type: "))
            {
                String contentType = getHeaderValue(line, "Content-Type: ");
                if (contentType == "application/octet-stream")
                {
                    isValidContentType = true;
                }
            }
        }
    }
    else
    {
        LOG_ERROR("Failed to connect to manager!");
        return false;
    }

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength, type);

        if (canBegin)
        {
            LOG_INFO("Starting OTA. This may take a few minutes. Be patient!");

            size_t written = Update.writeStream(client);
            if (written == contentLength)
            {
                LOG_INFO("Wrote ", written, " bytes successfully.");
            }
            else
            {
                LOG_ERROR("Wrote only ", written, "/", contentLength, " bytes.");
            }

            if (Update.end())
            {
                LOG_INFO("OTA Successful!");
                if (Update.isFinished())
                {
                    LOG_WARNING("OTA Done, will reboot!");
                    return true;
                }
                else
                {
                    LOG_ERROR("OTA Not finished. Something went wrong!");
                    return false;
                }
            }
            else
            {
                LOG_ERROR("OTA Update error #:", Update.getError());
                return false;
            }
        }
        else
        {
            LOG_ERROR("Not enough space for OTA update!");
            client.flush();
            return false;
        }
    }
    else
    {
        LOG_ERROR("There was no content in the OTA response!");
        client.flush();
        return false;
    }

    return true;
}
