#ifndef OPENHAB_SCENE
#include "room/room.hpp"

#include "scenes/scene.hpp"

class OpenhabScene : public Scene {
public:
  OpenhabScene(uint32_t id);
  void reload_config();
  void activate();
  void save();
  void remove();
  uint16_t get_id();
  std::string get_name();
  bool can_save();
  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();

private:
  bool _is_global_scene;
  uint16_t _id;
  std::string _name;
  std::string _entity_id;
  uint16_t _room_id;
};

#endif // !OPENHAB_SCENE
