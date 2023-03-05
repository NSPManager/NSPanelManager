#ifndef NSPANEL_H
#define NSPANEL_H

#include <Arduino.h>
#include <NSPMConfig.h>
#include <HardwareSerial.h>
#include <list>
#include <queue>

#define getHeaderValue(x, y) x.substring(strlen(y))

// milliseconds to wait between each command sent
#define COMMAND_SEND_WAIT_MS 2

struct NSPanelCommand
{
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
};

class NSPanel
{
public:
    inline static NSPanel *instance;
    static void attachTouchEventCallback(void (*callback)(uint8_t, uint8_t, bool));
    void init();
    void startOTAUpdate();
    void goToPage(const char *page);
    void setDimLevel(uint8_t dimLevel);
    void setSleep(bool sleep);
    void setComponentText(const char *componentId, const char *text);
    void setComponentVal(const char *componentId, uint8_t value);
    int getComponentIntVal(const char *componentId);
    void restart();

private:
    // Tasks
    TaskHandle_t _taskHandleSendCommandQueue;
    static void _taskSendCommandQueue(void *param);
    TaskHandle_t _taskHandleReadNSPanelData;
    static void _taskReadNSPanelData(void *param);
    static void _taskUpdateTFTConfigOTA(void *param);
    SemaphoreHandle_t _mutexReadSerialData;

    unsigned long _lastCommandSent = 0;
    std::queue<NSPanelCommand> _commandQueue;
    void _sendCommandWithoutResponse(const char *command);
    void _sendCommandClearResponse(const char *command);
    void _addCommandToQueue(NSPanelCommand command);
    void _sendCommand(NSPanelCommand *command);
    void _sendRawCommand(const char* command, int length);
    void _startListeningToPanel();
    void _stopListeningToPanel();

    static inline void (*_touchEventCallback)(uint8_t, uint8_t, bool);
    static void _clearSerialBuffer(NSPanelCommand *cmd);
    static void _clearSerialBuffer();
};

#endif
