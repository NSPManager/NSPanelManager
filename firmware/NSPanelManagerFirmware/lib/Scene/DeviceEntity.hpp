#ifndef DEVICE_ENTITY_HPP
#define DEVICE_ENTITY_HPP

#include <list>

enum DeviceEntityType {
  LIGHT,
  SCENE
};

class DeviceEntity; // Real Entity-class defined below
class DeviceEntityObserver {
public:
  virtual void entityDeconstructCallback(DeviceEntity *);
  virtual void entityUpdateCallback(DeviceEntity *);
};

class DeviceEntity {
public:
  virtual DeviceEntityType getType() = 0;
  virtual void attachDeconstructCallback(DeviceEntityObserver *callback) = 0;
  virtual void detachDeconstructCallback(DeviceEntityObserver *callback) = 0;
  virtual void callDeconstructCallbacks() = 0;

  virtual void attachUpdateCallback(DeviceEntityObserver *callback) = 0;
  virtual void detachUpdateCallback(DeviceEntityObserver *callback) = 0;
  virtual void callUpdateCallbacks() = 0;
};

#endif
