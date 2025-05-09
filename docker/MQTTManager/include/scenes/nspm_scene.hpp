#ifndef MQTT_MANAGER_NSPM_SCENE_H
#define MQTT_MANAGER_NSPM_SCENE_H

#include "entity/entity.hpp"
#include "light/light.hpp"
#include "protobuf_nspanel.pb.h"
#include <list>
#include <nlohmann/json.hpp>
#include <scenes/scene.hpp>
#include <string>

class NSPMScene : public Scene {
public:
  NSPMScene(uint32_t id);
  ~NSPMScene();

  void reload_config();
  void activate();
  void save();
  void remove();
  uint16_t get_id();
  void post_init();
  std::string get_name();
  bool can_save();
  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();
  std::string_view get_icon();
  uint16_t get_icon_color();
  uint16_t get_icon_active_color();

  void command_callback(NSPanelMQTTManagerCommand &command);

private:
  uint16_t _id;
  std::string _name;
  uint16_t _room_id;
};

#endif // !MQTT_MANAGER_NSPM_SCENE_H
