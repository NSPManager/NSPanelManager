#include "media_player/home_assistant_source_volume.hpp"
#include "media_player/home_assistant_source_player.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

/**
 * Read a string value from entity_data, or an empty string if it is missing or not a string.
 */
static std::string _string_from_entity_data(const nlohmann::json &entity_data, const std::string &key) {
  if (entity_data.contains(key) && entity_data[key].is_string()) {
    return entity_data[key];
  }
  return "";
}

std::unique_ptr<HomeAssistantSourceVolumeStrategy> HomeAssistantSourceVolumeStrategy::create(uint32_t media_player_id, const nlohmann::json &entity_data, std::function<void()> on_change) {
  std::string strategy = _string_from_entity_data(entity_data, "source_volume_strategy");
  std::string source_entity_attribute = _string_from_entity_data(entity_data, "source_entity_attribute");
  std::string source_volume_attribute = _string_from_entity_data(entity_data, "source_volume_attribute");

  // The web interface rejects invalid combinations, so anything below is either a media player saved
  // before source volume existed or a hand edited entity_data.
  if (strategy.compare("player_attributes") == 0) {
    if (source_entity_attribute.empty() || source_volume_attribute.empty()) {
      SPDLOG_ERROR("Media player {}: source volume strategy 'player_attributes' requires both a source entity attribute and a source volume attribute. Source volume will not be available.", media_player_id);
    } else {
      return std::make_unique<PlayerAttributesSourceVolumeStrategy>(source_entity_attribute, source_volume_attribute, on_change);
    }
  } else if (strategy.compare("source_entity") == 0) {
    if (source_entity_attribute.empty()) {
      SPDLOG_ERROR("Media player {}: source volume strategy 'source_entity' requires a source entity attribute. Source volume will not be available.", media_player_id);
    } else {
      return std::make_unique<SourceEntitySourceVolumeStrategy>(source_entity_attribute, on_change);
    }
  } else if (!strategy.empty() && strategy.compare("none") != 0) {
    SPDLOG_ERROR("Media player {}: unknown source volume strategy '{}'. Source volume will not be available.", media_player_id, strategy);
  }
  return std::make_unique<NoSourceVolumeStrategy>();
}

// No source volume

void NoSourceVolumeStrategy::update_from_player_attributes([[maybe_unused]] const nlohmann::json &attributes) {
}

std::optional<uint8_t> NoSourceVolumeStrategy::get_source_volume() {
  return std::nullopt;
}

void NoSourceVolumeStrategy::set_source_volume([[maybe_unused]] uint8_t volume) {
  SPDLOG_WARN("Received command to set source volume but no source volume strategy is configured.");
}

std::optional<MediaPlayerSources> NoSourceVolumeStrategy::get_source_player_sources() {
  return std::nullopt;
}

void NoSourceVolumeStrategy::select_source_player_source([[maybe_unused]] const std::string &source) {
  SPDLOG_WARN("Received command to select a source player source but no source volume strategy is configured.");
}

// Player attributes

PlayerAttributesSourceVolumeStrategy::PlayerAttributesSourceVolumeStrategy(std::string source_entity_attribute, std::string source_volume_attribute, std::function<void()> on_change) : _source_player(source_entity_attribute, on_change) {
  this->_source_volume_attribute = source_volume_attribute;
}

void PlayerAttributesSourceVolumeStrategy::update_from_player_attributes(const nlohmann::json &attributes) {
  this->_source_player.update_from_player_attributes(attributes);

  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  this->_source_volume.reset();
  // Only report a source volume if we also know where to send volume changes.
  if (!this->_source_player.get_entity_id().empty() && attributes.contains(this->_source_volume_attribute)) {
    this->_source_volume = HomeAssistantSourcePlayer::volume_from_json(attributes[this->_source_volume_attribute]);
  }
}

std::optional<uint8_t> PlayerAttributesSourceVolumeStrategy::get_source_volume() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  return this->_source_volume;
}

void PlayerAttributesSourceVolumeStrategy::set_source_volume(uint8_t volume) {
  this->_source_player.set_volume(volume);
}

std::optional<MediaPlayerSources> PlayerAttributesSourceVolumeStrategy::get_source_player_sources() {
  return this->_source_player.get_sources();
}

void PlayerAttributesSourceVolumeStrategy::select_source_player_source(const std::string &source) {
  this->_source_player.select_source(source);
}

// Source entity

SourceEntitySourceVolumeStrategy::SourceEntitySourceVolumeStrategy(std::string source_entity_attribute, std::function<void()> on_change) : _source_player(source_entity_attribute, on_change) {
}

void SourceEntitySourceVolumeStrategy::update_from_player_attributes(const nlohmann::json &attributes) {
  this->_source_player.update_from_player_attributes(attributes);
}

std::optional<uint8_t> SourceEntitySourceVolumeStrategy::get_source_volume() {
  return this->_source_player.get_volume();
}

void SourceEntitySourceVolumeStrategy::set_source_volume(uint8_t volume) {
  this->_source_player.set_volume(volume);
}

std::optional<MediaPlayerSources> SourceEntitySourceVolumeStrategy::get_source_player_sources() {
  return this->_source_player.get_sources();
}

void SourceEntitySourceVolumeStrategy::select_source_player_source(const std::string &source) {
  this->_source_player.select_source(source);
}
