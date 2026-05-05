#ifndef MQTT_MANAGER_SCENE_BASE_H
#define MQTT_MANAGER_SCENE_BASE_H
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include <expected>

class Scene : public MqttManagerEntity {
public:
  virtual void activate(std::expected<int32_t, EntityManager::EntityError> triggering_room_id = std::unexpected(EntityManager::EntityError::NOT_FOUND)) = 0;
  virtual void save() = 0;
  virtual void remove() = 0;
  virtual void reload_config() = 0;

  uint32_t get_entity_page_id();
  uint8_t get_entity_page_slot();

  bool can_toggle();

  bool is_global();

  // The same as activate()
  void toggle() override;

  virtual std::string get_name() = 0;
  virtual bool can_save() = 0;

  std::string get_mqtt_state_topic();

protected:
  uint32_t _page_id;
  uint8_t _page_slot;
  bool _is_global;

private:
};

#endif // !MQTT_MANAGER_SCENE_BASE_H
