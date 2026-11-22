#include "openhab_media_player.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "media_player/media_player.hpp"
#include "protobuf_nspanel_entity.pb.h"
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <openhab_manager/openhab_manager.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

OpenhabMediaPlayer::OpenhabMediaPlayer(uint32_t media_player_id) : MediaPlayerEntity(media_player_id) {
  // Process OpenHAB specific details. General media player data is loaded in the "MediaPlayerEntity" constructor.
  if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::OPENHAB) {
    SPDLOG_ERROR("OpenhabMediaPlayer has not been recognized as controlled by OPENHAB. Will stop processing media player.");
    return;
  }

  if (!this->_load_openhab_config()) {
    return;
  }

  // OpenHAB has no media player entity. Transport is controlled through a Player item and volume through a Dimmer/Number item,
  // so the panel can only offer what those two items can do.
  this->_can_play = true;
  this->_can_pause = true;
  this->_can_next_track = true;
  this->_can_previous_track = true;
  this->_can_set_volume = true;
  this->_can_mute = false;
  this->_has_source_volume = false;
  this->_has_media_position = false;

  SPDLOG_DEBUG("Loaded media player {}::{}, openhab control item: {}, volume item: {}", this->_id, this->_name, this->_openhab_item_control, this->_openhab_item_volume);
  this->_attach_observers();

  this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

OpenhabMediaPlayer::~OpenhabMediaPlayer() {
  this->_detach_observers(this->_openhab_item_control, this->_openhab_item_volume);
}

void OpenhabMediaPlayer::reload_config() {
  MediaPlayerEntity::reload_config();

  // Only reattach to OpenHAB when the items have changed, so that state updates are not missed while the config is reloaded.
  std::string previous_control_item = this->_openhab_item_control;
  std::string previous_volume_item = this->_openhab_item_volume;
  bool config_loaded = this->_load_openhab_config();

  bool items_changed = this->_openhab_item_control.compare(previous_control_item) != 0 || this->_openhab_item_volume.compare(previous_volume_item) != 0;
  if (items_changed) {
    this->_detach_observers(previous_control_item, previous_volume_item);
    // The known state belongs to the previous items.
    this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN;
    this->_volume = 0;
    if (config_loaded) {
      // OpenHABManager fetches the current state of the items when an observer is attached (ItemStateFetched).
      this->_attach_observers();
    }
    this->send_state_update_to_nspanel();
  }
}

bool OpenhabMediaPlayer::_load_openhab_config() {
  nlohmann::json entity_data;
  try {
    auto media_player = database_manager::database.get<database_manager::Entity>(this->_id);
    entity_data = media_player.get_entity_data_json();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to load media player {}: {}", this->_id, e.what());
    this->_openhab_item_control.clear();
    this->_openhab_item_volume.clear();
    return false;
  }

  // The same rules as put_media_player_entity in rest.py: both items are required for OpenHAB.
  if (entity_data.contains("openhab_item_control") && entity_data["openhab_item_control"].is_string()) {
    this->_openhab_item_control = entity_data["openhab_item_control"];
  } else {
    SPDLOG_ERROR("No openhab control item defined for media player {}::{}", this->_id, this->_name);
    this->_openhab_item_control.clear();
  }

  if (entity_data.contains("openhab_item_volume") && entity_data["openhab_item_volume"].is_string()) {
    this->_openhab_item_volume = entity_data["openhab_item_volume"];
  } else {
    SPDLOG_ERROR("No openhab volume item defined for media player {}::{}", this->_id, this->_name);
    this->_openhab_item_volume.clear();
  }

  return !this->_openhab_item_control.empty() && !this->_openhab_item_volume.empty();
}

void OpenhabMediaPlayer::_attach_observers() {
  OpenhabManager::attach_event_observer(this->_openhab_item_control, boost::bind(&OpenhabMediaPlayer::openhab_event_callback, this, _1));
  OpenhabManager::attach_event_observer(this->_openhab_item_volume, boost::bind(&OpenhabMediaPlayer::openhab_event_callback, this, _1));
}

void OpenhabMediaPlayer::_detach_observers(const std::string &control_item, const std::string &volume_item) {
  if (!control_item.empty()) {
    OpenhabManager::detach_event_observer(control_item, boost::bind(&OpenhabMediaPlayer::openhab_event_callback, this, _1));
  }
  if (!volume_item.empty()) {
    OpenhabManager::detach_event_observer(volume_item, boost::bind(&OpenhabMediaPlayer::openhab_event_callback, this, _1));
  }
}

std::optional<std::string> OpenhabMediaPlayer::_value_to_string(const nlohmann::json &value) {
  // OpenHAB normally sends values as strings, but be lenient about numbers.
  if (value.is_string()) {
    return value.get<std::string>();
  } else if (value.is_number()) {
    return std::to_string(value.get<double>());
  }
  return std::nullopt;
}

