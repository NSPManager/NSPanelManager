#include <InterfaceConfig.hpp>

NSPanelScene *InterfaceConfig::getSceneById(uint16_t id) {
  for (NSPanelScene scene : InterfaceConfig::global_scenes) {
    if (scene.scene_id == id) {
      return &scene;
    }
  }
  return nullptr;
}
