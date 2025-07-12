#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <codecvt>
#include <cstddef>
#include <fmt/core.h>
#include <fstream>
#include <iterator>
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <locale>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/socket.h>
#include <utility>
#include <websocket_server/websocket_server.hpp>

StompTopic::StompTopic(std::string topic, std::string current_value) {
  _topic_name = topic;
  _current_value = current_value;
}

std::string StompTopic::get_name() const {
  return _topic_name;
}

void StompTopic::subscribe(ix::WebSocket &webSocket, std::string subscription_id) {
  _subscribers.push_back(std::make_pair(&webSocket, subscription_id));

  if (this->_retained) {
    StompFrame frame;
    frame.type = StompFrame::MessageType::MESSAGE;
    frame.headers["message-id"] = boost::uuids::to_string(this->_uuid_generator());
    frame.headers["destination"] = this->_topic_name;
    frame.headers["subscription"] = subscription_id;

    if (this->_current_json_data.empty()) {
      frame.headers["content-type"] = "text/plain";
      frame.body = this->_current_value;
    } else {
      frame.headers["content-type"] = "application/json";
      frame.body = this->_current_json_data.dump();
    }
    WebsocketServer::send_stomp_frame(frame, webSocket);
  }
}

void StompTopic::unsubscribe(ix::WebSocket &webSocket, std::string subscription_id) {
  _subscribers.erase(std::remove_if(_subscribers.begin(), _subscribers.end(), [&webSocket, &subscription_id](const auto &pair) {
                       return pair.first == &webSocket && (pair.second == subscription_id || subscription_id.empty()); // An empty ID is used to unsubscribe all subscriptions for the given WebSocket.
                     }),
                     _subscribers.end());
}

void StompTopic::update_value(std::string value) {
  if (this->_retained) {
    this->_current_value = value;
  }

  if (this->get_subscriber_count() > 0) {
    StompFrame frame;
    frame.type = StompFrame::MessageType::MESSAGE;
    frame.headers["message-id"] = boost::uuids::to_string(this->_uuid_generator());
    frame.headers["content-type"] = "text/plain";
    frame.headers["destination"] = this->_topic_name;
    frame.body = value;

    for (auto &subscriber : _subscribers) {
      frame.headers["subscription"] = subscriber.second;
      WebsocketServer::send_stomp_frame(frame, *subscriber.first);
    }
  }
}

void StompTopic::update_value(nlohmann::json &data) {
  if (this->_retained) {
    this->_current_json_data = data;
  }

  if (this->get_subscriber_count() > 0) {
    StompTopic::update_value(data.dump());
  }
}

void StompTopic::set_retained(bool retained) {
  this->_retained = retained;
}

int StompTopic::get_subscriber_count() const {
  return this->_subscribers.size();
}