bool OpenhabMediaPlayer::_process_item_state(const std::string &item, const std::string &state) {
  if (item.compare(this->_openhab_item_control) == 0) {
    SPDLOG_DEBUG("Got control item state for openhab media player {}::{}: {}", this->_id, this->_name, state);
    if (state.compare("PLAY") == 0) {
      this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING;
    } else if (state.compare("PAUSE") == 0) {
      this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_PAUSED;
    } else {
      // REWIND, FASTFORWARD, NULL and UNDEF have no equivalent playback state.
      this->_state = NSPanelEntityState_MediaPlayer_PlaybackState_UNKNOWN;
    }
    return true;
  } else if (item.compare(this->_openhab_item_volume) == 0) {
    SPDLOG_DEBUG("Got volume item state for openhab media player {}::{}: {}", this->_id, this->_name, state);
    // The value is not a number while the item is NULL or UNDEF, keep the last known volume in that case.
    const char *begin = state.c_str();
    char *end = nullptr;
    double volume = std::strtod(begin, &end);
    if (end == begin) {
      SPDLOG_DEBUG("Volume item state '{}' for media player {}::{} is not a number, ignoring.", state, this->_id, this->_name);
      return false;
    }
    this->_volume = std::lround(std::clamp(volume, 0.0, 100.0));
    return true;
  }
  return false;
}

void OpenhabMediaPlayer::openhab_event_callback(nlohmann::json data) {
  if (!data.contains("type") || !data["type"].is_string()) {
    return;
  }
  std::string type = data["type"].get<std::string>();

  bool changed = false;
  try {
    if (type.compare("ItemStateChangedEvent") == 0) {
      // Topic is "openhab/items/<item name>/statechanged".
      if (!data.contains("topic") || !data["topic"].is_string() || !data.contains("payload") || !data["payload"].is_string()) {
        return;
      }
      std::vector<std::string> topic_parts;
      boost::split(topic_parts, data["topic"].get<std::string>(), boost::is_any_of("/"));
      if (topic_parts.size() < 3) {
        SPDLOG_ERROR("Received ItemStateChangedEvent with a topic with not enough parts, topic: {}", data["topic"].get<std::string>());
        return;
      }

      // The payload is a JSON document sent as a string.
      nlohmann::json payload = nlohmann::json::parse(data["payload"].get<std::string>(), nullptr, false);
      if (payload.is_discarded() || !payload.is_object() || !payload.contains("value")) {
        return;
      }
      if (auto state = OpenhabMediaPlayer::_value_to_string(payload["value"])) {
        changed = this->_process_item_state(topic_parts[2], state.value());
      }
    } else if (type.compare("ItemStateFetched") == 0) {
      // Custom event sent by the OpenHAB manager with the initial state of an item.
      if (!data.contains("payload") || !data["payload"].is_object() || !data["payload"].contains("name") || !data["payload"]["name"].is_string() || !data["payload"].contains("state")) {
        return;
      }
      if (auto state = OpenhabMediaPlayer::_value_to_string(data["payload"]["state"])) {
        changed = this->_process_item_state(data["payload"]["name"].get<std::string>(), state.value());
      }
    }
  } catch (const std::exception &e) {
    SPDLOG_WARN("Could not process OpenHAB event for media player {}::{}. Error: {}", this->_id, this->_name, e.what());
    return;
  }

  if (!changed) {
    return;
  }

  uint16_t previous_icon_color = this->get_icon_color();
  this->send_state_update_to_nspanel();
  if (previous_icon_color != this->get_icon_color()) {
    this->_signal_entity_changed();
  }
}

void OpenhabMediaPlayer::_send_command(const std::string &item, const std::string &command_type, const nlohmann::json &value) {
  if (item.empty()) {
    SPDLOG_ERROR("Cannot send command for media player {}::{}, no item configured.", this->_id, this->_name);
    return;
  }
  // Same format as OpenhabLight: the payload is a JSON document sent as a string.
  nlohmann::json payload_data;
  payload_data["type"] = command_type;
  payload_data["value"] = value;

  nlohmann::json service_data;
  service_data["type"] = "ItemCommandEvent";
  service_data["topic"] = fmt::format("openhab/items/{}/command", item);
  service_data["payload"] = payload_data.dump();
  OpenhabManager::send_json(service_data);
}

void OpenhabMediaPlayer::play() {
  this->_send_command(this->_openhab_item_control, "PlayPause", "PLAY");
}

void OpenhabMediaPlayer::pause() {
  this->_send_command(this->_openhab_item_control, "PlayPause", "PAUSE");
}

void OpenhabMediaPlayer::next_track() {
  this->_send_command(this->_openhab_item_control, "NextPrevious", "NEXT");
}

void OpenhabMediaPlayer::previous_track() {
  this->_send_command(this->_openhab_item_control, "NextPrevious", "PREVIOUS");
}

void OpenhabMediaPlayer::set_volume(uint8_t volume) {
  this->_send_command(this->_openhab_item_volume, "Percent", volume);
}

// The features below need attributes or items that OpenHAB media players do not have here.
// The panel does not offer them because can_mute and has_source_volume are false and no sources are reported.
void OpenhabMediaPlayer::set_muted(bool muted) {
  SPDLOG_WARN("Media player {}::{} is controlled by OpenHAB, which does not support mute.", this->_id, this->_name);
}

void OpenhabMediaPlayer::set_source_volume(uint8_t volume) {
  SPDLOG_WARN("Media player {}::{} is controlled by OpenHAB, which does not support source volume.", this->_id, this->_name);
}

void OpenhabMediaPlayer::select_player_source(const std::string &source) {
  SPDLOG_WARN("Media player {}::{} is controlled by OpenHAB, which does not support source selection.", this->_id, this->_name);
}

void OpenhabMediaPlayer::select_source_player_source(const std::string &source) {
  SPDLOG_WARN("Media player {}::{} is controlled by OpenHAB, which does not support source player sources.", this->_id, this->_name);
}

bool OpenhabMediaPlayer::_download_album_art(const std::string &source, std::string &image_data) {
  // No album art with only a Player and a volume item.
  return false;
}
