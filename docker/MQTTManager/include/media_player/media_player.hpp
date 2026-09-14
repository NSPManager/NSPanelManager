#ifndef MQTT_MANAGER_MEDIA_PLAYER_HPP
#define MQTT_MANAGER_MEDIA_PLAYER_HPP

#include "protobuf_nspanel.pb.h"
#include <cstdint>
#include <entity/entity.hpp>
#include <protobuf_nspanel_entity.pb.h>
#include <string>
#include <string_view>

class MediaPlayerEntity : public MqttManagerEntity {
public:
  MediaPlayerEntity(uint32_t media_player_id);

  /**
   * Update the config of the media player from DB.
   */
  virtual void reload_config();

  /**
   * Get the room ID of the media player.
   */
  uint16_t get_room_id();

  /**
   * Get the friendly name for the media player.
   */
  std::string get_name();

  /**
   * Send state update to NSPanel via MQTT as Protobuf object.
   */
  void send_state_update_to_nspanel();

  /**
   * Callback for NSPanelMQTTManagerCommand protobuf received from MQTT
   */
  void command_callback(NSPanelMQTTManagerCommand &command);

  /**
   * Start or resume playback.
   */
  virtual void play() = 0;

  /**
   * Pause playback.
   */
  virtual void pause() = 0;

  /**
   * Skip to the next track.
   */
  virtual void next_track() = 0;

  /**
   * Go back to the previous track.
   */
  virtual void previous_track() = 0;

  /**
   * Set the volume of the media player.
   * @param volume: Volume in 0-100%
   */
  virtual void set_volume(uint8_t volume) = 0;

  /**
   * Set the volume of the source currently feeding the media player.
   * @param volume: Volume in 0-100%
   */
  virtual void set_source_volume(uint8_t volume) = 0;

  /**
   * Mute or unmute the media player.
   */
  virtual void set_muted(bool muted) = 0;

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

  virtual ~MediaPlayerEntity();

protected:
  uint32_t _id;
  std::string _name;
  uint16_t _room_id;
  MQTT_MANAGER_ENTITY_CONTROLLER _controller;
  uint32_t _entity_page_id;
  uint8_t _entity_page_slot;

  NSPanelEntityState_MediaPlayer_PlaybackState _state = NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN;
  std::string _media_title;
  std::string _media_artist;
  uint8_t _volume = 0;
  bool _is_muted = false;
  bool _has_source_volume = false;
  uint8_t _source_volume = 0;

  bool _can_play = false;
  bool _can_pause = false;
  bool _can_next_track = false;
  bool _can_previous_track = false;
  bool _can_set_volume = false;
  bool _can_mute = false;

  NSPanelEntityState _last_media_player_state;
};

#endif // !MQTT_MANAGER_MEDIA_PLAYER_HPP
