#include <InterfaceConfig.hpp>
#include <Scene.hpp>

Scene *InterfaceConfig::getSceneById(uint16_t id) {
  for (Scene *scene : InterfaceConfig::global_scenes) {
    if (scene->id == id) {
      return scene;
    }
  }
  return nullptr;
}
