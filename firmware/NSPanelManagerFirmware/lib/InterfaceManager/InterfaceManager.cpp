#include <InterfaceManager.h>
#include <pages.h>
#include <TftDefines.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MqttLog.h>

void InterfaceManager::init(PubSubClient *mqttClient)
{
    this->instance = this;
    this->_mqttClient = mqttClient;
    this->_mqttClient->setCallback(&InterfaceManager::mqttCallback);
    this->_currentEditMode = editLightMode::all_lights;
    this->_ignoreNextTouchRelease = false;
    this->_lastMasterCeilingLightsButtonTouch = 0;
    this->_lastMasterCeilingLightsButtonRelease = 0;
    this->_lastMasterTableLightsButtonTouch = 0;
    this->_lastMasterTableLightsButtonRelease = 0;
    this->_isFingerOnDisplay = false;
    NSPanel::attachTouchEventCallback(InterfaceManager::processTouchEvent);
    NSPanel::attachSleepCallback(InterfaceManager::processSleepEvent);
    NSPanel::instance->goToPage("bootscreen");
    xTaskCreatePinnedToCore(_taskLoadConfigAndInit, "taskLoadConfigAndInit", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::_taskLoadConfigAndInit(void *param)
{
    unsigned long start = millis();
    while (!WiFi.isConnected() || !InterfaceManager::instance->_mqttClient->connected())
    {
        if (!WiFi.isConnected())
        {
            if(NSPMConfig::instance->NSPMConfig::instance->wifi_ssid.empty()) {
                NSPanel::instance->setComponentText("bootscreen.t_loading", "Connect to AP NSPMPanel");
            } else {
                NSPanel::instance->setComponentText("bootscreen.t_loading", "Connecting to WiFi...");
            }
        }
        else if (!InterfaceManager::instance->_mqttClient->connected())
        {
            NSPanel::instance->setComponentText("bootscreen.t_loading", "Connecting to MQTT...");
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    // Wait for panel to become ready
    if (millis() - start < 4000)
    {
        vTaskDelay((millis() - start) / portTICK_PERIOD_MS);
    }

    NSPanel::instance->setComponentText("bootscreen.t_loading", "Loading config...");

    InterfaceManager::instance->_roomDataJson = new DynamicJsonDocument(4096);
    uint8_t tries = 0;
    bool successDownloadingConfig = false;
    do
    {
        successDownloadingConfig = InterfaceManager::instance->_getPanelConfig();
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
    InterfaceManager::instance->_processPanelConfig();
    delete InterfaceManager::instance->_roomDataJson; // All JSON-data processed, delete data from memory

    // Set some default values before showing page
    InterfaceManager::instance->_changeMode(roomMode::room);

    // Start task for MQTT processing
    xTaskCreatePinnedToCore(_taskProcessMqttMessages, "taskLoadConfigAndInit", 5000, NULL, 1, &InterfaceManager::_taskHandleProcessMqttMessages, CONFIG_ARDUINO_RUNNING_CORE);

    // As there may be may be MANY topics to subscribe to, do it in checks of 5 with delays
    // between them to allow for processing all the incoming data.
    NSPanel::instance->setComponentText("bootscreen.t_loading", "Subscribing...");
    InterfaceManager::subscribeToMqttTopics();

    vTaskDelete(NULL); // Delete task, we are done
}

void InterfaceManager::subscribeToMqttTopics() {
	// Every light in every room
	for (roomConfig &roomCfg : InterfaceManager::instance->_cfg.rooms)
	{
		for (lightConfig &lightCfg : roomCfg.ceilingLights)
		{
			InterfaceManager::instance->_subscribeToLightTopics(&lightCfg);
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		for (lightConfig &lightCfg : roomCfg.tableLights)
		{
			InterfaceManager::instance->_subscribeToLightTopics(&lightCfg);
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
	}
}

void InterfaceManager::_subscribeToLightTopics(lightConfig *cfg) {
	// Build topic from name
	std::string levelStatusTopic = "nspanel/entities/light/";
	levelStatusTopic.append(cfg->name);
	levelStatusTopic.append("/state_brightness_pct");
	this->_mqttClient->subscribe(levelStatusTopic.c_str());

	if(cfg->canTemperature) {
		std::string colorTempStateTopic = "nspanel/entities/light/";
		colorTempStateTopic.append(cfg->name);
		colorTempStateTopic.append("/state_kelvin");
		this->_mqttClient->subscribe(colorTempStateTopic.c_str());
	}
}

void InterfaceManager::processSleepEvent() {
    LOG_DEBUG("Display went to sleep, resetting display to default.");
    // Display went to sleep, reset everything
    InterfaceManager::instance->_isFingerOnDisplay = false; // Reset in case it got stuck
    InterfaceManager::instance->_changeRoom(InterfaceManager::instance->_cfg.homeScreen);
    InterfaceManager::instance->_changeMode(roomMode::room);
    InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
    InterfaceManager::instance->_updatePanelLightStatus();
}

void InterfaceManager::processTouchEvent(uint8_t page, uint8_t component, bool pressed)
{
    InterfaceManager::instance->_isFingerOnDisplay = pressed;

    if(!pressed && InterfaceManager::instance->_ignoreNextTouchRelease) {
        InterfaceManager::instance->_ignoreNextTouchRelease = false; // Reset block
        return;
    }

    if (page == HOME_PAGE_ID && !pressed)
    {
        if (component == SWITCH_ROOM_BUTTON_ID)
        {
            InterfaceManager::instance->_goToNextRoom();
        }
        else if (component == SWITCH_MODE_BUTTON_ID)
        {
            InterfaceManager::instance->_goToNextMode();
        }
        else if (component == CEILING_LIGHTS_MASTER_BUTTON_ID)
        {
            InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
            InterfaceManager::instance->_lastMasterCeilingLightsButtonRelease = millis();
            InterfaceManager::instance->_ceilingMasterButtonEvent();
        }
        else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID)
		{
            InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
            InterfaceManager::instance->_lastMasterTableLightsButtonRelease = millis();
            InterfaceManager::instance->_tableMasterButtonEvent();
		}
        else if (component == LIGHT_LEVEL_CHANGE_BUTTON_ID)
        {
            // Dimmer slider changed
            if(InterfaceManager::instance->_currentRoomMode == roomMode::room && InterfaceManager::instance->_cfg.currentRoom->anyLightsOn()) {
                InterfaceManager::instance->_updateLightsThatAreOn();
            } else if(InterfaceManager::instance->_currentRoomMode == roomMode::house && InterfaceManager::instance->_cfg.anyLightsOn()) {
                InterfaceManager::instance->_updateLightsThatAreOn();
            } else {
                InterfaceManager::instance->_updateAllLights();
            }
        	
            InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
        } else if (component == LIGHT_COLOR_CHANGE_BUTTON_ID) {
        	InterfaceManager::instance->_updateLightsColorTemp();
            InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
        } else if (component == ROOM_BUTTON_ID) {
            // Show page with all lights
            NSPanel::instance->goToPage("Room1");
        }
    } else if (page == HOME_PAGE_ID && pressed) {
        if (component == CEILING_LIGHTS_MASTER_BUTTON_ID)
        {
            InterfaceManager::instance->_lastMasterCeilingLightsButtonTouch = millis();
            InterfaceManager::instance->_startSpecialModeTriggerTask(editLightMode::ceiling_lights);
        }
        else if (component == TABLE_LIGHTS_MASTER_BUTTON_ID)
		{
			InterfaceManager::instance->_lastMasterTableLightsButtonTouch = millis();
            InterfaceManager::instance->_startSpecialModeTriggerTask(editLightMode::table_lights);
		}
        LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
    } else {
    	LOG_DEBUG("Component ", page, ".", component, " ", pressed ? "PRESSED" : "DEPRESSED");
    }
}

void InterfaceManager::_ceilingMasterButtonEvent()
{
    if(this->_currentRoomMode == roomMode::room) {
        std::list<lightConfig*> onLights = this->_cfg.currentRoom->getCeilingLightsThatAreOn();
        if(onLights.size() > 0) {
            this->_changeLightsToLevel(&onLights, 0);
        } else {
            std::list<lightConfig*> lightList = this->_cfg.currentRoom->getAllCeilingLights();
            this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
        }
    } else if (this->_currentRoomMode == roomMode::house) {
        std::list<lightConfig*> onLights = this->_cfg.getCeilingLightsThatAreOn();
        if(onLights.size() > 0) {
            this->_changeLightsToLevel(&onLights, 0);
        } else {
            std::list<lightConfig*> lightList = this->_cfg.getAllCeilingLights();
            this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
        }
    }

    this->_updatePanelLightStatus();
}

void InterfaceManager::_tableMasterButtonEvent()
{
    if(this->_currentRoomMode == roomMode::room) {
        std::list<lightConfig*> onLights = this->_cfg.currentRoom->getTableLightsThatAreOn();

        if(onLights.size() > 0) {
            this->_changeLightsToLevel(&onLights, 0);
        } else {
            std::list<lightConfig*> lightList = this->_cfg.currentRoom->getAllTableLights();
            this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
        }
        this->_updatePanelLightStatus();
    } else if (this->_currentRoomMode == roomMode::house) {
        std::list<lightConfig*> onLights = this->_cfg.getTableLightsThatAreOn();
        if(onLights.size() > 0) {
            this->_changeLightsToLevel(&onLights, 0);
        } else {
            std::list<lightConfig*> lightList = this->_cfg.getAllTableLights();
            this->_changeLightsToLevel(&lightList, HomePage::getDimmingValue());
        }
    }
    
    this->_updatePanelLightStatus();
}

void InterfaceManager::_updateLightsThatAreOn() {
	std::list<lightConfig*> lights;
    if(this->_currentRoomMode == roomMode::room) {
        if(this->_currentEditMode == editLightMode::all_lights) {
            lights = this->_cfg.currentRoom->getAllLightsThatAreOn();
        } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
            lights = this->_cfg.currentRoom->getCeilingLightsThatAreOn();
        } else if (this->_currentEditMode == editLightMode::table_lights) {
            lights = this->_cfg.currentRoom->getTableLightsThatAreOn();
        }
    } else if (this->_currentRoomMode == roomMode::house) {
        if(this->_currentEditMode == editLightMode::all_lights) {
            lights = this->_cfg.getAllLightsThatAreOn();
        } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
            lights = this->_cfg.getCeilingLightsThatAreOn();
        } else if (this->_currentEditMode == editLightMode::table_lights) {
            lights = this->_cfg.getTableLightsThatAreOn();
        }
    }

	uint8_t newLevel = HomePage::getDimmingValue();
	this->_changeLightsToLevel(&lights, newLevel);
	this->_updatePanelLightStatus();
}

void InterfaceManager::_updateAllLights() {
	std::list<lightConfig*> lights;
    if(this->_currentRoomMode == roomMode::room) {
        if(this->_currentEditMode == editLightMode::all_lights) {
            lights = this->_cfg.currentRoom->getAllLights();
        } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
            lights = this->_cfg.currentRoom->getCeilingLightsThatAreOn();
        } else if (this->_currentEditMode == editLightMode::table_lights) {
            lights = this->_cfg.currentRoom->getTableLightsThatAreOn();
        }
    } else if (this->_currentRoomMode == roomMode::house) {
        if(this->_currentEditMode == editLightMode::all_lights) {
            lights = this->_cfg.getAllLights();
        } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
            lights = this->_cfg.getCeilingLightsThatAreOn();
        } else if (this->_currentEditMode == editLightMode::table_lights) {
            lights = this->_cfg.getTableLightsThatAreOn();
        }
    }

	uint8_t newLevel = HomePage::getDimmingValue();
	this->_changeLightsToLevel(&lights, newLevel);
	this->_updatePanelLightStatus();
}

void InterfaceManager::_updateLightsColorTemp() {
	std::list<lightConfig*> lights;
    if(this->_currentRoomMode == roomMode::room) {
        if(this->_currentEditMode == editLightMode::all_lights) {
            lights = this->_cfg.currentRoom->getAllLights();
        } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
            lights = this->_cfg.currentRoom->getAllCeilingLights();
        } else if (this->_currentEditMode == editLightMode::table_lights) {
            lights = this->_cfg.currentRoom->getAllTableLights();
        }
    } else if (this->_currentRoomMode == roomMode::house) {
        if(this->_currentEditMode == editLightMode::all_lights) {
            lights = this->_cfg.getAllLights();
        } else if (this->_currentEditMode == editLightMode::ceiling_lights) {
            lights = this->_cfg.getAllCeilingLights();
        } else if (this->_currentEditMode == editLightMode::table_lights) {
            lights = this->_cfg.getAllTableLights();
        }
    }

	// Get value from panel and calculate for 2000-6500 kelvin. Also inverse value, warm color temperature on top
	// TODO: Implement check in management web interface that allows for this to be reversed
	uint16_t newKelvin = HomePage::getColorTempValue();
	this->_changeLightsToKelvin(&lights, newKelvin);
	this->_updatePanelLightStatus();
}

void InterfaceManager::_setEditLightMode(editLightMode mode) {
    InterfaceManager::instance->_currentEditMode = mode; // Set current mode
    if(mode == editLightMode::all_lights) {
        HomePage::setSliderLightLevelColor(23243); // Reset to normal color
        HomePage::setSliderColorTempColor(23243);  // Reset to normal color
        HomePage::setHighlightCeilingVisibility(false);
        HomePage::setHighlightTableVisibility(false);
    } else {
        if(mode == editLightMode::ceiling_lights) {
            HomePage::setHighlightCeilingVisibility(true);
            HomePage::setHighlightTableVisibility(false);
        } else if(mode == editLightMode::table_lights) {
            HomePage::setHighlightTableVisibility(true);
            HomePage::setHighlightCeilingVisibility(false);
        }

        HomePage::setSliderLightLevelColor(65024); // Change slider color to indicate special mode
        HomePage::setSliderColorTempColor(65024);  // Change slider color to indicate special mode
    }
    this->_updatePanelLightStatus();
    InterfaceManager::instance->_startSpecialModeTimer();
}

void InterfaceManager::_startSpecialModeTimer() {
    this->_lastSpecialModeEventMillis = millis();
    if(this->_taskHandleSpecialModeTimer == NULL) {
        xTaskCreatePinnedToCore(_taskSpecialModeTimer, "taskSpecialModeTimer", 5000, NULL, 1, &InterfaceManager::_taskHandleSpecialModeTimer, CONFIG_ARDUINO_RUNNING_CORE);
    }
}

void InterfaceManager::_taskSpecialModeTimer(void* param) {
    // Wait until no event has occured for 5 seconds before returning to normal mode
    while(true) {
        // TODO: Make timeout configurable
        if(!InterfaceManager::instance->_isFingerOnDisplay && millis() > InterfaceManager::instance->_lastSpecialModeEventMillis + 5000) {
            break;
        } else if(!InterfaceManager::instance->_isFingerOnDisplay && millis() > InterfaceManager::instance->_lastSpecialModeEventMillis + 5000) {
            InterfaceManager::instance->_lastSpecialModeEventMillis = millis();
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    InterfaceManager::instance->_setEditLightMode(editLightMode::all_lights);
    InterfaceManager::_taskHandleSpecialModeTimer = NULL;
    vTaskDelete(NULL); // Task is complete, stop task
}

void InterfaceManager::_startSpecialModeTriggerTask(editLightMode triggerMode) {
    this->_triggerSpecialEditLightMode = triggerMode;
    xTaskCreatePinnedToCore(_taskSpecialModeTriggerTask, "taskSpecialModeTriggerTask", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void InterfaceManager::_taskSpecialModeTriggerTask(void* param) {
    unsigned long start = millis();
    unsigned long lastRelease;
    if(InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::ceiling_lights) {
        lastRelease = InterfaceManager::instance->_lastMasterCeilingLightsButtonRelease;
    } else if(InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::table_lights) {
        lastRelease = InterfaceManager::instance->_lastMasterTableLightsButtonRelease;
    }

    // TODO: Make trigger time configurable
    vTaskDelay(300 / portTICK_PERIOD_MS);

    if(InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::ceiling_lights) {
        if(lastRelease == InterfaceManager::instance->_lastMasterCeilingLightsButtonRelease) {
            InterfaceManager::instance->_ignoreNextTouchRelease = true;
            InterfaceManager::instance->_setEditLightMode(editLightMode::ceiling_lights);
        }
    } else if(InterfaceManager::instance->_triggerSpecialEditLightMode == editLightMode::table_lights) {
        if(lastRelease == InterfaceManager::instance->_lastMasterTableLightsButtonRelease) {
            InterfaceManager::instance->_ignoreNextTouchRelease = true;    
            InterfaceManager::instance->_setEditLightMode(editLightMode::table_lights);
        }
    }

    vTaskDelete(NULL); // Task is complete, stop task
}

void InterfaceManager::_processPanelConfig()
{
    this->_cfg.homeScreen = (*this->_roomDataJson)["home"].as<uint8_t>();
    this->_cfg.colorTempMin = (*this->_roomDataJson)["color_temp_min"].as<uint16_t>();
    this->_cfg.colorTempMax = (*this->_roomDataJson)["color_temp_min"].as<uint16_t>();
    this->_cfg.reverseColorTempSlider = (*this->_roomDataJson)["reverse_color_temp"].as<bool>();
    this->_cfg.raiseToMaxLightLevelAbove = (*this->_roomDataJson)["raise_to_100_light_level"].as<bool>();
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
    this->_cfg.currentRoom++;
    if (this->_cfg.currentRoom == this->_cfg.rooms.end())
    {
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
    this->_updatePanelLightStatus();
}

void InterfaceManager::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // If an event has just occured, ignore any new events for 
    // a while as it makes the display flicker
    if(millis() < InterfaceManager::instance->_ignoreMqttStatusUpdatesUntil) {
        return;
    }

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
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (;;)
    {
        // Wait for notification that we need to process messages.
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
                    domain = domain.substr(0, domain.find('/'));

                    std::string entity = msg.topic;
                    entity = entity.erase(0, strlen("nspanel/entities/"));
                    entity = entity.erase(0, entity.find('/')+1);
                    entity = entity.substr(0, entity.find('/'));

                    std::string attribute = msg.topic;
                    attribute = attribute.erase(0, attribute.find_last_of('/') + 1);

                    if (domain.compare("light") == 0 && attribute.compare("state_brightness_pct") == 0)
                    {
                        InterfaceManager::instance->_setLightLevel(entity, atoi(msg.payload.c_str()));
                    }
                    else if (domain.compare("light") == 0 && attribute.compare("state_kelvin") == 0)
                    {
                    	// TODO: Implement mechanism to reverse color temperature slider
                    	uint8_t colorTemp = (6000 - atoi(msg.payload.c_str())) / 40;
                    	InterfaceManager::instance->_setLightColorTemperature(entity, colorTemp);
                    }
                }
                catch (...)
                {
                    LOG_ERROR("Error processing MQTT message on topic ", msg.topic.c_str());
                }
                InterfaceManager::_mqttMessages.pop_front();
                vTaskDelay(10 / portTICK_PERIOD_MS); // Wait 10ms between processing each event to allow for other tasks, ie. more MQTT messages to arrive.
            }
        }
    }
}

void InterfaceManager::_setLightLevel(std::string light, uint8_t level)
{
	// TODO: Only update the displayed light level after all MQTT messages has processed
    for (roomConfig &roomCfg : InterfaceManager::instance->_cfg.rooms)
    {
        for (lightConfig &lightCfg : roomCfg.ceilingLights)
        {
            if (lightCfg.name.compare(light) == 0)
            {
            	if(lightCfg.level != level) {
            		lightCfg.level = level;
					this->_updatePanelLightStatus();
            	}
                return;
            }
        }

        for (lightConfig &lightCfg : roomCfg.tableLights)
        {
            if (lightCfg.name.compare(light) == 0)
            {
            	if(lightCfg.level != level) {
					lightCfg.level = level;
					this->_updatePanelLightStatus();
            	}
                return;
            }
        }
    }
}

void InterfaceManager::_setLightColorTemperature(std::string light, uint8_t level)
{
	// TODO: Only update the displayed light level after all MQTT messages has processed
    for (roomConfig &roomCfg : InterfaceManager::instance->_cfg.rooms)
    {
        for (lightConfig &lightCfg : roomCfg.ceilingLights)
        {
            if (lightCfg.name.compare(light) == 0)
            {
            	if(lightCfg.colorTemperature != level) {
            		lightCfg.colorTemperature = level;
					this->_updatePanelLightStatus();
            	}
                return;
            }
        }

        for (lightConfig &lightCfg : roomCfg.tableLights)
        {
            if (lightCfg.name.compare(light) == 0)
            {
            	if(lightCfg.colorTemperature != level) {
					lightCfg.colorTemperature = level;
					this->_updatePanelLightStatus();
            	}
                return;
            }
        }
    }
}

void InterfaceManager::_changeLightsToLevel(std::list<lightConfig*> *lights, uint8_t level)
{
    // TODO: Make timeout configurable
    this->_ignoreMqttStatusUpdatesUntil = millis() + 3000;

    for (lightConfig *light : (*lights))
    {
        std::string topic = "nspanel/entities/light/";
        topic.append(light->name);
        topic.append("/brightness_pct");
        if(this->_mqttClient->publish(topic.c_str(), std::to_string(level).c_str())) {
            light->level = level;
        } else {
            LOG_ERROR("Failed to send light update!");
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void InterfaceManager::_changeLightsToKelvin(std::list<lightConfig*> *lights, uint16_t kelvin)
{
    // TODO: Make timeout configurable
    this->_ignoreMqttStatusUpdatesUntil = millis() + 3000;

	// TODO: Implement mechanism to reverse color temp
	uint16_t sendKelvin = 6000 - (kelvin * 40);
    for (lightConfig *light : (*lights))
    {
    	if(light->canTemperature) {
            
    		std::string topic = "nspanel/entities/light/";
			topic.append(light->name);
			topic.append("/kelvin");
			if(this->_mqttClient->publish(topic.c_str(), std::to_string(sendKelvin).c_str())) {
                light->colorTemperature = kelvin;
			} else {
                LOG_ERROR("Failed to send light update!");
            }
    	}
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void InterfaceManager::_updatePanelLightStatus()
{
    uint totalBrightness = 0;
    uint totalBrightnessLights = 0;
    uint totalKelvin = 0;
    uint totalKelvinLights = 0;

    if(this->_currentEditMode == editLightMode::all_lights || this->_currentEditMode == editLightMode::ceiling_lights) {
        if(this->_currentRoomMode == roomMode::room) {
            for (lightConfig &light : this->_cfg.currentRoom->ceilingLights)
            {
                totalBrightnessLights++;
                totalBrightness += light.level;
                if(light.canTemperature) {
                    totalKelvinLights++;
                    totalKelvin += light.colorTemperature;
                }
            }
        } else if (this->_currentRoomMode == roomMode::house) {
            for (lightConfig *light : this->_cfg.getAllCeilingLights())
            {
                totalBrightnessLights++;
                totalBrightness += light->level;
                if(light->canTemperature) {
                    totalKelvinLights++;
                    totalKelvin += light->colorTemperature;
                }
            }
        }
    }

    uint8_t averageCeilingBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
    uint8_t averageCeilingKelvin = totalKelvinLights == 0 ? 0 : totalKelvin / totalKelvinLights;
    HomePage::setCeilingLightsState(averageCeilingBrightness > 0);
    HomePage::setCeilingBrightnessLabelText(averageCeilingBrightness);


	totalBrightness = 0;
	totalBrightnessLights = 0;
	totalKelvin = 0;
	totalKelvinLights = 0;
	if(this->_currentEditMode == editLightMode::all_lights || this->_currentEditMode == editLightMode::table_lights) {
        if(this->_currentRoomMode == roomMode::room) {
            for (lightConfig &light : this->_cfg.currentRoom->tableLights)
            {
                totalBrightnessLights++;
                totalBrightness += light.level;
                if(light.canTemperature) {
                    totalKelvinLights++;
                    totalKelvin += light.colorTemperature;
                }
            }
        } else if (this->_currentRoomMode == roomMode::house) {
            for (lightConfig *light : this->_cfg.getAllTableLights())
            {
                totalBrightnessLights++;
                totalBrightness += light->level;
                if(light->canTemperature) {
                    totalKelvinLights++;
                    totalKelvin += light->colorTemperature;
                }
            }
        }
    }
	uint8_t averageTableBrightness = totalBrightnessLights == 0 ? 0 : totalBrightness / totalBrightnessLights;
	uint8_t averageTableKelvin = totalKelvinLights == 0 ? 0 : totalKelvin / totalKelvinLights;
	HomePage::setTableLightsState(averageTableBrightness > 0);
	HomePage::setTableBrightnessLabelText(averageTableBrightness);

	uint8_t totalAverageBrightness;
	if(averageCeilingBrightness > 0 && averageTableBrightness > 0) {
		totalAverageBrightness = (averageCeilingBrightness + averageTableBrightness) / 2;
	} else if (averageCeilingBrightness > 0) {
		totalAverageBrightness = averageCeilingBrightness;
	} else if (averageTableBrightness > 0) {
		totalAverageBrightness = averageTableBrightness;
	} else {
		totalAverageBrightness = 0;
	}

	// Only set a new value if any lights are on.
	// This value will be used as the next "on" value.
	if(totalAverageBrightness > 0 && totalAverageBrightness != HomePage::getDimmingValue()) {
		HomePage::setDimmingValue(totalAverageBrightness);
	}

	// TODO: Implement so that direction of color temp slider can be reversed in web interface
	uint8_t totalAverageKelvin;
	if(averageCeilingKelvin > 0 && averageTableKelvin > 0) {
		totalAverageKelvin = (averageCeilingKelvin + averageTableKelvin) / 2;
	} else if (averageCeilingKelvin > 0) {
		totalAverageKelvin = averageCeilingKelvin;
	} else if (averageTableKelvin > 0) {
		totalAverageKelvin = averageTableKelvin;
	} else {
		totalAverageKelvin = 0;
	}
	// Only set a new value if it is not the same as already set.
	if(totalAverageKelvin != HomePage::getColorTempValue()) {
		HomePage::setColorTempValue(totalAverageKelvin);
	}
}

bool InterfaceManager::_getPanelConfig()
{
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