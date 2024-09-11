#ifndef HOME_PAGE_HPP
#define HOME_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
class Light;
#include <InterfaceConfigEnums.hpp>
#include <PageBase.hpp>
#include <RoomManagerObserver.hpp>

class HomePage : public PageBase, DeviceEntityObserver, RoomManagerObserver {
public:
  void init();
  void show();
  void update();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);

  void entityDeconstructCallback(DeviceEntity *);
  void entityUpdateCallback(DeviceEntity *);
  void roomChangedCallback();

  void setDimmingValue(uint8_t value);
  int getDimmingValue();
  void updateDimmerValueCache();
  void setColorTempValue(uint8_t value);
  int getColorTempValue();
  void updateColorTempValueCache();
  void setCeilingBrightnessLabelText(uint8_t value);
  void setTableBrightnessLabelText(uint8_t value);
  void setCeilingLightsState(bool state);
  void setTableLightsState(bool state);
  void setSliderLightLevelColor(uint color);
  void setSliderColorTempColor(uint color);
  void setHighlightCeilingVisibility(bool visable);
  void setHighlightTableVisibility(bool visable);
  void setScreensaverTimeout(uint16_t timeout);
  void setRoomText(const char *text);
  void setModeText(const char *text);

  void updateDeviceEntitySubscriptions();
  void updateLightStatus(bool updateLightLevel, bool updateColorTemperature);
  void updateRoomInfo();
  void updateModeText();

  void setEditLightMode(editLightMode new_mode);

  void goToNextMode();
  void setCurrentMode(roomMode mode);

private:
  uint8_t _dimmerValue;
  uint8_t _colorTemp;

  unsigned long _lastSpecialModeEventMillis;
  unsigned long _lastMasterCeilingLightsButtonTouch;
  unsigned long _lastMasterTableLightsButtonTouch;
  unsigned long _lastMasterCeilingLightsButtonRelease;
  unsigned long _lastMasterTableLightsButtonRelease;
  unsigned long _ignoreMqttMessagesUntil;
  bool _ignoreNextTouchRelease;
  bool _isFingerOnDisplay;

  void _ceilingMasterButtonEvent();
  void _tableMasterButtonEvent();

  void _updateLightsWithNewBrightness(uint8_t brightness);
  void _updateLightsColorTempAccordingToSlider();
  void _startSpecialModeTriggerTask(editLightMode mode);
  void _startSpecialModeTimerTask();
  void _stopSpecialMode();

  static void _taskTriggerSpecialModeTriggerTask(void *param);
  static inline TaskHandle_t _specialModeTimerTaskHandle;
  static void _taskSpecialModeTimerTask(void *param);
  static inline unsigned long _lastDeviceEntityUpdate;
  static inline TaskHandle_t _taskHandleUpdateDisplay = NULL;
  static void _taskUpdateDisplay(void *param);
};

#endif
