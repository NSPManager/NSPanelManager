#pragma once

#include <arpa/inet.h>
#include <atomic>
#include <cctype>
#include <cstdlib>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace nspm_test {

// A minimal HTTP server on 127.0.0.1 for code that calls REST APIs (OpenHAB). It answers each
// request with the body registered for its path, or 404, and records every request it gets.
class FakeHttpServer {
public:
  struct Request {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers; // Header names in lower case.
  };

  FakeHttpServer() {
    _listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    setsockopt(_listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0; // Any free port.
    if (bind(_listen_socket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != 0 || listen(_listen_socket, 16) != 0) {
      throw std::runtime_error("FakeHttpServer could not listen on 127.0.0.1");
    }
    socklen_t length = sizeof(address);
    getsockname(_listen_socket, reinterpret_cast<sockaddr *>(&address), &length);
    _port = ntohs(address.sin_port);
    _thread = std::thread(&FakeHttpServer::_serve, this);
  }

  ~FakeHttpServer() {
    _stopping = true;
    shutdown(_listen_socket, SHUT_RDWR); // Wakes accept().
    close(_listen_socket);
    _thread.join();
  }

  FakeHttpServer(const FakeHttpServer &) = delete;
  FakeHttpServer &operator=(const FakeHttpServer &) = delete;

  std::string address() const {
    return "http://127.0.0.1:" + std::to_string(_port);
  }

  void respond(const std::string &path, const std::string &body) {
    std::lock_guard<std::mutex> lock_guard(_mutex);
    _responses[path] = body;
  }

  std::vector<Request> requests() {
    std::lock_guard<std::mutex> lock_guard(_mutex);
    return _requests;
  }

private:
  void _serve() {
    while (!_stopping) {
      int connection = accept(_listen_socket, nullptr, nullptr);
      if (connection < 0) {
        continue;
      }
      _handle(connection);
      close(connection);
    }
  }

  void _handle(int connection) {
    std::string data;
    char buffer[4096];
    size_t header_end;
    while ((header_end = data.find("\r\n\r\n")) == std::string::npos) {
      ssize_t received = recv(connection, buffer, sizeof(buffer), 0);
      if (received <= 0) {
        return;
      }
      data.append(buffer, received);
    }

    Request request;
    size_t line_end = data.find("\r\n");
    std::string request_line = data.substr(0, line_end);
    size_t first_space = request_line.find(' ');
    size_t second_space = request_line.find(' ', first_space + 1);
    request.method = request_line.substr(0, first_space);
    request.path = request_line.substr(first_space + 1, second_space - first_space - 1);
    size_t position = line_end + 2;
    while (position < header_end) {
      size_t next = data.find("\r\n", position);
      std::string line = data.substr(position, next - position);
      size_t colon = line.find(':');
      if (colon != std::string::npos) {
        std::string name = line.substr(0, colon);
        for (auto &c : name) {
          c = std::tolower(c);
        }
        size_t value_start = line.find_first_not_of(' ', colon + 1);
        request.headers[name] = value_start == std::string::npos ? "" : line.substr(value_start);
      }
      position = next + 2;
    }

    // Read (and ignore) any request body so the client does not see a reset connection.
    size_t body_length = request.headers.contains("content-length") ? std::strtoul(request.headers["content-length"].c_str(), nullptr, 10) : 0;
    size_t body_received = data.size() - (header_end + 4);
    while (body_received < body_length) {
      ssize_t received = recv(connection, buffer, sizeof(buffer), 0);
      if (received <= 0) {
        break;
      }
      body_received += received;
    }

    std::string response;
    {
      std::lock_guard<std::mutex> lock_guard(_mutex);
      _requests.push_back(request);
      auto body = _responses.find(request.path);
      if (body != _responses.end()) {
        response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(body->second.size()) + "\r\nConnection: close\r\n\r\n" + body->second;
      } else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
      }
    }
    send(connection, response.data(), response.size(), MSG_NOSIGNAL);
  }

  int _listen_socket;
  uint16_t _port;
  std::atomic<bool> _stopping = false;
  std::thread _thread;
  std::mutex _mutex;
  std::map<std::string, std::string> _responses;
  std::vector<Request> _requests;
};

} // namespace nspm_test
