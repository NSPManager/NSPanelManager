#include <Light.hpp>
#include <LightManager.hpp>
#include <MqttLog.hpp>

void Light::initFromJson(ArduinoJson::JsonPair *json) {
  this->_id = atoi(json->key().c_str());
  this->_name = json->value()["name"] | "ERR-L";
  this->_canDim = json->value()["can_dim"];
  this->_canTemperature = json->value()["can_temperature"];
  this->_canRgb = json->value()["can_rgb"];
  this->_roomViewPosition = json->value()["view_position"] | 0;
  this->_isCeiling = json->value()["ceiling"];

  LOG_DEBUG("Loaded light ", this->_name.c_str(), " as type: ", this->_isCeiling ? "CEILING" : "TABLE");
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
