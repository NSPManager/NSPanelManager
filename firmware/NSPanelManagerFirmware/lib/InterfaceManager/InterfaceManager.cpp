#include <InterfaceManager.h>
#include <TftDefines.h>
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
    xTaskCreatePinnedToCore(_taskLoadConfigAndInit, "taskLoadConfigAndInit", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::_taskLoadConfigAndInit(void *param)
{
    unsigned long start = millis();
    while (!WiFi.isConnected() || !InterfaceManager::_instance->_mqttClient->connected())
    {
        if (!WiFi.isConnected())
        {
            NSPanel::instance->setComponentText("bootscreen.t_loading", "Connecting to WiFi...");
        }
        else if (!InterfaceManager::_instance->_mqttClient->connected())
        {
            NSPanel::instance->setComponentText("bootscreen.t_loading", "Connecting to MQTT...");
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if (millis() - start < 8000)
    {
        LOG_DEBUG("WiFi and MQTT ready. Will delay for panel to become ready...");
        vTaskDelay((millis() - start) / portTICK_PERIOD_MS);
    }

    NSPanel::instance->setComponentText("bootscreen.t_loading", "Loading config...");

    InterfaceManager::_instance->_roomDataJson = new DynamicJsonDocument(2048);
    uint8_t tries = 0;
    bool successDownloadingConfig = false;
    do
    {
        successDownloadingConfig = InterfaceManager::_instance->_getPanelConfig();
        if (!successDownloadingConfig)
        {
            tries++;
            LOG_ERROR("Failed to download config, will try again in 5 seconds.");
            vTaskDelay(5000 / portTICK_PERIOD_MS);

            if (tries == 5)
            {
                LOG_ERROR("Failed to download config, will restart and try again.");
                NSPanel::instance->setComponentText("bootscreen.t_loading", "Restarting...");
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                ESP.restart();
            }
        }
    } while (!successDownloadingConfig);
    // Config downloaded, process the raw data
    InterfaceManager::_instance->_processPanelConfig();
    delete InterfaceManager::_instance->_roomDataJson; // All JSON-data processed, delete data from memory

    // Set some default values before showing page
    InterfaceManager::_instance->_changeMode(roomMode::room);

    LOG_DEBUG("Free HEAP: ", ESP.getFreeHeap());
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Start task for MQTT processing
    xTaskCreatePinnedToCore(_taskProcessMqttMessages, "taskLoadConfigAndInit", 5000, NULL, 1, &InterfaceManager::_taskHandleProcessMqttMessages, CONFIG_ARDUINO_RUNNING_CORE);

    // As there may be may be MANY topics to subscribe to, do it in checks of 5 with delays
    // between them to allow for processing all the incoming data.
    NSPanel::instance->setComponentText("bootscreen.t_loading", "Subscribing...");
    uint8_t numSubscribed = 0;
    // Every light in every room
    for (roomConfig &roomCfg : InterfaceManager::_instance->_cfg.rooms)
    {
        for (lightConfig &lightCfg : roomCfg.ceilingLights)
        {
            // Build topic from name
            std::string levelStatusTopic = "nspanel/entities/light.";
            levelStatusTopic.append(lightCfg.name);
            levelStatusTopic.append("/state_brightness_pct");
            InterfaceManager::_instance->_mqttClient->subscribe(levelStatusTopic.c_str());
            // Check if it is time to delay to allow for processing of incoming MQTT data
            numSubscribed++;
            if (numSubscribed == 5)
            {
                vTaskDelay(50 / portTICK_PERIOD_MS);
                numSubscribed = 0;
            }
        }
        // TODO: Implement table light logic
        // for (lightConfig &lightCfg : roomCfg.tableLights)
        // {
        // }
    }

    vTaskDelete(NULL); // Delete task, we are done
}

void InterfaceManager::processTouchEvent(uint8_t page, uint8_t component, bool pressed)
{
    LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");

    if (page == HOME_PAGE_ID && pressed)
    {
        if (component == SWITCH_ROOM_BUTTON_ID)
        {
            InterfaceManager::_instance->_goToNextRoom();
        }
        else if (component == SWITCH_MODE_BUTTON_ID)
        {
            InterfaceManager::_instance->_goToNextMode();
        }
        else if (component == CEILING_LIGHTS_RAISE_BUTTON_ID)
        {
            // Raise light level of all turned on lights to next step of 10 from average light level
            // If no lights are turned on, turn them all on to level 10
            std::list<lightConfig> lights;
            int totalLights = 0;
            int totalBrightness = 0;
            for (lightConfig &cfg : InterfaceManager::_instance->_cfg.currentRoom->ceilingLights)
            {
                if (cfg.level > 0)
                {
                    lights.push_back(cfg);
                    totalLights++;
                    totalBrightness += cfg.level;
                }
            }
            // No lights are currently on. Switch ALL the ceiling lights on to level 10.
            if (totalLights == 0)
            {
                InterfaceManager::_instance->_changeLightsToLevel(&InterfaceManager::_instance->_cfg.currentRoom->ceilingLights, 10);
            }
            else
            {
                uint8_t averageBrightness = (totalBrightness / totalLights);
                uint8_t newBrightness = InterfaceManager::roundToNearest(averageBrightness + 10, 10);
                if (newBrightness > 100)
                {
                    newBrightness == 100;
                }
                InterfaceManager::_instance->_changeLightsToLevel(&lights, newBrightness);
            }
        }
        else if (component == CEILING_LIGHTS_LOWER_BUTTON_ID)
        {
            // Lower light level of all turned on lights to next step of 10 from average light level
            std::list<lightConfig> lights;
            int totalLights = 0;
            int totalBrightness = 0;
            for (lightConfig &cfg : InterfaceManager::_instance->_cfg.currentRoom->ceilingLights)
            {
                if (cfg.level > 0)
                {
                    lights.push_back(cfg);
                    totalLights++;
                    totalBrightness += cfg.level;
                }
            }
            // No lights are currently on. Switch ALL the ceiling lights on to level 10.
            if (totalLights > 0)
            {
                uint8_t averageBrightness = (totalBrightness / totalLights);
                uint8_t newBrightness = InterfaceManager::roundToNearest(averageBrightness - 10, 10);
                if (newBrightness < 0)
                {
                    newBrightness == 0;
                }
                InterfaceManager::_instance->_changeLightsToLevel(&lights, newBrightness);
            }
        }
        else if (component == CEILING_LIGHTS_MASTER_BUTTON_ID)
        {
            int totalLights = 0;
            for (lightConfig &cfg : InterfaceManager::_instance->_cfg.currentRoom->ceilingLights)
            {
                if (cfg.level > 0)
                {
                    totalLights++;
                }
            }
            // Some lights are turned on, turn them all off
            if (totalLights > 0)
            {
                InterfaceManager::_instance->_changeLightsToLevel(&InterfaceManager::_instance->_cfg.currentRoom->ceilingLights, 0);
            }
            else
            {
                // All light are turned off, turn them all to level 50
                InterfaceManager::_instance->_changeLightsToLevel(&InterfaceManager::_instance->_cfg.currentRoom->ceilingLights, 50);
            }
        }
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

        JsonVariant ceilingLights = kv.value()["lights"];
        for (JsonPair lightPair : ceilingLights.as<JsonObject>())
        {
            lightConfig lightCfg;
            lightCfg.id = atoi(lightPair.key().c_str());
            lightCfg.name = lightPair.value()["name"] | "ERR-L";
            lightCfg.canDim = lightPair.value()["can_dim"];
            lightCfg.canTemperature = lightPair.value()["can_temperature"];
            lightCfg.canRgb = lightPair.value()["can_rgb"];
            LOG_DEBUG("Loaded light ID: ", lightCfg.id);
            if (lightPair.value()["ceiling"] == true)
            {
                roomCfg.ceilingLights.push_back(lightCfg);
            }
            else
            {
                roomCfg.tableLights.push_back(lightCfg);
            }
        }

        this->_cfg.rooms.push_back(roomCfg);
    }

    // Panel config processed, clear Json data
    this->_roomDataJson->clear();

    this->_changeRoom(this->_cfg.homeScreen);
    NSPanel::instance->goToPage("home");
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
    this->_updatePanelLightStatus();
}

void InterfaceManager::_goToNextMode()
{
    roomMode newMode = static_cast<roomMode>(static_cast<int>(this->_currentRoomMode) + 1);
    if (newMode == roomMode::END)
    {
        newMode = roomMode::room;
    }
    this->_changeMode(newMode);
}

void InterfaceManager::_changeMode(roomMode mode)
{
    this->_currentRoomMode = mode;
    if (this->_currentRoomMode == roomMode::room)
    {
        NSPanel::instance->setComponentText("home.mode", "Room");
    }
    else if (this->_currentRoomMode == roomMode::house)
    {
        NSPanel::instance->setComponentText("home.mode", "House");
    }
    else
    {
        NSPanel::instance->setComponentText("home.mode", "UNKNOWN");
    }
}

void InterfaceManager::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    mqttMessage msg;
    msg.topic = topic;
    msg.payload = std::string((char *)payload, length);
    LOG_DEBUG("Got message in ", msg.payload.c_str());
    InterfaceManager::_mqttMessages.push_back(msg);
    // Notify task that a new message needs processing
    xTaskNotifyGive(InterfaceManager::_taskHandleProcessMqttMessages);
}

void InterfaceManager::_taskProcessMqttMessages(void *param)
{
    LOG_DEBUG("Starting _taskProcessMqttMessages");
    for (;;)
    {
        // Wait for notification that we need to process messages.
        LOG_DEBUG("Pausing for new MQTT messages");
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
        {
            // Process all the messages
            while (InterfaceManager::_mqttMessages.size() > 0)
            {
                mqttMessage msg = InterfaceManager::_mqttMessages.front();
                LOG_DEBUG("Processing message from: ", msg.topic.c_str());
                try
                {
                    std::string domain = msg.topic;
                    domain = domain.erase(0, strlen("nspanel/entities/"));
                    domain = domain.substr(0, domain.find('.'));

                    std::string entity = msg.topic;
                    entity = entity.erase(0, entity.find('.') + 1);
                    entity = entity.substr(0, entity.find('/'));

                    std::string attribute = msg.topic;
                    attribute = attribute.erase(0, attribute.find_last_of('/') + 1);

                    if (domain.compare("light") == 0 && attribute.compare("state_brightness_pct") == 0)
                    {
                        InterfaceManager::_instance->_setLightLevel(entity, atoi(msg.payload.c_str()));
                    }
                }
                catch (...)
                {
                    LOG_ERROR("Error processing MQTT message on topic ", msg.topic.c_str());
                }
                InterfaceManager::_mqttMessages.pop_front();
                vTaskDelay(5); // Wait 5ms between processing each event to allow for other tasks.
            }
        }
    }
}

void InterfaceManager::_setLightLevel(std::string light, uint8_t level)
{
    for (roomConfig &roomCfg : InterfaceManager::_instance->_cfg.rooms)
    {
        for (lightConfig &lightCfg : roomCfg.ceilingLights)
        {
            if (lightCfg.name.compare(light) == 0)
            {
                lightCfg.level = level;
                this->_updatePanelLightStatus();
                return;
            }
        }

        for (lightConfig &lightCfg : roomCfg.tableLights)
        {
            if (lightCfg.name.compare(light) == 0)
            {
                lightCfg.level = level;
                this->_updatePanelLightStatus();
                return;
            }
        }
    }
}

void InterfaceManager::_changeLightsToLevel(std::list<lightConfig> *lights, uint8_t level)
{
    for (lightConfig &light : (*lights))
    {
        std::string topic = "nspanel/entities/light.";
        topic.append(light.name);
        topic.append("/brightness_pct");
        this->_mqttClient->publish(topic.c_str(), std::to_string(level).c_str());
    }
}

void InterfaceManager::_updatePanelLightStatus()
{
    uint totalLights = 0;
    uint totalBrightness = 0;
    for (lightConfig &light : this->_cfg.currentRoom->ceilingLights)
    {
        if (light.level > 0)
        {
            totalLights++;
            totalBrightness += light.level;
        }
    }

    LOG_DEBUG("Total light: ", totalLights);
    LOG_DEBUG("Total brightness: ", totalBrightness);

    uint averageBrightness = totalLights == 0 ? 0 : totalBrightness / totalLights;
    if (averageBrightness > 0)
    {
        NSPanel::instance->setComponentVal("home.b_ceiling", 1);
    }
    else
    {
        NSPanel::instance->setComponentVal("home.b_ceiling", 0);
    }
    NSPanel::instance->setComponentVal("home.n_ceiling", averageBrightness);
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

uint8_t InterfaceManager::roundToNearest(uint8_t original, uint8_t step)
{
    uint8_t lower = (original / step) * step;
    uint8_t upper = original + step;
    return (original - lower > upper - original) ? upper : lower;
}