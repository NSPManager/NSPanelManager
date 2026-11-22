#ifndef MQTT_MANAGER_HOME_ASSISTANT_SOURCE_VOLUME_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_SOURCE_VOLUME_HPP

#include "media_player/home_assistant_source_player.hpp"
#include "media_player/media_player.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

/**
 * Some setups (for example multi-room audio matrices) have a separate volume for the source
 * feeding a media player. How that source and its volume are exposed in Home Assistant differs
 * between integrations, a strategy selected per media player decides how to handle it.
 * The strategy also gives access to the sources of the source player.
 */
class HomeAssistantSourceVolumeStrategy {
public:
  virtual ~HomeAssistantSourceVolumeStrategy() = default;

  /**
   * Process new state attributes of the media player.
   */
  virtual void update_from_player_attributes(const nlohmann::json &attributes) = 0;

  /**
   * Get the current source volume in 0-100%, if available.
   */
  virtual std::optional<uint8_t> get_source_volume() = 0;

  /**
   * Set the volume of the source currently feeding the media player.
   * @param volume: Volume in 0-100%
   */
  virtual void set_source_volume(uint8_t volume) = 0;

  /**
   * Get the sources of the source player currently feeding the media player, or nothing if there is no source player.
   */
  virtual std::optional<MediaPlayerSources> get_source_player_sources() = 0;

  /**
   * Select a source of the source player currently feeding the media player.
   */
  virtual void select_source_player_source(const std::string &source) = 0;

  /**
   * Create the strategy selected for the media player.
   * @param media_player_id: ID of the media player, used for logging.
   * @param entity_data: The media player's entity_data from the DB.
   * @param on_change: Called when the source volume or sources change without the media player itself changing.
   */
  static std::unique_ptr<HomeAssistantSourceVolumeStrategy> create(uint32_t media_player_id, const nlohmann::json &entity_data, std::function<void()> on_change);
};

/**
 * No source volume, the media player only has its own volume.
 */
class NoSourceVolumeStrategy : public HomeAssistantSourceVolumeStrategy {
public:
  void update_from_player_attributes(const nlohmann::json &attributes);
  std::optional<uint8_t> get_source_volume();
  void set_source_volume(uint8_t volume);
  std::optional<MediaPlayerSources> get_source_player_sources();
  void select_source_player_source(const std::string &source);
};

/**
 * The media player reports both the source volume and the entity ID of the source as state attributes.
 * Source volume changes are sent to the source entity.
 */
class PlayerAttributesSourceVolumeStrategy : public HomeAssistantSourceVolumeStrategy {
public:
  PlayerAttributesSourceVolumeStrategy(std::string source_entity_attribute, std::string source_volume_attribute, std::function<void()> on_change);
  void update_from_player_attributes(const nlohmann::json &attributes);
  std::optional<uint8_t> get_source_volume();
  void set_source_volume(uint8_t volume);
  std::optional<MediaPlayerSources> get_source_player_sources();
  void select_source_player_source(const std::string &source);

private:
  std::string _source_volume_attribute;
  HomeAssistantSourcePlayer _source_player;

  std::mutex _mutex;
  std::optional<uint8_t> _source_volume;
};

/**
 * The media player reports the entity ID of the source as a state attribute. The source is itself a
 * media player in Home Assistant and its own volume is used as the source volume.
 */
class SourceEntitySourceVolumeStrategy : public HomeAssistantSourceVolumeStrategy {
public:
  SourceEntitySourceVolumeStrategy(std::string source_entity_attribute, std::function<void()> on_change);
  void update_from_player_attributes(const nlohmann::json &attributes);
  std::optional<uint8_t> get_source_volume();
  void set_source_volume(uint8_t volume);
  std::optional<MediaPlayerSources> get_source_player_sources();
  void select_source_player_source(const std::string &source);

private:
  HomeAssistantSourcePlayer _source_player;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_SOURCE_VOLUME_HPP
