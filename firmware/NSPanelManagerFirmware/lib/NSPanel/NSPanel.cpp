#include <NSPanel.h>
#include <MqttLog.h>

#define NEX_RET_CMD_FINISHED (0x01)
#define NEX_RET_EVENT_LAUNCHED (0x88)
#define NEX_RET_EVENT_UPGRADED (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD (0x65)
#define NEX_RET_EVENT_POSITION_HEAD (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD (0x66)
#define NEX_RET_STRING_HEAD (0x70)
#define NEX_RET_NUMBER_HEAD (0x71)
#define NEX_RET_INVALID_CMD (0x00)
#define NEX_RET_INVALID_COMPONENT_ID (0x02)
#define NEX_RET_INVALID_PAGE_ID (0x03)
#define NEX_RET_INVALID_PICTURE_ID (0x04)
#define NEX_RET_INVALID_FONT_ID (0x05)
#define NEX_RET_INVALID_BAUD (0x11)
#define NEX_RET_INVALID_VARIABLE (0x1A)
#define NEX_RET_INVALID_OPERATION (0x1B)

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
    this->sendCommand("boguscommand=0");
    this->sendCommand("connect");

    // Wake up the nextion
    this->sendCommand("bkcmd=0");
    this->sendCommand("sleep=0");

    this->sendCommand("bkcmd=0");
    this->sendCommand("sleep=0");

    // Reboot it
    this->sendCommand("rest");

    this->sendCommand("bkcmd=3");

    this->sendCommand("page bootscreen");
    this->sendCommand("dim=1.0");

    vTaskDelay(5000 / portTICK_PERIOD_MS);
    this->sendCommand("page Bathroom");
}

void NSPanel::init()
{
    // Pin 4 controls screen on/off.
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);

    uint8_t RX_PIN = 17;
    uint8_t TX_PIN = 16;

    LOG_INFO("Starting communication with NSPanel.");
    // xTaskCreatePinnedToCore(_taskUartListen, "taskUartListen", 5000, NULL, 1, &this->_taskHandleUartListen, CONFIG_ARDUINO_RUNNING_CORE);
    LOG_INFO("Trying 115200, RX TX");
    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    this->tryBaud();
    // Serial2.end();
    // Serial2.flush();
    // LOG_INFO("Trying 9600, RX TX");
    // Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    // this->tryBaud();
    // Serial2.end();
    // Serial2.flush();
    // LOG_INFO("Trying 115200, TX RX");
    // Serial2.begin(115200, SERIAL_8N1, TX_PIN, RX_PIN);
    // this->tryBaud();
    // Serial2.end();
    // Serial2.flush();
    // LOG_INFO("Trying 9600, TX RX");
    // Serial2.begin(9600, SERIAL_8N1, TX_PIN, RX_PIN);
    // this->tryBaud();

    // this->sendCommand("boguscommand=0");
    // this->sendCommand("connect");

    // // Wake up the nextion
    // this->sendCommand("bkcmd=0");
    // this->sendCommand("sleep=0");

    // this->sendCommand("bkcmd=0");
    // this->sendCommand("sleep=0");

    // // Reboot it
    // this->sendCommand("rest");

    // this->sendCommand("bkcmd=3");

    // // Reboot it
    // this->sendCommand("page \"bootscreen\"");

    // this->sendCommand("dim=1.0");
}

void NSPanel::sendCommand(const char *command)
{
    // Clear buffer before sending
    while (Serial2.available())
    {
        Serial2.read();
    }

    Serial2.print(command);
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    this->_lastCommandSent = millis();

    LOG_INFO(recvRetCommandFinished() ? "TRUE" : "FALSE");
}

void NSPanel::_taskUartListen(void *param)
{
    LOG_INFO("Starting taskUartListen.");
    for (;;)
    {
        while (Serial2.available() > 0)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            LOG_INFO(char(Serial2.read()));
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}