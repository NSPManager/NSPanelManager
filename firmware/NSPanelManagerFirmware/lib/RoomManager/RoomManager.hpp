#ifndef ROOMMANAGER_HPP
#define ROOMMANAGER_HPP

#include <Arduino.h>
class Room;
#include <list>
#include <vector>

class RoomManagerObserver {
public:
  virtual void roomChangedCallback() = 0;
};

class RoomManager {
public:
  static inline std::vector<Room *> rooms;
  static inline std::vector<Room *>::iterator currentRoom;
  static void loadAllRooms();
  static Room *loadRoom(uint16_t roomId);
  static void goToNextRoom();
  static void goToPreviousRoom();
  static void goToRoomId(uint16_t id);
  static void attachRoomChangeCallback(RoomManagerObserver *observer);
  static void detachRoomChangeCallback(RoomManagerObserver *observer);

private:
  static inline std::list<RoomManagerObserver *> _roomChangeObservers;
  static void _callRoomChangeCallbacks();
};

#endif
