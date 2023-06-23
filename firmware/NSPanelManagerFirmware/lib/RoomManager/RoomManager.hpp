#ifndef ROOMMANAGER_HPP
#define ROOMMANAGER_HPP

#include <Arduino.h>
class Room;
#include <list>

class RoomManagerObserver {
public:
  virtual void roomChangedCallback() = 0;
};

class RoomManager {
public:
  static void init();
  static void reloadCallback(char *topic, byte *payload, unsigned int length);
  static inline std::list<Room *> rooms;
  static inline std::list<Room *>::iterator currentRoom;
  static void loadAllRooms(bool is_update);
  static Room *loadRoom(uint16_t roomId, bool is_update);
  static void goToNextRoom();
  static void goToPreviousRoom();
  static void goToRoomId(uint16_t id);
  static void attachRoomChangeCallback(RoomManagerObserver *observer);
  static void detachRoomChangeCallback(RoomManagerObserver *observer);
  static Room *getRoomById(uint16_t room_id);

private:
  static inline std::list<RoomManagerObserver *> _roomChangeObservers;
  static void _callRoomChangeCallbacks();
};

#endif
