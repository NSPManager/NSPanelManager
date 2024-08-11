#ifndef MQTT_MANAGER_SCENE_BASE_H
#define MQTT_MANAGER_SCENE_BASE_H
#include "entity/entity.hpp"

class Scene : public MqttManagerEntity {
public:
  virtual void activate() = 0;
  virtual void save() = 0;
  virtual void remove() = 0;
  virtual void update_config(nlohmann::json &config) = 0;
};

#endif // !MQTT_MANAGER_SCENE_BASE_H
