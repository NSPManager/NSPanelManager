#include "home_assistant_media_player.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "media_player/home_assistant_source_player.hpp"
#include "media_player/home_assistant_source_volume.hpp"
#include "media_player/media_player.hpp"
#include "protobuf_nspanel_entity.pb.h"
#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <list>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <web_helper/WebHelper.hpp>

HomeAssistantMediaPlayer::HomeAssistantMediaPlayer(uint32_t media_player_id) : MediaPlayerEntity(media_player_id) {
  // Process Home Assistant specific details. General media player data is loaded in the "MediaPlayerEntity" constructor.
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT) {
    SPDLOG_ERROR("HomeAssistantMediaPlayer has not been recognized as controlled by HOME_ASSISTANT. Will stop processing media player.");
    return;
  }

  auto entity_data = this->_load_home_assistant_config();
  // The source volume strategy is configured per media player, so it is created from the same entity_data.
  // A failed load leaves the media player unusable, drop any source volume strategy with it.
  this->_create_source_volume_strategy(entity_data.value_or(nlohmann::json::object()));
  if (!entity_data.has_value()) {
    return;
  }

  SPDLOG_DEBUG("Loaded media player {}::{}, home assistant entity ID: {}", this->_id, this->_name, this->_home_assistant_name);
  HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantMediaPlayer::home_assistant_event_callback, this, _1));

  if (this->_home_assistant_name.rfind("media_player.", 0) != 0) {
    SPDLOG_ERROR("Unknown type of home assistant entity '{}'. Expected media_player.", this->_home_assistant_name);
  }

  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

HomeAssistantMediaPlayer::~HomeAssistantMediaPlayer() {
  HomeAssistantManager::detach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantMediaPlayer::home_assistant_event_callback, this, _1));
}

void HomeAssistantMediaPlayer::reload_config() {
  MediaPlayerEntity::reload_config();

  // Only reattach to Home Assistant and rebuild the source volume strategy when their settings have changed,
  // so that state updates are not missed while the config is reloaded.
  std::string previous_home_assistant_name = this->_home_assistant_name;
  auto entity_data = this->_load_home_assistant_config();
  bool home_assistant_name_changed = this->_home_assistant_name.compare(previous_home_assistant_name) != 0;

  if (home_assistant_name_changed) {
    if (!previous_home_assistant_name.empty()) {
      HomeAssistantManager::detach_event_observer(previous_home_assistant_name, boost::bind(&HomeAssistantMediaPlayer::home_assistant_event_callback, this, _1));
    }
    // The saved attributes belong to the previous Home Assistant entity.
    std::lock_guard<std::mutex> lock_guard(this->_source_volume_strategy_mutex);
    this->_last_player_attributes = nlohmann::json::object();
  }

  this->_create_source_volume_strategy(entity_data.value_or(nlohmann::json::object()));
  // The name or source volume might have changed, unchanged states are not sent again.
  this->_update_source_player_and_send_state();

  if (home_assistant_name_changed && !this->_home_assistant_name.empty()) {
    // This will also request the current state from Home Assistant.
    HomeAssistantManager::attach_event_observer(this->_home_assistant_name, boost::bind(&HomeAssistantMediaPlayer::home_assistant_event_callback, this, _1));
  }
}

std::optional<nlohmann::json> HomeAssistantMediaPlayer::_load_home_assistant_config() {
  nlohmann::json entity_data;
  try {
    auto media_player = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = media_player.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load media player {}: {}", this->_id, e.what());
    this->_home_assistant_name.clear();
    return std::nullopt;
  }

  if (entity_data.contains("home_assistant_name") && entity_data["home_assistant_name"].is_string()) {
    this->_home_assistant_name = entity_data["home_assistant_name"];
  } else {
    SPDLOG_ERROR("No home assistant name defined for media player {}::{}", this->_id, this->_name);
    this->_home_assistant_name.clear();
    return std::nullopt;
  }

  return entity_data;
}

