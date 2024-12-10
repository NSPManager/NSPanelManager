#pragma once

#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(NSPM_CONFIGMANAGER_EVENT);

enum nspm_configmanager_event {
  CONFIG_LOADED,
};