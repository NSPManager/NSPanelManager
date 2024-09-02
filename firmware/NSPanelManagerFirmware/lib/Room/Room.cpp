#include <Room.hpp>
#include <Scene.hpp>

Room::Room(int32_t id) {
  this->id = id;
}

std::list<NSPanelLightStatus *> Room::getAllLights() {
  std::list<NSPanelLightStatus *> returnList;
  for (int i = 0; i < this->status.n_lights; i++) {
    returnList.push_back(this->status.lights[i]);
  }
  return returnList;
}

bool Room::anyLightsOn() {
  for (int i = 0; i < this->status.n_lights; i++) {
    if (this->status.lights[i]->light_level > 0) {
      return true;
    }
  }
  return false;
}

NSPanelLightStatus *Room::getLightAtRoomViewPosition(int room_view_position) {
  for (int i = 0; i < this->status.n_lights; i++) {
    if (this->status.lights[i]->room_view_position == room_view_position) {
      return this->status.lights[i];
    }
  }

  return nullptr;
}

Scene *Room::getSceneById(uint16_t id) {
  for (auto scene : this->scenes) {
    if (scene->id == id) {
      return scene;
    }
  }
  return nullptr;
}