void HomeAssistantMediaPlayer::_create_source_volume_strategy(const nlohmann::json &entity_data) {
  nlohmann::json source_volume_settings = nlohmann::json::object();
  for (const char *key : {"source_volume_strategy", "source_entity_attribute", "source_volume_attribute"}) {
    if (entity_data.contains(key)) {
      source_volume_settings[key] = entity_data[key];
    }
  }

  {
    std::lock_guard<std::mutex> lock_guard(this->_source_volume_strategy_mutex);
    if (this->_source_volume_strategy && this->_source_volume_settings == source_volume_settings) {
      return;
    }
  }

  auto strategy = std::shared_ptr<HomeAssistantSourceVolumeStrategy>(HomeAssistantSourceVolumeStrategy::create(this->_id, entity_data, [this]() {
    this->_update_source_player_and_send_state();
  }));

  nlohmann::json player_attributes;
  {
    std::lock_guard<std::mutex> lock_guard(this->_source_volume_strategy_mutex);
    this->_source_volume_strategy = strategy;
    this->_source_volume_settings = source_volume_settings;
    player_attributes = this->_last_player_attributes;
  }
  // Start the new strategy from the latest known player state instead of waiting for the next update from Home Assistant.
  strategy->update_from_player_attributes(player_attributes);
}

std::shared_ptr<HomeAssistantSourceVolumeStrategy> HomeAssistantMediaPlayer::_get_source_volume_strategy() {
  std::lock_guard<std::mutex> lock_guard(this->_source_volume_strategy_mutex);
  return this->_source_volume_strategy;
}

void HomeAssistantMediaPlayer::_update_source_player_and_send_state() {
  auto strategy = this->_get_source_volume_strategy();
  std::optional<uint8_t> source_volume = strategy ? strategy->get_source_volume() : std::nullopt;
  this->_has_source_volume = source_volume.has_value();
  this->_source_volume = source_volume.value_or(0);
  this->_set_source_player_sources(strategy ? strategy->get_source_player_sources() : std::nullopt);
  this->send_state_update_to_nspanel();
}

