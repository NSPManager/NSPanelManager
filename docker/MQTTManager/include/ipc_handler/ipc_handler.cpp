#include "ipc_handler/ipc_handler.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <zmq.hpp>

void IPCHandler::start() {
  IPCHandler::_instance = std::thread(IPCHandler::_listen);
}

void IPCHandler::_listen() {
  try {
    SPDLOG_INFO("Starting ZMQ IPCHandler.");
    zmq::context_t zmq_context{1};                            // Number of ZMQ threads
    zmq::socket_t socket{zmq_context, zmq::socket_type::rep}; // Create reply socket with above context
    socket.bind("ipc:///tmp/mqttmanager_ipc");

    std::string response_data = "";
    while (true) {
      try {
        zmq::message_t request;
        zmq::recv_result_t result = socket.recv(request, zmq::recv_flags::none);
        std::string message = request.to_string();

        // Parse data to JSON and check validity of message
        nlohmann::json data = nlohmann::json::parse(message);
        if (data.contains("topic") && data["topic"].is_string()) {
          SPDLOG_TRACE("Received '{}' on IPC.", message);
          std::string topic = data["topic"];

          bool found_handler = false;
          for (auto topic_handler_pair : IPCHandler::_registered_handlers) {
            if (topic_handler_pair.first.compare(topic) == 0) {
              if (IPCHandler::_registered_handlers[topic](message, &response_data)) {
                found_handler = true;
                socket.send(zmq::buffer(response_data), zmq::send_flags::none);
                break;
              }
            }
          }

          if (!found_handler) {
            SPDLOG_ERROR("Received IPC command on topic '{}' but no handler was found. Sending status error.", topic);
            socket.send(zmq::buffer("{\"status\": \"error\"}"), zmq::send_flags::none);
          }
        } else {
          SPDLOG_ERROR("Received invalid IPC data. topic is missing or not a string: {}", request.to_string());
          socket.send(zmq::buffer("{\"status\": \"error\"}"), zmq::send_flags::none);
        }
      } catch (const std::exception &e) {
        SPDLOG_ERROR("Caught exception when trying to process message from IPC: {}", boost::diagnostic_information(e, true));
        socket.send(zmq::buffer("{\"status\": \"error\"}"), zmq::send_flags::none);
      }
    }
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Caught exception when trying to use ZMQ for IPC: {}", boost::diagnostic_information(e, true));
  }
}
