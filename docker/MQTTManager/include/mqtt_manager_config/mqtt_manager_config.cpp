#include "mqtt_manager_config.hpp"
#include "openssl/evp.h"
#include "web_helper/WebHelper.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <database_manager/database_manager.hpp>
#include <exception>
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

MqttManagerSettingsHolder MqttManagerConfig::get_settings() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_settings_mutex);
  return MqttManagerConfig::_settings;
}

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

  {
    SPDLOG_INFO("Loading MQTT Manager settings.");
    std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_settings_mutex);

    MqttManagerConfig::_settings.manager_address = MqttManagerConfig::get_setting_with_default("manager_address", "");
    MqttManagerConfig::_settings.manager_port = std::stoi(MqttManagerConfig::get_setting_with_default("manager_port", "8000"));
    MqttManagerConfig::_settings.color_temp_min = std::stoi(MqttManagerConfig::get_setting_with_default("color_temp_min", "2000"));
    MqttManagerConfig::_settings.color_temp_max = std::stoi(MqttManagerConfig::get_setting_with_default("color_temp_max", "6000"));
    MqttManagerConfig::_settings.reverse_color_temperature_slider = MqttManagerConfig::get_setting_with_default("reverse_color_temp", "False").compare("True") == 0;
    MqttManagerConfig::_settings.date_format = MqttManagerConfig::get_setting_with_default("date_format", "%a %d/%m/ %Y");
    MqttManagerConfig::_settings.clock_24_hour_format = MqttManagerConfig::get_setting_with_default("clock_us_style", "False").compare("False") == 0;
    MqttManagerConfig::_settings.optimistic_mode = MqttManagerConfig::get_setting_with_default("optimistic_mode", "True").compare("True") == 0;
    MqttManagerConfig::_settings.mqtt_wait_time = std::stoi(MqttManagerConfig::get_setting_with_default("mqtt_wait_time", "1000"));

    const char *is_home_assistant_addon = std::getenv("IS_HOME_ASSISTANT_ADDON");
    if (is_home_assistant_addon != nullptr) {
      if (std::string(is_home_assistant_addon).compare("true") == 0) {
        MqttManagerConfig::_settings.is_home_assistant_addon = true;
      } else {
        MqttManagerConfig::_settings.is_home_assistant_addon = false;
      }
    } else {
      MqttManagerConfig::_settings.is_home_assistant_addon = false;
    }

    std::string turn_on_bevaiour = MqttManagerConfig::get_setting_with_default("turn_on_behaviour", "color_temp");
    if (turn_on_bevaiour.compare("color_temp") == 0) {
      MqttManagerConfig::_settings.light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;
    } else if (turn_on_bevaiour.compare("restore") == 0) {
      MqttManagerConfig::_settings.light_turn_on_behaviour = LightTurnOnBehaviour::RESTORE_PREVIOUS;
    } else {
      SPDLOG_WARN("Failed to determine turn on bevaiour for lights, assuming color temp. Value set: {}", turn_on_bevaiour);
      MqttManagerConfig::_settings.light_turn_on_behaviour = LightTurnOnBehaviour::COLOR_TEMPERATURE;
    }
  }

  MqttManagerConfig::update_firmware_checksum();
  MqttManagerConfig::update_tft_checksums();

  // Notify all listeners that the config has been loaded
  MqttManagerConfig::_config_loaded_listeners();
}

std::string MqttManagerConfig::get_setting_with_default(std::string key, std::string default_value) {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_database_access_mutex);
  try {
    auto result = database_manager::database.get_all<database_manager::SettingHolder>(sqlite_orm::where(sqlite_orm::c(&database_manager::SettingHolder::name) == key));
    if (result.size() > 0) [[likely]] {
      SPDLOG_TRACE("Found setting {} with value {}", key, result[0].value);
      return result[0].value;
    } else {
      SPDLOG_TRACE("Did not find setting {}. Returning default:   {}", key, default_value);
      return default_value;
    }
  } catch (std::exception &ex) {
    SPDLOG_ERROR("Caught exception while trying to access database to retrieve setting {}. Exception: {}", key, boost::diagnostic_information(ex));
  }
  SPDLOG_TRACE("Did not find setting {}. Returning default:   {}", key, default_value);
  return default_value;
}

