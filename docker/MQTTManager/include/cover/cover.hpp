#ifndef MQTT_MANAGER_COVER_HPP
#define MQTT_MANAGER_COVER_HPP

#include "protobuf_nspanel.pb.h"
#include <cstdint>
#include <entity/entity.hpp>
#include <protobuf_nspanel_entity.pb.h>
#include <string>
#include <string_view>

class CoverEntity : public MqttManagerEntity {
public:
  CoverEntity(uint32_t cover_id);

  /**
   * Update the common config of the cover from the DB.
   */
  virtual void reload_config();

  /**
   * Get the room ID of the cover.
   */
  uint16_t get_room_id();

  /**
   * Get the friendly name of the cover.
   */
  std::string get_name();

  /**
   * Send the current state to the NSPanels via MQTT as a retained protobuf message.
   */
  void send_state_update_to_nspanel();

  /**
   * Callback for NSPanelMQTTManagerCommand protobuf received from MQTT.
   */
  void command_callback(NSPanelMQTTManagerCommand &command);

  /**
   * Open the cover.
   */
  virtual void open() = 0;

  /**
   * Close the cover.
   */
  virtual void close() = 0;

  /**
   * Stop the cover.
   */
  virtual void stop() = 0;

  /**
   * Move the cover to a position.
   * @param position: 0 = closed, 100 = open.
   */
  virtual void set_position(uint8_t position) = 0;

  virtual void open_tilt() = 0;
  virtual void close_tilt() = 0;
  virtual void stop_tilt() = 0;
  virtual void set_tilt_position(uint8_t position) = 0;

  uint16_t get_id();
  MQTT_MANAGER_ENTITY_TYPE get_type();
  MQTT_MANAGER_ENTITY_CONTROLLER get_controller();
  uint32_t get_entity_page_id();
  uint8_t get_entity_page_slot();
  bool can_toggle();
  void toggle();
  std::string_view get_icon();
  uint16_t get_icon_color();
  uint16_t get_icon_active_color();
  std::string get_mqtt_state_topic();

  // Current state, for tests and for the controller implementations.
  NSPanelEntityState_Cover_CoverState get_state();
  bool has_position();
  uint8_t get_position();
  bool has_tilt_position();
  uint8_t get_tilt_position();
  bool can_open();
  bool can_close();
  bool can_stop();
  bool can_set_position();
  std::string get_device_class();

  virtual ~CoverEntity();

protected:
  /**
   * Read the common fields from the database row.
   */
  void _load_common_config();

  uint32_t _id;
  std::string _name;
  uint16_t _room_id;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;
  uint32_t _entity_page_id;
  uint8_t _entity_page_slot;

  NSPanelEntityState_Cover_CoverState _state = NSPanelEntityState_Cover_CoverState_UNKNOWN;
  bool _has_position = false;
  uint8_t _position = 0;
  bool _has_tilt_position = false;
  uint8_t _tilt_position = 0;

  bool _can_open = false;
  bool _can_close = false;
  bool _can_stop = false;
  bool _can_set_position = false;
  bool _can_open_tilt = false;
  bool _can_close_tilt = false;
  bool _can_stop_tilt = false;
  bool _can_set_tilt_position = false;

  std::string _device_class;

  NSPanelEntityState _last_cover_state;
};

#endif // !MQTT_MANAGER_COVER_HPP
