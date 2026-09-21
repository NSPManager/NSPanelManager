#ifndef MQTT_MANAGER_HOME_ASSISTANT_SOURCE_VOLUME_HPP
#define MQTT_MANAGER_HOME_ASSISTANT_SOURCE_VOLUME_HPP

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
   * Create the strategy selected for the media player.
   * @param media_player_id: ID of the media player, used for logging.
   * @param entity_data: The media player's entity_data from the DB.
   * @param on_change: Called when the source volume changes without the media player itself changing.
   */
  static std::unique_ptr<HomeAssistantSourceVolumeStrategy> create(uint32_t media_player_id, const nlohmann::json &entity_data, std::function<void()> on_change);

protected:
  /**
   * Convert a Home Assistant volume level (0.0-1.0) to 0-100%.
   */
  static std::optional<uint8_t> _volume_from_json(const nlohmann::json &volume_level);

  /**
   * Set the volume of the given Home Assistant media player.
   */
  static void _send_volume_set(const std::string &entity_id, uint8_t volume);
};

/**
 * No source volume, the media player only has its own volume.
 */
class NoSourceVolumeStrategy : public HomeAssistantSourceVolumeStrategy {
public:
  void update_from_player_attributes(const nlohmann::json &attributes);
  std::optional<uint8_t> get_source_volume();
  void set_source_volume(uint8_t volume);
};

/**
 * The media player reports both the source volume and the entity ID of the source as state attributes.
 * Source volume changes are sent to the source entity.
 */
class PlayerAttributesSourceVolumeStrategy : public HomeAssistantSourceVolumeStrategy {
public:
  PlayerAttributesSourceVolumeStrategy(std::string source_entity_attribute, std::string source_volume_attribute);
  void update_from_player_attributes(const nlohmann::json &attributes);
  std::optional<uint8_t> get_source_volume();
  void set_source_volume(uint8_t volume);

private:
  std::string _source_entity_attribute;
  std::string _source_volume_attribute;

  std::mutex _mutex;
  std::string _source_entity_id;
  std::optional<uint8_t> _source_volume;
};

/**
 * The media player reports the entity ID of the source as a state attribute. The source is itself a
 * media player in Home Assistant and its own volume is used as the source volume.
 */
class SourceEntitySourceVolumeStrategy : public HomeAssistantSourceVolumeStrategy {
public:
  SourceEntitySourceVolumeStrategy(std::string source_entity_attribute, std::function<void()> on_change);
  ~SourceEntitySourceVolumeStrategy();
  void update_from_player_attributes(const nlohmann::json &attributes);
  std::optional<uint8_t> get_source_volume();
  void set_source_volume(uint8_t volume);
  void home_assistant_event_callback(nlohmann::json event_data);

private:
  std::string _source_entity_attribute;
  std::function<void()> _on_change;

  std::mutex _mutex;
  std::string _source_entity_id;
  std::optional<uint8_t> _source_volume;
};

#endif // !MQTT_MANAGER_HOME_ASSISTANT_SOURCE_VOLUME_HPP
