#include "mqtt_manager_config.hpp"
#include "light/light.hpp"
#include "openssl/evp.h"
#include "web_helper/WebHelper.hpp"
#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <database_manager/database_manager.hpp>
#include <exception>
#include <expected>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iterator>
#include <mutex>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <nlohmann/detail/exceptions.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <protobuf/protobuf_mqttmanager.pb.h>
#include <protobuf_general.pb.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>
#include <system_error>
#include <websocket_server/websocket_server.hpp>

void MqttManagerConfig::load() {
  SPDLOG_TRACE("Loading timezone from /etc/timezone.");
  // Begin by loading timezone
  std::ifstream f("/etc/timezone", std::ios::in | std::ios::binary);
  const size_t file_size = std::filesystem::file_size("/etc/timezone");
  std::string timezone_str(file_size, '\0');
  f.read(timezone_str.data(), file_size);
  f.close();
  boost::algorithm::trim(timezone_str);
  MqttManagerConfig::timezone = timezone_str;
  SPDLOG_INFO("Read timezone {} from /etc/timezone.", timezone_str);
  MqttManagerConfig::timezone_posix_rule = MqttManagerConfig::read_posix_tz_rule(fmt::format("/usr/share/zoneinfo/{}", timezone_str));
  if (MqttManagerConfig::timezone_posix_rule.empty()) {
    SPDLOG_WARN("Could not read a POSIX TZ rule for timezone {}. NSPanels will show the time sent by the manager.", timezone_str);
  } else {
    SPDLOG_INFO("Using POSIX TZ rule {} for timezone {}.", MqttManagerConfig::timezone_posix_rule, timezone_str);
  }

  SPDLOG_DEBUG("Clearing config values cache.");
  MqttManagerConfig::_settings_values_cache.clear();

  {
    SPDLOG_INFO("Loading MQTT Manager settings.");
    std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_settings_mutex);

    const char *is_home_assistant_addon = std::getenv("IS_HOME_ASSISTANT_ADDON");
    if (is_home_assistant_addon != nullptr) {
      if (std::string(is_home_assistant_addon).compare("true") == 0) {
        MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::IS_HOME_ASSISTANT_ADDON, "true");
      } else {
        MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::IS_HOME_ASSISTANT_ADDON, "false");
      }
    } else {
      MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::IS_HOME_ASSISTANT_ADDON, "false");
    }

    std::string turn_on_bevaiour = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::TURN_ON_BEHAVIOR);
    if (turn_on_bevaiour.compare("color_temp") == 0) {
      MqttManagerConfig::_light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;
    } else if (turn_on_bevaiour.compare("restore") == 0) {
      MqttManagerConfig::_light_turn_on_behaviour = LightTurnOnBehaviour::RESTORE_PREVIOUS;
    } else {
      SPDLOG_WARN("Failed to determine turn on bevaiour for lights, assuming color temp. Value set: {}", turn_on_bevaiour);
      MqttManagerConfig::_light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;
    }
  }

  MqttManagerConfig::update_firmware_checksum();
  MqttManagerConfig::update_tft_checksums();
  SPDLOG_DEBUG("Checksum calculation done.");

  MqttManagerConfig::populate_default_and_clean();

  // Notify all listeners that the config has been loaded
  MqttManagerConfig::_config_loaded_listeners();
}

void MqttManagerConfig::populate_default_and_clean() {
  auto settings = database_manager::database.get_all<database_manager::SettingHolder>();
  // Check for set settings that are invalid/unused.
  for (const auto &setting : settings) {
    bool found = false;
    for (const auto &setting_pair : MqttManagerConfig::_setting_key_map) {
      if (setting_pair.second.first == setting.name) {
        found = true;
        break;
      }
    }
    // Cleanup old setting.
    if (!found) {
      SPDLOG_WARN("Removing invalid setting {} from DB.", setting.name);
      database_manager::database.remove<database_manager::SettingHolder>(setting.id);
    }
  }

  // Check if all default settings are set.
  for (const auto &setting : MqttManagerConfig::_setting_key_map) {
    bool found = false;
    for (const auto &setting_pair : settings) {
      if (setting_pair.name == setting.second.first) {
        found = true;
        break;
      }
    }
    if (!found) {
      SPDLOG_INFO("Setting {} not found in DB. Setting default value: '{}'.", setting.second.first, setting.second.second);
      database_manager::SettingHolder new_setting;
      new_setting.name = setting.second.first;
      new_setting.value = setting.second.second;
      database_manager::database.insert(new_setting);
    }
  }
}

void MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING key, std::string value) {
  std::string setting_db_key = MqttManagerConfig::_setting_key_map[key].first;
  SPDLOG_DEBUG("Setting '{}' to value '{}'", setting_db_key, value);

  auto result = database_manager::database.get_all<database_manager::SettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::SettingHolder::name) == setting_db_key));
  if (!result.empty()) {
    result[0].value = value;
    SPDLOG_DEBUG("Set settings key '{}' to value '{}'", setting_db_key, value);
    while (true) {
      try {
        database_manager::database.update(result[0]);
        break;
      } catch (std::exception &ex) {
        SPDLOG_WARN("Caught exception while trying to update setting '{}'. Retrying...", setting_db_key);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  } else {
    SPDLOG_ERROR("Failed to find existing setting for key '{}'. Will create a new key.", setting_db_key);
    database_manager::SettingHolder setting;
    setting.name = setting_db_key;
    setting.value = value;
    while (true) {
      try {
        database_manager::database.insert(setting);
        break;
      } catch (std::exception &ex) {
        SPDLOG_WARN("Caught exception while trying to insert setting '{}'. Retrying...", setting_db_key);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }

  MqttManagerConfig::_settings_values_cache[key] = value;
}

void MqttManagerConfig::set_nspanel_setting_value(int32_t nspanel_id, std::string key, std::string value) {
  using namespace sqlite_orm;
  SPDLOG_DEBUG("Setting '{}' to value '{}' for NSPanel with ID {}", key, value, nspanel_id);

  auto result = database_manager::database.get_all<database_manager::NSPanelSettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::NSPanelSettingHolder::name) = key) and sqlite_orm::c(&database_manager::NSPanelSettingHolder::nspanel_id) == nspanel_id);
  if (!result.empty()) {
    result[0].value = value;
    database_manager::database.update(result[0]);
  } else {
    database_manager::NSPanelSettingHolder setting;
    setting.nspanel_id = nspanel_id;
    setting.name = key;
    setting.value = value;
    database_manager::database.insert(setting);
  }
}

bool MqttManagerConfig::is_home_assistant_addon() {
  return MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::IS_HOME_ASSISTANT_ADDON);
}

LightTurnOnBehaviour MqttManagerConfig::get_light_turn_on_behaviour() {
  return MqttManagerConfig::_light_turn_on_behaviour;
}

void MqttManagerConfig::update_firmware_checksum() {
  SPDLOG_INFO("Updating/calculating MD5 checksums for all firmware files.");

  std::vector<std::string> files = {
      "/usr/src/app/nspanelmanager/firmware/sonoff/firmware.bin",
      "/usr/src/app/nspanelmanager/firmware/custom/firmware.bin",
      "/usr/src/app/nspanelmanager/firmware/sonoff/data_file.bin",
      "/usr/src/app/nspanelmanager/firmware/custom/data_file.bin",
  };

  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  bool recalculate_file = false;
  for (auto &file : files) {
    // Check if a cached key already exists for the file.
    bool file_in_map = MqttManagerConfig::_checksums.find(file) != MqttManagerConfig::_checksums.end();
    std::expected<std::filesystem::file_time_type, std::string> file_mtime;
    if (file_in_map) [[likely]] {
      file_mtime = MqttManagerConfig::_get_file_mtime(file);
      if (file_mtime.has_value()) [[likely]] {
        if (MqttManagerConfig::_checksums[file].mtime != *file_mtime) {
          recalculate_file = true;
        } else [[likely]] {
          recalculate_file = false;
        }
      } else {
        SPDLOG_ERROR("Failed to check mtime for file '{}'. Will try to recalculate file checksum anyway.", file);
      }
    } else {
      recalculate_file = true;
    }

    if (recalculate_file) [[unlikely]] {
      auto checksum = MqttManagerConfig::_get_file_md5_checksum(file);
      if (checksum.has_value()) [[likely]] {
        MqttManagerConfig::_checksums[file] = {
            .mtime = file_mtime.has_value() ? std::optional<std::filesystem::file_time_type>{*file_mtime} : std::nullopt,
            .checksum = *checksum,
        };
      } else {
        SPDLOG_ERROR("Failed to calculate checksum for file '{}'. Error: {}", file, checksum.error());
      }
    }
  }
}

void MqttManagerConfig::update_tft_checksums() {
  SPDLOG_INFO("Updating/calculating MD5 checksums for all .tft GUI files.");

  std::vector<std::string> files = {
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft1/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft2/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft3/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft4/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft1/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft2/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft3/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft4/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft1/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft2/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft3/gui.tft",
      "/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft4/gui.tft",
  };

  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  bool recalculate_file = false;
  for (auto &file : files) {
    // Check if a cached key already exists for the file.
    bool file_in_map = MqttManagerConfig::_checksums.find(file) != MqttManagerConfig::_checksums.end();
    std::expected<std::filesystem::file_time_type, std::string> file_mtime;
    if (file_in_map) [[likely]] {
      file_mtime = MqttManagerConfig::_get_file_mtime(file);
      if (file_mtime.has_value()) [[likely]] {
        if (MqttManagerConfig::_checksums[file].mtime != *file_mtime) {
          recalculate_file = true;
        } else [[likely]] {
          recalculate_file = false;
        }
      } else {
        SPDLOG_ERROR("Failed to check mtime for file '{}'. Will try to recalculate file checksum anyway.", file);
      }
    } else {
      recalculate_file = true;
    }

    if (recalculate_file) [[unlikely]] {
      auto checksum = MqttManagerConfig::_get_file_md5_checksum(file);
      if (checksum.has_value()) [[likely]] {
        MqttManagerConfig::_checksums[file] = {
            .mtime = file_mtime.has_value() ? std::optional<std::filesystem::file_time_type>{*file_mtime} : std::nullopt,
            .checksum = *checksum,
        };
      } else {
        SPDLOG_ERROR("Failed to calculate checksum for file '{}'. Error: {}", file, checksum.error());
      }
    }
  }
}

