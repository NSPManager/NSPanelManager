#ifndef NSPANEL_H
#define NSPANEL_H

#include <Arduino.h>
#include <HardwareSerial.h>

class NSPanel
{
public:
    void init();

private:
    TaskHandle_t _taskHandleUartListen;
    static void _taskUartListen(void *param);

    void sendCommand(const char *command);
    void tryBaud();

    unsigned long _lastCommandSent = 0;
};

#endif