void WebsocketServer::start() {
  if (WebsocketServer::_server == nullptr) {
    SPDLOG_DEBUG("Creating new ix::WebSocketServer.");
    WebsocketServer::_server = new ix::WebSocketServer(8002, "0.0.0.0");
  }

  WebsocketServer::_server->setOnClientMessageCallback(WebsocketServer::_websocket_message_callback);

  SPDLOG_DEBUG("Start listening for websocket connection on 0.0.0.0:8002.");
  while (true) {
    auto result = WebsocketServer::_server->listen();
    if (result.first) {
      SPDLOG_DEBUG("Successfully bound to 0.0.0.0:8002");
      break;
    } else {
      SPDLOG_ERROR("Failed to open socket on port 8002. Will try again in 1 second. Message: {}", result.second);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }

  WebsocketServer::_server->disablePerMessageDeflate();
  WebsocketServer::_server->start();
  WebsocketServer::_server->wait();
}

void WebsocketServer::_websocket_message_callback(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) {
  try {
    if (msg->type == ix::WebSocketMessageType::Open) {
      SPDLOG_DEBUG("Websocket connected. URL: {}", msg->openInfo.uri);

      if (boost::algorithm::ends_with(msg->openInfo.uri, "/stomp")) {
        std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
        WebsocketServer::_connected_websockets_stomps.push_back(&webSocket);
      } else if (boost::algorithm::ends_with(msg->openInfo.uri, "/websocket")) {
        // This is a "legacy" websocket connection that will be bit by bit replaced by STOMP.
        {
          std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
          WebsocketServer::_connected_websockets.push_back(&webSocket);
        }

        // Client just connected, send updated warnings
        std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
        nlohmann::json base;
        base["event_data"] = nlohmann::json::object();
        base["event_data"]["warnings"] = nlohmann::json::array();
        base["event_type"] = "mqttmanager_active_warnings";
        for (auto &warning : WebsocketServer::_active_warnings) {
          base["event_data"]["warnings"].push_back({{"level", warning.level}, {"text", warning.warning_text}});
        }
        std::string json = nlohmann::to_string(base);
        webSocket.sendText(json);
      }
    } else if (msg->type == ix::WebSocketMessageType::Close) {
      std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
      SPDLOG_DEBUG("Websocket closed. Code: {}, Reason: {}", msg->closeInfo.code, msg->closeInfo.reason);
      WebsocketServer::_connected_websockets.remove(&webSocket);
      WebsocketServer::_connected_websockets_stomps.remove(&webSocket);

      for (auto &topic : WebsocketServer::_stomp_topics) {
        topic.unsubscribe(webSocket, "");
      }
    } else if (msg->type == ix::WebSocketMessageType::Message) {
      if (std::find(WebsocketServer::_connected_websockets.begin(), WebsocketServer::_connected_websockets.end(), &webSocket) != WebsocketServer::_connected_websockets.end()) {
        // "Legacy" connection
        try {
          nlohmann::json json = nlohmann::json::parse(msg->str);
          if (std::string(json["type"]).compare("broadcast") == 0) {
            for (auto client : WebsocketServer::_server->getClients()) {
              if (json["data"].is_string()) {
                client->sendText(json["data"]);
              } else {
                client->sendText(nlohmann::to_string(json["data"]));
              }
            }
            return;
          }
        } catch (...) {
        }

        SPDLOG_TRACE("Got message: {}", msg->str);
        std::string message = msg->str;
        std::string response_buffer;
        for (auto callback : WebsocketServer::_callbacks) {
          if (callback(message, &response_buffer)) {
            webSocket.sendText(response_buffer);
            break;
          }
        }
      } else if (std::find(WebsocketServer::_connected_websockets_stomps.begin(), WebsocketServer::_connected_websockets_stomps.end(), &webSocket) != WebsocketServer::_connected_websockets_stomps.end()) {
        // This is a STOMP heartbeat. Ignore it.
        if (msg->str.length() == 1 && *msg->str.data() == '\n') {
          return;
        }

        // STOMP connection
        std::string data = msg->str;
        std::optional<StompFrame> frame = WebsocketServer::decode_stomp_frame(data);
        if (frame.has_value()) {
          // Process STOMP frame

          if (frame->type == StompFrame::CONNECT) {
            // Client sent CONNECT frame, answer with CONNECTED frame if same protocol version can be negotiated
            if (frame->headers.find("accept-version") != frame->headers.end()) {
              SPDLOG_DEBUG("Got STOMP connection request, accepting request, sending CONNECTED");
              std::string accept_version_header = frame->headers["accept-version"];
              std::vector<std::string> accepted_versions;
              boost::split(accepted_versions, accept_version_header, boost::is_any_of(","));

              std::string heartbeat_header = "0,0";
              if (frame->headers.find("heart-beat") != frame->headers.end()) {
                heartbeat_header = frame->headers["heart-beat"];
              }
              std::vector<std::string> accepted_heartbeats;
              boost::split(accepted_heartbeats, heartbeat_header, boost::is_any_of(","));

              if (std::find(accepted_versions.begin(), accepted_versions.end(), "1.2") != accepted_versions.end()) {
                // Generate UUID for this connection.
                boost::uuids::uuid uuid = WebsocketServer::_uuid_generator();

                StompFrame connected_frame;
                connected_frame.type = StompFrame::CONNECTED;
                connected_frame.headers["session"] = boost::uuids::to_string(uuid);
                connected_frame.headers["version"] = "1.2";
                connected_frame.headers["server"] = "NSPanelManager";
                if (std::stoi(accepted_heartbeats[0]) != 0) {
                  connected_frame.headers["heart-beat"] = "0,1000";
                }

                WebsocketServer::send_stomp_frame(connected_frame, webSocket);
              } else {
                SPDLOG_ERROR("STOMP client tried to connect but no supported versions exists. MQTTManager supports version 1.2, client supports {}", accept_version_header);
                StompFrame error_frame;
                error_frame.type = StompFrame::ERROR;
                error_frame.headers["version"] = "1.2";
                error_frame.body = "Supported protocaol versions are 1.2";
                WebsocketServer::send_stomp_frame(error_frame, webSocket);
              }
            } else {
              SPDLOG_ERROR("Client did not specify supported versions!");
            }
          } else if (frame->type == StompFrame::SUBSCRIBE) {
            if (frame->headers.find("destination") == frame->headers.end()) {
              SPDLOG_ERROR("Got STOMP frame to subscribe to topic but not 'destination' was set!");
              return;
            } else if (frame->headers.find("id") == frame->headers.end()) {
              SPDLOG_ERROR("Got STOMP frame to subscribe to topic but not 'id' was set!");
              return;
            }

            SPDLOG_DEBUG("STOMP subscribing to topic {} (id: {})", frame->headers["destination"], frame->headers["id"]);
            for (auto &topic : WebsocketServer::_stomp_topics) {
              if (topic.get_name() == frame->headers["destination"]) {
                std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
                topic.subscribe(webSocket, frame->headers["id"]);
                return; // We found the topics and subscribed to it, nothing else to do.
              }
            }

            // Topic was not found, create topic and subscribe to it
            WebsocketServer::_stomp_topics.push_back(StompTopic(frame->headers["destination"], ""));
            WebsocketServer::_stomp_topics.back().subscribe(webSocket, frame->headers["id"]);
          } else if (frame->type == StompFrame::UNSUBSCRIBE) {
            SPDLOG_DEBUG("STOMP subscription ID {} unsubscribing.", frame->headers["id"]);
            for (auto &topic : WebsocketServer::_stomp_topics) {
              if (topic.get_name() == frame->headers["destination"]) {
                std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
                topic.unsubscribe(webSocket, frame->headers["id"]);
                return; // We found the topics and unsubscribed from it, nothing else to do.
              }
            }
          } else if (frame->type == StompFrame::SEND) {
            std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
            for (auto &topic : WebsocketServer::_stomp_topics) {
              if (topic.get_name() == frame->headers["destination"]) {
                topic.update_value(frame->body);
                return; // We found the topic and published to it, nothing else to do.
              }
            }
          } else {
            SPDLOG_WARN("Received unknown STOMP frame type {}.", static_cast<int>(frame->type));
          }
        } else {
          SPDLOG_ERROR("Failed to parse STOMP frame");
        }
      }
    }
  } catch (std::exception ex) {
    SPDLOG_ERROR("Caught std::exception while processing websocket event. Exception: {}", ex.what());
  } catch (...) {
    SPDLOG_ERROR("Caught exception while processing WebSocket event.");
  }
}

void WebsocketServer::broadcast_json(nlohmann::json &json) {
  std::string data = json.dump();
  WebsocketServer::broadcast_string(data);
}

void WebsocketServer::broadcast_string(std::string &data) {
  if (WebsocketServer::_server != nullptr) {
    std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
    for (auto &websocket : WebsocketServer::_connected_websockets) {
      websocket->send(data);
    }
  } else {
    SPDLOG_ERROR("Trying to send data over WebSocket before websocket exists.");
  }
}

void WebsocketServer::update_stomp_topic_value(std::string topic_name, std::string value) {
  std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
  for (auto &topic : WebsocketServer::_stomp_topics) {
    if (topic.get_name().compare(topic_name) == 0) {
      topic.update_value(value);
      return;
    }
  }
}

void WebsocketServer::update_stomp_topic_value(std::string topic_name, nlohmann::json &value) {
  std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
  for (auto &topic : WebsocketServer::_stomp_topics) {
    if (topic.get_name().compare(topic_name) == 0) {
      topic.update_value(value);
      return;
    }
  }
}

void WebsocketServer::set_stomp_topic_retained(std::string topic_name, bool retained) {
  std::lock_guard<std::mutex> lock_guard(WebsocketServer::_server_mutex);
  for (auto &topic : WebsocketServer::_stomp_topics) {
    if (topic.get_name().compare(topic_name) == 0) {
      topic.set_retained(retained);
      return;
    }
  }

  // We didn't find topic, create it and set retained flag
  WebsocketServer::_stomp_topics.push_back(StompTopic(topic_name, ""));
  WebsocketServer::_stomp_topics.back().set_retained(retained);
}

std::optional<StompFrame> WebsocketServer::decode_stomp_frame(std::string &data) {
  try {
    StompFrame frame;

    std::istringstream iss(data);
    std::string line;
    // Parse the STOMP frame and command, leave data
    while (std::getline(iss, line)) {
      if (line.empty()) {
        break;
      }
      size_t colon_pos = line.find(':');
      if (colon_pos != std::string::npos) {
        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);
        boost::replace_all(value, "\\r", "\r");  // Replace escaped carriage return characters with actual carriage return characters
        boost::replace_all(value, "\\n", "\n");  // Replace escaped newline characters with actual newline characters
        boost::replace_all(value, "\\c", ":");   // Replace escaped colon characters with actual colon characters
        boost::replace_all(value, "\\\\", "\\"); // Replace escaped backslash characters with actual backslash characters
        // As per specification https://stomp.github.io/stomp-specification-1.2.html#Standard_Headers only the first header is used
        if (frame.headers.find(key) == frame.headers.end()) {
          frame.headers[key] = value;
        }
      } else {
        if (line.compare("CONNECT") == 0) {
          frame.type = StompFrame::CONNECT;
        } else if (line.compare("DISCONNECT") == 0) {
          frame.type = StompFrame::DISCONNECT;
        } else if (line.compare("SEND") == 0) {
          frame.type = StompFrame::SEND;
        } else if (line.compare("SUBSCRIBE") == 0) {
          frame.type = StompFrame::SUBSCRIBE;
        } else if (line.compare("UNSUBSCRIBE") == 0) {
          frame.type = StompFrame::UNSUBSCRIBE;
        } else if (line.compare("ACK") == 0) {
          frame.type = StompFrame::ACK;
        } else if (line.compare("NACK") == 0) {
          frame.type = StompFrame::NACK;
        } else if (line.compare("CONNECTED") == 0) {
          frame.type = StompFrame::CONNECTED;
        } else if (line.compare("MESSAGE") == 0) {
          frame.type = StompFrame::MESSAGE;
        } else if (line.compare("RECEIPT") == 0) {
          frame.type = StompFrame::RECEIPT;
        } else if (line.compare("ERROR") == 0) {
          frame.type = StompFrame::ERROR;
        }
      }
    }

    if (frame.headers.find("content-length") != frame.headers.end()) {
      int content_length = std::stoi(frame.headers["content-length"]);
      frame.body.resize(content_length);
      iss.read(frame.body.data(), content_length);
    } else {
      char data_byte;
      int count = 0;
      while ((data_byte = iss.get()) != '\0' && !iss.eof() && !iss.fail()) {
        frame.body.push_back(data_byte);

        count++;
        if (count > 100000) {
          SPDLOG_WARN("Possible infinite loop detected while decoding STOMP frame. Will stop reading and discard the frame.");
          return {};
        }
      }
    }
    return frame;
  } catch (...) {
    return {}; // Return an empty optional to indicate failure to decode STOMP frame
  }
}

