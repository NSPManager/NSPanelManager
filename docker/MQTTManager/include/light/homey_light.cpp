#include "homey_light.hpp"
#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "light/light.hpp"
#include "web_helper/WebHelper.hpp"
#include "mqtt_manager/mqtt_manager.hpp"
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <cstdint>
#include <homey_manager/homey_manager.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

HomeyLight::HomeyLight(uint32_t light_id) : Light(light_id)
{
    // Process Homey specific details. General light data is loaded in the "Light" constructor.

    this->_current_brightness = 0;
    this->_current_color_temperature = 0;
    this->_current_hue = 0;
    this->_current_saturation = 0;
    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
    this->_current_state = false;
    this->_requested_brightness = 0;
    this->_requested_color_temperature = 0;
    this->_requested_hue = 0;
    this->_requested_saturation = 0;
    this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
    this->_requested_state = false;

    if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY)
    {
        SPDLOG_ERROR("HomeyLight has not been recognized as controlled by HOMEY. Will stop processing light.");
        return;
    }

    nlohmann::json entity_data;
    try
    {
        auto light = database_manager::database.get<database_manager::Entity>(this->_id);
        entity_data = light.get_entity_data_json();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to load light {}: {}", this->_id, e.what());
        return;
    }

    if (entity_data.contains("homey_device_id"))
    {
        this->_homey_device_id = entity_data["homey_device_id"];
    }
    else
    {
        SPDLOG_ERROR("No homey_device_id defined for Light {}::{}", this->_id, this->_name);
        return;
    }

    if (entity_data.contains("capabilities") && entity_data["capabilities"].is_array())
    {
        for (const auto &cap : entity_data["capabilities"])
        {
            this->_capabilities.push_back(cap);
        }
    }

    SPDLOG_DEBUG("Loaded Homey light {}::{}, device ID: {}", this->_id, this->_name, this->_homey_device_id);
    HomeyManager::attach_event_observer(this->_homey_device_id, boost::bind(&HomeyLight::homey_event_callback, this, _1));

    this->send_state_update_to_nspanel(); // Send initial state to NSPanel
}

HomeyLight::~HomeyLight()
{
    HomeyManager::detach_event_observer(this->_homey_device_id, boost::bind(&HomeyLight::homey_event_callback, this, _1));
}

bool HomeyLight::_has_capability(const std::string &capability)
{
    return std::find(this->_capabilities.begin(), this->_capabilities.end(), capability) != this->_capabilities.end();
}

void HomeyLight::_send_capability_update(const std::string &capability, nlohmann::json value)
{
    SPDLOG_DEBUG("Homey light {}::{} sending capability {} = {}", this->_id, this->_name, capability, value.dump());

    // Get Homey connection settings
    auto homey_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS);
    auto homey_token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN);

    if (homey_address.empty() || homey_token.empty())
    {
        SPDLOG_ERROR("Homey address or token not configured for light {}::{}", this->_id, this->_name);
        return;
    }

    // Construct URL: http://{homey_address}/api/device/{device_id}/capability/{capability}
    std::string url = fmt::format("http://{}/api/device/{}/capability/{}", homey_address, this->_homey_device_id, capability);

    // Create request body with capability value
    nlohmann::json request_body;
    request_body["value"] = value;

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
            SPDLOG_DEBUG("Homey light {}::{} capability {} update response: {}", this->_id, this->_name, capability, response_data);
        }
        else
        {
            SPDLOG_ERROR("Failed to send capability update to Homey for light {}::{}", this->_id, this->_name);
        }
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to send capability update to Homey for light {}::{}: {}", this->_id, this->_name, e.what());
    }
}

void HomeyLight::send_state_update_to_controller()
{
    SPDLOG_DEBUG("Homey light {}::{} send_state_update_to_controller", this->_id, this->_name);

    // Turn on/off
    if (this->_has_capability("onoff"))
    {
        this->_send_capability_update("onoff", this->_requested_state);

        if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
        {
            this->_current_state = this->_requested_state;
        }
    }

    // Only send other values if turning on
    if (this->_requested_state)
    {
        // Brightness
        if (this->_has_capability("dim") && this->_requested_brightness != this->_current_brightness)
        {
            // Homey expects dim as 0.0 to 1.0
            float dim_value = (float)this->_requested_brightness / 100.0;
            this->_send_capability_update("dim", dim_value);

            if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
            {
                this->_current_brightness = this->_requested_brightness;
            }
        }

        // Color temperature mode
        if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::DEFAULT)
        {
            if (this->_has_capability("light_temperature") && this->_requested_color_temperature != this->_current_color_temperature)
            {
                this->_send_capability_update("light_temperature", this->_requested_color_temperature);

                if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
                {
                    this->_current_color_temperature = this->_requested_color_temperature;
                    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
                }
            }
        }
        // RGB mode
        else if (this->_requested_mode == MQTT_MANAGER_LIGHT_MODE::RGB)
        {
            if (this->_has_capability("light_hue") && this->_requested_hue != this->_current_hue)
            {
                // Homey expects hue as 0.0 to 1.0 (representing 0-360 degrees)
                float hue_value = (float)this->_requested_hue / 360.0;
                this->_send_capability_update("light_hue", hue_value);

                if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
                {
                    this->_current_hue = this->_requested_hue;
                }
            }

            if (this->_has_capability("light_saturation") && this->_requested_saturation != this->_current_saturation)
            {
                // Homey expects saturation as 0.0 to 1.0
                float saturation_value = (float)this->_requested_saturation / 100.0;
                this->_send_capability_update("light_saturation", saturation_value);

                if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
                {
                    this->_current_saturation = this->_requested_saturation;
                    this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
                }
            }
        }
    }

    if (MqttManagerConfig::get_setting_with_default<bool>(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE))
    {
        this->send_state_update_to_nspanel();
        this->_entity_changed_callbacks(this);
    }
}

