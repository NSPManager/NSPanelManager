#include "freertos/portmacro.h"
#include <Arduino.h>
#include <ChunkDownloader.hpp>
#include <HTTPClient.h>
#include <HttpLib.hpp>
#include <MqttLog.hpp>
#include <MqttManager.hpp>
#include <NSPMConfig.h>
#include <NSPanel.hpp>
#include <NSPanelReturnData.h>
#include <WiFiClient.h>
#include <cstddef>
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

void NSPanel::setTimerTimeout(const char *componentId, uint16_t timeout) {
  std::string cmd = componentId;
  cmd.append(".tim=");
  cmd.append(std::to_string(timeout));
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

  if (xSemaphoreTake(this->_mutexReadSerialData, 1000 / portTICK_PERIOD_MS) != pdTRUE) {
    LOG_ERROR("Failed to get Serial read mutex when reading response from panel!");
    return -255;
  }

  this->_clearSerialBuffer();

  std::string cmd = "get ";
  cmd.append(componentId);
  cmd.append(".val");
  this->_sendRawCommand(cmd.c_str(), cmd.length());

  // Wait for data to become available
  int value = -1;
  unsigned long _startWait = millis();
  while (Serial2.available() == 0) {
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // 3 second timeout for reading data.
    if (millis() >= _startWait + 3000) {
      LOG_ERROR("3 second timeout while getting int value from display trigged. Return -253");
      value = -253;
      break;
    }
  }

  if (Serial2.available() > 0 && Serial2.read() == 0x71) {
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

bool NSPanel::init() {
  NSPanel::instance = this;
  this->_mutexReadSerialData = xSemaphoreCreateMutex();
  this->_mutexWriteSerialData = xSemaphoreCreateMutex();
  this->_writeCommandsToSerial = true;
  this->_isUpdating = false;
  this->_update_progress = 0;

  Serial2.setTxBufferSize(128);
  Serial2.setRxBufferSize(256);
  Serial2.begin(115200, SERIAL_8N1, 17, 16);
  // Clear Serial2 read buffer
  // this->_clearSerialBuffer();

  // Pin 4 controls screen on/off.
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH); // Turn off power to the display
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  digitalWrite(4, LOW); // Turn on power to the display
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  std::string result = "";
  this->_readDataToString(&result, 2500, false);
  if (result.compare("NSPM") == 0) {
    this->_has_received_nspm = true;
  }

  LOG_DEBUG("Got text from panel: ", result.c_str());

  while (true) {
    if (xSemaphoreTake(NSPanel::instance->_mutexWriteSerialData, portMAX_DELAY)) {
      break;
    } else {
      LOG_ERROR("Failed to take serial write mutex, trying again in 3 seconds.");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }

  while (true) {
    if (xSemaphoreTake(NSPanel::instance->_mutexReadSerialData, portMAX_DELAY)) {
      break;
    } else {
      LOG_ERROR("Failed to take serial read mutex, trying again in 3 seconds.");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }

  LOG_INFO("Trying to connect to display."); // Send bogus to the panel to make is "clear" any reading state
  Serial2.print("DRAKJHSUYDGBNCJHGJKSHBDN");
  NSPanel::_sendCommandEndSequence();
  vTaskDelay((1000000 / Serial2.baudRate()) + 30 / portTICK_PERIOD_MS);
  // Clear Serial2 read buffer
  while (Serial2.available() > 0) {
    Serial2.read();
    if (Serial2.available() == 0) {
      vTaskDelay(250 / portTICK_PERIOD_MS);
    }
  }

  LOG_DEBUG("Sending final connect to panel");
  Serial2.print("connect");
  NSPanel::_sendCommandEndSequence();
  vTaskDelay((1000000 / Serial2.baudRate()) + 30 / portTICK_PERIOD_MS);

  std::string reply_data = "";
  while (Serial2.available() > 0) {
    reply_data.push_back(Serial2.read());

    if (Serial2.available() == 0) {
      vTaskDelay(25 / portTICK_PERIOD_MS);
    }
  }
  if (reply_data.length() > 3) {
    reply_data.erase(reply_data.length() - 3); // Remove last three 0xFF bytes from panel connect line.
  }
  LOG_INFO("Got reply from display: ", reply_data.c_str());
  reply_data.clear();

  if (NSPMConfig::instance->md5_tft_file.empty()) {
    LOG_WARNING("No stored MD5 for TFT. Is the panel flashed with NSPanel Manager TFT?");
    // LOG_ERROR("TFT file not flashed! Will not init NSPanel communication further.");
    // return false;
  }

  LOG_INFO("Trying to init NSPanel.");
  xTaskCreatePinnedToCore(_taskSendCommandQueue, "taskSendCommandQueue", 5000, NULL, 1, &this->_taskHandleSendCommandQueue, CONFIG_ARDUINO_RUNNING_CORE);
  this->_startListeningToPanel();

  // Connect to display and start it
  this->restart();
  vTaskDelay(250 / portTICK_PERIOD_MS);
  this->_sendCommandWithoutResponse("bkcmd=0");
  this->_sendCommandWithoutResponse("sleep=0");
  this->_sendCommandWithoutResponse("bkcmd=0");
  this->_sendCommandWithoutResponse("sleep=0");

  xSemaphoreGive(NSPanel::instance->_mutexWriteSerialData);
  xSemaphoreGive(NSPanel::instance->_mutexReadSerialData);
  LOG_INFO("NSPanel::init complete.");
  return this->_has_received_nspm;
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

void NSPanel::_sendCommandEndSequence() {
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.flush(true);
}

void NSPanel::_sendCommandClearResponse(const char *command, uint16_t timeout) {
  NSPanelCommand cmd;
  cmd.command = command;
  cmd.expectResponse = true;
  cmd.callback = &NSPanel::_clearSerialBuffer;
  cmd.timeout = timeout;
  this->_addCommandToQueue(cmd);
}

void NSPanel::_addCommandToQueue(NSPanelCommand command) {
  if (this->_taskHandleSendCommandQueue != NULL) {
    this->_commandQueue.push(command);
    xTaskNotifyGive(this->_taskHandleSendCommandQueue);
  } else {
    LOG_ERROR("Trying to add command to queue before a queue exists!");
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
    while (Serial2.available() == 0) {
      vTaskDelay(25 / portTICK_PERIOD_MS);
    }

    if (xSemaphoreTake(NSPanel::instance->_mutexReadSerialData, portMAX_DELAY) == pdTRUE) {
      if (Serial2.available() > 0) {
        std::vector<char> data;
        bool read_to_end = false;
        uint8_t num_ff_read_in_row = 0;

        while (Serial2.available() > 0) {
          uint8_t read = Serial2.read();
          data.push_back((char)read);
          if (read == 0xFF) {
            num_ff_read_in_row++;
            if (num_ff_read_in_row >= 3) { // End of command, clear bugger and read next if any
              NSPanel::instance->_processQueue.push(data);
              xTaskNotifyGive(NSPanel::instance->_taskHandleProcessPanelOutput);
              data.clear();
            }
          } else {
            num_ff_read_in_row = 0;
          }
          if (Serial2.available() == 0) {
            vTaskDelay(50 / portTICK_PERIOD_MS);
          }
        }

        if (data.size() > 0) { // This will only trigger if data did not end in 0xFF 0xFF 0xFF which should never happen
          NSPanel::instance->_processQueue.push(data);
          xTaskNotifyGive(NSPanel::instance->_taskHandleProcessPanelOutput);
        }
      }
      xSemaphoreGive(NSPanel::instance->_mutexReadSerialData);
    }
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

  while (!NSPanel::instance->_writeCommandsToSerial) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  if (command->expectResponse) {
    if (xSemaphoreTake(this->_mutexReadSerialData, 500 / portTICK_PERIOD_MS) != pdTRUE) {
      LOG_ERROR("Failed to get serial read mutex! Will not continue call.");
      return;
    }
  }

  while (true) {
    if (xSemaphoreTake(NSPanel::instance->_mutexWriteSerialData, portMAX_DELAY)) {
      break;
    } else {
      LOG_ERROR("Failed to take serial write mutex, trying again in 3 seconds.");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }

  Serial2.print(command->command.c_str());
  NSPanel::instance->_sendCommandEndSequence();
  this->_lastCommandSent = millis();

  if (command->expectResponse) {
    unsigned int start_wait = millis();
    while (Serial2.available() == 0) {
      vTaskDelay(20);
      if (millis() - start_wait >= command->timeout) {
        break;
      }
    }

    // Call callback function for command when data is available at Serial2
    command->callback(command);

    // Give back serial read mutex.
    xSemaphoreGive(this->_mutexReadSerialData);
  }

  xSemaphoreGive(this->_mutexWriteSerialData);
}

void NSPanel::_sendRawCommand(const char *command, int length) {
  while (!NSPanel::instance->_writeCommandsToSerial) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  while (true) {
    if (xSemaphoreTake(NSPanel::instance->_mutexWriteSerialData, portMAX_DELAY)) {
      break;
    } else {
      LOG_ERROR("Failed to take serial write mutex, trying again in 3 seconds.");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }

  for (int i = 0; i < length; i++) {
    Serial2.print(command[i]);
  }
  NSPanel::instance->_sendCommandEndSequence();
  xSemaphoreGive(this->_mutexWriteSerialData);
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
    if (Serial2.available() == 0) {
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
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

      if (Serial2.available() == 0) {
        vTaskDelay(20 / portTICK_PERIOD_MS);
      }
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

  while (true) {
    if (xSemaphoreTake(NSPanel::instance->_mutexReadSerialData, 1000 / portTICK_PERIOD_MS)) {
      break;
    } else {
      LOG_ERROR("Failed to take serial read mutex, trying again in 3 seconds.");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }

  while (true) {
    if (xSemaphoreTake(NSPanel::instance->_mutexWriteSerialData, portTICK_PERIOD_MS)) {
      break;
    } else {
      LOG_ERROR("Failed to take serial write mutex, trying again in 3 seconds.");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }

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

bool NSPanel::getUpdateState() {
  return this->_isUpdating;
}

uint8_t NSPanel::getUpdateProgress() {
  return this->_update_progress;
}

std::string NSPanel::getWarnings() {
  std::string return_string = "";
  if (!NSPanel::instance->_has_received_nspm) {
    return_string.append("Did not receive NSPM-flag from screen. Is the screen running the NSPanel Manager TFT file?\n");
  }
  return return_string;
}

bool NSPanel::ready() {
  return this->_has_received_nspm;
}

bool NSPanel::_updateTFTOTA() {
  LOG_INFO("_updateTFTOTA Started.");

  NSPanel::instance->_writeCommandsToSerial = false;
  NSPanel::instance->_update_progress = 0;
  NSPanel::instance->_isUpdating = true;

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
  size_t file_size = HttpLib::GetFileSize(downloadUrl.c_str());

  LOG_INFO("Force restarting screen via power switch.");
  digitalWrite(4, HIGH); // Turn off power to the display
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  digitalWrite(4, LOW); // Turn on power to the display
  vTaskDelay(5000 / portTICK_PERIOD_MS);

  // Switch to desiered upload buad rate.
  int32_t baud_diff = NSPMConfig::instance->tft_upload_baud - Serial2.baudRate();
  if (baud_diff < 0) {
    baud_diff = baud_diff / -1;
  }

  // Wait for panel to finish whatever it is doing.
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  if (baud_diff >= 10) {
    std::string uploadBaudRateString = "baud=";
    uploadBaudRateString.append(std::to_string(NSPMConfig::instance->tft_upload_baud));
    Serial2.print(uploadBaudRateString.c_str());
    NSPanel::instance->_sendCommandEndSequence();

    std::string read_data = "";
    NSPanel::instance->_readDataToString(&read_data, 5000, false);
    if (!read_data.empty()) {
      LOG_ERROR("Baud rate switch failed. Will restart NSPanel and continue anyways.");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(4, HIGH); // Turn off power to the display
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(4, LOW); // Turn on power to the display
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    LOG_INFO("Baud rate switching Serial2 from ", Serial2.baudRate(), " to ", NSPMConfig::instance->tft_upload_baud);
    NSPanel::_clearSerialBuffer();
    Serial2.end();
    Serial2.setTxBufferSize(0);
    Serial2.begin(NSPMConfig::instance->tft_upload_baud, SERIAL_8N1, 17, 16);
  }

  // Send "connect" string to get data
  Serial2.print("DRAKJHSUYDGBNCJHGJKSHBDN");
  NSPanel::instance->_sendCommandEndSequence();
  vTaskDelay((1000000 / Serial2.baudRate()) + 30 / portTICK_PERIOD_MS);
  // Clear Serial2 read buffer
  while (Serial2.available() > 0) {
    Serial2.read();
    if (Serial2.available() == 0) {
      vTaskDelay(250 / portTICK_PERIOD_MS);
    }
  }

  LOG_DEBUG("Sending connect to panel");
  Serial2.print("connect");
  NSPanel::instance->_sendCommandEndSequence();
  vTaskDelay((1000000 / Serial2.baudRate()) + 30 / portTICK_PERIOD_MS);

  std::string comok_string = "";
  NSPanel::instance->_readDataToString(&comok_string, 5000, false);
  NSPanel::instance->_clearSerialBuffer();
  if (comok_string.length() > 3) {
    comok_string.erase(comok_string.length() - 3);
  }
  LOG_DEBUG("Got comok: ", comok_string.c_str());

  LOG_DEBUG("Will start TFT upload, TFT file size: ", file_size);
  // TODO: Detect if new protocol is not supported, in that case set flag in flash and restart and then continue flash with legacy mode.
  // Send whmi-wri command to initiate upload
  std::string commandString;
  if (NSPMConfig::instance->use_new_upload_protocol) {
    LOG_INFO("Starting upload using v1.2 protocol.");
    commandString = "whmi-wris ";
    commandString.append(std::to_string(file_size));
    commandString.append(",");
    commandString.append(std::to_string(NSPMConfig::instance->tft_upload_baud));
    commandString.append(",1");
  } else {
    LOG_INFO("Starting upload using v1.1 protocol.");
    commandString = "whmi-wri ";
    commandString.append(std::to_string(file_size));
    commandString.append(",1");
    commandString.append(std::to_string(NSPMConfig::instance->tft_upload_baud));
    commandString.append(",1");
  }
  LOG_DEBUG("Doing one last serial2 clear.");
  NSPanel::_clearSerialBuffer();

  LOG_DEBUG("Sending TFT upload command: ", commandString.c_str());
  Serial2.print(commandString.c_str());
  NSPanel::instance->_sendCommandEndSequence();

  // Wait until Nextion returns okay to transmit tft
  LOG_DEBUG("Waiting for panel reponse");
  while (Serial2.available() == 0) {
    vTaskDelay(25 / portTICK_PERIOD_MS);
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
    LOG_ERROR("Will try to upload anyway. If nothing happens, please reboot and try again.");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    return false;
  }

  unsigned long startWaitingForOKForNextChunk = 0;
  uint32_t nextStartWriteOffset = 0;
  size_t lastReadByte = 0;
  uint8_t dataBuffer[4096];

  // Loop until break when all firmware has finished uploading (data available in stream == 0)
  while (true) {
    // Calculate next chunk size
    int next_write_size;
    if (file_size - lastReadByte > 4096) {
      next_write_size = 4096;
    } else {
      next_write_size = file_size - lastReadByte;
    }
    size_t bytesReceived = HttpLib::DownloadChunk(dataBuffer, downloadUrl.c_str(), nextStartWriteOffset, next_write_size);
    LOG_DEBUG("Bytes received: ", bytesReceived, " requested ", next_write_size);

    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial2.write(dataBuffer, bytesReceived);
    nextStartWriteOffset += bytesReceived;
    lastReadByte = nextStartWriteOffset;
    NSPanel::instance->_update_progress = ((float)lastReadByte / (float)file_size) * 100;

    std::string return_string;
    uint16_t recevied_bytes = NSPanel::instance->_readDataToString(&return_string, 5000, true);
    if (lastReadByte >= file_size) {
      NSPanel::instance->_update_progress = 100;
      LOG_INFO("TFT Upload complete, processed ", lastReadByte, " bytes.");
      break;
    } else if (return_string[0] == 0x05) {
      // Old protocol, just upload next chunk.
      LOG_DEBUG("Got 0x05, uploading next chunk.");
    } else if (return_string[0] == 0x08) {
      while (return_string.length() < 4) {
        LOG_DEBUG("Waiting for offset data byte ", return_string.length() - 1);
        while (Serial2.available() <= 0) {
          vTaskDelay(20 / portTICK_PERIOD_MS);
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
    } else {
      LOG_DEBUG("Got unexpected return data from panel. Received ", recevied_bytes, " bytes: ");
      for (int i = 0; i < recevied_bytes; i++) {
        LOG_DEBUG("0x", String(return_string[i], HEX).c_str());
      }
    }

    // vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  LOG_INFO("Getting TFT MD5 checksum to store in flash.");
  char checksum_holder[33];
  while (true) {
    std::string checksumUrl = "http://";
    checksumUrl.append(NSPMConfig::instance->manager_address);
    checksumUrl.append(":");
    checksumUrl.append(std::to_string(NSPMConfig::instance->manager_port));
    checksumUrl.append("/checksum_tft_file");
    if (HttpLib::GetMD5sum(checksumUrl.c_str(), checksum_holder)) {
      break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  NSPMConfig::instance->md5_tft_file = checksum_holder;
  NSPMConfig::instance->saveToLittleFS();

  LOG_INFO("Will restart in 5 seconds.");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  ESP.restart();

  vTaskDelay(portMAX_DELAY);
  vTaskDelete(NULL);
  return false;
}
