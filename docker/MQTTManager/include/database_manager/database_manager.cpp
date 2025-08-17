#include <database_manager/database_manager.hpp>
#include <nlohmann/json.hpp>

// This file is strictly here to make CMake understand that this is a C++ library
void database_manager::Entity::set_entity_data_json(const nlohmann::json &json) {
  this->entity_data = json.dump();
  database_manager::database.update(*this);
}

nlohmann::json database_manager::Entity::get_entity_data_json() const {
  return nlohmann::json::parse(entity_data);
}
