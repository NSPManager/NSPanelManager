#ifndef ROOMMANAGER_HPP
#define ROOMMANAGER_HPP

#include <Arduino.h>
#include <Room.hpp>
#include <vector>

class RoomManager {
public:
  static inline std::vector<Room *> rooms;
  static inline std::vector<Room *>::iterator currentRoom;
  static void loadAllRooms();
  static Room *loadRoom(uint16_t roomId);
  static void goToNextRoom();
  static void goToPreviousRoom();
  static void goToRoomId(uint16_t id);
};

#endif
