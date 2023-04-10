#include <Arduino.h>
#include <HTTPClient.h>
#include <MqttLog.h>
#include <NSPanel.h>
#include <NSPanelReturnData.h>
#include <WiFiClient.h>
#include <esp_task_wdt.h>
#include <math.h>
#include <string>

bool recvRetCommandFinished() {
  bool ret = false;
  uint8_t temp[4] = {0};

  Serial2.setTimeout(500);
  if (sizeof(temp) != Serial2.readBytes((char *)temp, sizeof(temp))) {
    ret = false;
  }

  if (temp[0] == NEX_RET_CMD_FINISHED && temp[1] == 0xFF && temp[2] == 0xFF && temp[3] == 0xFF) {
    ret = true;
  }

  return ret;
}

void NSPanel::goToPage(const char *page) {
  std::string cmd_string = "page ";
  cmd_string.append(page);
  this->_sendCommandWithoutResponse(cmd_string.c_str());
}

void NSPanel::setDimLevel(uint8_t dimLevel) {
  uint8_t dimLevelToPanel = dimLevel;
  std::string cmd_string = "dim=";
  cmd_string.append(std::to_string(dimLevelToPanel));
  this->_sendCommandWithoutResponse(cmd_string.c_str());
}

void NSPanel::setSleep(bool sleep) {
  this->_sendCommandWithoutResponse(sleep ? "sleep=1" : "sleep=0");
}

void NSPanel::setComponentText(const char *componentId, const char *text) {
  std::string cmd = componentId;
  cmd.append(".txt=\"");
  cmd.append(text);
  cmd.append("\"");
  this->_sendCommandWithoutResponse(cmd.c_str());
}

void NSPanel::setComponentVal(const char *componentId, uint8_t value) {
  std::string cmd = componentId;
  cmd.append(".val=");
  cmd.append(std::to_string(value));
  this->_sendCommandWithoutResponse(cmd.c_str());
}

void NSPanel::setComponentPic(const char *componentId, uint8_t value) {
  std::string cmd = componentId;
  cmd.append(".pic=");
  cmd.append(std::to_string(value));
  this->_sendCommandWithoutResponse(cmd.c_str());
}

void NSPanel::setComponentPic1(const char *componentId, uint8_t value) {
  std::string cmd = componentId;
  cmd.append(".pic1=");
  cmd.append(std::to_string(value));
  this->_sendCommandWithoutResponse(cmd.c_str());
}

void NSPanel::setComponentForegroundColor(const char *componentId, uint value) {
  std::string cmd = componentId;
  cmd.append(".pco=");
  cmd.append(std::to_string(value));
  this->_sendCommandWithoutResponse(cmd.c_str());
}

void NSPanel::setComponentVisible(const char *componentId, bool visible) {
  std::string cmd = "vis ";
  cmd.append(componentId);
  cmd.append(",");
  cmd.append(visible ? "1" : "0");
  this->_sendCommandWithoutResponse(cmd.c_str());
}

int NSPanel::getComponentIntVal(const char *componentId) {
  // Wait for command queue to clear
  while (this->_commandQueue.size() > 0) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  if (!xSemaphoreTake(this->_mutexReadSerialData, 1000 / portTICK_PERIOD_MS) == pdTRUE) {
    LOG_ERROR("Failed to get Serial read mutex when reading response from panel!");
    return -255;
  }

  this->_clearSerialBuffer();

  std::string cmd = "get ";
  cmd.append(componentId);
  cmd.append(".val");
  this->_sendRawCommand(cmd.c_str(), cmd.length());

  // Wait for data to become available
  while (Serial2.available() == 0) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  int value = -1;
  if (Serial2.read() == 0x71) {
    value = Serial2.read();
    value += Serial2.read() << 8;
    value += Serial2.read() << 16;
    value += Serial2.read() << 24;
    this->_clearSerialBuffer();
  } else {
    LOG_ERROR("Failed to get response, not expected return data.");
    value = -254;
  }

  xSemaphoreGive(this->_mutexReadSerialData);

  return value;
}

