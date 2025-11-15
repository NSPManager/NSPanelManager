#include "homey_switch.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "web_helper/WebHelper.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <cstdint>
#include <homey_manager/homey_manager.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <switch/switch.hpp>

HomeySwitch::HomeySwitch(uint32_t switch_id) : SwitchEntity(switch_id)
{
    if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY)
    {
        SPDLOG_ERROR("HomeySwitch has not been recognized as controlled by HOMEY. Will stop processing switch.");
        return;
    }

    nlohmann::json entity_data;
    try
    {
        auto switch_entity = database_manager::database.get<database_manager::Entity>(this->_id);
        entity_data = switch_entity.get_entity_data_json();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to load switch {}: {}", this->_id, e.what());
        return;
    }

    if (entity_data.contains("homey_device_id"))
    {
        this->_homey_device_id = entity_data["homey_device_id"];
    }
    else
    {
        SPDLOG_ERROR("No homey_device_id defined for Switch {}::{}", this->_id, this->_name);
        return;
    }

    SPDLOG_DEBUG("Loaded Homey switch {}::{}, device ID: {}", this->_id, this->_name, this->_homey_device_id);
    HomeyManager::attach_event_observer(this->_homey_device_id, boost::bind(&HomeySwitch::homey_event_callback, this, _1));
}

HomeySwitch::~HomeySwitch()
{
    HomeyManager::detach_event_observer(this->_homey_device_id, boost::bind(&HomeySwitch::homey_event_callback, this, _1));
}

void HomeySwitch::send_state_update_to_controller()
{
    SPDLOG_DEBUG("Homey switch {}::{} send_state_update_to_controller. State: {}", this->_id, this->_name, this->_requested_state ? "ON" : "OFF");

    // Get Homey connection settings
    auto homey_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS);
    auto homey_token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN);

    if (homey_address.empty() || homey_token.empty())
    {
        SPDLOG_ERROR("Homey address or token not configured for switch {}::{}", this->_id, this->_name);
        return;
    }

    // Construct URL: http://{homey_address}/api/manager/devices/device/{device_id}/capability/onoff
    std::string url = fmt::format("http://{}/api/manager/devices/device/{}/capability/onoff", homey_address, this->_homey_device_id);

    // Create request body with state value
    nlohmann::json request_body;
    request_body["value"] = this->_requested_state;

    // Send HTTP PUT request with bearer token authentication
    try
    {
        // Create header strings with proper lifetime management
        std::string auth_header = fmt::format("Authorization: Bearer {}", homey_token);
        std::list<const char *> headers = {
            auth_header.c_str(),
            "Content-Type: application/json"};

        std::string response_data;
        std::string put_data = request_body.dump();

        if (WebHelper::perform_put_request(&url, &response_data, &headers, &put_data))
        {
            SPDLOG_DEBUG("Homey switch {}::{} state update response: {}", this->_id, this->_name, response_data);

            if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
            {
                this->_current_state = this->_requested_state;
                this->_signal_entity_changed();
            }
        }
        else
        {
            SPDLOG_ERROR("Failed to send state update to Homey for switch {}::{}", this->_id, this->_name);
        }
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to send state update to Homey for switch {}::{}: {}", this->_id, this->_name, e.what());
    }
}

void HomeySwitch::homey_event_callback(nlohmann::json data)
{
    try
    {
        // Homey WebSocket sends: {"id": "device-uuid", "capabilitiesObj": {...}}
        if (!data.contains("capabilitiesObj") || data["capabilitiesObj"].is_null())
        {
            SPDLOG_DEBUG("Homey event for switch {}::{} has no capabilitiesObj", this->_id, this->_name);
            return;
        }

        SPDLOG_DEBUG("Got event update for Homey switch {}::{}.", this->_id, this->_name);
        nlohmann::json capabilities = data["capabilitiesObj"];
        bool changed_attribute = false;

        // Handle onoff capability
        if (capabilities.contains("onoff") && !capabilities["onoff"].is_null())
        {
            if (capabilities["onoff"].contains("value") && !capabilities["onoff"]["value"].is_null())
            {
                bool new_state = capabilities["onoff"]["value"];

                if (new_state != this->_current_state)
                {
                    changed_attribute = true;
                }

                this->_current_state = new_state;
                this->_requested_state = new_state;
            }
        }

        if (changed_attribute)
        {
            this->_signal_entity_changed();
        }
    }
    catch (std::exception &e)
    {
        SPDLOG_ERROR("Caught exception when processing Homey event for switch {}::{}: {}",
                     this->_id, this->_name, boost::diagnostic_information(e, true));
    }
}
