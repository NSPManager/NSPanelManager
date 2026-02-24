#include <Magick++.h>
#include <iostream>
#include <ixwebsocket/IXHttpServer.h>
#include <nextion_image_server/nextion_image_server.hpp>
#include <spdlog/spdlog.h>

void NextionImageServer::start() {
  if (NextionImageServer::_server == nullptr) {
    SPDLOG_DEBUG("Creating new ix::HTTPServer.");
    NextionImageServer::_server = new ix::HttpServer(8003, "0.0.0.0");

    NextionImageServer::_server->setOnConnectionCallback(
        [](ix::HttpRequestPtr request,
           std::shared_ptr<ix::ConnectionState> connectionState) -> ix::HttpResponsePtr {
          // Build a string for the response
          std::stringstream ss;
          ss << connectionState->getRemoteIp()
             << " "
             << request->method
             << " "
             << request->uri
             << std::endl;

          std::string content = ss.str();

          return std::make_shared<ix::HttpResponse>(200, "OK",
                                                    ix::HttpErrorCode::Ok,
                                                    ix::WebSocketHttpHeaders(),
                                                    content);
        });
  }
  auto res = _server->listen();
  if (!res.first) {
    std::cerr << res.second << std::endl;
    return;
  }
  NextionImageServer::_server->start();
  NextionImageServer::_server->wait();
}
