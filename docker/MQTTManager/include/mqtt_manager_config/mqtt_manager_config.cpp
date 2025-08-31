#include "mqtt_manager_config.hpp"
#include "light/light.hpp"
#include "openssl/evp.h"
#include "web_helper/WebHelper.hpp"
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
#include <fmt/core.h>
#include <fstream>
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

  SPDLOG_DEBUG("Clearing config values cache.");
  MqttManagerConfig::_settings_values_cache.clear();

  {
    SPDLOG_INFO("Loading MQTT Manager settings.");
    std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_settings_mutex);

    const char *is_home_assistant_addon = std::getenv("IS_HOME_ASSISTANT_ADDON");
    if (is_home_assistant_addon != nullptr) {
      if (std::string(is_home_assistant_addon).compare("true") == 0) {
        MqttManagerConfig::_is_home_assistant_addon = true;
      } else {
        MqttManagerConfig::_is_home_assistant_addon = false;
      }
    } else {
      MqttManagerConfig::_is_home_assistant_addon = false;
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

  // Notify all listeners that the config has been loaded
  MqttManagerConfig::_config_loaded_listeners();
}

void MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING key, std::string value) {
  std::string setting_db_key = MqttManagerConfig::_setting_key_map[key].first;
  SPDLOG_DEBUG("Setting '{}' to value '{}'", setting_db_key, value);

  try {
    database_manager::SettingHolder setting = database_manager::database.get<database_manager::SettingHolder>(setting_db_key);
    setting.value = value;
    database_manager::database.update(setting);
  } catch (const std::exception &e) {
    database_manager::SettingHolder setting;
    setting.name = setting_db_key;
    setting.value = value;
    database_manager::database.insert(setting);
  }

  MqttManagerConfig::_settings_values_cache[key] = value;
}

void MqttManagerConfig::set_nspanel_setting_value(int32_t nspanel_id, std::string key, std::string value) {
  SPDLOG_DEBUG("Setting '{}' to value '{}' for NSPanel with ID {}", key, value, nspanel_id);

  auto result = database_manager::database.get_all<database_manager::NSPanelSettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::NSPanelSettingHolder::name) == key) and sqlite_orm::c(&database_manager::NSPanelSettingHolder::nspanel_id) == nspanel_id);
  if (result.size() == 0) {
    database_manager::NSPanelSettingHolder setting;
    setting.value = value;
    database_manager::database.update(setting);
  } else {
    database_manager::NSPanelSettingHolder setting;
    setting.nspanel_id = nspanel_id;
    setting.name = key;
    setting.value = value;
    database_manager::database.insert(setting);
  }
}

bool MqttManagerConfig::is_home_assistant_addon() {
  return MqttManagerConfig::_is_home_assistant_addon;
}

LightTurnOnBehaviour MqttManagerConfig::get_light_turn_on_behaviour() {
  return MqttManagerConfig::_light_turn_on_behaviour;
}

