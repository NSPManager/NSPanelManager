#pragma once
#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(PAGEMANAGER_EVENT);

enum pagemanager_event_t {
  PAGE_SWITCHED,
};