#include "room/room.hpp"
#ifndef OPENHAB_SCENE

#include "scenes/scene.hpp"

class OpenhabScene : public Scene {
public:
  OpenhabScene(nlohmann::json &data);
  void update_config(nlohmann::json &config);
  void activate();
  void save();
  void remove();
  uint16_t get_id();
  void post_init();
  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();

private:
  bool _is_global_scene;
  uint16_t _id;
  std::string _name;
  std::string _entity_id;
  uint16_t _room_id;
  Room *_room;
};

#endif // !OPENHAB_SCENE
