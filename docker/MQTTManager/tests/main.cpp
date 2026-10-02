#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <database_manager/database_manager.hpp>
#include <execinfo.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <string>

namespace {

// Demangles the symbol in a backtrace_symbols() line, "binary(_ZN4Room...+0x1f) [0x...]".
std::string demangle_frame(const char *frame) {
  std::string line = frame;
  size_t open = line.find('(');
  size_t plus = line.find('+', open);
  if (open == std::string::npos || plus == std::string::npos || plus == open + 1) {
    return line;
  }
  int status = 0;
  char *name = abi::__cxa_demangle(line.substr(open + 1, plus - open - 1).c_str(), nullptr, nullptr, &status);
  if (status != 0 || name == nullptr) {
    return line;
  }
  line = line.substr(0, open + 1) + name + line.substr(plus);
  std::free(name);
  return line;
}

// A crash otherwise only shows up as exit code 139, with the last buffered output lost. Not
// async-signal-safe, but the process is going down anyway and this is only for diagnostics.
void report_crash(int signal_number) {
  auto *test = ::testing::UnitTest::GetInstance()->current_test_info();
  std::fprintf(stderr, "\n*** Caught signal %d (%s) during %s%s%s\n", signal_number, strsignal(signal_number), test ? test->test_suite_name() : "(no test running)", test ? "." : "", test ? test->name() : "");
  void *frames[64];
  int count = backtrace(frames, 64);
  char **symbols = backtrace_symbols(frames, count);
  for (int i = 0; i < count; i++) {
    std::fprintf(stderr, "  %s\n", symbols ? demangle_frame(symbols[i]).c_str() : "?");
  }
  std::fflush(stderr);
  std::signal(signal_number, SIG_DFL);
  std::raise(signal_number);
}

} // namespace

// In TEST_MODE the database is a scratch file (/tmp/nspanelmanager_db_test.sqlite3) whose
// schema is created from the sqlite_orm table definitions by database_manager::init().
int main(int argc, char **argv) {
  std::setvbuf(stdout, nullptr, _IOLBF, 0); // Keep output up to a crash, even when piped.
  for (int signal_number : {SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT}) {
    std::signal(signal_number, report_crash);
  }

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