std::expected<std::string, std::string> MqttManagerConfig::_get_file_md5_checksum(std::string file_path) {
  std::ifstream file(file_path, std::ifstream::binary);
  if (!file) {
    return std::unexpected<std::string>("Failed to open file.");
  }

  EVP_MD_CTX *mdctx;
  unsigned char *md5_digest;
  unsigned int md5_digest_len = EVP_MD_size(EVP_md5());

  // MD5_Init
  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

  unsigned char buffer[8192];
  while (file.read(reinterpret_cast<char *>(buffer), sizeof(buffer)) || file.gcount() > 0) {
    if (EVP_DigestUpdate(mdctx, buffer, file.gcount()) != 1) {
      EVP_MD_CTX_free(mdctx);
      return std::unexpected<std::string>("Failed to update EVP digiest while processing file");
    }
  }

  // MD5_Final
  md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
  EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
  EVP_MD_CTX_free(mdctx);

  std::stringstream ss;
  ss << std::hex << std::uppercase << std::setfill('0');
  for (int i = 0; i < EVP_MD_size(EVP_md5()); i++) {
    ss << std::setw(2) << (int)md5_digest[i];
  }

  EVP_cleanup();
  return boost::algorithm::to_lower_copy(ss.str()); // Convert to lowercase as the md5 checksum calculated in Django is calculated with lower case latters.
}

std::expected<std::filesystem::file_time_type, std::string> MqttManagerConfig::_get_file_mtime(std::string file_path) {
  std::error_code ec;
  auto mtime = std::filesystem::last_write_time(file_path, ec);
  if (ec) [[unlikely]] {
    return std::unexpected<std::string>(fmt::format("Failed to get last write time (mtime) file file '{}'. Error code: '{}', message: {}", file_path, ec.value(), ec.message()));
  }
  return mtime;
}

std::expected<std::string, bool> MqttManagerConfig::get_firmware_sonoff_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/firmware/sonoff/firmware.bin") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/firmware/sonoff/firmware.bin"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_firmware_custom_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/firmware/custom/firmware.bin") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/firmware/custom/firmware.bin"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_littlefs_sonoff_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/firmware/sonoff/data_file.bin") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/firmware/sonoff/data_file.bin"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_littlefs_custom_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/firmware/custom/data_file.bin") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/firmware/custom/data_file.bin"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft1/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft1/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft2/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft2/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft3/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft3/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft4/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft4/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft1/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft1/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft2/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft2/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft3/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft3/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft4/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft4/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft1/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft1/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft2/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft2/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft3/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft3/gui.tft"].checksum;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_checksums_mutex);
  if (MqttManagerConfig::_checksums.find("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft4/gui.tft") == MqttManagerConfig::_checksums.end()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_checksums["/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft4/gui.tft"].checksum;
}

std::string MqttManagerConfig::read_posix_tz_rule(const std::string &tzif_path) {
  try {
    std::ifstream file(tzif_path, std::ios::in | std::ios::binary);
    if (!file) {
      return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Version 2 and later files end with the rule between two newlines (RFC 8536, section 3.3).
    // Version 1 files have a zero version byte and no rule.
    if (content.size() < 5 || content.compare(0, 4, "TZif") != 0 || content[4] < '2' || content.back() != '\n') {
      return "";
    }
    size_t rule_start = content.rfind('\n', content.size() - 2);
    if (rule_start == std::string::npos) {
      return "";
    }
    return content.substr(rule_start + 1, content.size() - rule_start - 2);
  } catch (std::exception &ex) {
    SPDLOG_WARN("Caught exception while reading POSIX TZ rule from '{}': {}", tzif_path, ex.what());
    return "";
  }
}

// TESTING
#if defined(TEST_MODE) && TEST_MODE == 1
#include <gtest/gtest.h>

TEST(MqttManagerConfigTest, verify_all_settings_exists_and_have_db_key) {
  for (int i = 0; i < static_cast<int>(MQTT_MANAGER_SETTING::LAST); i++) {
    MQTT_MANAGER_SETTING setting = static_cast<MQTT_MANAGER_SETTING>(i);
    bool found = MqttManagerConfig::_setting_key_map.contains(setting);
    EXPECT_TRUE(found) << "Setting (enum value) " << i << " not found in the settings list. Cannot test if it has a setting key.";
    if (found) {
      EXPECT_TRUE(!MqttManagerConfig::_setting_key_map[setting].first.empty()) << "Setting (enum value) " << i << " does not name a DB key.";
    }
  }
}

TEST(MqttManagerConfigTest, verify_settings_cache_is_used) {
  bool value = MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::CLOCK_US_STYLE);
  EXPECT_TRUE(MqttManagerConfig::_settings_values_cache.contains(MQTT_MANAGER_SETTING::CLOCK_US_STYLE));
}

#endif
