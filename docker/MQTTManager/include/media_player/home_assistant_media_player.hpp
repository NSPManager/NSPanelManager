#ifndef MQTT_MANAGER_HOME_ASSISTANT_MEDIA_PLAYER_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_MEDIA_PLAYER_HPP

#include "media_player.hpp"
#include "media_player/home_assistant_source_volume.hpp"
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>

class HomeAssistantMediaPlayer : public MediaPlayerEntity {
public:
  HomeAssistantMediaPlayer(uint32_t media_player_id);
  ~HomeAssistantMediaPlayer();
  void reload_config();
  void home_assistant_event_callback(nlohmann::json event_data);

  void play();
  void pause();
  void next_track();
  void previous_track();
  void set_volume(uint8_t volume);
  void set_source_volume(uint8_t volume);
  void set_muted(bool muted);

private:
  /**
   * Load Home Assistant specific settings from entity_data in the DB.
   */
  void _load_home_assistant_config();

  /**
   * Create the source volume strategy selected in the global settings.
   */
  void _create_source_volume_strategy();

  /**
   * Get the current source volume strategy.
   */
  std::shared_ptr<HomeAssistantSourceVolumeStrategy> _get_source_volume_strategy();

  /**
   * Update source volume from the strategy and send the new state to the NSPanel.
   */
  void _update_source_volume_and_send_state();

  /**
   * Call a "media_player" domain service in Home Assistant.
   */
  void _call_service(const std::string &service, nlohmann::json service_data = nlohmann::json::object());

  std::string _home_assistant_name;

  std::mutex _source_volume_strategy_mutex;
  std::shared_ptr<HomeAssistantSourceVolumeStrategy> _source_volume_strategy;

  // Media player feature flags from Home Assistant (MediaPlayerEntityFeature).
  static constexpr uint32_t feature_pause = 1;
  static constexpr uint32_t feature_volume_set = 4;
  static constexpr uint32_t feature_volume_mute = 8;
  static constexpr uint32_t feature_previous_track = 16;
  static constexpr uint32_t feature_next_track = 32;
  static constexpr uint32_t feature_play = 16384;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_MEDIA_PLAYER_HPP
