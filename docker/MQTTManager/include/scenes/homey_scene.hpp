#include "room/room.hpp"
#ifndef MQTT_MANAGER_HOMEY_SCENE

#include "scenes/scene.hpp"
#include <string>
#include <string_view>

enum HOMEY_SCENE_TYPE
{
    HOMEY_FLOW,
    HOMEY_MOOD
};

class HomeyScene : public Scene
{
public:
    HomeyScene(uint32_t id);
    void reload_config() override;
    void activate() override;
    void save() override;
    void remove() override;
    uint16_t get_id() override;
    void post_init();
    std::string get_name() override;
    bool can_save() override;
    MQTT_MANAGER_ENTITY_TYPE get_type() override;
    MQTT_MANAGER_ENTITY_CONTROLLER get_controller() override;
    std::string_view get_icon() override;
    uint16_t get_icon_color() override;
    uint16_t get_icon_active_color() override;

private:
    uint16_t _id;
    std::string _name;
    std::string _entity_id;
    std::string _homey_id;
    uint16_t _room_id;
    std::shared_ptr<Room> _room;
    HOMEY_SCENE_TYPE _homey_scene_type;
};

#endif // !MQTT_MANAGER_HOMEY_SCENE
