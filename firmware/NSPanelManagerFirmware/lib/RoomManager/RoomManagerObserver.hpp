#ifndef ROOM_MANAGER_OBSERVER_HPP
#define ROOM_MANAGER_OBSERVER_HPP

class RoomManagerObserver {
public:
  virtual void roomChangedCallback() = 0;
};

#endif