void MqttManagerConfig::update_firmware_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  SPDLOG_INFO("Updating/calculating MD5 checksums for all firmware files.");

  auto firmware_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/firmware.bin");
  if (firmware_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_firmware = firmware_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for firmware was not able to be calculated.");
    SPDLOG_INFO("Firmware checksum: {}", firmware_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for firmware was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for firmware!");
  }

  auto littlefs_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/data_file.bin");
  if (littlefs_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_littlefs = littlefs_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for littlefs/data file was not able to be calculated.");
    SPDLOG_INFO("LittleFS checksum: {}", littlefs_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for littlefs/data file was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for LittleFS!");
  }
}

void MqttManagerConfig::update_tft_checksums() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  SPDLOG_INFO("Updating/calculating MD5 checksums for all .tft GUI files.");

  auto eu_tft1_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft1/gui.tft");
  if (eu_tft1_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft1 = eu_tft1_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for EU tft1 was not able to be calculated.");
    SPDLOG_INFO("EU TFT1 GUI file checksum: {}", eu_tft1_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for EU tft1 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT1 Nextion GUI file!");
  }

  auto eu_tft2_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft2/gui.tft");
  if (eu_tft2_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft2 = eu_tft2_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for EU tft2 was not able to be calculated.");
    SPDLOG_INFO("EU TFT2 GUI file checksum: {}", eu_tft2_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for EU tft2 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT2 Nextion GUI file!");
  }

  auto eu_tft3_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft3/gui.tft");
  if (eu_tft3_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft3 = eu_tft3_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for EU tft3 was not able to be calculated.");
    SPDLOG_INFO("EU TFT3 GUI file checksum: {}", eu_tft3_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for EU tft3 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT3 Nextion GUI file!");
  }

  auto eu_tft4_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft4/gui.tft");
  if (eu_tft4_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft4 = eu_tft4_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for EU tft4 was not able to be calculated.");
    SPDLOG_INFO("EU TFT4 GUI file checksum: {}", eu_tft4_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for EU tft4 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT4 Nextion GUI file!");
  }

  auto us_tft1_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft1/gui.tft");
  if (us_tft1_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft1 = us_tft1_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft1 was not able to be calculated.");
    SPDLOG_INFO("US TFT1 GUI file checksum: {}", us_tft1_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft1 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US TFT1 Nextion GUI file!");
  }

  auto us_tft2_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft2/gui.tft");
  if (us_tft2_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft2 = us_tft2_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft2 was not able to be calculated.");
    SPDLOG_INFO("US TFT2 GUI file checksum: {}", us_tft2_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft2 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US TFT2 Nextion GUI file!");
  }

  auto us_tft3_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft3/gui.tft");
  if (us_tft3_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft3 = us_tft3_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft3 was not able to be calculated.");
    SPDLOG_INFO("US TFT3 GUI file checksum: {}", us_tft3_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft3 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US TFT3 Nextion GUI file!");
  }

  auto us_tft4_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft4/gui.tft");
  if (us_tft4_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft4 = us_tft4_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft4 was not able to be calculated.");
    SPDLOG_INFO("US TFT4 GUI file checksum: {}", us_tft4_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft4 was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US TFT4 Nextion GUI file!");
  }

  auto us_tft1_horizontal_mirrored_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft1/gui.tft");
  if (us_tft1_horizontal_mirrored_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft1 = us_tft1_horizontal_mirrored_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft1 (mirrored) was not able to be calculated.");
    SPDLOG_INFO("US landscape mirrored TFT1 GUI file checksum: {}", us_tft1_horizontal_mirrored_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft1 (mirrored) was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US landscape mirrored TFT1 Nextion GUI file!");
  }

  auto us_tft2_horizontal_mirrored_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft2/gui.tft");
  if (us_tft2_horizontal_mirrored_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft2 = us_tft2_horizontal_mirrored_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft2 (mirrored) was not able to be calculated.");
    SPDLOG_INFO("US landscape mirrored TFT2 GUI file checksum: {}", us_tft2_horizontal_mirrored_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft2 (mirrored) was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US landscape mirrored TFT2 Nextion GUI file!");
  }

  auto us_tft3_horizontal_mirrored_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft3/gui.tft");
  if (us_tft3_horizontal_mirrored_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft3 = us_tft3_horizontal_mirrored_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft3 (mirrored) was not able to be calculated.");
    SPDLOG_INFO("US landscape mirrored TFT3 GUI file checksum: {}", us_tft3_horizontal_mirrored_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft3 (mirrored) was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US landscape mirrored TFT3 Nextion GUI file!");
  }

  auto us_tft4_horizontal_mirrored_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us_horizontal_mirrored/output_tft4/gui.tft");
  if (us_tft4_horizontal_mirrored_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft4 = us_tft4_horizontal_mirrored_checksum.value();
    WebsocketServer::remove_warning("MD5 checksum for US tft4 (mirrored) was not able to be calculated.");
    SPDLOG_INFO("US landscape mirrored TFT4 GUI file checksum: {}", us_tft4_horizontal_mirrored_checksum.value());
  } else {
    WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "MD5 checksum for US tft4 (mirrored) was not able to be calculated.");
    SPDLOG_ERROR("Failed to calculate checksum for US landscape mirrored TFT4 Nextion GUI file!");
  }
}

std::optional<std::string> MqttManagerConfig::_get_file_md5_checksum(std::string file_path) {
  std::ifstream file(file_path, std::ifstream::binary);
  if (!file) {
    return std::nullopt;
  }

  EVP_MD_CTX *mdctx;
  unsigned char *md5_digest;
  unsigned int md5_digest_len = EVP_MD_size(EVP_md5());

  // MD5_Init
  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

  // MD5_Update
  // char buffer[1024];
  // while (file.read(buffer, 1024)) {
  //   EVP_DigestUpdate(mdctx, buffer, file.gcount());
  // }

  unsigned char buffer[8192];
  while (file.read(reinterpret_cast<char *>(buffer), sizeof(buffer)) || file.gcount() > 0) {
    if (EVP_DigestUpdate(mdctx, buffer, file.gcount()) != 1) {
      EVP_MD_CTX_free(mdctx);
      return "";
    }
  }

  // MD5_Final
  md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
  EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
  EVP_MD_CTX_free(mdctx);

  // char tmp[3] = {0};
  // std::string return_string;
  // for (int i = 0; i < md5_digest_len; i++) {
  //   sprintf(tmp, "%02x", md5_digest[i]);
  //   return_string.append(tmp);
  // }

  std::stringstream ss;
  ss << std::hex << std::uppercase << std::setfill('0');
  for (int i = 0; i < EVP_MD_size(EVP_md5()); i++) {
    ss << std::setw(2) << (int)md5_digest[i];
  }

  EVP_cleanup();
  return boost::algorithm::to_lower_copy(ss.str()); // Convert to lowercase as the md5 checksum calculated in Django is calculated with lower case latters.
}

std::expected<std::string, bool> MqttManagerConfig::get_firmware_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_firmware.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_firmware;
}

std::expected<std::string, bool> MqttManagerConfig::get_littlefs_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_littlefs.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_littlefs;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_eu_tft1.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_eu_tft1;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_eu_tft2.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_eu_tft2;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_eu_tft3.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_eu_tft3;
}

std::expected<std::string, bool> MqttManagerConfig::get_eu_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_eu_tft4.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_eu_tft4;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_tft1.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_tft1;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_tft2.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_tft2;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_tft3.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_tft3;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_tft4.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_tft4;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft1.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft1;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft2.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft2;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft3.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft3;
}

std::expected<std::string, bool> MqttManagerConfig::get_us_horizontal_mirrored_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  if (MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft4.empty()) {
    return std::unexpected(false);
  }
  return MqttManagerConfig::_md5_checksum_us_horizontal_mirrored_tft4;
}
