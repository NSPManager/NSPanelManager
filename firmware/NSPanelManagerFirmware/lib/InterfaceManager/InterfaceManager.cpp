#include <InterfaceManager.h>
#include <pages.h>
#include <TftDefines.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MqttLog.h>

std::list<lightConfig*> roomConfig::getCeilingLightsThatAreOn() {
	std::list<lightConfig*> returnList;
	for(lightConfig &light : this->ceilingLights) {
		if(light.level > 0) {
			returnList.push_back(&light);
		}
	}
	return returnList;
}

std::list<lightConfig*> roomConfig::getTableLightsThatAreOn() {
	std::list<lightConfig*> returnList;
	for(lightConfig &light : this->tableLights) {
		if(light.level > 0) {
			returnList.push_back(&light);
		}
	}
	return returnList;
}

std::list<lightConfig*> roomConfig::getAllCeilingLights() {
	std::list<lightConfig*> returnList;
	for(lightConfig &cfg : this->ceilingLights) {
		returnList.push_back(&cfg);
	}
	return returnList;
}

std::list<lightConfig*> roomConfig::getAllTableLights() {
	std::list<lightConfig*> returnList;
	for(lightConfig &cfg : this->tableLights) {
		returnList.push_back(&cfg);
	}
	return returnList;
}

bool roomConfig::anyCeilingLighstOn() {
	for(lightConfig &cfg : this->ceilingLights) {
		if(cfg.level > 0) {
			return true;
		}
	}
	return false;
}

bool roomConfig::anyTableLighstOn() {
	for(lightConfig &cfg : this->tableLights) {
		if(cfg.level > 0) {
			return true;
		}
	}
	return false;
}

bool roomConfig::anyLightsOn() {
	return this->anyCeilingLighstOn() || this->anyTableLighstOn();
}

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

            // 30 failed tries to download config, restart and try again.
            if (tries == 30)
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
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // TODO: Implement table light logic
    }

    vTaskDelete(NULL); // Delete task, we are done
}

void InterfaceManager::processTouchEvent(uint8_t page, uint8_t component, bool pressed)
{
    if (page == HOME_PAGE_ID && !pressed)
    {
        if (component == SWITCH_ROOM_BUTTON_ID)
        {
            InterfaceManager::_instance->_goToNextRoom();
        }
        else if (component == SWITCH_MODE_BUTTON_ID)
        {
            InterfaceManager::_instance->_goToNextMode();
        }
        else if (component == CEILING_LIGHTS_MASTER_BUTTON_ID)
        {
            InterfaceManager::_instance->_ceilingMasterButtonEvent();
        }
        else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID)
		{
			InterfaceManager::_instance->_tableMasterButtonEvent();
		}
        else if (component == LIGHT_LEVEL_CHANGE_BUTTON_ID)
        {
        	// TODO: Adjust only light that are on or if none are on, turn them all to the new value
//        	InterfaceManager::_instance->_changeLightsToLevel(&InterfaceManager::_instance->_cfg.currentRoom->ceilingLights, HomePage::getSaturationValue());
        	if(InterfaceManager::_instance->_cfg.currentRoom->anyLightsOn()) {
        		InterfaceManager::_instance->_updateLightsThatAreOn();
        	} else {
        		InterfaceManager::_instance->_updateAllLights();
        	}
        } else if (component == LIGHT_COLOR_CHANGE_BUTTON_ID) {
        	LOG_DEBUG("Got new color val: ", HomePage::getColorTempValue());
        }
    } else {
    	LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
    }
}

void InterfaceManager::_ceilingMasterButtonEvent()
{
    std::list<lightConfig*> onLights = this->_cfg.currentRoom->getCeilingLightsThatAreOn();

    if(onLights.size() > 0) {
    	this->_changeLightsToLevel(&onLights, 0);
    } else {
    	std::list<lightConfig*> lightList = this->_cfg.currentRoom->getAllCeilingLights();
    	this->_changeLightsToLevel(&lightList, HomePage::getSaturationValue());
    }
}

void InterfaceManager::_tableMasterButtonEvent()
{
    std::list<lightConfig*> onLights = this->_cfg.currentRoom->getTableLightsThatAreOn();

    if(onLights.size() > 0) {
		this->_changeLightsToLevel(&onLights, 0);
	} else {
		std::list<lightConfig*> lightList = this->_cfg.currentRoom->getAllTableLights();
		this->_changeLightsToLevel(&lightList, HomePage::getSaturationValue());
	}
}

void InterfaceManager::_updateLightsThatAreOn() {
	std::list<lightConfig*> ceilingLightsOn = this->_cfg.currentRoom->getCeilingLightsThatAreOn();
	std::list<lightConfig*> tableLightsOn = this->_cfg.currentRoom->getTableLightsThatAreOn();

	uint8_t newLevel = HomePage::getSaturationValue();
	LOG_DEBUG("Setting light level ", newLevel, " for all lights currently on.");
	this->_changeLightsToLevel(&ceilingLightsOn, newLevel);
	this->_changeLightsToLevel(&tableLightsOn, newLevel);
}

void InterfaceManager::_updateAllLights() {
	std::list<lightConfig*> ceilingLightsOn = this->_cfg.currentRoom->getAllCeilingLights();
	std::list<lightConfig*> tableLightsOn = this->_cfg.currentRoom->getAllTableLights();

	uint8_t newLevel = HomePage::getSaturationValue();
	LOG_DEBUG("Setting light level ", newLevel, " for all lights.");
	this->_changeLightsToLevel(&ceilingLightsOn, newLevel);
	this->_changeLightsToLevel(&tableLightsOn, newLevel);
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

        JsonVariant ceilingLights = kv.value()["lights"];
        for (JsonPair lightPair : ceilingLights.as<JsonObject>())
        {
            lightConfig lightCfg;
            lightCfg.id = atoi(lightPair.key().c_str());
            lightCfg.name = lightPair.value()["name"] | "ERR-L";
            lightCfg.canDim = lightPair.value()["can_dim"];
            lightCfg.canTemperature = lightPair.value()["can_temperature"];
            lightCfg.canRgb = lightPair.value()["can_rgb"];
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
        NSPanel::instance->setComponentText("home.mode", "Room lights");
    }
    else if (this->_currentRoomMode == roomMode::house)
    {
        NSPanel::instance->setComponentText("home.mode", "All lights");
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
    InterfaceManager::_mqttMessages.push_back(msg);
    // Notify task that a new message needs processing
    vTaskNotifyGiveFromISR(InterfaceManager::_taskHandleProcessMqttMessages, NULL);
    portYIELD_FROM_ISR();
}

void InterfaceManager::_taskProcessMqttMessages(void *param)
{
    LOG_DEBUG("Starting _taskProcessMqttMessages");
    vTaskDelay(100 / portTICK_PERIOD_MS);
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

void InterfaceManager::_changeLightsToLevel(std::list<lightConfig*> *lights, uint8_t level)
{
    for (lightConfig *light : (*lights))
    {
        std::string topic = "nspanel/entities/light.";
        topic.append(light->name);
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
        if (light.canDim && light.level > 0)
        {
            totalLights++;
            totalBrightness += light.level;
        }
    }

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

    // TODO: Implement check for table lights
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
            vTaskDelay(20 / portTICK_PERIOD_MS);
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
