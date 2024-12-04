#ifndef ROOM_PAGE_HPP
#define ROOM_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
#include <PageBase.hpp>
#include <RoomManagerObserver.hpp>
#include <protobuf_defines.h>

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
  PROTOBUF_NSPANEL_LIGHT_STATUS *_selectedLight;
  void _toggleSelectedLight();
  PROTOBUF_NSPANEL_LIGHT_STATUS *_getLightAtRoomViewPosition(uint32_t position);
};

#endif
