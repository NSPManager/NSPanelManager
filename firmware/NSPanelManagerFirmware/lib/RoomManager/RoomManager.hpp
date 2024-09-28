#ifndef ROOMMANAGER_HPP
#define ROOMMANAGER_HPP

#include <Arduino.h>
#include <RoomManagerObserver.hpp>
#include <list>
#include <protobuf_nspanel.pb-c.h>
#include <vector>

class RoomManager {
public:
  static void init();
  static inline std::vector<NSPanelRoomStatus> rooms;
  static inline NSPanelRoomStatus *currentRoom;
  static uint16_t selected_entity_id;
  static void loadAllRooms(int32_t *room_ids, uint32_t n_room_ids);
  // static void loadRoom(uint16_t roomId, bool is_update);
  static void goToNextRoom();
  static void goToPreviousRoom();
  static bool goToRoomId(uint16_t id);
  static bool hasValidCurrentRoom();
  static void attachRoomChangeCallback(RoomManagerObserver *observer);
  static void detachRoomChangeCallback(RoomManagerObserver *observer);
  static void handleNSPanelRoomStatusUpdate(char *topic, byte *payload, unsigned int length);
  static NSPanelRoomStatus *getRoomById(uint16_t room_id);

private:
  static inline TaskHandle_t _notify_task_room_load_complete; // Notify this task whenever a room is loaded/updated.
  static inline std::list<RoomManagerObserver *> _roomChangeObservers;
  static inline unsigned long _lastReloadCommand;
  static void _callRoomChangeCallbacks();
};

#endif
