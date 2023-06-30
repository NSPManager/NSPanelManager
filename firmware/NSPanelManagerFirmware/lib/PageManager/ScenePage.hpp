#ifndef SCENE_PAGE_HPP
#define SCENE_PAGE_HPP

#include <Arduino.h>
#include <DeviceEntity.hpp>
#include <PageBase.hpp>
#include <RoomManager.hpp>

class ScenePage : public PageBase, DeviceEntityObserver, RoomManagerObserver {
public:
  void update();
  void show();
  void unshow();
  void processTouchEvent(uint8_t page, uint8_t component, bool pressed);

  void entityDeconstructCallback(DeviceEntity *);
  void entityUpdateCallback(DeviceEntity *);

  void roomChangedCallback();

private:
  void _updateDisplay();
  void _setRoomLabelText(const char *text);
};

#endif
