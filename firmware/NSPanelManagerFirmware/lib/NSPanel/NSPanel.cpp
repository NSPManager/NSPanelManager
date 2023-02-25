#include <NSPanel.h>
#include <MqttLog.h>
#include <NSPanelReturnData.h>
#include <WiFiClient.h>
#include <string>

bool recvRetCommandFinished()
{
    bool ret = false;
    uint8_t temp[4] = {0};

    Serial2.setTimeout(500);
    if (sizeof(temp) != Serial2.readBytes((char *)temp, sizeof(temp)))
    {
        ret = false;
    }

    if (temp[0] == NEX_RET_CMD_FINISHED && temp[1] == 0xFF && temp[2] == 0xFF && temp[3] == 0xFF)
    {
        ret = true;
    }

    return ret;
}

void NSPanel::tryBaud()
{
    this->_sendCommandWithoutResponse("boguscommand=0");
    this->_sendCommandWithoutResponse("connect");

    // Wake up the nextion
    this->_sendCommandWithoutResponse("bkcmd=0");
    this->_sendCommandWithoutResponse("sleep=0");

    this->_sendCommandWithoutResponse("bkcmd=0");
    this->_sendCommandWithoutResponse("sleep=0");

    // Reboot it
    this->_sendCommandWithoutResponse("rest");

    this->_sendCommandWithoutResponse("page bootscreen");
    this->_sendCommandWithoutResponse("dim=1.0");

    vTaskDelay(5000 / portTICK_PERIOD_MS);
    this->_sendCommandWithoutResponse("page Bathroom");
}

