#pragma once
#include "media_player/media_player.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

class OpenhabMediaPlayer : public MediaPlayerEntity {
public:
  OpenhabMediaPlayer(uint32_t media_player_id);
  ~OpenhabMediaPlayer();

  void reload_config() override;
  void openhab_event_callback(nlohmann::json data);

  void play() override;
  void pause() override;
  void next_track() override;
  void previous_track() override;
  void set_volume(uint8_t volume) override;
  void set_muted(bool muted) override;
  void set_source_volume(uint8_t volume) override;
  void select_player_source(const std::string &source) override;
  void select_source_player_source(const std::string &source) override;

private:
  bool _download_album_art(const std::string &source, std::string &image_data) override;

  bool _load_openhab_config();
  void _attach_observers();
  void _detach_observers(const std::string &control_item, const std::string &volume_item);
  void _send_command(const std::string &item, const std::string &command_type, const nlohmann::json &value);
  bool _process_item_state(const std::string &item, const std::string &state);
  static std::optional<std::string> _value_to_string(const nlohmann::json &value);

  std::string _openhab_item_control;
  std::string _openhab_item_volume;
};
