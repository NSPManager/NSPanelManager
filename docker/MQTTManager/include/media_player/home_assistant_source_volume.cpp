#include "media_player/home_assistant_source_volume.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <cmath>
#include <home_assistant_manager/home_assistant_manager.hpp>
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
      return std::make_unique<PlayerAttributesSourceVolumeStrategy>(source_entity_attribute, source_volume_attribute);
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

std::optional<uint8_t> HomeAssistantSourceVolumeStrategy::_volume_from_json(const nlohmann::json &volume_level) {
  if (!volume_level.is_number()) {
    return std::nullopt;
  }
  return std::lround(std::clamp(volume_level.get<double>(), 0.0, 1.0) * 100);
}

void HomeAssistantSourceVolumeStrategy::_send_volume_set(const std::string &entity_id, uint8_t volume) {
  nlohmann::json command = {
      {"type", "call_service"},
      {"domain", "media_player"},
      {"target", {{"entity_id", entity_id}}},
      {"service", "volume_set"},
      {"service_data", {{"volume_level", volume / 100.0}}}};
  HomeAssistantManager::send_json(command);
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

// Player attributes

PlayerAttributesSourceVolumeStrategy::PlayerAttributesSourceVolumeStrategy(std::string source_entity_attribute, std::string source_volume_attribute) {
  this->_source_entity_attribute = source_entity_attribute;
  this->_source_volume_attribute = source_volume_attribute;
}

void PlayerAttributesSourceVolumeStrategy::update_from_player_attributes(const nlohmann::json &attributes) {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  this->_source_entity_id.clear();
  this->_source_volume.reset();

  if (attributes.contains(this->_source_entity_attribute) && attributes[this->_source_entity_attribute].is_string()) {
    this->_source_entity_id = attributes[this->_source_entity_attribute];
  }
  // Only report a source volume if we also know where to send volume changes.
  if (!this->_source_entity_id.empty() && attributes.contains(this->_source_volume_attribute)) {
    this->_source_volume = HomeAssistantSourceVolumeStrategy::_volume_from_json(attributes[this->_source_volume_attribute]);
  }
}

std::optional<uint8_t> PlayerAttributesSourceVolumeStrategy::get_source_volume() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  return this->_source_volume;
}

void PlayerAttributesSourceVolumeStrategy::set_source_volume(uint8_t volume) {
  std::string source_entity_id;
  {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    source_entity_id = this->_source_entity_id;
  }
  if (source_entity_id.empty()) {
    SPDLOG_WARN("Received command to set source volume but the media player does not report a source entity.");
    return;
  }
  HomeAssistantSourceVolumeStrategy::_send_volume_set(source_entity_id, volume);
}

// Source entity

SourceEntitySourceVolumeStrategy::SourceEntitySourceVolumeStrategy(std::string source_entity_attribute, std::function<void()> on_change) {
  this->_source_entity_attribute = source_entity_attribute;
  this->_on_change = on_change;
}

SourceEntitySourceVolumeStrategy::~SourceEntitySourceVolumeStrategy() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  if (!this->_source_entity_id.empty()) {
    HomeAssistantManager::detach_event_observer(this->_source_entity_id, boost::bind(&SourceEntitySourceVolumeStrategy::home_assistant_event_callback, this, _1));
  }
}

void SourceEntitySourceVolumeStrategy::update_from_player_attributes(const nlohmann::json &attributes) {
  std::string new_source_entity_id;
  if (attributes.contains(this->_source_entity_attribute) && attributes[this->_source_entity_attribute].is_string()) {
    new_source_entity_id = attributes[this->_source_entity_attribute];
  }

  std::string old_source_entity_id;
  {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    if (new_source_entity_id.compare(this->_source_entity_id) == 0) {
      return;
    }
    old_source_entity_id = this->_source_entity_id;
    this->_source_entity_id = new_source_entity_id;
    this->_source_volume.reset();
  }

  SPDLOG_DEBUG("Media player source changed from '{}' to '{}'.", old_source_entity_id, new_source_entity_id);
  if (!old_source_entity_id.empty()) {
    HomeAssistantManager::detach_event_observer(old_source_entity_id, boost::bind(&SourceEntitySourceVolumeStrategy::home_assistant_event_callback, this, _1));
  }
  if (!new_source_entity_id.empty()) {
    // Attaching an observer requests all states from Home Assistant, the current source volume will arrive through the event callback.
    HomeAssistantManager::attach_event_observer(new_source_entity_id, boost::bind(&SourceEntitySourceVolumeStrategy::home_assistant_event_callback, this, _1));
  }
}

std::optional<uint8_t> SourceEntitySourceVolumeStrategy::get_source_volume() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  return this->_source_volume;
}

void SourceEntitySourceVolumeStrategy::set_source_volume(uint8_t volume) {
  std::string source_entity_id;
  {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    source_entity_id = this->_source_entity_id;
  }
  if (source_entity_id.empty()) {
    SPDLOG_WARN("Received command to set source volume but the media player does not report a source entity.");
    return;
  }
  HomeAssistantSourceVolumeStrategy::_send_volume_set(source_entity_id, volume);
}

void SourceEntitySourceVolumeStrategy::home_assistant_event_callback(nlohmann::json event_data) {
  try {
    if (!event_data.contains("event") || !event_data["event"].contains("data")) {
      return;
    }
    nlohmann::json data = event_data["event"]["data"];
    if (!data.contains("entity_id") || !data["entity_id"].is_string()) {
      return;
    }

    bool changed = false;
    {
      std::lock_guard<std::mutex> lock_guard(this->_mutex);
      if (data["entity_id"].get<std::string>().compare(this->_source_entity_id) != 0) {
        return; // Event for a previous source.
      }

      std::optional<uint8_t> new_volume = this->_source_volume;
      if (!data.contains("new_state") || data["new_state"].is_null()) {
        new_volume.reset();
      } else if (data["new_state"].contains("attributes") && data["new_state"]["attributes"].contains("volume_level")) {
        new_volume = HomeAssistantSourceVolumeStrategy::_volume_from_json(data["new_state"]["attributes"]["volume_level"]);
      }
      // Home Assistant does not report volume while a media player is off, keep the last known value in that case.

      changed = new_volume != this->_source_volume;
      this->_source_volume = new_volume;
    }

    if (changed && this->_on_change) {
      this->_on_change();
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception during processing of media player source event. Error: {}", e.what());
  }
}
