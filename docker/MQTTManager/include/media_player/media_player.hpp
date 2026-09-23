#ifndef MQTT_MANAGER_MEDIA_PLAYER_HPP
#define MQTT_MANAGER_MEDIA_PLAYER_HPP

#include "protobuf_nspanel.pb.h"
#include <cstdint>
#include <entity/entity.hpp>
#include <memory>
#include <mutex>
#include <optional>
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

  /**
   * Get the current album art as the original image data (for example a JPEG) from the controller.
   * The image is downloaded on first request and cached until the album art changes. The same pointer is
   * returned for as long as the album art stays the same, so it can be used to tell when the art has changed.
   * @return The image data or nullptr if there is no album art or it could not be downloaded.
   */
  std::shared_ptr<const std::string> get_album_art();

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
  /**
   * Set where the album art is fetched from on the controller. An empty source means there is no album art.
   * The album art URL sent to the NSPanel changes whenever the source changes. It is sent with the next state update.
   */
  void _set_album_art_source(const std::string &source);

  /**
   * Download album art from the controller.
   * @param source: The album art source as given to _set_album_art_source.
   * @param image_data: Where to store the downloaded image data.
   * @return true on success.
   */
  virtual bool _download_album_art(const std::string &source, std::string &image_data) = 0;

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

private:
  /**
   * Get the URL the NSPanel should fetch album art from, or an empty string if there is none.
   */
  std::string _get_album_art_url();

  // Guards _album_art_source. Only held briefly, never while downloading.
  std::mutex _album_art_source_mutex;
  std::string _album_art_source;

  // Held while checking the cache and downloading so that panels requesting the same art
  // at the same time share one download instead of each fetching it.
  std::mutex _album_art_download_mutex;
  std::string _album_art_cache_source;
  std::shared_ptr<const std::string> _album_art_cache;
};

#endif // !MQTT_MANAGER_MEDIA_PLAYER_HPP
