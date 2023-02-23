#include <WebManager.h>
#include <ArduLog.h>
#include <LittleFS.h>
#include <NSPMConfig.h>
#include <list>
#include <WiFi.h>
#include <Arduino.h>
#include <Update.h>

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

    request->redirect("/");
}

void WebManager::respondAvailableWiFiNetworks(AsyncWebServerRequest *request)
{
    String json = "[";
    int n = WiFi.scanComplete();
    if (n == -2)
    {
        WiFi.scanNetworks(true);
    }
    else if (n)
    {
        for (int i = 0; i < n; ++i)
        {
            if (!WiFi.SSID(i).isEmpty())
            {
                LOG_DEBUG("Found WiFi ", WiFi.SSID(i).c_str());
                if (i)
                    json += ",";
                json += "{";
                json += "\"rssi\":" + String(WiFi.RSSI(i));
                json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
                json += ",\"channel\":" + String(WiFi.channel(i));

                switch (WiFi.encryptionType(i))
                {
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
        if (WiFi.scanComplete() == -2)
        {
            WiFi.scanNetworks(true);
        }
    }
    json += "]";
    request->send(200, "application/json", json);
    json = String();
}

void WebManager::startOTAUpdate(AsyncWebServerRequest *request)
{
    xTaskCreatePinnedToCore(WebManager::_taskPerformOTAUpdate, "taskPerformOTAUpdate", 5000, NULL, 0, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    request->send(200);
}

void WebManager::_taskPerformOTAUpdate(void *param)
{
    if (WebManager::_update(U_FLASH, "/download_firmware"))
    {
        LOG_INFO("Update of U_FLASH successful, will continue with update of U_SPIFFS");
        if (WebManager::_update(U_SPIFFS, "/download_data_file"))
        {
            // Resave config to the new file system
            NSPMConfig::instance->saveToLittleFS();
            LOG_INFO("OTA Update complete! Will reboot.");
            ESP.restart();
        }
        else
        {
            LOG_ERROR("Something went wrong when updating U_SPIFFS!");
        }
    }
    else
    {
        LOG_ERROR("Something went wrong during OTA update of U_FLASH!");
    }
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
            LOG_INFO("Starting OTA. This may take a few minutes. Be patience!");

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