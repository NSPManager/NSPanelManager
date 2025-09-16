#ifndef MQTT_MANAGER_THERMOSTAT_HPP
#define MQTT_MANAGER_THERMOSTAT_HPP

#include "protobuf_nspanel.pb.h"
#include <boost/signals2.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <nlohmann/json.hpp>
#include <protobuf_nspanel_entity.pb.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

class ThermostatEntity : public MqttManagerEntity {
public:
  ThermostatEntity(uint32_t thermostat_id);

  /**
   * Update the config of the switch from DB.
   */
  void reload_config();

  /**
   * Get the room ID of the switch.
   */
  uint16_t get_room_id();

  /**
   * Set the thermostat mode
   */
  void set_mode(std::string mode);

  /**
   * Get the current mode of the thermostat.
   */
  std::string get_mode();

  /**
   * Get all supported modes.
   */
  std::vector<std::string> get_supported_modes();

  /**
   * Set the thermostat temperature
   */
  void set_temperature(float temperature);

  /**
   * Get the current temperature of the thermostat.
   */
  float get_temperature();

  /**
   * Set the thermostat fan mode
   */
  void set_fan_mode(std::string fan_mode);

  /**
   * Get the thermostat fan mode
   */
  std::string get_fan_mode();

  /**
   * Get all supported fan modes.
   */
  std::vector<std::string> get_supported_fan_modes();

  /**
   * Set the thermostat preset
   */
  void set_preset(std::string preset);

  /**
   * Get the thermostat preset
   */
  std::string get_preset();

  /**
   * Get all supported presets.
   */
  std::vector<std::string> get_supported_presets();

  /**
   * Set the thermostat swing mode
   */
  void set_swing_mode(std::string swing_mode);

  /**
   * Get the thermostat swing mode
   */
  std::string get_swing_mode();

  /**
   * Get all supported swing modes.
   */
  std::vector<std::string> get_supported_swing_modes();

  /**
   * Send state update to NSPanel via MQTT as Protobuf object.
   */
  void send_state_update_to_nspanel();

  /**
   * Get the ID of the thermostat.
   */
  uint16_t get_id();

  /**
   * Get the friendly name for the switch.
   */
  std::string get_name();

  /**
   * Get the ID of the entity page this entity is placed on.
   */
  uint32_t get_entity_page_id();

  /**
   * Get the slot in which this entity is placed on the given entity page.
   */
  uint8_t get_entity_page_slot();

  /**
   * Attach a callback for when the thermostat is destroyed
   */
  void attach_delete_callback(void (*callback)(ThermostatEntity *thermostat_entity));

  /**
   * Detach a callback for when the thermostat is destroyed
   */
  void detach_delete_callback(void (*callback)(ThermostatEntity *thermostat_entity));

  /**
   * Compare requested and current values and change commands to controller to set the values as per request.
   */
  virtual void send_state_update_to_controller() = 0;

  /**
   * Set all requested values equal to current values.
   */
  void reset_requests();

  /**
   * Callback for NSPanelMQTTManagerCommand protobuf received from MQTT
   */
  void command_callback(NSPanelMQTTManagerCommand &command);

  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();
  bool can_toggle();
  void toggle();
  std::string_view get_icon();
  uint16_t get_icon_color();
  uint16_t get_icon_active_color();
  std::string get_mqtt_state_topic();

  ~ThermostatEntity();

protected:
  uint _id;
  std::string _name;
  uint16_t _room_id;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;
  uint32_t _entity_page_id;
  uint8_t _entity_page_slot;

  std::vector<std::string> _supported_modes;
  std::vector<std::string> _supported_swing_modes;
  std::vector<std::string> _supported_fan_modes;
  std::vector<std::string> _supported_presets;

  std::string _current_mode;
  std::string _current_swing_mode;
  std::string _current_fan_mode;
  std::string _current_preset;
  float _current_temperature;
  NSPanelEntityState _last_thermostat_state;

  std::string _requested_mode;
  std::string _requested_swing_mode;
  std::string _requested_fan_mode;
  std::string _requested_preset;
  float _requested_temperature;

  bool _current_state;
  bool _requested_state;

  boost::signals2::signal<void(ThermostatEntity *)> _thermostat_destroyed_callbacks;
};

#endif // !MQTT_MANAGER_SWITCH