void HomeyLight::homey_event_callback(nlohmann::json data)
{
    try
    {
        // Homey WebSocket sends: {"id": "device-uuid", "capabilitiesObj": {...}}
        if (!data.contains("capabilitiesObj") || data["capabilitiesObj"].is_null())
        {
            SPDLOG_DEBUG("Homey event for light {}::{} has no capabilitiesObj", this->_id, this->_name);
            return;
        }

        SPDLOG_DEBUG("Got event update for Homey light {}::{}.", this->_id, this->_name);
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

                if (!new_state)
                {
                    this->_requested_brightness = 0;
                }
            }
        }

        // Handle dim capability (brightness)
        if (capabilities.contains("dim") && !capabilities["dim"].is_null())
        {
            if (capabilities["dim"].contains("value") && !capabilities["dim"]["value"].is_null())
            {
                float dim_value = capabilities["dim"]["value"];
                uint8_t new_brightness = (uint8_t)(dim_value * 100.0); // Convert 0.0-1.0 to 0-100

                if (new_brightness != this->_current_brightness)
                {
                    changed_attribute = true;
                }

                this->_current_brightness = new_brightness;
                this->_requested_brightness = new_brightness;
            }
        }
        else if (this->_current_state && this->_can_dim)
        {
            // If light is on but no brightness given, assume 100%
            if (this->_current_brightness != 100)
            {
                changed_attribute = true;
            }
            this->_current_brightness = 100;
            this->_requested_brightness = 100;
        }

        // Handle color temperature
        if (capabilities.contains("light_temperature") && !capabilities["light_temperature"].is_null())
        {
            if (capabilities["light_temperature"].contains("value") && !capabilities["light_temperature"]["value"].is_null())
            {
                uint32_t new_temp = capabilities["light_temperature"]["value"];

                if (new_temp != this->_current_color_temperature)
                {
                    changed_attribute = true;
                }

                this->_current_color_temperature = new_temp;
                this->_requested_color_temperature = new_temp;
                this->_current_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
                this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::DEFAULT;
            }
        }

        // Handle hue
        if (capabilities.contains("light_hue") && !capabilities["light_hue"].is_null())
        {
            if (capabilities["light_hue"].contains("value") && !capabilities["light_hue"]["value"].is_null())
            {
                float hue_value = capabilities["light_hue"]["value"];
                uint16_t new_hue = (uint16_t)(hue_value * 360.0); // Convert 0.0-1.0 to 0-360

                if (new_hue != this->_current_hue)
                {
                    changed_attribute = true;
                }

                this->_current_hue = new_hue;
                this->_requested_hue = new_hue;
                this->_current_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
                this->_requested_mode = MQTT_MANAGER_LIGHT_MODE::RGB;
            }
        }

        // Handle saturation
        if (capabilities.contains("light_saturation") && !capabilities["light_saturation"].is_null())
        {
            if (capabilities["light_saturation"].contains("value") && !capabilities["light_saturation"]["value"].is_null())
            {
                float saturation_value = capabilities["light_saturation"]["value"];
                uint8_t new_saturation = (uint8_t)(saturation_value * 100.0); // Convert 0.0-1.0 to 0-100

                if (new_saturation != this->_current_saturation)
                {
                    changed_attribute = true;
                }

                this->_current_saturation = new_saturation;
                this->_requested_saturation = new_saturation;
            }
        }

        if (changed_attribute)
        {
            this->send_state_update_to_nspanel();
            this->_signal_entity_changed();
        }
    }
    catch (std::exception &e)
    {
        SPDLOG_ERROR("Caught exception when processing Homey event for light {}::{}: {}",
                     this->_id, this->_name, boost::diagnostic_information(e, true));
    }
}
