#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <pipe_handler/pipe_handler.hpp>
#include <spdlog/spdlog.h>

void PipeHandler::start() {
  if (!PipeHandler::_instance.joinable()) {
    SPDLOG_INFO("No running instance of Pipe Handler found. Will start new thread.");
    PipeHandler::_instance = std::thread(PipeHandler::_listen);
  } else {
    SPDLOG_INFO("Pipe Handler already running.");
  }
}

void PipeHandler::_listen() {
  if (boost::filesystem::exists("/test_pipe")) {
    boost::filesystem::remove("/test_pipe");
    SPDLOG_INFO("Removed existing /test_pipe.");
  }

  boost::asio::io_service ios;
  std::string buf;
  boost::process::async_pipe ap(ios, "/test_pipe");
  while (true) {
    SPDLOG_DEBUG("Start listening on pipe.");
    boost::asio::async_read_until(ap, boost::asio::dynamic_buffer(buf), '\n',
                                  [&](const boost::system::error_code &ec, std::size_t size) {
                                    if (!ec) {
                                      SPDLOG_DEBUG("Pipe recieved {} bytes: {}", size, buf);
                                    } else {
                                      SPDLOG_ERROR("Error while reading from pipe: {}", ec.message());
                                    }
                                  });
    ios.run();
    ios.reset();
  }
}
