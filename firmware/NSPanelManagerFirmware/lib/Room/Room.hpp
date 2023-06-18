#ifndef ROOM_HPP
#define ROOM_HPP

#include <Arduino.h>
// #include <Light.hpp>
class Light;
class Scene;
#include <list>
#include <unordered_map>
#include <vector>

// TODO: Create LightManager that can handle lights so that, for example the following can be done
// light->setLightLevel(100);
// light->setColorTemperature(2200);
// LightManager::sendLightUpdates();

class Room {
public:
  uint8_t id = 0;
  std::string name;
  std::unordered_map<uint16_t, Light *> ceilingLights;
  std::unordered_map<uint16_t, Light *> tableLights;
  std::vector<Scene *> scenes;
  std::list<Light *> getCeilingLightsThatAreOn();
  std::list<Light *> getTableLightsThatAreOn();
  std::list<Light *> getAllLightsThatAreOn();
  std::list<Light *> getAllCeilingLights();
  std::list<Light *> getAllTableLights();
  std::list<Light *> getAllLights();
  std::list<Light *> getAllRoomViewLights();
  Light *getLightAtRoomViewPosition(int room_view_position);
  bool anyCeilingLightsOn();
  bool anyTableLightsOn();
  bool anyLightsOn();
};

#endif
