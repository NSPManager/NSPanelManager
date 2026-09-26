#include <database_manager/database_manager.hpp>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

// In TEST_MODE the database is a scratch file (/tmp/nspanelmanager_db_test.sqlite3) whose
// schema is created from the sqlite_orm table definitions by database_manager::init().
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  spdlog::set_level(spdlog::level::warn); // Keep test output readable; raise when debugging.
  database_manager::init();
  return RUN_ALL_TESTS();
}
