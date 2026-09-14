#include "media_player/media_player.hpp"
#include "command_manager/command_manager.hpp"
#include "database_manager/database_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include "protobuf_nspanel_entity.pb.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <cstdint>
#include <entity/entity.hpp>
#include <entity/entity_icons.hpp>
#include <google/protobuf/util/message_differencer.h>
#include <mqtt_manager/mqtt_manager.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

MediaPlayerEntity::MediaPlayerEntity(uint32_t media_player_id) {
  this->_id = media_player_id;
  this->reload_config();

  CommandManager::attach_callback(boost::bind(&MediaPlayerEntity::command_callback, this, _1));

  SPDLOG_DEBUG("Media player {}::{} base loaded.", this->_id, this->_name);
}

uint16_t MediaPlayerEntity::get_room_id() {
  return this->_room_id;
}

std::string MediaPlayerEntity::get_name() {
  return this->_name;
}

void MediaPlayerEntity::reload_config() {
  auto media_player_entity = database_manager::database.get<database_manager::Entity>(this->_id);
  this->_name = media_player_entity.friendly_name;
  SPDLOG_DEBUG("Loading media player {}::{}.", this->_id, this->_name);

  this->_room_id = media_player_entity.room_id;
  this->_entity_page_id = media_player_entity.entities_page_id;
  this->_entity_page_slot = media_player_entity.room_view_position;

  nlohmann::json entity_data = media_player_entity.get_entity_data_json();
  if (entity_data.contains("controller")) {
    std::string controller = entity_data["controller"];
    if (controller.compare("home_assistant") == 0) {
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    } else {
      SPDLOG_ERROR("Got unknown controller ({}) for media player {}::{}. Will default to HOME_ASSISTANT.", controller, this->_id, this->_name);
      this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
    }
  } else {
    SPDLOG_ERROR("No controller defined for media player {}::{}. Will default to HOME_ASSISTANT.", this->_id, this->_name);
    this->_controller = MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT;
  }
}

void MediaPlayerEntity::send_state_update_to_nspanel() {
  NSPanelEntityState state;
  NSPanelEntityState_MediaPlayer *media_player_state = state.mutable_media_player();
  media_player_state->set_media_player_id(this->_id);
  media_player_state->set_name(this->_name);
  media_player_state->set_state(this->_state);
  media_player_state->set_media_title(this->_media_title);
  media_player_state->set_media_artist(this->_media_artist);
  media_player_state->set_volume(this->_volume);
  media_player_state->set_is_muted(this->_is_muted);
  media_player_state->set_has_source_volume(this->_has_source_volume);
  media_player_state->set_source_volume(this->_has_source_volume ? this->_source_volume : 0);
  media_player_state->set_can_play(this->_can_play);
  media_player_state->set_can_pause(this->_can_pause);
  media_player_state->set_can_next_track(this->_can_next_track);
  media_player_state->set_can_previous_track(this->_can_previous_track);
  media_player_state->set_can_set_volume(this->_can_set_volume);
  media_player_state->set_can_mute(this->_can_mute);

  google::protobuf::util::MessageDifferencer differencer;
  if (!differencer.Compare(this->_last_media_player_state, state)) {
    SPDLOG_DEBUG("Sending updated state for media player {}::{} over MQTT.", this->_id, this->_name);
    this->_last_media_player_state = state;

    MQTT_Manager::publish_protobuf(this->get_mqtt_state_topic(), state, true);
  } else {
    SPDLOG_DEBUG("Did not send state update for media player {}::{} as there were no changes.", this->_id, this->_name);
  }
}

void MediaPlayerEntity::command_callback(NSPanelMQTTManagerCommand &command) {
  if (!command.has_media_player_command() || static_cast<uint32_t>(command.media_player_command().media_player_id()) != this->_id) {
    return;
  }

  auto media_player_command = command.media_player_command();
  SPDLOG_DEBUG("Received command from NSPanel for media player {}::{}.", this->_id, this->_name);

  switch (media_player_command.playback_action()) {
  case NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PLAY:
    this->play();
    break;
  case NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PAUSE:
    this->pause();
    break;
  case NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_NEXT_TRACK:
    this->next_track();
    break;
  case NSPanelMQTTManagerCommand_MediaPlayerCommand_PlaybackAction_PREVIOUS_TRACK:
    this->previous_track();
    break;
  default:
    break;
  }

  if (media_player_command.has_volume()) {
    this->set_volume(std::clamp(media_player_command.volume(), 0, 100));
  }

  if (media_player_command.has_source_volume()) {
    this->set_source_volume(std::clamp(media_player_command.source_volume(), 0, 100));
  }

  if (media_player_command.has_muted()) {
    this->set_muted(media_player_command.muted());
  }
}

MediaPlayerEntity::~MediaPlayerEntity() {
  SPDLOG_DEBUG("Destructor for media player {}::{} called.", this->_id, this->_name);
  this->_signal_entity_destroyed();
  CommandManager::detach_callback(boost::bind(&MediaPlayerEntity::command_callback, this, _1));
}

uint16_t MediaPlayerEntity::get_id() {
  return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE MediaPlayerEntity::get_type() {
  return MQTT_MANAGER_ENTITY_TYPE::MEDIA_PLAYER;
}

MQTT_MANAGER_ENTITY_CONTROLLER MediaPlayerEntity::get_controller() {
  return this->_controller;
}

uint32_t MediaPlayerEntity::get_entity_page_id() {
  return this->_entity_page_id;
}

uint8_t MediaPlayerEntity::get_entity_page_slot() {
  return this->_entity_page_slot;
}

bool MediaPlayerEntity::can_toggle() {
  // Media players are opened on the NSPanel entity page instead of being toggled.
  return false;
}

void MediaPlayerEntity::toggle() {
}

std::string_view MediaPlayerEntity::get_icon() {
  return EntityIcons::media_player;
}

uint16_t MediaPlayerEntity::get_icon_color() {
  if (this->_state == NSPanelEntityState_MediaPlayer_PlaybackState_PLAYING || this->_state == NSPanelEntityState_MediaPlayer_PlaybackState_BUFFERING) {
    return GUI_Colors::icon_color_on;
  }
  return GUI_Colors::icon_color_off;
}

uint16_t MediaPlayerEntity::get_icon_active_color() {
  return this->get_icon_color();
}

std::string MediaPlayerEntity::get_mqtt_state_topic() {
  std::string manager_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS);
  return fmt::format("nspanel/mqttmanager_{}/entities/media_players/{}/state", manager_address, this->get_id());
}
