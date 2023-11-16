#include <boost/bind.hpp>
#include <entity/entity.hpp>

void MqttManagerEntity::signal_entity_changed() {
  this->_entity_changed_callbacks(this);
}