void NSPanel::restart() {
  this->_sendCommandWithoutResponse("rest");
}

void NSPanel::init() {
  // Pin 4 controls screen on/off.
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  digitalWrite(4, LOW);
  Serial2.begin(115200, SERIAL_8N1, 17, 16);
  NSPanel::instance = this;
  this->_mutexReadSerialData = xSemaphoreCreateMutex();
  this->_writeCommandsToSerial = true;

  LOG_INFO("Init NSPanel.");
  xTaskCreatePinnedToCore(_taskSendCommandQueue, "taskSendCommandQueue", 5000, NULL, 1, &this->_taskHandleSendCommandQueue, CONFIG_ARDUINO_RUNNING_CORE);
  this->_startListeningToPanel();

  // Connect to display and start it
  this->_sendCommandWithoutResponse("boguscommand=0");
  this->_sendCommandWithoutResponse("connect");
  this->_sendCommandWithoutResponse("bkcmd=0");
  this->_sendCommandWithoutResponse("sleep=0");
  this->_sendCommandWithoutResponse("bkcmd=0");
  this->_sendCommandWithoutResponse("sleep=0");
  this->_sendCommandClearResponse("rest");
  this->_sendCommandClearResponse("dim=1.0");
}

void NSPanel::_startListeningToPanel() {
  xTaskCreatePinnedToCore(_taskProcessPanelOutput, "taskProcessPanelOutput", 5000, NULL, 1, &this->_taskHandleProcessPanelOutput, CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(_taskReadNSPanelData, "taskReadNSPanelData", 5000, NULL, 1, &this->_taskHandleReadNSPanelData, CONFIG_ARDUINO_RUNNING_CORE);
}

void NSPanel::_stopListeningToPanel() {
  if (this->_taskHandleReadNSPanelData != NULL) {
    vTaskDelete(this->_taskHandleReadNSPanelData);
    this->_taskHandleReadNSPanelData = NULL;
  }

  if (this->_taskHandleProcessPanelOutput != NULL) {
    vTaskDelete(this->_taskHandleProcessPanelOutput);
    this->_taskHandleProcessPanelOutput = NULL;
  }
}

void NSPanel::_sendCommandWithoutResponse(const char *command) {
  NSPanelCommand cmd;
  cmd.command = command;
  this->_addCommandToQueue(cmd);
}

void NSPanel::_sendCommandClearResponse(const char *command) {
  NSPanelCommand cmd;
  cmd.command = command;
  cmd.expectResponse = true;
  cmd.callback = &NSPanel::_clearSerialBuffer;
  this->_addCommandToQueue(cmd);
}

void NSPanel::_addCommandToQueue(NSPanelCommand command) {
  if (this->_taskHandleSendCommandQueue != NULL) {
    this->_commandQueue.push(command);
    xTaskNotifyGive(this->_taskHandleSendCommandQueue);
  }
}

void NSPanel::_taskSendCommandQueue(void *param) {
  LOG_INFO("Starting taskSendCommandQueue.");
  for (;;) {
    // Wait for commands
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
      while (NSPanel::_writeCommandsToSerial == false) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }

      // Process all commands in queue
      while (NSPanel::instance->_commandQueue.size() > 0) {
        NSPanelCommand cmd = NSPanel::instance->_commandQueue.front();
        NSPanel::instance->_sendCommand(&cmd);
        // Command processed, remove it from queue
        NSPanel::instance->_commandQueue.pop();
        vTaskDelay(COMMAND_SEND_WAIT_MS / portTICK_PERIOD_MS);
      }
    }
  }
}

void NSPanel::attachTouchEventCallback(void (*callback)(uint8_t, uint8_t, bool)) {
  NSPanel::_touchEventCallback = callback;
}

void NSPanel::attachSleepCallback(void (*callback)()) {
  NSPanel::_sleepCallback = callback;
}

void NSPanel::attachWakeCallback(void (*callback)()) {
  NSPanel::_wakeCallback = callback;
}

