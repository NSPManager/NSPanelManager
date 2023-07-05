#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
#include <list>
#include <map>

class Light : public DeviceEntity {
public:
  void initFromMap(std::map<std::string, std::string> &json);

  void setLightLevel(uint8_t lightLevel);
  void setColorTemperature(uint16_t colorTemperature);
  void setHue(uint16_t colorHue);
  void setSaturation(uint16_t colorSaturation);

  uint16_t getId();
  std::string getName();
  uint16_t getLightLevel();
  uint16_t getColorTemperature();
  uint16_t getHue();
  uint16_t getSaturation();
  uint8_t getRoomViewPosition();

  bool canDim();
  bool canTemperature();
  bool canRgb();

  bool hasLevelChanged();
  bool hasColorTemperatureChanged();
  bool hasHueChanged();
  bool hasSaturationChanged();

  std::string getLevelStateTopic();
  std::string getColorTemperatureStateTopic();
  std::string getHueStateTopic();
  std::string getSaturationStateTopic();

  DeviceEntityType getType();
  void attachDeconstructCallback(DeviceEntityObserver *callback);
  void detachDeconstructCallback(DeviceEntityObserver *callback);
  void callDeconstructCallbacks();

  void attachUpdateCallback(DeviceEntityObserver *callback);
  void detachUpdateCallback(DeviceEntityObserver *callback);
  void callUpdateCallbacks();

private:
  uint16_t _id = 0;
  uint16_t _level = 0;
  uint16_t _colorTemperature = 0;
  uint16_t _colorHue = 0;
  uint16_t _colorSaturation = 0;
  uint8_t _roomViewPosition = 0;
  uint8_t _isCeiling = 0;
  bool _canDim = false;
  bool _canTemperature = false;
  bool _canRgb = false;
  std::string _name;

  bool _hasLevelChanged = false;
  bool _hasColorTemperatureChanged = false;
  bool _hasHueChanged = false;
  bool _hasSaturationChaned = false;
  std::list<DeviceEntityObserver *> _deconstructObservers;
  std::list<DeviceEntityObserver *> _updateObservers;
};

#endif
