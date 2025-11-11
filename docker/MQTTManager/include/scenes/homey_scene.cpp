#include "homey_scene.hpp"
#include "database_manager/database_manager.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

HomeyScene::HomeyScene(uint32_t scene_id) : Scene(scene_id)
{
    if (this->_controller != MQTT_MANAGER_ENTITY_CONTROLLER::HOMEY)
    {
        SPDLOG_ERROR("HomeyScene has not been recognized as controlled by HOMEY. Will stop processing scene.");
        return;
    }

    // Parse backend_name to get Homey ID and type
    // Format: "homey_flow_<uuid>" or "homey_mood_<uuid>"
    if (boost::starts_with(this->_backend_name, "homey_flow_"))
    {
        this->_homey_scene_type = HOMEY_SCENE_TYPE::HOMEY_FLOW;
        this->_homey_id = this->_backend_name.substr(11); // Remove "homey_flow_" prefix
    }
    else if (boost::starts_with(this->_backend_name, "homey_mood_"))
    {
        this->_homey_scene_type = HOMEY_SCENE_TYPE::HOMEY_MOOD;
        this->_homey_id = this->_backend_name.substr(11); // Remove "homey_mood_" prefix
    }
    else
    {
        SPDLOG_ERROR("Invalid Homey scene backend_name format: {}. Expected 'homey_flow_<id>' or 'homey_mood_<id>'", this->_backend_name);
        return;
    }

    SPDLOG_DEBUG("Loaded Homey scene {}::{}, type: {}, ID: {}",
                 this->_id,
                 this->_scene_name,
                 this->_homey_scene_type == HOMEY_SCENE_TYPE::HOMEY_FLOW ? "Flow" : "Mood",
                 this->_homey_id);
}

HomeyScene::~HomeyScene()
{
    // No cleanup needed for scenes
}

void HomeyScene::activate()
{
    SPDLOG_DEBUG("Activating Homey scene {}::{}, type: {}, ID: {}",
                 this->_id,
                 this->_scene_name,
                 this->_homey_scene_type == HOMEY_SCENE_TYPE::HOMEY_FLOW ? "Flow" : "Mood",
                 this->_homey_id);

    // Get Homey connection settings
    auto homey_address = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_ADDRESS, "");
    auto homey_token = MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::HOMEY_TOKEN, "");

    if (homey_address.empty() || homey_token.empty())
    {
        SPDLOG_ERROR("Homey address or token not configured for scene {}::{}", this->_id, this->_scene_name);
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
        url = fmt::format("http://{}/api/mood/{}/trigger", homey_address, this->_homey_id);
        request_body = nlohmann::json::object();
    }

    // Send HTTP POST request with bearer token authentication
    try
    {
        std::vector<std::string> headers = {
            fmt::format("Authorization: Bearer {}", homey_token),
            "Content-Type: application/json"};

        std::string response = WebHelper::send_authorized_request(url, request_body.dump(), headers, WebHelper::HTTP_METHOD::POST);
        SPDLOG_DEBUG("Homey scene {}::{} activation response: {}", this->_id, this->_scene_name, response);
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Failed to activate Homey scene {}::{}: {}", this->_id, this->_scene_name, e.what());
    }
}
