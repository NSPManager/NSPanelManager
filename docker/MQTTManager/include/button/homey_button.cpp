#include "homey_button.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "web_helper/WebHelper.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <cstdint>
#include <homey_manager/homey_manager.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <switch/switch.hpp>

HomeyButton::HomeyButton(uint32_t button_id) : ButtonEntity(button_id)
{
    if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY)
    {
        SPDLOG_ERROR("HomeyButton has not been recognized as controlled by HOMEY. Will stop processing button.");
        return;
    }

    nlohmann::json entity_data;
    try
    {
        auto button_entity = database_manager::database.get<database_manager::Entity>(this->_id);
        entity_data = button_entity.get_entity_data_json();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to load button {}: {}", this->_id, e.what());
        return;
    }

    if (entity_data.contains("homey_device_id"))
    {
        this->_homey_device_id = entity_data["homey_device_id"];
    }
    else
    {
        SPDLOG_ERROR("No homey_device_id defined for Button {}::{}", this->_id, this->_name);
        return;
    }

    SPDLOG_DEBUG("Loaded Homey button {}::{}, device ID: {}", this->_id, this->_name, this->_homey_device_id);
    HomeyManager::attach_event_observer(this->_homey_device_id, boost::bind(&HomeyButton::homey_event_callback, this, _1));
}

HomeyButton::~HomeyButton()
{
    HomeyManager::detach_event_observer(this->_homey_device_id, boost::bind(&HomeyButton::homey_event_callback, this, _1));
}

void HomeyButton::send_state_update_to_controller()
{
    SPDLOG_DEBUG("Homey button {}::{} send_state_update_to_controller (trigger button press)", this->_id, this->_name);

    // Get Homey connection settings
    auto homey_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS);
    auto homey_token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN);

    if (homey_address.empty() || homey_token.empty())
    {
        SPDLOG_ERROR("Homey address or token not configured for button {}::{}", this->_id, this->_name);
        return;
    }

    // Construct URL: http://{homey_address}/api/device/{device_id}/capability/button
    std::string url = fmt::format("http://{}/api/device/{}/capability/button", homey_address, this->_homey_device_id);

    // Create request body - button trigger uses null value
    nlohmann::json request_body;
    request_body["value"] = nullptr;

    // Send HTTP PUT request with bearer token authentication
    try
    {
        std::list<const char *> headers = {
            fmt::format("Authorization: Bearer {}", homey_token.c_str(),
                        "Content-Type: application/json")
                .c_str()};

        std::string response_data;
        std::string put_data = request_body.dump();

        if (WebHelper::perform_put_request(&url, &response_data, &headers, &put_data))
        {
            SPDLOG_DEBUG("Homey button {}::{} trigger response: {}", this->_id, this->_name, response_data);
        }
        else
        {
            SPDLOG_ERROR("Failed to trigger Homey button {}::{}", this->_id, this->_name);
        }
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to trigger Homey button {}::{}: {}", this->_id, this->_name, e.what());
    }

    // Buttons don't have persistent state, so no optimistic mode handling needed
}

void HomeyButton::homey_event_callback(nlohmann::json data)
{
    try
    {
        // Homey WebSocket sends: {"id": "device-uuid", "capabilitiesObj": {...}}
        // Buttons typically don't send state updates, but we'll handle them if they do
        SPDLOG_DEBUG("Got event update for Homey button {}::{}.", this->_id, this->_name);

        // Buttons are typically one-way (trigger only), so no state to process
        // But we keep this callback for potential future use
    }
    catch (std::exception &e)
    {
        SPDLOG_ERROR("Caught exception when processing Homey event for button {}::{}: {}",
                     this->_id, this->_name, boost::diagnostic_information(e, true));
    }
}
