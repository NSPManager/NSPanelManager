#include <InterfaceManagerHelpers.h>

std::list<lightConfig *> roomConfig::getCeilingLightsThatAreOn() {
  std::list<lightConfig *> returnList;
  for (lightConfig &light : this->ceilingLights) {
    if (light.level > 0) {
      returnList.push_back(&light);
    }
  }
  return returnList;
}

std::list<lightConfig *> roomConfig::getTableLightsThatAreOn() {
  std::list<lightConfig *> returnList;
  for (lightConfig &light : this->tableLights) {
    if (light.level > 0) {
      returnList.push_back(&light);
    }
  }
  return returnList;
}

std::list<lightConfig *> roomConfig::getAllLightsThatAreOn() {
  std::list<lightConfig *> returnList;
  for (lightConfig &light : this->tableLights) {
    if (light.level > 0) {
      returnList.push_back(&light);
    }
  }
  for (lightConfig &light : this->ceilingLights) {
    if (light.level > 0) {
      returnList.push_back(&light);
    }
  }
  return returnList;
}

std::list<lightConfig *> roomConfig::getAllCeilingLights() {
  std::list<lightConfig *> returnList;
  for (lightConfig &cfg : this->ceilingLights) {
    returnList.push_back(&cfg);
  }
  return returnList;
}

std::list<lightConfig *> roomConfig::getAllTableLights() {
  std::list<lightConfig *> returnList;
  for (lightConfig &cfg : this->tableLights) {
    returnList.push_back(&cfg);
  }
  return returnList;
}

std::list<lightConfig *> roomConfig::getAllLights() {
  std::list<lightConfig *> returnList;
  for (lightConfig &cfg : this->tableLights) {
    returnList.push_back(&cfg);
  }
  for (lightConfig &cfg : this->ceilingLights) {
    returnList.push_back(&cfg);
  }
  return returnList;
}

bool roomConfig::anyCeilingLightsOn() {
  for (lightConfig &cfg : this->ceilingLights) {
    if (cfg.level > 0) {
      return true;
    }
  }
  return false;
}

bool roomConfig::anyTableLightstOn() {
  for (lightConfig &cfg : this->tableLights) {
    if (cfg.level > 0) {
      return true;
    }
  }
  return false;
}

bool roomConfig::anyLightsOn() {
  return this->anyCeilingLightsOn() || this->anyTableLightstOn();
}

std::list<lightConfig *> roomConfig::getAllRoomViewLights() {
  std::list<lightConfig *> returnList;
  for (lightConfig &light : this->ceilingLights) {
    if (light.roomViewPosition > 0) {
      returnList.push_back(&light);
    }
  }

  for (lightConfig &light : this->tableLights) {
    if (light.roomViewPosition > 0) {
      returnList.push_back(&light);
    }
  }
  return returnList;
}

lightConfig *roomConfig::getLightAtRoomViewPosition(int room_view_position) {
  for (lightConfig &light : this->ceilingLights) {
    if (light.roomViewPosition == room_view_position) {
      return &light;
    }
  }

  for (lightConfig &light : this->tableLights) {
    if (light.roomViewPosition == room_view_position) {
      return &light;
    }
  }

  return nullptr;
}

std::list<lightConfig *> interfaceConfig::getCeilingLightsThatAreOn() {
  std::list<lightConfig *> returnList;
  for (roomConfig &room : this->rooms) {
    std::list<lightConfig *> lights = room.getCeilingLightsThatAreOn();
    returnList.insert(returnList.end(), lights.begin(), lights.end());
  }
  return returnList;
}

std::list<lightConfig *> interfaceConfig::getTableLightsThatAreOn() {
  std::list<lightConfig *> returnList;
  for (roomConfig &room : this->rooms) {
    std::list<lightConfig *> lights = room.getTableLightsThatAreOn();
    returnList.insert(returnList.end(), lights.begin(), lights.end());
  }
  return returnList;
}

std::list<lightConfig *> interfaceConfig::getAllLightsThatAreOn() {
  std::list<lightConfig *> returnList;
  std::list<lightConfig *> ceilingLightsOn = this->getCeilingLightsThatAreOn();
  std::list<lightConfig *> tableLightsOn = this->getTableLightsThatAreOn();
  returnList.insert(returnList.end(), ceilingLightsOn.begin(), ceilingLightsOn.end());
  returnList.insert(returnList.end(), tableLightsOn.begin(), tableLightsOn.end());
  return returnList;
}

lightConfig *interfaceConfig::getLightById(uint16_t id) {
  for (roomConfig &room : this->rooms) {
    for (lightConfig &light : room.ceilingLights) {
      if (light.id == id) {
        return &light;
      }
    }
    for (lightConfig &light : room.tableLights) {
      if (light.id == id) {
        return &light;
      }
    }
  }
  return nullptr;
}

std::list<lightConfig *> interfaceConfig::getAllCeilingLights() {
  std::list<lightConfig *> returnList;
  for (roomConfig &room : this->rooms) {
    std::list<lightConfig *> lights = room.getAllCeilingLights();
    returnList.insert(returnList.end(), lights.begin(), lights.end());
  }
  return returnList;
}

std::list<lightConfig *> interfaceConfig::getAllTableLights() {
  std::list<lightConfig *> returnList;
  for (roomConfig &room : this->rooms) {
    std::list<lightConfig *> lights = room.getAllTableLights();
    returnList.insert(returnList.end(), lights.begin(), lights.end());
  }
  return returnList;
}

std::list<lightConfig *> interfaceConfig::getAllLights() {
  std::list<lightConfig *> returnList;
  std::list<lightConfig *> ceilingLightsOn = this->getAllCeilingLights();
  std::list<lightConfig *> tableLightsOn = this->getAllTableLights();
  returnList.insert(returnList.end(), ceilingLightsOn.begin(), ceilingLightsOn.end());
  returnList.insert(returnList.end(), tableLightsOn.begin(), tableLightsOn.end());
  return returnList;
}

bool interfaceConfig::anyCeilingLightsOn() {
  for (roomConfig &room : this->rooms) {
    if (room.anyCeilingLightsOn()) {
      return true;
    }
  }
  return false;
}

bool interfaceConfig::anyTableLightstOn() {
  for (roomConfig &room : this->rooms) {
    if (room.anyTableLightstOn()) {
      return true;
    }
  }
  return false;
}

bool interfaceConfig::anyLightsOn() {
  return this->anyCeilingLightsOn() || this->anyTableLightstOn();
}
