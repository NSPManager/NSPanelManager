#ifndef SCENE_HPP
#define SCENE_HPP

#include <Arduino.h>
class Room; // Defined in Room.hpp

class Scene {
public:
  uint16_t id = 0;
  std::string name;
  Room *room = nullptr;
  void save();
  void activate();
};

#endif
