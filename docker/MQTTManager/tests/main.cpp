#include <cstdio>
#include <cstdlib>
#include <database_manager/database_manager.hpp>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

// In TEST_MODE the database is a scratch file (/tmp/nspanelmanager_db_test.sqlite3) whose
// schema is created from the sqlite_orm table definitions by database_manager::init().
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  spdlog::set_level(spdlog::level::warn); // Keep test output readable; raise when debugging.
  database_manager::init();
  int result = RUN_ALL_TESTS();

  // Skip static destructors. Each Room runs a detached thread that waits on the room's own mutex
  // and condition variable forever; destroying EntityManager's static room list at exit pulls
  // those out from under the threads and segfaults after every test has passed. The manager
  // itself never exits this way. Results, including the JUnit XML, are written by now.
  spdlog::default_logger()->flush();
  std::fflush(nullptr);
  std::_Exit(result);
}
