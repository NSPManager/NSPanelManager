#ifndef MQTT_MANAGER_HOME_ASSISTANT_SOURCE_PLAYER_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_SOURCE_PLAYER_HPP

#include "media_player/media_player.hpp"
#include <cstdint>
#include <functional>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

/**
 * Follows the source player feeding a media player in Home Assistant. The media player reports the
 * entity ID of its current source player in a state attribute, the source player is itself a media
 * player in Home Assistant.
 */
class HomeAssistantSourcePlayer {
public:
  /**
   * @param source_entity_attribute: Media player state attribute holding the entity ID of the source player.
   * @param on_change: Called when the source player changes state without the media player itself changing.
   */
  HomeAssistantSourcePlayer(std::string source_entity_attribute, std::function<void()> on_change);
  ~HomeAssistantSourcePlayer();

  /**
   * Process new state attributes of the media player. Follows the source player when it changes.
   */
  void update_from_player_attributes(const nlohmann::json &attributes);

  /**
   * Get the entity ID of the current source player, or an empty string if there is none.
   */
  std::string get_entity_id();

  /**
   * Get the volume of the source player in 0-100%, if known.
   */
  std::optional<uint8_t> get_volume();

  /**
   * Get the sources of the source player, or nothing if there is no source player.
   */
  std::optional<MediaPlayerSources> get_sources();

  /**
   * Set the volume of the source player.
   * @param volume: Volume in 0-100%
   */
  void set_volume(uint8_t volume);

  /**
   * Select a source of the source player.
   */
  void select_source(const std::string &source);

  void home_assistant_event_callback(nlohmann::json event_data);

  /**
   * Read the sources of a media player from its Home Assistant state attributes.
   */
  static MediaPlayerSources sources_from_attributes(const nlohmann::json &attributes);

  /**
   * Convert a Home Assistant volume level (0.0-1.0) to 0-100%.
   * Home Assistant always reports volume_level as 0.0-1.0, whatever the device uses. 0-100 is only used towards the NSPanel.
   */
  static std::optional<uint8_t> volume_from_json(const nlohmann::json &volume_level);

  /**
   * Call a "media_player" domain service in Home Assistant on the given entity.
   */
  static void call_service(const std::string &entity_id, const std::string &service, nlohmann::json service_data = nlohmann::json::object());

  // Media player feature flag from Home Assistant (MediaPlayerEntityFeature).
  static constexpr uint32_t feature_select_source = 2048;

private:
  std::string _source_entity_attribute;
  std::function<void()> _on_change;

  std::mutex _mutex;
  std::string _entity_id;
  std::optional<uint8_t> _volume;
  MediaPlayerSources _sources;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_SOURCE_PLAYER_HPP
