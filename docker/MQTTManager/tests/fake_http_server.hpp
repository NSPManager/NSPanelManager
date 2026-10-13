#pragma once

#include <ixwebsocket/IXHttpServer.h>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace nspm_test {

// A minimal HTTP server on 127.0.0.1 for code that calls REST APIs (OpenHAB). It answers each
// request with the body registered for its path, or 404, and records every request it gets.
// Built on ix::HttpServer, which the Nextion image server already uses.
class FakeHttpServer {
public:
  struct Request {
    std::string method;
    std::string path;
    ix::WebSocketHttpHeaders headers; // Case-insensitive keys.
  };

  FakeHttpServer() {
    // ix::HttpServer can't bind to port 0 and report the port it got, so try ports until one is free.
    for (int port = 18800; port < 18900 && !_server; port++) {
      auto server = std::make_unique<ix::HttpServer>(port, "127.0.0.1");
      if (server->listen().first) {
        _server = std::move(server);
        _port = port;
      }
    }
    if (!_server) {
      throw std::runtime_error("FakeHttpServer could not listen on 127.0.0.1");
    }
    _server->setOnConnectionCallback([this](ix::HttpRequestPtr request, std::shared_ptr<ix::ConnectionState>) { return _handle(request); });
    _server->start();
  }

  ~FakeHttpServer() {
    _server->stop();
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
  ix::HttpResponsePtr _handle(ix::HttpRequestPtr request) {
    std::lock_guard<std::mutex> lock_guard(_mutex);
    _requests.push_back({request->method, request->uri, request->headers});
    auto body = _responses.find(request->uri);
    if (body == _responses.end()) {
      return std::make_shared<ix::HttpResponse>(404, "Not Found", ix::HttpErrorCode::Ok, ix::WebSocketHttpHeaders(), "");
    }
    ix::WebSocketHttpHeaders headers;
    headers["Content-Type"] = "application/json";
    return std::make_shared<ix::HttpResponse>(200, "OK", ix::HttpErrorCode::Ok, headers, body->second);
  }

  std::unique_ptr<ix::HttpServer> _server;
  int _port = 0;
  std::mutex _mutex;
  std::map<std::string, std::string> _responses;
  std::vector<Request> _requests;
};

} // namespace nspm_test
