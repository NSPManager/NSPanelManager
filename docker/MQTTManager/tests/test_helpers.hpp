#pragma once

#include <database_manager/database_manager.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace nspm_test {

// Inserts an Entity row the way the Django web app writes it and removes it again when
// the fixture goes out of scope, so each test starts from the rows it creates itself.
class ScopedEntity {
public:
  ScopedEntity(const std::string &entity_type, const std::string &friendly_name, const nlohmann::json &entity_data, int room_id = 1, int entities_page_id = 1, int room_view_position = 0) {
    database_manager::Entity entity;
    entity.entity_type = entity_type;
    entity.friendly_name = friendly_name;
    entity.room_id = room_id;
    entity.entities_page_id = entities_page_id;
    entity.room_view_position = room_view_position;
    entity.entity_data = entity_data.dump();
    id = database_manager::database.insert(entity);
  }

  ~ScopedEntity() {
    database_manager::database.remove<database_manager::Entity>(id);
  }

  ScopedEntity(const ScopedEntity &) = delete;
  ScopedEntity &operator=(const ScopedEntity &) = delete;

  int id;
};

// entity_data for a light exactly as web/rest.py put_light_entity() stores it.
inline nlohmann::json django_light_data(const std::string &controller = "home_assistant") {
  return {
      {"controller", controller},
      {"home_assistant_name", controller == "home_assistant" ? "light.kitchen_ceiling" : ""},
      {"openhab_control_mode", "dimmer"},
      {"openhab_item_dimmer", controller == "openhab" ? "Kitchen_Dimmer" : ""},
      {"openhab_item_color_temp", ""},
      {"openhab_item_rgb", ""},
      {"can_dim", true},
      {"can_color_temperature", true},
      {"can_rgb", false},
      {"is_ceiling_light", true},
      {"controlled_by_nspanel_main_page", true},
  };
}

} // namespace nspm_test
