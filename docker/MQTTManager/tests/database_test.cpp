#include <chrono>
#include <database_manager/database_manager.hpp>
#include <gtest/gtest.h>
#include <sqlite3.h>
#include <thread>

namespace {

// A second connection to the same database file, as the web interface's is.
class OtherConnection {
public:
  OtherConnection() {
    EXPECT_EQ(sqlite3_open(database_manager::database.filename().c_str(), &_db), SQLITE_OK);
  }
  ~OtherConnection() { sqlite3_close(_db); }

  void exec(const char *sql) {
    char *error = nullptr;
    EXPECT_EQ(sqlite3_exec(_db, sql, nullptr, nullptr, &error), SQLITE_OK) << (error ? error : "");
    sqlite3_free(error);
  }

private:
  sqlite3 *_db = nullptr;
};

} // namespace

// The web interface triggers a reload on every save, so a reload's reads often run while
// it still holds the write lock. They must wait for it instead of failing with "database
// is locked", which used to make a reload read settings as missing and then crash on
// std::stoi("").
TEST(DatabaseManagerTest, reads_wait_while_another_connection_holds_the_write_lock) {
  OtherConnection web;
  web.exec("BEGIN EXCLUSIVE");
  std::thread release([&web] {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    web.exec("COMMIT");
  });

  EXPECT_NO_THROW(database_manager::database.count<database_manager::SettingHolder>());

  release.join();
}
