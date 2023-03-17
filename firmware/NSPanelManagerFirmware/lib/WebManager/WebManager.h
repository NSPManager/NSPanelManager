#ifndef NSPM_WEB_MANAGER
#define NSPM_WEB_MANAGER

#define getHeaderValue(x, y) x.substring(strlen(y))

#include <NSPMConfig.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class WebManager
{
public:
    void init(const char *nspmFirmwareVersion);
    static void doRebootAt(AsyncWebServerRequest *request);
    /// @brief Indicate wether a reboot should be done or not
    /// @return True = time to reboot
    bool doReboot();
    static WebManager *instance;
    static String processIndexTemplate(const String &templateVar);
    static void saveConfigFromWeb(AsyncWebServerRequest *request);
    static void respondAvailableWiFiNetworks(AsyncWebServerRequest *request);
    static void startOTAUpdate(AsyncWebServerRequest *request);
    static void startTFTOTAUpdate(AsyncWebServerRequest *request);
    static void factoryReset(AsyncWebServerRequest *request);

private:
    AsyncWebServer _server = AsyncWebServer(80);
    static bool _update(uint8_t type, const char *url);
    static bool _httpGetMD5(const char *path, char *buffer);
    static void _taskPerformOTAUpdate(void *param);
    std::string _nspmFirmwareVersion;
};

#endif