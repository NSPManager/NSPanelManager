#ifndef NSPANEL_H
#define NSPANEL_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <NSPMConfig.h>
#include <list>
#include <queue>
#include <vector>

#define getHeaderValue(x, y) x.substring(strlen(y))

// milliseconds to wait between each command sent
#define COMMAND_SEND_WAIT_MS 2

struct NSPanelCommand {
  /// @brief The command to be sent
  std::string command;
  /// @brief we expect response that the command finished?
  bool expectFinishedResponse = false;
  /// @brief Do we expect a response with data?
  bool expectResponse = false;
  /// @brief The buffer to save the response in
  std::list<uint8_t> *buffer = nullptr;
  /// @brief The callback function when data is returned
  void (*callback)(NSPanelCommand *cmd);
  /// @brief Used to indicate that the callback function is done
  bool callbackFinished = false;
  /// @brief Timeout for reading any response data, 0 = no timeout.
  uint16_t timeout = 3000;
};

class NSPanel {
public:
  inline static NSPanel *instance;
  static void attachTouchEventCallback(void (*callback)(uint8_t, uint8_t, bool));
  static void attachSleepCallback(void (*callback)());
  static void attachWakeCallback(void (*callback)());
  bool ready();
  bool init();
  bool startOTAUpdate();
  void goToPage(const char *page);
  void setDimLevel(uint8_t dimLevel);
  void setSleep(bool sleep);
  void setComponentText(const char *componentId, const char *text);
  void setComponentVal(const char *componentId, int16_t value);
  void setTimerTimeout(const char *componentId, uint16_t timeout);
  void setComponentPic(const char *componentId, uint8_t value);
  void setComponentPic1(const char *componentId, uint8_t value);
  void setComponentForegroundColor(const char *componentId, uint value);
  void setComponentVisible(const char *componentId, bool visible);
  bool getUpdateState();
  uint8_t getUpdateProgress();
  int getComponentIntVal(const char *componentId);
  void restart();

private:
  // Tasks
  static inline TaskHandle_t _taskHandleSendCommandQueue;
  static void _taskSendCommandQueue(void *param);
  static void _onSerialData(void);
  static void _taskUpdateTFTConfigOTA(void *param);
  /// @brief Download a chunk of data from given addres, to the buffer at the given offset
  /// @param buffer The buffer to store data into
  /// @param address The address to download the firmware from
  /// @param offset Offset to request data from (bytes)
  /// @param size Maximum download chunk size
  /// @return The number of bytes downloaded
  static bool _initTFTUpdate(int communication_baud_rate);
  static bool _updateTFTOTA();
  std::queue<std::vector<char>> _processQueue;
  TaskHandle_t _taskHandleProcessPanelOutput;
  static void _taskProcessPanelOutput(void *param);
  SemaphoreHandle_t _mutexReadSerialData;
  SemaphoreHandle_t _mutexWriteSerialData;

  unsigned long _lastCommandSent = 0;
  std::queue<NSPanelCommand> _commandQueue;
  void _sendCommandWithoutResponse(const char *command);
  void _sendCommandClearResponse(const char *command);
  void _sendCommandClearResponse(const char *command, uint16_t timeout);
  void _sendCommandEndSequence();
  void _addCommandToQueue(NSPanelCommand command);
  void _sendCommand(NSPanelCommand *command);
  void _sendRawCommand(const char *command, int length);
  void _startListeningToPanel();
  void _stopListeningToPanel();
  uint16_t _readDataToString(std::string *data, uint32_t timeout, bool receive_flag);

  /// @brief Call reigstered callback when a touch event occured
  static inline void (*_touchEventCallback)(uint8_t, uint8_t, bool);
  /// @brief Call registered callback when screen goes to sleep
  static inline void (*_sleepCallback)() = nullptr;
  static inline void (*_wakeCallback)() = nullptr;
  static void _clearSerialBuffer(NSPanelCommand *cmd);
  static void _clearSerialBuffer();
  static inline bool _writeCommandsToSerial;
  /// @brief Flag indicating wether or not a TFT update is ongoing
  bool _isUpdating;
  /// @brief If updating, contains the % done of the update
  uint8_t _update_progress;
  /// @brief A boolean indicating wether or not we have received the "NSPM"-text from the panel once it started.
  bool _has_received_nspm;
};

#endif
