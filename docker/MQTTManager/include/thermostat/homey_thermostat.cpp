#include "homey_thermostat.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "protobuf_nspanel.pb.h"
#include "thermostat/thermostat.hpp"
#include "web_helper/web_helper.hpp"
#include <cstdint>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

HomeyThermostat::HomeyThermostat(uint32_t thermostat_id) : ThermostatEntity(thermostat_id)
{
    // Process Homey specific details. General thermostat data is loaded in the "ThermostatEntity" constructor.
    if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY)
    {
        SPDLOG_ERROR("HomeyThermostat has not been recognized as controlled by HOMEY. Will stop processing thermostat.");
        return;
    }

    nlohmann::json entity_data;
    try
    {
        auto thermostat = database_manager::database.get<database_manager::Entity>(this->_id);
        entity_data = thermostat.get_entity_data_json();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to load thermostat {}: {}", this->_id, e.what());
        return;
    }

    if (entity_data.contains("homey_device_id"))
    {
        this->_homey_device_id = entity_data["homey_device_id"];
    }
    else
    {
        SPDLOG_ERROR("No Homey device ID defined for Thermostat {}::{}", this->_id, this->_name);
        return;
    }

    SPDLOG_DEBUG("Loaded thermostat {}::{}, Homey device ID: {}", this->_id, this->_name, this->_homey_device_id);

    this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

HomeyThermostat::~HomeyThermostat()
{
    // Cleanup if needed
}

void HomeyThermostat::_send_capability_update(const std::string &capability, const nlohmann::json &value)
{
    std::string homey_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS);
    std::string homey_token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN);

    if (homey_address.empty() || homey_token.empty())
    {
        SPDLOG_ERROR("Homey address or token not configured for thermostat {}::{}", this->_id, this->_name);
        return;
    }

    nlohmann::json request_body;
    request_body["value"] = value;

    std::string url = fmt::format("http://{}/api/device/{}/capability/{}", homey_address, this->_homey_device_id, capability);
    std::vector<std::string> headers = {
        fmt::format("Authorization: Bearer {}", homey_token),
        "Content-Type: application/json"};

    try
    {
        std::string response = WebHelper::send_authorized_request(url, request_body.dump(), headers, WebHelper::HTTP_METHOD::PUT);
        SPDLOG_DEBUG("Thermostat {}::{} sent {} update to Homey: {}", this->_id, this->_name, capability, value.dump());
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to send {} update to Homey for thermostat {}::{}: {}", capability, this->_id, this->_name, e.what());
    }
}

void HomeyThermostat::send_state_update_to_controller()
{
    // Send target temperature if it has changed
    if (this->_requested_temperature != this->_current_temperature)
    {
        this->_send_capability_update("target_temperature", this->_requested_temperature);
    }

    // Note: Homey thermostats typically use target_temperature as the main control
    // Mode, fan mode, and other advanced features may not be supported by all Homey devices
    // For now, we focus on the core temperature control functionality
}

void HomeyThermostat::command_callback(NSPanelMQTTManagerCommand &command)
{
    // Handle commands from NSPanel via MQTT
    if (!command.has_thermostat_command())
    {
        SPDLOG_WARN("Thermostat {}::{} received command without thermostat_command", this->_id, this->_name);
        return;
    }

    const NSPanelMQTTManagerCommand_ThermostatCommand &thermostat_cmd = command.thermostat_command();

    // Handle temperature change
    if (thermostat_cmd.has_temperature())
    {
        float new_temperature = thermostat_cmd.temperature();
        SPDLOG_DEBUG("Thermostat {}::{} received temperature command: {}", this->_id, this->_name, new_temperature);
        this->set_temperature(new_temperature);
        this->send_state_update_to_controller();
    }

    // Handle mode changes if provided
    if (thermostat_cmd.has_mode())
    {
        std::string new_mode = thermostat_cmd.mode();
        SPDLOG_DEBUG("Thermostat {}::{} received mode command: {}", this->_id, this->_name, new_mode);
        this->set_mode(new_mode);
        this->send_state_update_to_controller();
    }

    // Send updated state to NSPanel
    this->send_state_update_to_nspanel();
}
