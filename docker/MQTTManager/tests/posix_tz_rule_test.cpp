#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <string>

namespace {

// A tzdata file of the given version: the header, a stand-in for the binary data (which can hold
// newlines), then, from version 2, the rule between two newlines.
std::string tzif(char version, const std::string &rule) {
  std::string content = std::string("TZif") + version + std::string(15, '\0') + "\x01\n\x02\n\x03";
  if (version != '\0') {
    content += "\n" + rule + "\n";
  }
  return content;
}

class PosixTzRuleTest : public ::testing::Test {
protected:
  void SetUp() override {
    path = std::filesystem::temp_directory_path() / "nspm_posix_tz_rule_test";
  }

  void TearDown() override {
    std::filesystem::remove(path);
  }

  std::string read(const std::string &content) {
    std::ofstream(path, std::ios::binary) << content;
    return MqttManagerConfig::read_posix_tz_rule(path);
  }

  std::filesystem::path path;
};

} // namespace

TEST_F(PosixTzRuleTest, reads_the_rule_from_a_version_2_file) {
  EXPECT_EQ(read(tzif('2', "CET-1CEST,M3.5.0,M10.5.0/3")), "CET-1CEST,M3.5.0,M10.5.0/3");
}

TEST_F(PosixTzRuleTest, reads_the_rule_from_a_version_3_file) {
  EXPECT_EQ(read(tzif('3', "IST-2IDT,M3.4.4/26,M10.5.0")), "IST-2IDT,M3.4.4/26,M10.5.0");
}

TEST_F(PosixTzRuleTest, version_1_file_has_no_rule) {
  EXPECT_EQ(read(tzif('\0', "")), "");
}

TEST_F(PosixTzRuleTest, empty_rule_is_empty) {
  EXPECT_EQ(read(tzif('2', "")), "");
}

TEST_F(PosixTzRuleTest, file_that_is_not_tzdata_has_no_rule) {
  EXPECT_EQ(read("Europe/Stockholm\n"), "");
  EXPECT_EQ(read(""), "");
}

TEST_F(PosixTzRuleTest, missing_file_has_no_rule) {
  EXPECT_EQ(MqttManagerConfig::read_posix_tz_rule("/nonexistent/zoneinfo/Europe/Stockholm"), "");
}

// Against the real tzdata the manager image ships, if this one has it too.
TEST(PosixTzRuleSystemTest, reads_the_rule_from_installed_tzdata) {
  const std::string utc = "/usr/share/zoneinfo/UTC";
  if (!std::filesystem::exists(utc)) {
    GTEST_SKIP() << "No tzdata installed at " << utc;
  }
  EXPECT_EQ(MqttManagerConfig::read_posix_tz_rule(utc), "UTC0");
}
