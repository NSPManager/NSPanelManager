#ifndef HOME_PAGE_HPP
#define HOME_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
class Light;
#include <PageBase.hpp>

class HomePage : public PageBase, DeviceEntityObserver {
public:
  void show();
  void update();
  void unshow();

  virtual void entityDeconstructCallback(DeviceEntity *);
  virtual void entityUpdateCallback(DeviceEntity *);

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

private:
  uint8_t _dimmerValue;
  uint8_t _colorTemp;
};

#endif
