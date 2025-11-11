#ifndef MQTT_MANAGER_HOMEY_SCENE
#define MQTT_MANAGER_HOMEY_SCENE

#include "scene.hpp"
#include <string>

enum HOMEY_SCENE_TYPE
{
    HOMEY_FLOW,
    HOMEY_MOOD
};

class HomeyScene : public Scene
{
public:
    HomeyScene(uint32_t scene_id);
    ~HomeyScene();
    void activate();

private:
    std::string _homey_id;
    HOMEY_SCENE_TYPE _homey_scene_type;
};

#endif // !MQTT_MANAGER_HOMEY_SCENE
