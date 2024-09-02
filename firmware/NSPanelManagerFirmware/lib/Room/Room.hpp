#ifndef ROOM_HPP
#define ROOM_HPP

#include <Arduino.h>
// #include <Light.hpp>
class Light;
class Scene;
#include <list>
#include <protobuf_nspanel.pb-c.h>
#include <unordered_map>
#include <vector>

class Room {
public:
  Room(int32_t id);
  NSPanelRoomStatus status;
  uint8_t id = 0;
  std::string name;
  std::vector<Scene *> scenes;
  std::list<NSPanelLightStatus *> getAllLights();
  NSPanelLightStatus *getLightAtRoomViewPosition(int room_view_position);
  Scene *getSceneById(uint16_t id);
  bool anyLightsOn();
};

#endif