void HomeAssistantMediaPlayer::home_assistant_event_callback(nlohmann::json data) {
  if (!data.contains("event")) [[unlikely]] {
    SPDLOG_ERROR("Media player {}::{} received malformed event data from HA. Data contains no 'event' key.", this->_id, this->_name);
    return;
  } else if (!data["event"].contains("event_type")) [[unlikely]] {
    SPDLOG_ERROR("Media player {}::{} received malformed event data from HA. Data contains no 'event.event_type' key.", this->_id, this->_name);
    return;
  } else if (!data["event"].contains("data")) [[unlikely]] {
    SPDLOG_ERROR("Media player {}::{} received malformed event data from HA. Data contains no 'event.data' key.", this->_id, this->_name);
    return;
  } else if (!data["event"]["data"].contains("entity_id")) [[unlikely]] {
    SPDLOG_ERROR("Media player {}::{} received malformed event data from HA. Data contains no 'event.data.entity_id' key.", this->_id, this->_name);
    return;
  }

  if (std::string(data["event"]["event_type"]).compare("state_changed") != 0 || std::string(data["event"]["data"]["entity_id"]).compare(this->_home_assistant_name) != 0) {
    return;
  }

  SPDLOG_DEBUG("Got event update for HA media player {}::{}.", this->_id, this->_name);
  uint16_t previous_icon_color = this->get_icon_color();

  try {
    nlohmann::json new_state_data = data["event"]["data"]["new_state"];
    nlohmann::json attributes = nlohmann::json::object();
    if (new_state_data.is_null()) {
      // Entity was removed from Home Assistant.
      this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN;
      this->_set_album_art_source("");
      this->_set_player_sources(MediaPlayerSources());
    } else {
      std::string new_state = new_state_data.contains("state") && new_state_data["state"].is_string() ? new_state_data["state"].get<std::string>() : "";
      if (new_state.compare("playing") == 0) {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING;
      } else if (new_state.compare("paused") == 0) {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_PAUSED;
      } else if (new_state.compare("buffering") == 0) {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_BUFFERING;
      } else if (new_state.compare("idle") == 0) {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_IDLE;
      } else if (new_state.compare("on") == 0) {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_ON;
      } else if (new_state.compare("off") == 0 || new_state.compare("standby") == 0) {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_OFF;
      } else {
        this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN;
      }

      if (new_state_data.contains("attributes") && new_state_data["attributes"].is_object()) {
        attributes = new_state_data["attributes"];
      }

      this->_media_title = attributes.contains("media_title") && attributes["media_title"].is_string() ? attributes["media_title"].get<std::string>() : "";
      this->_media_artist = attributes.contains("media_artist") && attributes["media_artist"].is_string() ? attributes["media_artist"].get<std::string>() : "";
      this->_set_album_art_source(attributes.contains("entity_picture") && attributes["entity_picture"].is_string() ? attributes["entity_picture"].get<std::string>() : "");

      // Home Assistant does not report volume while a media player is off, keep the last known values in that case.
      if (attributes.contains("volume_level") && attributes["volume_level"].is_number()) {
        this->_volume = std::lround(std::clamp(attributes["volume_level"].get<double>(), 0.0, 1.0) * 100);
      }
      if (attributes.contains("is_volume_muted") && attributes["is_volume_muted"].is_boolean()) {
        this->_is_muted = attributes["is_volume_muted"];
      }

      uint32_t supported_features = 0;
      if (attributes.contains("supported_features") && attributes["supported_features"].is_number_unsigned()) {
        supported_features = attributes["supported_features"];
      }
      this->_can_play = supported_features & HomeAssistantMediaPlayer::feature_play;
      this->_can_pause = supported_features & HomeAssistantMediaPlayer::feature_pause;
      this->_can_next_track = supported_features & HomeAssistantMediaPlayer::feature_next_track;
      this->_can_previous_track = supported_features & HomeAssistantMediaPlayer::feature_previous_track;
      this->_can_set_volume = supported_features & HomeAssistantMediaPlayer::feature_volume_set;
      this->_can_mute = supported_features & HomeAssistantMediaPlayer::feature_volume_mute;
      this->_set_player_sources(HomeAssistantSourcePlayer::sources_from_attributes(attributes));
    }
    this->_update_media_position(attributes);

    std::shared_ptr<HomeAssistantSourceVolumeStrategy> strategy;
    {
      std::lock_guard<std::mutex> lock_guard(this->_source_volume_strategy_mutex);
      this->_last_player_attributes = attributes;
      strategy = this->_source_volume_strategy;
    }
    if (strategy) {
      strategy->update_from_player_attributes(attributes);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("Caught exception during processing of HA media player {}::{} event. Error: {}", this->_id, this->_name, e.what());
    return;
  }

  this->_update_source_player_and_send_state();
  if (previous_icon_color != this->get_icon_color()) {
    this->_signal_entity_changed();
  }
}

void HomeAssistantMediaPlayer::_call_service(const std::string &service, nlohmann::json service_data) {
  nlohmann::json command = {
      {"type", "call_service"},
      {"domain", "media_player"},
      {"target", {{"entity_id", this->_home_assistant_name}}},
      {"service", service}};
  if (!service_data.empty()) {
    command["service_data"] = service_data;
  }
  HomeAssistantManager::send_json(command);
}

void HomeAssistantMediaPlayer::_update_media_position(const nlohmann::json &attributes) {
  // Home Assistant only has a position for media that is playing or paused. The position is
  // only meaningful together with the time it was taken at, so both are needed.
  std::optional<int64_t> updated_at;
  if (attributes.contains("media_position_updated_at") && attributes["media_position_updated_at"].is_string()) {
    updated_at = HomeAssistantMediaPlayer::parse_timestamp(attributes["media_position_updated_at"].get<std::string>());
    if (!updated_at.has_value()) {
      SPDLOG_WARN("Could not parse media_position_updated_at '{}' for HA media player {}::{}.", attributes["media_position_updated_at"].get<std::string>(), this->_id, this->_name);
    }
  }
  if (updated_at.has_value() && attributes.contains("media_position") && attributes["media_position"].is_number()) {
    this->_has_media_position = true;
    this->_media_position = std::lround(std::max(attributes["media_position"].get<double>(), 0.0));
    this->_media_position_updated_at = updated_at.value();
  } else {
    this->_has_media_position = false;
    this->_media_position = 0;
    this->_media_position_updated_at = 0;
  }

  if (attributes.contains("media_duration") && attributes["media_duration"].is_number()) {
    this->_media_duration = std::lround(std::max(attributes["media_duration"].get<double>(), 0.0));
  } else {
    this->_media_duration = 0;
  }
}

std::optional<int64_t> HomeAssistantMediaPlayer::parse_timestamp(const std::string &timestamp) {
  // ISO 8601 as Home Assistant writes it: "YYYY-MM-DDTHH:MM:SS", optional fraction, then "Z" or "+HH:MM"/"-HH:MM".
  std::tm time = {};
  int consumed = 0;
  if (std::sscanf(timestamp.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d%n", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec, &consumed) != 6) {
    return std::nullopt;
  }
  if (time.tm_mon < 1 || time.tm_mon > 12 || time.tm_mday < 1 || time.tm_mday > 31 || time.tm_hour > 23 || time.tm_min > 59 || time.tm_sec > 60) {
    return std::nullopt;
  }
  time.tm_year -= 1900;
  time.tm_mon -= 1;

  size_t position = consumed;
  if (position < timestamp.size() && timestamp[position] == '.') {
    position++;
    while (position < timestamp.size() && std::isdigit(static_cast<unsigned char>(timestamp[position]))) {
      position++;
    }
  }

  int64_t offset_seconds = 0;
  std::string_view zone = std::string_view(timestamp).substr(position);
  if (zone == "Z") {
    offset_seconds = 0;
  } else if (zone.size() == 6 && (zone[0] == '+' || zone[0] == '-') && zone[3] == ':' && std::isdigit(static_cast<unsigned char>(zone[1])) && std::isdigit(static_cast<unsigned char>(zone[2])) && std::isdigit(static_cast<unsigned char>(zone[4])) && std::isdigit(static_cast<unsigned char>(zone[5]))) {
    int offset_hours = (zone[1] - '0') * 10 + (zone[2] - '0');
    int offset_minutes = (zone[4] - '0') * 10 + (zone[5] - '0');
    offset_seconds = (offset_hours * 3600 + offset_minutes * 60) * (zone[0] == '-' ? -1 : 1);
  } else {
    return std::nullopt;
  }

  return static_cast<int64_t>(timegm(&time)) - offset_seconds;
}

void HomeAssistantMediaPlayer::play() {
  this->_call_service("media_play");
}

void HomeAssistantMediaPlayer::pause() {
  this->_call_service("media_pause");
}

void HomeAssistantMediaPlayer::next_track() {
  this->_call_service("media_next_track");
}

void HomeAssistantMediaPlayer::previous_track() {
  this->_call_service("media_previous_track");
}

void HomeAssistantMediaPlayer::set_volume(uint8_t volume) {
  this->_call_service("volume_set", {{"volume_level", volume / 100.0}});
}

void HomeAssistantMediaPlayer::set_source_volume(uint8_t volume) {
  auto strategy = this->_get_source_volume_strategy();
  if (strategy) {
    strategy->set_source_volume(volume);
  }
}

bool HomeAssistantMediaPlayer::_download_album_art(const std::string &source, std::string &image_data) {
  std::string url;
  std::string authorization_header;
  std::list<const char *> headers;

  if (boost::algorithm::starts_with(source, "http://") || boost::algorithm::starts_with(source, "https://")) {
    // The integration serves the image from somewhere else. Do not send the Home Assistant token to it.
    url = source;
  } else {
    // A path on Home Assistant, normally /api/media_player_proxy/<entity>?token=...&cache=...
    url = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOME_ASSISTANT_ADDRESS);
    if (url.empty()) {
      SPDLOG_ERROR("No Home Assistant address configured. Cannot download album art for media player {}::{}.", this->_id, this->_name);
      return false;
    }
    while (!url.empty() && url.back() == '/') {
      url.pop_back();
    }
    if (MqttManagerConfig::is_home_assistant_addon()) {
      url.append("/core"); // The Home Assistant API is reached through the supervisor proxy when running as an add-on.
    }
    url.append(source);

    // The token in the proxy path is enough on its own, but the supervisor proxy requires an authorization header.
    authorization_header = fmt::format("Authorization: Bearer {}", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOME_ASSISTANT_TOKEN));
    headers.push_back(authorization_header.c_str());
  }

  SPDLOG_DEBUG("Downloading album art for media player {}::{}.", this->_id, this->_name);
  // WebHelper treats an empty header list as an error, so only pass the list when there is something in it.
  return WebHelper::perform_get_request(&url, &image_data, headers.empty() ? nullptr : &headers);
}

void HomeAssistantMediaPlayer::set_muted(bool muted) {
  this->_call_service("volume_mute", {{"is_volume_muted", muted}});
}

void HomeAssistantMediaPlayer::select_player_source(const std::string &source) {
  this->_call_service("select_source", {{"source", source}});
}

void HomeAssistantMediaPlayer::select_source_player_source(const std::string &source) {
  auto strategy = this->_get_source_volume_strategy();
  if (strategy) {
    strategy->select_source_player_source(source);
  }
}