void MqttManagerConfig::set_nspanel_setting_value(int32_t nspanel_id, std::string key, std::string value) {
  SPDLOG_DEBUG("Setting '{}' to value '{}' for NSPanel with ID {}", key, value, nspanel_id);

  database_manager::NSPanelSettingHolder setting;
  setting.nspanel_id = nspanel_id;
  setting.name = key;
  setting.value = value;
  database_manager::database.insert(setting);
}

void MqttManagerConfig::update_firmware_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  SPDLOG_INFO("Updating/calculating MD5 checksums for all firmware files.");

  auto firmware_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/firmware.bin");
  if (firmware_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_firmware = firmware_checksum.value();
    SPDLOG_INFO("Firmware checksum: {}", firmware_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for firmware!");
  }

  auto littlefs_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/data_file.bin");
  if (littlefs_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_littlefs = littlefs_checksum.value();
    SPDLOG_INFO("LittleFS checksum: {}", littlefs_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for LittleFS!");
  }
}

void MqttManagerConfig::update_tft_checksums() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  SPDLOG_INFO("Updating/calculating MD5 checksums for all .tft GUI files.");

  auto eu_tft1_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft1/gui.tft");
  if (eu_tft1_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft1 = eu_tft1_checksum.value();
    SPDLOG_INFO("EU TFT1 GUI file checksum: {}", eu_tft1_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT1 Nextion GUI file!");
  }

  auto eu_tft2_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft2/gui.tft");
  if (eu_tft2_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft2 = eu_tft2_checksum.value();
    SPDLOG_INFO("EU TFT2 GUI file checksum: {}", eu_tft2_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT2 Nextion GUI file!");
  }

  auto eu_tft3_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft3/gui.tft");
  if (eu_tft3_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft3 = eu_tft3_checksum.value();
    SPDLOG_INFO("EU TFT3 GUI file checksum: {}", eu_tft3_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT3 Nextion GUI file!");
  }

  auto eu_tft4_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/eu/output_tft4/gui.tft");
  if (eu_tft4_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_eu_tft4 = eu_tft4_checksum.value();
    SPDLOG_INFO("EU TFT4 GUI file checksum: {}", eu_tft4_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for EU TFT4 Nextion GUI file!");
  }

  auto us_tft1_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft1/gui.tft");
  if (us_tft1_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft1 = us_tft1_checksum.value();
    SPDLOG_INFO("US TFT1 GUI file checksum: {}", us_tft1_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for US TFT1 Nextion GUI file!");
  }

  auto us_tft2_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft2/gui.tft");
  if (us_tft2_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft2 = us_tft2_checksum.value();
    SPDLOG_INFO("US TFT2 GUI file checksum: {}", us_tft2_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for US TFT2 Nextion GUI file!");
  }

  auto us_tft3_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft3/gui.tft");
  if (us_tft3_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft3 = us_tft3_checksum.value();
    SPDLOG_INFO("US TFT3 GUI file checksum: {}", us_tft3_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for US TFT3 Nextion GUI file!");
  }

  auto us_tft4_checksum = MqttManagerConfig::_get_file_md5_checksum("/usr/src/app/nspanelmanager/HMI_files/tft_automation/us/output_tft4/gui.tft");
  if (us_tft4_checksum.has_value()) {
    MqttManagerConfig::_md5_checksum_us_tft4 = us_tft4_checksum.value();
    SPDLOG_INFO("US TFT4 GUI file checksum: {}", us_tft4_checksum.value());
  } else {
    SPDLOG_ERROR("Failed to calculate checksum for US TFT4 Nextion GUI file!");
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

std::string MqttManagerConfig::get_firmware_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_firmware;
}

std::string MqttManagerConfig::get_littlefs_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_littlefs;
}

std::string MqttManagerConfig::get_eu_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_eu_tft1;
}

std::string MqttManagerConfig::get_eu_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_eu_tft2;
}

std::string MqttManagerConfig::get_eu_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_eu_tft3;
}

std::string MqttManagerConfig::get_eu_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_eu_tft4;
}

std::string MqttManagerConfig::get_us_tft1_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_us_tft1;
}

std::string MqttManagerConfig::get_us_tft2_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_us_tft2;
}

std::string MqttManagerConfig::get_us_tft3_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_us_tft3;
}

std::string MqttManagerConfig::get_us_tft4_checksum() {
  std::lock_guard<std::mutex> lock_guard(MqttManagerConfig::_md5_checksum_files_mutex);
  return MqttManagerConfig::_md5_checksum_us_tft4;
}
