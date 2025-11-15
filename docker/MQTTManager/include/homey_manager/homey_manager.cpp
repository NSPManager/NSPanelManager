#include "homey_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXSocketTLSOptions.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessage.h>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <websocket_server/websocket_server.hpp>

void HomeyManager::connect()
{
    SPDLOG_DEBUG("Initializing Homey Manager component.");
    ix::initNetSystem();

    HomeyManager::reload_config();

    bool init_success = false;
    while (!init_success)
    {
        try
        {
            std::lock_guard<std::mutex> lock_guard(HomeyManager::_settings_mutex);
            if (HomeyManager::_websocket == nullptr)
            {
                HomeyManager::_websocket = new ix::WebSocket();
                HomeyManager::_websocket->setPingInterval(30);
            }

            std::string homey_websocket_url = HomeyManager::_homey_address;
            if (homey_websocket_url.empty())
            {
                SPDLOG_ERROR("No Homey address configured. Will not continue to load Homey component.");
                return;
            }

            WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "Homey not connected.");

            // Construct WebSocket URL: ws://homey.local/api/manager/devices/device?token=TOKEN
            boost::algorithm::replace_first(homey_websocket_url, "https://", "ws://");
            boost::algorithm::replace_first(homey_websocket_url, "http://", "ws://");

            // Remove trailing slash if present
            if (boost::algorithm::ends_with(homey_websocket_url, "/"))
            {
                homey_websocket_url = homey_websocket_url.substr(0, homey_websocket_url.length() - 1);
            }

            homey_websocket_url.append("/api/manager/devices/device?token=");
            homey_websocket_url.append(HomeyManager::_homey_token);

            if (boost::algorithm::starts_with(homey_websocket_url, "wss://"))
            {
                SPDLOG_DEBUG("Setting TLS options for Homey WebSocket");
                ix::SocketTLSOptions tls_options;
                tls_options.tls = true;
                tls_options.caFile = "NONE";
                HomeyManager::_websocket->setTLSOptions(tls_options);
            }

            SPDLOG_INFO("Will connect to Homey websocket at {}", homey_websocket_url);
            HomeyManager::_websocket->setUrl(homey_websocket_url);
            HomeyManager::_websocket->setOnMessageCallback(&HomeyManager::_websocket_message_callback);
            HomeyManager::_websocket->start();

            init_success = true; // Successfully reached end of section without exception
        }
        catch (std::exception &e)
        {
            SPDLOG_ERROR("Caught exception: {}", e.what());
            SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
        }
    }
}

void HomeyManager::reload_config()
{
    bool reconnect = false;
    {
        std::lock_guard<std::mutex> lock_guard(HomeyManager::_settings_mutex);
        std::string address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS);
        std::string token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN);

        if (HomeyManager::_homey_address.compare(address) != 0 || HomeyManager::_homey_token.compare(token) != 0)
        {
            HomeyManager::_homey_address = address;
            HomeyManager::_homey_token = token;
            reconnect = true;
        }
    }

    if (reconnect)
    {
        SPDLOG_INFO("Will connect to Homey with new settings. Server address: {}", HomeyManager::_homey_address);
        if (HomeyManager::_websocket != nullptr)
        {
            HomeyManager::_websocket->close();
            HomeyManager::connect();
        }
    }
}

void HomeyManager::_websocket_message_callback(const ix::WebSocketMessagePtr &msg)
{
    if (msg->type == ix::WebSocketMessageType::Message)
    {
        HomeyManager::_process_websocket_message(msg->str);
    }
    else if (msg->type == ix::WebSocketMessageType::Open)
    {
        SPDLOG_INFO("Connected to Homey websocket.");
        HomeyManager::_connected = true;
        WebsocketServer::remove_warning("Homey not connected.");
    }
    else if (msg->type == ix::WebSocketMessageType::Close)
    {
        WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "Homey not connected.");
        SPDLOG_WARN("Disconnected from Homey websocket.");
        HomeyManager::_connected = false;
    }
    else if (msg->type == ix::WebSocketMessageType::Error)
    {
        WebsocketServer::register_warning(WebsocketServer::ActiveWarningLevel::ERROR, "Homey not connected.");
        SPDLOG_ERROR("Failed to connect to Homey websocket. Reason: {}", msg->errorInfo.reason);
        HomeyManager::_connected = false;
    }
}

void HomeyManager::_process_websocket_message(const std::string &message)
{
    try
    {
        nlohmann::json data = nlohmann::json::parse(message);

        // Homey WebSocket sends device update events in this format:
        // {"event": "device.update", "args": [{"id": "device-uuid", "capabilitiesObj": {...}}]}
        if (data.contains("event") && data.contains("args"))
        {
            std::string event_type = data["event"];

            if (event_type == "device.update" && data["args"].is_array() && !data["args"].empty())
            {
                HomeyManager::_process_homey_event(data);
            }
            else
            {
                SPDLOG_DEBUG("Received Homey event: {}", event_type);
            }
        }
        else
        {
            SPDLOG_DEBUG("Got unhandled Homey message: {}", message);
        }
    }
    catch (std::exception &ex)
    {
        SPDLOG_ERROR("Exception processing Homey WebSocket message: {}", boost::diagnostic_information(ex, true));
    }
    catch (...)
    {
        SPDLOG_ERROR("Caught unknown exception while processing Homey WebSocket message: {}", message);
    }
}

void HomeyManager::send_json(nlohmann::json &data)
{
    std::string buffer = data.dump();
    HomeyManager::_send_string(buffer);
}

void HomeyManager::_send_string(std::string &data)
{
    if (HomeyManager::_websocket != nullptr && HomeyManager::_connected)
    {
        std::lock_guard<std::mutex> mtex_lock(HomeyManager::_mutex_websocket_write_access);
        SPDLOG_TRACE("[Homey WS] Sending data: {}", data);
        HomeyManager::_websocket->send(data);
    }
}

void HomeyManager::_process_homey_event(nlohmann::json &event_data)
{
    try
    {
        // Extract device info from the event
        // Format: {"event": "device.update", "args": [{"id": "device-uuid", "capabilitiesObj": {...}}]}
        if (event_data.contains("args") && event_data["args"].is_array() && !event_data["args"].empty())
        {
            nlohmann::json device_data = event_data["args"][0];

            if (device_data.contains("id"))
            {
                std::string device_id = device_data["id"];

                if (HomeyManager::_homey_observers.find(device_id) != HomeyManager::_homey_observers.end())
                {
                    try
                    {
                        SPDLOG_TRACE("Homey Event for device {}: {}", device_id, device_data.dump());
                        HomeyManager::_homey_observers.at(device_id)(device_data);
                    }
                    catch (const std::exception &e)
                    {
                        SPDLOG_ERROR("Caught exception during processing of Homey event. Diagnostic information: {}", boost::diagnostic_information(e, true));
                        SPDLOG_ERROR("Stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
                    }
                }
            }
        }
    }
    catch (std::exception &ex)
    {
        SPDLOG_ERROR("Exception in _process_homey_event: {}", boost::diagnostic_information(ex, true));
    }
}
