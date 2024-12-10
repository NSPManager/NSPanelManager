#ifndef ROOMMANAGER_HPP
#define ROOMMANAGER_HPP

#include <Arduino.h>
#include <RoomManagerObserver.hpp>
#include <list>
#include <protobuf_defines.h>
#include <protobuf_nspanel.h>

class RoomManager {
public:
  static void init();
  static inline std::list<PROTOBUF_NSPANEL_ROOM_STATUS *> rooms;
  static inline PROTOBUF_NSPANEL_ROOM_STATUS *currentRoom;
  static uint16_t selected_entity_id;
  static void loadAllRooms();
  // static void loadRoom(uint16_t roomId, bool is_update);
  static void goToNextRoom();
  static void goToPreviousRoom();
  static bool goToRoomId(uint16_t id);
  static bool hasValidCurrentRoom();
  static void attachRoomChangeCallback(RoomManagerObserver *observer);
  static void detachRoomChangeCallback(RoomManagerObserver *observer);
  static void handleNSPanelRoomStatusUpdate(MQTTMessage *message);
  static PROTOBUF_NSPANEL_ROOM_STATUS *getRoomById(uint16_t room_id);

private:
  static inline TaskHandle_t _notify_task_room_load_complete; // Notify this task whenever a room is loaded/updated.
  static inline std::list<RoomManagerObserver *> _roomChangeObservers;
  static inline unsigned long _lastReloadCommand;
  static void _callRoomChangeCallbacks();

  static inline SemaphoreHandle_t _room_status_decode_buffer_mutex = NULL; // Lock usage of RoomStatus decode buffer to one task at the time.
  static inline PROTOBUF_NSPANEL_ROOM_STATUS _room_status_decode_buffer;
};

#endif
