#include <InterfaceManager.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MqttLog.h>

void InterfaceManager::init(PubSubClient *mqttClient)
{
    this->_instance = this;
    this->_mqttClient = mqttClient;
    this->_mqttClient->setCallback(&InterfaceManager::mqttCallback);
    NSPanel::attachTouchEventCallback(InterfaceManager::processTouchEvent);
    NSPanel::instance->goToPage("bootscreen");
    NSPanel::instance->setComponentText("bootscreen.t_loading", "Loading config from managerment server...");

    this->_roomDataJson = new DynamicJsonDocument(2048);
    uint8_t tries = 0;
    bool successDownloadingConfig = false;
    do
    {
        successDownloadingConfig = this->_getPanelConfig();
        if (!successDownloadingConfig)
        {
            tries++;
            LOG_ERROR("Failed to download config, will try again in 5 seconds.");
            vTaskDelay(5000 / portTICK_RATE_MS);

            if (tries == 5)
            {
                LOG_ERROR("Failed to download config, will restart and try again.");
                ESP.restart();
            }
        }
    } while (!successDownloadingConfig);
    // Config downloaded, process the raw data
    this->_processPanelConfig();
    delete this->_roomDataJson; // All JSON-data processed, delete data from memory
}

void InterfaceManager::processTouchEvent(uint8_t page, uint8_t component, bool pressed)
{
    LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");

    if (page == 2 && component == 21 && pressed)
    {
        InterfaceManager::_instance->_goToNextRoom();
    }
}

void InterfaceManager::_processPanelConfig()
{
    LOG_DEBUG("Processing rooms");
    this->_cfg.homeScreen = (*this->_roomDataJson)["home"].as<uint8_t>();
    for (JsonPair kv : (*this->_roomDataJson)["rooms"].as<JsonObject>())
    {
        roomConfig roomCfg;
        roomCfg.id = atoi(kv.key().c_str());
        roomCfg.name = kv.value()["name"] | "ERR";
        LOG_DEBUG("Loaded room ID: ", roomCfg.id);
        this->_cfg.rooms.push_back(roomCfg);
    }

    // Panel config processed, clear Json data
    this->_roomDataJson->clear();

    this->_changeRoom(this->_cfg.homeScreen);
    NSPanel::instance->goToPage("page home");
}

void InterfaceManager::_goToNextRoom()
{
    LOG_DEBUG("Switching to next room.");
    this->_cfg.currentRoom++;
    if (this->_cfg.currentRoom == this->_cfg.rooms.end())
    {
        LOG_DEBUG("End of rooms. Going to first.");
        this->_cfg.currentRoom = this->_cfg.rooms.begin();
    }
    this->_updatePanelWithNewRoomInfo();
}

void InterfaceManager::_changeRoom(uint8_t roomId)
{
    bool foundRoom = false;
    for (std::list<roomConfig>::iterator it = this->_cfg.rooms.begin(); it != this->_cfg.rooms.end(); it++)
    {
        if (it->id == roomId)
        {
            LOG_DEBUG("Found requested room with ID: ", roomId);
            this->_cfg.currentRoom = it;
            this->_updatePanelWithNewRoomInfo();
            return;
        }
    }

    LOG_ERROR("Did not find requested room. Will cancel operation.");
    return;
}

void InterfaceManager::_updatePanelWithNewRoomInfo()
{
    NSPanel::instance->setComponentText("home.room", this->_cfg.currentRoom->name.c_str());
}

void InterfaceManager::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    std::string tpc = topic;
    StaticJsonDocument<128> data;
    deserializeJson(data, payload, length);
}

bool InterfaceManager::_getPanelConfig()
{
    LOG_INFO("Trying to download NSPanel config.");
    WiFiClient client;
    unsigned long contentLength = 0;
    bool isValidContentType = false;
    if (client.connect(NSPMConfig::instance->manager_address.c_str(), NSPMConfig::instance->manager_port))
    {
        client.print(String("GET /api/get_nspanel_config?mac=") + WiFi.macAddress() + " HTTP/1.1\r\n" +
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
                if (contentType == "application/json")
                {
                    isValidContentType = true;
                }
                else
                {
                    LOG_ERROR("Received invalid content type: ", contentType.c_str());
                }
            }
        }
    }
    else
    {
        LOG_ERROR("Failed to connect to manager!");
    }

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        DeserializationError error = deserializeJson((*this->_roomDataJson), client);
        if (!error)
        {
            return true;
        }
    }
    else
    {
        LOG_ERROR("There was no content in the OTA response!");
        client.flush();
    }
    return false;
}