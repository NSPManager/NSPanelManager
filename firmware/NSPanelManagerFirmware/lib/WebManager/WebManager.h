#ifndef NSPM_WEB_MANAGER
#define NSPM_WEB_MANAGER

#define getHeaderValue(x, y) x.substring(strlen(y))

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <NSPMConfig.h>

enum WebManagerState {
  ONLINE,
  UPDATING_FIRMWARE,
  UPDATING_LITTLEFS,
};

class WebManager {
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
  static void doRebootNow(AsyncWebServerRequest *request);
  static WebManagerState getState();
  static uint8_t getUpdateProgress();

private:
  AsyncWebServer _server = AsyncWebServer(80);
  static bool _update(uint8_t type, const char *url);
  static bool _httpGetMD5(const char *path, char *buffer);
  static void _taskPerformOTAUpdate(void *param);
  std::string _nspmFirmwareVersion;
  /// @brief Contains the current state of actions.
  static inline WebManagerState _state;
  /// @brief If updating, contains the % done of the update
  static inline uint8_t _update_progress;
};

#endif