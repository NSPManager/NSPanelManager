#ifndef ROOM_PAGE_HPP
#define ROOM_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
#include <PageBase.hpp>
#include <RoomManager.hpp>
class Light;

class RoomPage : public PageBase, DeviceEntityObserver, RoomManagerObserver {
public:
  void update();
  void show();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);

  void entityDeconstructCallback(DeviceEntity *);
  void entityUpdateCallback(DeviceEntity *);

  void roomChangedCallback();

private:
  void setLightVisibility(uint8_t position, bool visibility);
  void setLightName(uint8_t position, const char *name);
  void setLightState(uint8_t position, bool state);
  void setCurrentRoomLabel(const char *label);
  Light *_selectedLight;
  void _toggleSelectedLight();
};

#endif
