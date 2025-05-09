#include <InterfaceConfig.hpp>

PROTOBUF_NSPANEL_SCENE *InterfaceConfig::getSceneById(uint16_t id) {
  for (PROTOBUF_NSPANEL_SCENE *scene : InterfaceConfig::global_scenes) {
    if (scene->scene_id() == id) {
      return scene;
    }
  }
  return nullptr;
}
