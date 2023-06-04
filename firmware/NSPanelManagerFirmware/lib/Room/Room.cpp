#include <Light.hpp>
#include <Room.hpp>

std::list<Light *> Room::getCeilingLightsThatAreOn() {
  std::list<Light *> returnList;
  for (auto lightPair : this->ceilingLights) {
    if (lightPair.second->getLightLevel() > 0) {
      returnList.push_back(lightPair.second);
    }
  }
  return returnList;
}

std::list<Light *> Room::getTableLightsThatAreOn() {
  std::list<Light *> returnList;
  for (auto lightPair : this->tableLights) {
    if (lightPair.second->getLightLevel() > 0) {
      returnList.push_back(lightPair.second);
    }
  }
  return returnList;
}

std::list<Light *> Room::getAllLightsThatAreOn() {
  std::list<Light *> returnList;
  for (auto lightPair : this->tableLights) {
    if (lightPair.second->getLightLevel() > 0) {
      returnList.push_back(lightPair.second);
    }
  }
  for (auto lightPair : this->ceilingLights) {
    if (lightPair.second->getLightLevel() > 0) {
      returnList.push_back(lightPair.second);
    }
  }
  return returnList;
}

std::list<Light *> Room::getAllCeilingLights() {
  std::list<Light *> returnList;
  for (auto lightPair : this->ceilingLights) {
    returnList.push_back(lightPair.second);
  }
  return returnList;
}

std::list<Light *> Room::getAllTableLights() {
  std::list<Light *> returnList;
  for (auto lightPair : this->tableLights) {
    returnList.push_back(lightPair.second);
  }
  return returnList;
}

std::list<Light *> Room::getAllLights() {
  std::list<Light *> returnList;
  for (auto lightPair : this->tableLights) {
    returnList.push_back(lightPair.second);
  }
  for (auto lightPair : this->ceilingLights) {
    returnList.push_back(lightPair.second);
  }
  return returnList;
}

bool Room::anyCeilingLightsOn() {
  for (auto lightPair : this->ceilingLights) {
    if (lightPair.second->getLightLevel() > 0) {
      return true;
    }
  }
  return false;
}

bool Room::anyTableLightsOn() {
  for (auto lightPair : this->tableLights) {
    if (lightPair.second->getLightLevel() > 0) {
      return true;
    }
  }
  return false;
}

bool Room::anyLightsOn() {
  return this->anyCeilingLightsOn() || this->anyTableLightsOn();
}

std::list<Light *> Room::getAllRoomViewLights() {
  std::list<Light *> returnList;
  for (auto lightPair : this->ceilingLights) {
    if (lightPair.second->getRoomViewPosition() > 0) {
      returnList.push_back(lightPair.second);
    }
  }

  for (auto lightPair : this->tableLights) {
    if (lightPair.second->getRoomViewPosition() > 0) {
      returnList.push_back(lightPair.second);
    }
  }
  return returnList;
}

Light *Room::getLightAtRoomViewPosition(int room_view_position) {
  for (auto lightPair : this->ceilingLights) {
    if (lightPair.second->getRoomViewPosition() == room_view_position) {
      return lightPair.second;
    }
  }

  for (auto lightPair : this->tableLights) {
    if (lightPair.second->getRoomViewPosition() == room_view_position) {
      return lightPair.second;
    }
  }

  return nullptr;
}
