#ifndef NSPANEL_H
#define NSPANEL_H

#include <Arduino.h>
#include <NSPMConfig.h>
#include <HardwareSerial.h>
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
    /// @brief The callback function when data is returned
    void (*callback)(std::string);
};

class NSPanel
{
public:
    inline static NSPanel *instance;
    void init();
    void startOTAUpdate();

private:
    // Tasks
    TaskHandle_t _taskHandleSendCommandQueue;
    static void _taskSendCommandQueue(void *param);
    TaskHandle_t _taskHandleReadNSPanelData;
    static void _taskReadNSPanelData(void *param);
    static void _taskUpdateTFTConfigOTA(void *param);

    unsigned long _lastCommandSent = 0;
    std::queue<NSPanelCommand> _commandQueue;
    void _sendCommandWithoutResponse(const char *command);
    void _addCommandToQueue(NSPanelCommand command);
    void _sendCommand(NSPanelCommand *command);
    void tryBaud();
};

#endif