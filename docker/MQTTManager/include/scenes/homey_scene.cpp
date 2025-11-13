#include "database_manager/database_manager.hpp"
#include "entity/entity.hpp"
#include "entity_manager/entity_manager.hpp"
#include <homey_manager/homey_manager.hpp>
#include "mqtt_manager_config/mqtt_manager_config.hpp"
#include "web_helper/WebHelper.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <entity/entity_icons.hpp>
#include <nlohmann/json.hpp>
#include <scenes/homey_scene.hpp>
#include <spdlog/spdlog.h>
#include <system_error>

HomeyScene::HomeyScene(uint32_t id)
{
    this->_id = id;
    this->reload_config();
}

void HomeyScene::reload_config()
{
    try
    {
        auto scene_config = database_manager::database.get<database_manager::Scene>(this->_id);
        this->_name = scene_config.friendly_name;
        this->_entity_id = scene_config.backend_name;
        this->_page_id = scene_config.entities_page_id;
        this->_page_slot = scene_config.room_view_position;
        std::string backend_name = scene_config.backend_name;

        if (scene_config.room_id == nullptr)
        {
            this->_is_global = true;
        }
        else
        {
            this->_is_global = false;
            this->_room_id = *scene_config.room_id;
        }

        // Validate backend_name is not empty - skip scenes that aren't properly configured as Homey scenes
        if (backend_name.empty())
        {
            SPDLOG_WARN("Scene {}::{} is marked as 'homey' type but has empty backend_name. This scene may be incorrectly classified. Skipping Homey initialization.", this->_id, this->_name);
            // Set defaults to prevent crashes
            this->_homey_scene_type = HOMEY_SCENE_TYPE::HOMEY_FLOW;
            this->_homey_id = "";
            return;
        }

        // Parse backend_name to get Homey ID and type
        // Format: "homey_flow_<uuid>" or "homey_mood_<uuid>"
        if (boost::starts_with(backend_name, "homey_flow_"))
        {
            this->_homey_scene_type = HOMEY_SCENE_TYPE::HOMEY_FLOW;
            this->_homey_id = backend_name.substr(11); // Remove "homey_flow_" prefix

            if (this->_homey_id.empty())
            {
                SPDLOG_ERROR("Empty Homey flow ID in backend_name: {}. Expected 'homey_flow_<id>'", backend_name);
                return;
            }
        }
        else if (boost::starts_with(backend_name, "homey_mood_"))
        {
            this->_homey_scene_type = HOMEY_SCENE_TYPE::HOMEY_MOOD;
            this->_homey_id = backend_name.substr(11); // Remove "homey_mood_" prefix

            if (this->_homey_id.empty())
            {
                SPDLOG_ERROR("Empty Homey mood ID in backend_name: {}. Expected 'homey_mood_<id>'", backend_name);
                return;
            }
        }
        else
        {
            SPDLOG_ERROR("Invalid Homey scene backend_name format: '{}'. Expected 'homey_flow_<id>' or 'homey_mood_<id>'", backend_name);
            return;
        }

        SPDLOG_DEBUG("Loaded Homey scene {}::{}, type: {}, ID: {}",
                     this->_id,
                     this->_name,
                     this->_homey_scene_type == HOMEY_SCENE_TYPE::HOMEY_FLOW ? "Flow" : "Mood",
                     this->_homey_id);
    }
    catch (std::system_error &ex)
    {
        SPDLOG_ERROR("Failed to update config for Homey scene {}::{}.", this->_id, this->_name);
    }
}

void HomeyScene::activate()
{
    // Don't try to activate scenes that weren't properly configured as Homey scenes
    if (this->_homey_id.empty())
    {
        SPDLOG_WARN("Cannot activate scene {}::{} - it was not properly configured as a Homey scene", this->_id, this->_name);
        return;
    }

    SPDLOG_DEBUG("Activating Homey scene {}::{}, type: {}, ID: {}",
                 this->_id,
                 this->_name,
                 this->_homey_scene_type == HOMEY_SCENE_TYPE::HOMEY_FLOW ? "Flow" : "Mood",
                 this->_homey_id);

    // Get Homey connection settings
    auto homey_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS);
    auto homey_token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN);

    if (homey_address.empty() || homey_token.empty())
    {
        SPDLOG_ERROR("Homey address or token not configured for scene {}::{}", this->_id, this->_name);
        return;
    }

    std::string url;
    nlohmann::json request_body;

    if (this->_homey_scene_type == HOMEY_SCENE_TYPE::HOMEY_FLOW)
    {
        // For Flows: POST /api/manager/flow/flow/{flow_id}/trigger
        url = fmt::format("http://{}/api/manager/flow/flow/{}/trigger", homey_address, this->_homey_id);
        // Flow trigger doesn't require a body, but we'll send empty JSON
        request_body = nlohmann::json::object();
    }
    else
    {
        // For Moods: Similar trigger endpoint
        url = fmt::format("http://{}/api/manager/moods/mood/{}/set", homey_address, this->_homey_id);
        request_body = nlohmann::json::object();
    }

    // Send HTTP POST request with bearer token authentication
    try
    {
        // Create header strings with proper lifetime management
        std::string auth_header = fmt::format("Authorization: Bearer {}", homey_token);
        std::list<const char *> headers = {
            auth_header.c_str(),
            "Content-Type: application/json"};

        std::string response_data;
        std::string post_data = request_body.dump();

        if (WebHelper::perform_post_request(&url, &response_data, &headers, &post_data))
        {
            SPDLOG_DEBUG("Homey scene {}::{} activation response: {}", this->_id, this->_name, response_data);
        }
        else
        {
            SPDLOG_ERROR("Failed to activate Homey scene {}::{}", this->_id, this->_name);
        }
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to activate Homey scene {}::{}: {}", this->_id, this->_name, e.what());
    }
}

void HomeyScene::save()
{
    SPDLOG_ERROR("Save is not a possible action for Homey scenes.");
}

void HomeyScene::remove()
{
}

uint16_t HomeyScene::get_id()
{
    return this->_id;
}

MQTT_MANAGER_ENTITY_TYPE HomeyScene::get_type()
{
    return MQTT_MANAGER_ENTITY_TYPE::SCENE;
}

MQTT_MANAGER_ENTITY_CONTROLLER HomeyScene::get_controller()
{
    return MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY;
}

void HomeyScene::post_init()
{
    if (!this->_is_global)
    {
        auto room_entity = EntityManager::get_room(this->_room_id);
        if (room_entity)
        {
            this->_room = *room_entity;
        }
        else
        {
            SPDLOG_ERROR("Did not find any room with room ID: {}. Will not continue loading.", this->_room_id);
            return;
        }
    }
}

std::string HomeyScene::get_name()
{
    return this->_name;
}

bool HomeyScene::can_save()
{
    return false;
}

std::string_view HomeyScene::get_icon()
{
    return EntityIcons::homey_icon;
}

uint16_t HomeyScene::get_icon_color()
{
    return GUI_Colors::icon_color_off;
}

uint16_t HomeyScene::get_icon_active_color()
{
    return GUI_Colors::icon_color_off;
}