void NSPanel::init()
{
    // Pin 4 controls screen on/off.
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    Serial2.begin(115200, SERIAL_8N1, 17, 16);
    NSPanel::instance = this;

    LOG_INFO("Starting communication with NSPanel.");
    xTaskCreatePinnedToCore(_taskSendCommandQueue, "taskUartListen", 5000, NULL, 1, &this->_taskHandleSendCommandQueue, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(_taskReadNSPanelData, "taskUartListen", 5000, NULL, 1, &this->_taskHandleReadNSPanelData, CONFIG_ARDUINO_RUNNING_CORE);
    this->tryBaud();
}

void NSPanel::_sendCommandWithoutResponse(const char *command)
{
    NSPanelCommand cmd;
    cmd.command = command;
    this->_addCommandToQueue(cmd);
}

void NSPanel::_addCommandToQueue(NSPanelCommand command)
{
    this->_commandQueue.push(command);
    xTaskNotifyGive(this->_taskHandleSendCommandQueue);
}

void NSPanel::_taskSendCommandQueue(void *param)
{
    LOG_INFO("Starting taskSendCommandQueue.");
    for (;;)
    {
        // Wait for commands
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
        {
            // Process all commands in queue
            while (NSPanel::instance->_commandQueue.size() > 0)
            {
                NSPanelCommand cmd = NSPanel::instance->_commandQueue.front();
                NSPanel::instance->_sendCommand(&cmd);
                // Command processed, remove it from queue
                NSPanel::instance->_commandQueue.pop();
                vTaskDelay(COMMAND_SEND_WAIT_MS / portTICK_PERIOD_MS);
            }
            LOG_DEBUG("All commands processed, pausing for new commands.");
        }
    }
}

void NSPanel::_taskReadNSPanelData(void *param)
{
    LOG_INFO("Starting taskReadNSPanelData.");
    for (;;)
    {
        // Process all commands in queue
        while (Serial2.available() > 0)
        {
            uint8_t readByte = Serial2.read();
            switch (readByte)
            {
            case NEX_OUT_TOUCH_EVENT:
                LOG_DEBUG("page: ", String(Serial2.read(), HEX).c_str());
                LOG_DEBUG("component: ", String(Serial2.read(), HEX).c_str());
                LOG_DEBUG("Type: ", Serial2.read() == 0x01 ? "PRESS" : "RELEASE");
                // Read the rest of the bytes
                for (int i = 0; i < 3; i++)
                {
                    Serial2.read();
                }
                break;

            default:
                LOG_DEBUG("Read ", String(readByte, HEX).c_str());
                break;
            }

            if (Serial2.available() == 0)
            {
                LOG_DEBUG("--- READ COMPLETE ---");
            }
        }
        // Wait 5ms between each read.
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void NSPanel::_sendCommand(NSPanelCommand *command)
{
    // Clear buffer before sending
    while (Serial2.available())
    {
        Serial2.read();
    }

    Serial2.print(command->command.c_str());
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    this->_lastCommandSent = millis();
}

void NSPanel::startOTAUpdate()
{
    xTaskCreatePinnedToCore(_taskUpdateTFTConfigOTA, "taskUpdateTFTConfigOTA", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void NSPanel::_taskUpdateTFTConfigOTA(void *param)
{
    LOG_INFO("Starting TFT update...");
    WiFiClient client;
    unsigned long contentLength = 0;
    bool isValidContentType = false;
    if (client.connect(NSPMConfig::instance->manager_address.c_str(), NSPMConfig::instance->manager_port))
    {
        client.print(String("GET /download_tft HTTP/1.1\r\n") +
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
    }

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Stop all other tasks using the panel
        vTaskDelete(NSPanel::instance->_taskHandleSendCommandQueue);
        vTaskDelete(NSPanel::instance->_taskHandleReadNSPanelData);

        // Clear current read buffer
        Serial2.flush();

        int uploadBaudRate = 921600;

        // Set fastest baud rate
        std::string uploadBaudRateString = "baud=";
        uploadBaudRateString.append(std::to_string(uploadBaudRate));
        Serial2.print(uploadBaudRateString.c_str());
        Serial2.write(0xFF);
        Serial2.write(0xFF);
        Serial2.write(0xFF);

        // Wait for 1 second to see if any data is returned, if it is
        // we failed to set baud data
        unsigned long startMillis = millis();
        while (Serial2.available() == 0 && millis() - startMillis < 1000)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        if (Serial2.available() == 0)
        {
            LOG_INFO("Baud rate switch successful, switching Serial2 to baud ", uploadBaudRate);
        }
        else
        {
            LOG_ERROR("Baud rate switch failed. Will restart.");
            ESP.restart();
            return;
        }

        Serial2.flush();
        Serial2.end();
        Serial2.begin(uploadBaudRate, SERIAL_8N1, 17, 16);

        // Send whmi-wri command to initiate upload
        std::string commandString = "whmi-wri ";
        commandString.append(std::to_string(contentLength));
        commandString.append(",");
        commandString.append(std::to_string(uploadBaudRate));
        commandString.append(",0");
        Serial2.print(commandString.c_str());
        Serial2.write(0xFF);
        Serial2.write(0xFF);
        Serial2.write(0xFF);

        // Wait until Nextion returns okay to transmit tft
        while (Serial2.available() == 0)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        uint8_t returnData = Serial2.read();
        if (returnData == 0x05)
        {
            LOG_INFO("Starting to write to TFT.");
        }
        else
        {
            LOG_ERROR("Failed to init upload to TFT. Got return code: ", String(returnData, HEX).c_str());
            client.flush();
            LOG_DEBUG("More data:");
            while (Serial2.available() > 0)
            {
                LOG_INFO(String(Serial2.read(), HEX).c_str());
                vTaskDelay(5 / portTICK_PERIOD_MS);
            }
            LOG_ERROR("Will now restart.");
            ESP.restart();
            return;
        }

        // Upload data to Nextion in 4096 blocks or smaller
        while (client.available() > 0)
        {
            // Write bytes left or a maximum of 4096
            uint16_t bytesToWrite = (client.available() < 4096 ? client.available() : 4096);
            for (int i = 0; i < bytesToWrite; i++)
            {
                Serial2.write(client.read());
            }

            // Wait for 0x05 to indicate that the display is ready for new data
            while (Serial2.available() == 0)
            {
                vTaskDelay(5); // Leave time for other tasks and display to process
            }

            returnData = Serial2.read();
            if (returnData != 0x05)
            {
                LOG_ERROR("Something went wrong during tft update. Got return code: ", String(returnData, HEX).c_str());
            }
        }

        LOG_INFO("TFT Upload complete. Will restart.");
        ESP.restart();
    }
    else
    {
        LOG_ERROR("There was no content in the OTA response!");
        client.flush();
    }
    vTaskDelete(NULL);
}