void WebsocketServer::send_stomp_frame(StompFrame &frame, ix::WebSocket &websocket) {
  std::string message;
  switch (frame.type) {
  case StompFrame::CONNECT:
    message = "CONNECT\n";
    break;
  case StompFrame::DISCONNECT:
    message = "DISCONNECT\n";
    break;
  case StompFrame::SEND:
    message = "SEND\n";
    break;
  case StompFrame::SUBSCRIBE:
    message = "SUBSCRIBE\n";
    break;
  case StompFrame::UNSUBSCRIBE:
    message = "UNSUBSCRIBE\n";
    break;
  case StompFrame::ACK:
    message = "ACK\n";
    break;
  case StompFrame::NACK:
    message = "NACK\n";
    break;

  case StompFrame::CONNECTED:
    message = "CONNECTED\n";
    break;
  case StompFrame::MESSAGE:
    message = "MESSAGE\n";
    break;
  case StompFrame::RECEIPT:
    message = "RECEIPT\n";
    break;
  case StompFrame::ERROR:
    message = "ERROR\n";
    break;
  }

  if (frame.body.length() > 0) {
    frame.headers["content-length"] = std::to_string(frame.body.length());
  }

  // TODO: Make content-type adjustable
  frame.headers["content-type"] = "text/plain;charset=utf-8";

  for (auto &header : frame.headers) {
    message.append(header.first);
    message.append(":");
    message.append(header.second);
    message.append("\n");
  }

  message.append("\n");
  message.append(frame.body);
  message.push_back('\0');
  message.push_back('\n');

  websocket.send(message);
}

