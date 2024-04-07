#include <Light.hpp>
#include <LightManager.hpp>
#include <MqttLog.hpp>
#include <functional>
#include <map>

void Light::initFromMap(std::map<std::string, std::string> &data) {
  this->_id = atoi(data["id"].c_str());
  this->_name = data["name"];
  this->_canDim = data["can_dim"].compare("true") == 0;
  this->_canTemperature = data["can_temperature"].compare("true") == 0;
  this->_canRgb = data["can_rgb"].compare("true") == 0;
  this->_roomViewPosition = atoi(data["view_position"].c_str());
  this->_isCeiling = data["ceiling"].compare("true") == 0;

  this->callUpdateCallbacks();
  LOG_TRACE("Loaded light ", this->_name.c_str(), " as type: ", this->_isCeiling ? "CEILING" : "TABLE");
}

void Light::setLightLevel(uint8_t lightLevel) {
  this->_level = lightLevel;
  this->_hasLevelChanged = true;
}

void Light::setColorTemperature(uint16_t colorTemperature) {
  this->_colorTemperature = colorTemperature;
  this->_hasColorTemperatureChanged = true;
}

void Light::setHue(uint16_t colorHue) {
  this->_colorHue = colorHue;
  this->_hasHueChanged = true;
}

void Light::setSaturation(uint16_t colorSaturation) {
  this->_colorSaturation = colorSaturation;
  this->_hasSaturationChaned = true;
}

uint16_t Light::getId() {
  return this->_id;
}

std::string Light::getName() {
  return this->_name;
}

uint16_t Light::getLightLevel() {
  return this->_level;
}

uint16_t Light::getColorTemperature() {
  return this->_colorTemperature;
}

uint16_t Light::getHue() {
  return this->_colorHue;
}

uint16_t Light::getSaturation() {
  return this->_colorSaturation;
}

uint8_t Light::getRoomViewPosition() {
  return this->_roomViewPosition;
}

void Light::attachDeconstructCallback(DeviceEntityObserver *observer) {
  // Do not add a subsriber twice
  for (DeviceEntityObserver *obs : this->_deconstructObservers) {
    if (observer == obs) {
      return;
    }
  }
  this->_deconstructObservers.push_back(observer);
}

void Light::detachDeconstructCallback(DeviceEntityObserver *observer) {
  this->_deconstructObservers.remove(observer);
}

void Light::callDeconstructCallbacks() {
  LOG_DEBUG("Trying to call deconstruct callback, number of callbacks: ", this->_deconstructObservers.size());
  vTaskDelay(500 / portTICK_PERIOD_MS);
  std::list<DeviceEntityObserver *> loop_list;
  std::copy(this->_deconstructObservers.begin(), this->_deconstructObservers.end(), std::back_inserter(loop_list));
  for (DeviceEntityObserver *observer : loop_list) {
    LOG_DEBUG("Calling deconstruct callback!");
    observer->entityDeconstructCallback(this);
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

void Light::attachUpdateCallback(DeviceEntityObserver *observer) {
  // Do not add a subsriber twice
  for (DeviceEntityObserver *obs : this->_updateObservers) {
    if (observer == obs) {
      return;
    }
  }
  this->_updateObservers.push_back(observer);
}

void Light::detachUpdateCallback(DeviceEntityObserver *observer) {
  this->_updateObservers.remove(observer);
}

void Light::callUpdateCallbacks() {
  std::list<DeviceEntityObserver *> loop_list;
  std::copy(this->_updateObservers.begin(), this->_updateObservers.end(), std::back_inserter(loop_list));
  for (DeviceEntityObserver *observer : loop_list) {
    observer->entityUpdateCallback(this);
  }
}

DeviceEntityType Light::getType() {
  return DeviceEntityType::LIGHT;
}

bool Light::canDim() {
  return this->_canDim;
}

bool Light::canTemperature() {
  return this->_canTemperature;
}

bool Light::canRgb() {
  return this->_canRgb;
}

bool Light::hasLevelChanged() {
  return this->_hasLevelChanged;
}

bool Light::hasColorTemperatureChanged() {
  return this->_hasColorTemperatureChanged;
}

bool Light::hasHueChanged() {
  return this->_hasHueChanged;
}

bool Light::hasSaturationChanged() {
  return this->_hasSaturationChaned;
}

std::string Light::getLevelStateTopic() {
  std::string levelStatusTopic = "nspanel/entities/light/";
  levelStatusTopic.append(std::to_string(this->getId()));
  levelStatusTopic.append("/state_brightness_pct");
  return levelStatusTopic;
}

std::string Light::getColorTemperatureStateTopic() {
  std::string kelvinStatusTopic = "nspanel/entities/light/";
  kelvinStatusTopic.append(std::to_string(this->getId()));
  kelvinStatusTopic.append("/state_kelvin");
  return kelvinStatusTopic;
}

std::string Light::getHueStateTopic() {
  std::string hueStatusTopic = "nspanel/entities/light/";
  hueStatusTopic.append(std::to_string(this->getId()));
  hueStatusTopic.append("/state_hue");
  return hueStatusTopic;
}

std::string Light::getSaturationStateTopic() {
  std::string saturationStatusTopic = "nspanel/entities/light/";
  saturationStatusTopic.append(std::to_string(this->getId()));
  saturationStatusTopic.append("/state_sat");
  return saturationStatusTopic;
}