void NSPanel::_taskReadNSPanelData(void *param) {
  LOG_INFO("Starting taskReadNSPanelData.");
  for (;;) {
    // Wait until access is given to serial
    if (xSemaphoreTake(NSPanel::instance->_mutexReadSerialData, portMAX_DELAY) == pdTRUE) {
      // Read the output from the panel if any and add the payload to the process queue
      if (Serial2.available() > 0) {
        std::vector<char> data;
        while (Serial2.available() > 0) {
          data.push_back(Serial2.read());
        }
        NSPanel::instance->_processQueue.push(data);
        xTaskNotifyGive(NSPanel::instance->_taskHandleProcessPanelOutput);
      }
      xSemaphoreGive(NSPanel::instance->_mutexReadSerialData);
    }

    // Wait 10ms between each read.
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void NSPanel::_taskProcessPanelOutput(void *param) {
  for (;;) {
    // Wait for things that needs processing
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
      while (!NSPanel::instance->_processQueue.empty()) {
        std::vector<char> itemPayload = NSPanel::instance->_processQueue.front();

        // Select correct action depending on type of event
        switch (itemPayload[0]) {
        case NEX_OUT_TOUCH_EVENT:
          NSPanel::_touchEventCallback(itemPayload[1], itemPayload[2], itemPayload[3] == 0x01);
          break;

        case NEX_OUT_SLEEP:
          NSPanel::_sleepCallback();
          break;

        case NEX_OUT_WAKE:
          NSPanel::_wakeCallback();
          break;

        default:
          LOG_DEBUG("Read type ", String(itemPayload[0], HEX).c_str());
          break;
        }

        // Done with item, pop it off the queue
        NSPanel::instance->_processQueue.pop();
        // Wait at least 10ms between each processing of event to allow for other functions to execute.
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
    }
  }
}

void NSPanel::_sendCommand(NSPanelCommand *command) {
  // Clear buffer before sending
  while (Serial2.available()) {
    Serial2.read();
  }

  if (command->expectResponse && !xSemaphoreTake(this->_mutexReadSerialData, 250 / portTICK_PERIOD_MS) == pdTRUE) {
    LOG_ERROR("Failed to get serial read mutex! Will not continue call.");
    return;
  }

  Serial2.print(command->command.c_str());
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  this->_lastCommandSent = millis();

  if (command->expectResponse) {
    while (Serial2.available() == 0) {
      vTaskDelay(5);
    }

    // Give back serial read mutex.
    xSemaphoreGive(this->_mutexReadSerialData);

    // Call callback function for command when data is available at Serial2
    command->callback(command);
  }
}

void NSPanel::_sendRawCommand(const char *command, int length) {
  for (int i = 0; i < length; i++) {
    Serial2.print(command[i]);
  }
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

void NSPanel::startOTAUpdate() {
  xTaskCreatePinnedToCore(_taskUpdateTFTConfigOTA, "taskUpdateTFTConfigOTA", 30000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void NSPanel::_clearSerialBuffer(NSPanelCommand *cmd) {
  NSPanel::_clearSerialBuffer();
  cmd->callbackFinished = true;
}

void NSPanel::_clearSerialBuffer() {
  while (Serial2.available() > 0) {
    Serial2.read();
  }
}

uint16_t NSPanel::_readDataToString(std::string *data, uint32_t timeout, bool find_05_return) {
  uint16_t number_of_FF_bytes = 0;
  unsigned long start_read = millis();
  bool recevied_ff_flag = false;
  bool recevied_05_flag = false;
  while (millis() - start_read <= timeout) {
    while (millis() - start_read <= timeout && Serial2.available() <= 0) {
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    if (Serial2.available() > 0) {
      uint8_t received_byte = Serial2.read();
      data->push_back(received_byte);
      if (received_byte == 0xFF) {
        number_of_FF_bytes++;

        if (number_of_FF_bytes >= 3) {
          recevied_ff_flag = true;
        }
      } else {
        number_of_FF_bytes = 0; // Reset counter as new data is provided, any previous 0xFF byte was part of payload
        recevied_ff_flag = false;
      }

      if (find_05_return && data->find(0x05) != std::string::npos) {
        recevied_05_flag = true;
      }

      if (recevied_05_flag || recevied_ff_flag) {
        break;
      }

      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
  }

  if (recevied_ff_flag) {
    // Remove last three 0xFF off of the end to only return payload
    *data = data->substr(0, data->length() - 3);
  }

  return data->length();
}

void NSPanel::_taskUpdateTFTConfigOTA(void *param) {
  LOG_INFO("Starting TFT update...");
  for (;;) {
    bool updateResult = NSPanel::_updateTFTOTA();
    if (updateResult) {
      LOG_INFO("Update of TFT successful, will reboot.");
      ESP.restart();
    } else {
      LOG_ERROR("Failed to update TFT. Will try again in 5 seconds.");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }

  vTaskDelete(NULL);
}

size_t NSPanel::_downloadTFTChunk(uint8_t *buffer, const char *address, size_t offset, size_t size) {
  HTTPClient httpClient;
  httpClient.begin(address);

  std::string rangeHeader = "bytes=";
  rangeHeader.append(std::to_string(offset));
  rangeHeader.append("-");
  rangeHeader.append(std::to_string(offset + size));
  httpClient.addHeader("Range", rangeHeader.c_str());

  int httpReturnCode = httpClient.GET();
  // LOG_TRACE("Downloading TFT ", rangeHeader.c_str(), " got return code: ", httpReturnCode);
  if (httpReturnCode != 200 && httpReturnCode != 206) {
    httpClient.end();
    LOG_ERROR("Failed to retrive firmware, got return code: ", httpReturnCode);
    return 0;
  }

  size_t sizeReceived = 0;
  size_t dataAvailable = httpClient.getStreamPtr()->available();
  if (dataAvailable > 0) {
    sizeReceived = httpClient.getStreamPtr()->readBytes(buffer, dataAvailable >= size ? size : dataAvailable);
  }
  httpClient.end();

  return sizeReceived;
}

size_t NSPanel::_getTFTFileSize(const char *address) {
  HTTPClient httpClient;
  httpClient.begin(address);
  const char *header_names[] = {"Content-Length"};
  httpClient.collectHeaders(header_names, 1);
  httpClient.addHeader("Range", "bytes=0-255");
  int httpReturnCode = httpClient.GET();

  if (httpReturnCode != 200) {
    LOG_ERROR("Failed to retrive firmware, got return code: ", httpReturnCode);
    return 0;
  }

  try {
    size_t content_length = httpClient.header("Content-Length").toInt();
    httpClient.end();
    return content_length;
  } catch (...) {
    LOG_ERROR("Something went wrong when checking TFT file size.");
  }
  httpClient.end();
  return 0;
}

bool NSPanel::_updateTFTOTA() {
  LOG_INFO("_updateTFTOTA Started.");

  // Stop all other tasks using the panel
  if (NSPanel::instance->_taskHandleSendCommandQueue != NULL) {
    vTaskDelete(NSPanel::instance->_taskHandleSendCommandQueue);
    NSPanel::instance->_taskHandleSendCommandQueue = NULL;
  }
  NSPanel::instance->_stopListeningToPanel();

  // Clear current read buffer
  Serial2.flush();

  // URL to download TFT file from
  std::string downloadUrl = "http://";
  downloadUrl.append(NSPMConfig::instance->manager_address);
  downloadUrl.append(":");
  downloadUrl.append(std::to_string(NSPMConfig::instance->manager_port));
  downloadUrl.append("/download_tft");

  LOG_INFO("Will download TFT file from: ", downloadUrl.c_str());

  HTTPClient httpClient;
  if (!httpClient.begin(downloadUrl.c_str())) {
    LOG_ERROR("Failed to create httpClient object with .begin.");
    return false;
  }
  const char *header_names[] = {"Content-Length"};
  httpClient.collectHeaders(header_names, 1);
  httpClient.setTimeout(20);
  int httpReturnCode = httpClient.GET();

  if (httpReturnCode != 200) {
    LOG_ERROR("Failed get TFT-file from server. Got HTTP return code: ", httpReturnCode);
    httpClient.end();
    return false;
  }

  // Change baud rate if needed
  int32_t baud_diff = NSPMConfig::instance->tft_upload_baud - Serial2.baudRate();
  if (baud_diff < 0) {
    baud_diff = baud_diff / -1;
  }
  if (baud_diff >= 10) {
    std::string uploadBaudRateString = "baud=";
    uploadBaudRateString.append(std::to_string(NSPMConfig::instance->tft_upload_baud));
    Serial2.print(uploadBaudRateString.c_str());
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    Serial2.write(0xFF);

    // Wait for 1 second to see if any data is returned, if it is
    // we failed to set baud data
    unsigned long startMillis = millis();
    while (Serial2.available() == 0 && millis() - startMillis < 1000) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    if (Serial2.available() == 0) {
      LOG_INFO("Baud rate switch successful, switching Serial2 from ", Serial2.baudRate(), " to ", NSPMConfig::instance->tft_upload_baud);

      Serial2.flush();
      Serial2.end();
      Serial2.begin(NSPMConfig::instance->tft_upload_baud, SERIAL_8N1, 17, 16);
    } else {
      LOG_ERROR("Baud rate switch failed. Will restart.");
      ESP.restart();
      return false;
    }

    Serial2.print("DRAKJHSUYDGBNCJHGJKSHBDN"); // "disconnect"
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    Serial2.write(0xFF);

    vTaskDelay(50 / portTICK_PERIOD_MS);
    // Send "connect" string to get data
    Serial2.print("connect");
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    LOG_DEBUG("Sent connect, waiting for comok string.");

    // Wait for comok return data.
    while (Serial2.available() == 0) {
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    LOG_DEBUG("Waiting for comok");
    std::string comok_string = "";
    while (Serial2.available() > 0) {
      uint8_t read_char = Serial2.read();
      if (read_char != 0xFF) {
        comok_string.push_back(read_char);
      }
    }
    LOG_DEBUG("Got comok: ", comok_string.c_str());
  }

  // Get size of TFT file
  size_t totalTftFileSize = httpClient.header("Content-Length").toInt();
  while (totalTftFileSize == 0) {
    LOG_ERROR("Failed to get content length for TFT-file.");
    return false;
  }

  LOG_DEBUG("Will start TFT upload, TFT file size: ", totalTftFileSize);
  // TODO: Detect if new protocol is not supported, in that case set flag in flash and restart and then continue flash with legacy mode.
  // Send whmi-wri command to initiate upload
  std::string commandString;
  if (NSPMConfig::instance->use_new_upload_protocol) {
    LOG_INFO("Starting upload using v1.2 protocol.");
    commandString = "whmi-wris ";
    commandString.append(std::to_string(totalTftFileSize));
    commandString.append(",");
    commandString.append(std::to_string(NSPMConfig::instance->tft_upload_baud));
    commandString.append(",1");
  } else {
    LOG_INFO("Starting upload using v1.1 protocol.");
    commandString = "whmi-wri ";
    commandString.append(std::to_string(totalTftFileSize));
    commandString.append(",");
    commandString.append(std::to_string(NSPMConfig::instance->tft_upload_baud));
    commandString.append(",1");
  }
  Serial2.print(commandString.c_str());
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);

  // Wait until Nextion returns okay to transmit tft
  LOG_DEBUG("Waiting for panel reponse");
  while (Serial2.available() == 0) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  uint8_t returnData = Serial2.read();
  if (returnData == 0x05) {
    LOG_INFO("Starting to write to TFT.");
  } else {
    LOG_ERROR("Failed to init upload to TFT. Got return code: ", String(returnData, HEX).c_str());
    while (Serial2.available() > 0) {
      LOG_INFO(String(Serial2.read(), HEX).c_str());
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    LOG_ERROR("Will now restart.");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP.restart();
    return false;
  }

  unsigned long startWaitingForOKForNextChunk = 0;
  uint32_t nextStartWriteOffset = 0;
  size_t lastReadByte = 0;

  // Download next chunk
  uint8_t dataBuffer[4096];

  // Loop until break when all firmware has finished uploading (data available in stream == 0)
  while (true) {
    // Jump to read offset
    while (lastReadByte < nextStartWriteOffset) {
      size_t seekLength;
      if (httpClient.getStreamPtr()->available() - nextStartWriteOffset > 4096) {
        seekLength = 4096;
      } else {
        seekLength = httpClient.getStreamPtr()->available() - nextStartWriteOffset;
      }
      httpClient.getStreamPtr()->readBytes(dataBuffer, seekLength);
      lastReadByte += seekLength;
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    // Calculate next chunk size
    int next_write_size;
    if (httpClient.getStreamPtr()->available() > 4096) {
      next_write_size = 4096;
    } else {
      next_write_size = httpClient.getStreamPtr()->available();
    }

    // Read chunk and write it
    httpClient.getStreamPtr()->readBytes(dataBuffer, next_write_size);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    for (int i = 0; i < next_write_size; i++) {
      Serial2.write(dataBuffer[i]);
      nextStartWriteOffset++;
    }
    lastReadByte = nextStartWriteOffset;

    // Wait for 0x05 to indicate that the display is ready for new data
    // unsigned long startWaitingForOKForNextChunk = millis();
    // while (Serial2.available() == 0) {
    //   vTaskDelay(10 / portTICK_PERIOD_MS); // Leave time for other tasks and display to process
    //   if (startWaitingForOKForNextChunk + 5000 <= millis()) {
    //     LOG_ERROR("Something went wrong during tft update. Got no response after 5 seconds, will continue with next chunk anyway.");
    //     break;
    //   }
    // }

    std::string return_string;
    uint16_t recevied_bytes = 0;
    unsigned long start_wait = millis();
    while (recevied_bytes == 0) {
      return_string.clear();
      recevied_bytes = NSPanel::instance->_readDataToString(&return_string, 3000, true);

      if (recevied_bytes == 0) {
        LOG_DEBUG("Still waiting for NSPanel reponse data.");
      }
    }
    LOG_DEBUG("Received ", recevied_bytes, " bytes: ");
    for (int i = 0; i < recevied_bytes; i++) {
      LOG_DEBUG("0x", String(return_string[i], HEX).c_str());
    }
    if (return_string[0] == 0x05) {
      // Old protocol, just upload next chunk.
      LOG_DEBUG("Got 0x05, uploading next chunk.");
    } else if (return_string[0] == 0x08) {
      while (return_string.length() < 4) {
        LOG_DEBUG("Waiting for offset data byte ", return_string.length() - 1);
        while (Serial2.available() <= 0) {
          vTaskDelay(5 / portTICK_PERIOD_MS);
        }
        return_string.push_back(Serial2.read());
      }
      uint32_t readNextOffset = return_string[1];
      readNextOffset |= return_string[2] << 8;
      readNextOffset |= return_string[3] << 16;
      readNextOffset |= return_string[4] << 24;
      if (readNextOffset > 0) {
        nextStartWriteOffset = readNextOffset;
        LOG_INFO("Got 0x08 with offset, jumping to: ", nextStartWriteOffset, " please wait.");
      }
    } else if (httpClient.getStreamPtr()->available() == 0) {
      LOG_INFO("TFT Upload complete, wrote ", nextStartWriteOffset, " bytes.");
      break;
    } else {
      LOG_DEBUG("Got unexpected return data from panel. Received ", recevied_bytes, " bytes: ");
      for (int i = 0; i < recevied_bytes; i++) {
        LOG_DEBUG("0x", String(return_string[i], HEX).c_str());
      }
    }

    // vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  LOG_INFO(" Will restart in 10 seconds.");
  vTaskDelay(10000 / portTICK_PERIOD_MS);
  ESP.restart();

  vTaskDelay(portMAX_DELAY);
  vTaskDelete(NULL);
  return false;
}