void WebsocketServer::attach_message_callback(std::function<bool(std::string &message, std::string *response_buf)> callback) {
  WebsocketServer::_callbacks.push_back(callback);
}

void WebsocketServer::register_warning(ActiveWarningLevel level, std::string warning_text) {
  bool send_update = false;
  {
    bool found = false;
    std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
    for (auto &warning : WebsocketServer::_active_warnings) {
      if (warning.warning_text.compare(warning_text) == 0) {
        if (warning.level != level) {
          warning.level = level;
          send_update = true;
        }
        send_update = true;
        found = true;
        break;
      }
    }

    if (!found) {
      SPDLOG_DEBUG("Adding warning {}", warning_text);
      WebsocketServer::_active_warnings.push_back({level, warning_text});
      send_update = true;
    }
  }

  if (send_update) {
    WebsocketServer::_send_active_warnings();
  }
}

void WebsocketServer::remove_warning(std::string warning_text) {
  bool send_update = false;
  {
    std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
    auto it = WebsocketServer::_active_warnings.begin();
    while (it != WebsocketServer::_active_warnings.end()) {
      if (it->warning_text.compare(warning_text) == 0) {
        SPDLOG_DEBUG("Removing warning {}", warning_text);
        it = WebsocketServer::_active_warnings.erase(it);

        send_update = true;
        break;
      } else {
        ++it;
      }
    }
  }

  if (send_update) {
    WebsocketServer::_send_active_warnings();
  }
}

void WebsocketServer::_send_active_warnings() {
  std::lock_guard<std::mutex> lock_guard(WebsocketServer::_active_warnings_mutex);
  SPDLOG_DEBUG("Broadcasting active warnings to all connected weboscket clients.");
  nlohmann::json base;
  base["event_data"] = nlohmann::json::object();
  base["event_data"]["warnings"] = nlohmann::json::array();
  base["event_type"] = "mqttmanager_active_warnings";
  for (auto &warning : WebsocketServer::_active_warnings) {
    SPDLOG_DEBUG("Sending warnings, found warning: {}", warning.warning_text);
    base["event_data"]["warnings"].push_back({{"level", warning.level}, {"text", warning.warning_text}});
  }

  WebsocketServer::broadcast_json(base);
}
