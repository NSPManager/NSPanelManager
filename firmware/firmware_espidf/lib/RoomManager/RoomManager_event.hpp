#pragma once

#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(ROOMMANAGER_EVENT);

enum roommanager_event_t {
  ROOM_SWITCHED,
  ROOM_LOADED,
  CURRENT_ROOM_UPDATED,
  ALL_ROOMS_LOADED,
};