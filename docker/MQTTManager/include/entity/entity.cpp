#include <boost/bind.hpp>
#include <entity/entity.hpp>

void MqttManagerEntity::_signal_entity_changed() {
  this->_entity_changed_callbacks(this);
}

void MqttManagerEntity::_signal_entity_destroyed() {
  this->_entity_destroy_callback(this);
}
