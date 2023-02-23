#ifndef NSPM_WEB_MANAGER
#define NSPM_WEB_MANAGER

#define getHeaderValue(x, y) x.substring(strlen(y))

#include <NSPMConfig.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class WebManager
{
public:
    void init();
    static void doRebootAt(AsyncWebServerRequest *request);
    /// @brief Indicate wether a reboot should be done or not
    /// @return True = time to reboot
    bool doReboot();
    static WebManager *instance;
    static void saveConfigFromWeb(AsyncWebServerRequest *request);
    static void respondAvailableWiFiNetworks(AsyncWebServerRequest *request);
    static void startOTAUpdate(AsyncWebServerRequest *request);

private:
    AsyncWebServer _server = AsyncWebServer(80);
    static void _taskPerformOTAUpdate(void *param);
};

#endif