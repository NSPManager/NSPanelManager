#include "room/room.hpp"
#ifndef HOME_ASSISTANT_SCENE

#include "scenes/scene.hpp"

class HomeAssistantScene : public Scene {
public:
  HomeAssistantScene(uint32_t id);
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

private:
  bool _is_global_scene;
  uint16_t _id;
  std::string _name;
  std::string _entity_id;
  uint16_t _room_id;
  std::shared_ptr<Room> _room;
};

#endif // !HOME_ASSISTANT_SCENE
