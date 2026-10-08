#include "media_player/home_assistant_source_player.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <cmath>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

HomeAssistantSourcePlayer::HomeAssistantSourcePlayer(std::string source_entity_attribute, std::function<void()> on_change) {
  this->_source_entity_attribute = source_entity_attribute;
  this->_on_change = on_change;
}

HomeAssistantSourcePlayer::~HomeAssistantSourcePlayer() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  if (!this->_entity_id.empty()) {
    HomeAssistantManager::detach_event_observer(this->_entity_id, boost::bind(&HomeAssistantSourcePlayer::home_assistant_event_callback, this, _1));
  }
}

void HomeAssistantSourcePlayer::update_from_player_attributes(const nlohmann::json &attributes) {
  std::string new_entity_id;
  if (attributes.contains(this->_source_entity_attribute) && attributes[this->_source_entity_attribute].is_string()) {
    new_entity_id = attributes[this->_source_entity_attribute];
  }

  std::string old_entity_id;
  {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    if (new_entity_id.compare(this->_entity_id) == 0) {
      return;
    }
    old_entity_id = this->_entity_id;
    this->_entity_id = new_entity_id;
    // Forget the previous source player straight away, so its sources are never offered for the new one.
    this->_volume.reset();
    this->_sources = MediaPlayerSources();
  }

  SPDLOG_DEBUG("Media player source changed from '{}' to '{}'.", old_entity_id, new_entity_id);
  if (!old_entity_id.empty()) {
    HomeAssistantManager::detach_event_observer(old_entity_id, boost::bind(&HomeAssistantSourcePlayer::home_assistant_event_callback, this, _1));
  }
  if (!new_entity_id.empty()) {
    // Attaching an observer requests all states from Home Assistant, the current state of the source player will arrive through the event callback.
    HomeAssistantManager::attach_event_observer(new_entity_id, boost::bind(&HomeAssistantSourcePlayer::home_assistant_event_callback, this, _1));
  }
}

std::string HomeAssistantSourcePlayer::get_entity_id() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  return this->_entity_id;
}

std::optional<uint8_t> HomeAssistantSourcePlayer::get_volume() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  return this->_volume;
}

std::optional<MediaPlayerSources> HomeAssistantSourcePlayer::get_sources() {
  std::lock_guard<std::mutex> lock_guard(this->_mutex);
  if (this->_entity_id.empty()) {
    return std::nullopt;
  }
  return this->_sources;
}

void HomeAssistantSourcePlayer::set_volume(uint8_t volume) {
  std::string entity_id = this->get_entity_id();
  if (entity_id.empty()) {
    SPDLOG_WARN("Received command to set source volume but the media player does not report a source entity.");
    return;
  }
  HomeAssistantSourcePlayer::call_service(entity_id, "volume_set", {{"volume_level", volume / 100.0}});
}

void HomeAssistantSourcePlayer::select_source(const std::string &source) {
  std::string entity_id = this->get_entity_id();
  if (entity_id.empty()) {
    SPDLOG_WARN("Received command to select a source player source but the media player does not report a source entity.");
    return;
  }
  HomeAssistantSourcePlayer::call_service(entity_id, "select_source", {{"source", source}});
}

void HomeAssistantSourcePlayer::home_assistant_event_callback(nlohmann::json event_data) {
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
      if (data["entity_id"].get<std::string>().compare(this->_entity_id) != 0) {
        return; // Event for a previous source.
      }

      std::optional<uint8_t> new_volume = this->_volume;
      MediaPlayerSources new_sources;
      if (!data.contains("new_state") || data["new_state"].is_null()) {
        new_volume.reset();
      } else if (data["new_state"].contains("attributes") && data["new_state"]["attributes"].is_object()) {
        const nlohmann::json &attributes = data["new_state"]["attributes"];
        // Home Assistant does not report volume while a media player is off, keep the last known value in that case.
        if (attributes.contains("volume_level")) {
          new_volume = HomeAssistantSourcePlayer::volume_from_json(attributes["volume_level"]);
        }
        new_sources = HomeAssistantSourcePlayer::sources_from_attributes(attributes);
      }

      changed = new_volume != this->_volume || new_sources != this->_sources;
      this->_volume = new_volume;
      this->_sources = std::move(new_sources);
    }

    if (changed && this->_on_change) {
      this->_on_change();
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception during processing of media player source event. Error: {}", e.what());
  }
}

MediaPlayerSources HomeAssistantSourcePlayer::sources_from_attributes(const nlohmann::json &attributes) {
  MediaPlayerSources sources;
  if (attributes.contains("source_list") && attributes["source_list"].is_array()) {
    for (const nlohmann::json &source : attributes["source_list"]) {
      if (source.is_string()) {
        sources.sources.push_back(source);
      }
    }
  }
  if (attributes.contains("source") && attributes["source"].is_string()) {
    sources.current_source = attributes["source"];
  }
  uint32_t supported_features = 0;
  if (attributes.contains("supported_features") && attributes["supported_features"].is_number_unsigned()) {
    supported_features = attributes["supported_features"];
  }
  sources.can_select = (supported_features & HomeAssistantSourcePlayer::feature_select_source) && !sources.sources.empty();
  return sources;
}

std::optional<uint8_t> HomeAssistantSourcePlayer::volume_from_json(const nlohmann::json &volume_level) {
  if (!volume_level.is_number()) {
    return std::nullopt;
  }
  return std::lround(std::clamp(volume_level.get<double>(), 0.0, 1.0) * 100);
}

void HomeAssistantSourcePlayer::call_service(const std::string &entity_id, const std::string &service, nlohmann::json service_data) {
  nlohmann::json command = {
      {"type", "call_service"},
      {"domain", "media_player"},
      {"target", {{"entity_id", entity_id}}},
      {"service", service}};
  if (!service_data.empty()) {
    command["service_data"] = service_data;
  }
  HomeAssistantManager::send_json(command);
}
