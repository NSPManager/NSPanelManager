#ifndef SCENE_HPP
#define SCENE_HPP

#include <Arduino.h>
class Room; // Defined in Room.hpp
#include <DeviceEntity.hpp>

class Scene : public DeviceEntity {
public:
  uint16_t id = 0;
  std::string name;
  Room *room = nullptr;
  void save();
  void activate();
  uint16_t getId();
  std::string getName();

  DeviceEntityType getType();
  void attachDeconstructCallback(DeviceEntityObserver *callback);
  void detachDeconstructCallback(DeviceEntityObserver *callback);
  void callDeconstructCallbacks();

  void attachUpdateCallback(DeviceEntityObserver *callback);
  void detachUpdateCallback(DeviceEntityObserver *callback);
  void callUpdateCallbacks();

private:
  std::list<DeviceEntityObserver *> _deconstructObservers;
  std::list<DeviceEntityObserver *> _updateObservers;
};

#endif
