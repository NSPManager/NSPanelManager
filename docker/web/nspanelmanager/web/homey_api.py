import json
import traceback
import logging
import requests
from web.settings_helper import get_setting_with_default


def get_all_homey_devices(filter={}):
    """
    Fetch all devices from Homey that match supported capabilities.
    Filters for lights (onoff, dim, light_hue, light_saturation, light_temperature),
    switches (onoff), and buttons (button).
    """
    return_json = {"items": [], "errors": []}

    if (
        get_setting_with_default("homey_address") == ""
        or get_setting_with_default("homey_token") == ""
    ):
        return_json["errors"].append("Homey address and/or API token not configured")
        return return_json

    homey_request_headers = {
        "Authorization": "Bearer " + get_setting_with_default("homey_token"),
        "content-type": "application/json",
    }

    try:
        homey_api_address = (
            "http://"
            + get_setting_with_default("homey_address")
            + "/api/manager/devices/device"
        )
        logging.debug(
            "Trying to get Homey devices via api address: " + homey_api_address
        )

        homey_response = requests.get(
            homey_api_address, headers=homey_request_headers, timeout=5, verify=False
        )

        if homey_response.status_code == 200:
            devices = homey_response.json()

            # Handle both dict and list response formats
            if isinstance(devices, dict):
                devices = devices.values()

            for device in devices:
                if not isinstance(device, dict):
                    continue

                device_id = device.get("id", "")
                device_name = device.get("name", "Unknown Device")
                capabilities = device.get("capabilities", [])

                # Check if device has any supported capabilities
                supported_capabilities = []
                device_type = None

                # Handle both list and dict formats
                if isinstance(capabilities, list):
                    cap_list = capabilities
                elif isinstance(capabilities, dict):
                    cap_list = list(capabilities.keys())
                else:
                    cap_list = []

                for cap_name in cap_list:
                    if cap_name in [
                        "onoff",
                        "dim",
                        "light_hue",
                        "light_saturation",
                        "light_temperature",
                        "light_mode",
                        "button",
                    ]:
                        supported_capabilities.append(cap_name)

                logging.debug(
                    f"Device {device_name}: capabilities={cap_list}, supported={supported_capabilities}"
                )

                # Determine device type based on capabilities
                if (
                    "button" in supported_capabilities
                    and len(supported_capabilities) == 1
                ):
                    device_type = "button"
                elif "onoff" in supported_capabilities and not any(
                    light_cap in supported_capabilities
                    for light_cap in [
                        "dim",
                        "light_hue",
                        "light_saturation",
                        "light_temperature",
                    ]
                ):
                    device_type = "switch"
                elif "onoff" in supported_capabilities or any(
                    light_cap in supported_capabilities
                    for light_cap in [
                        "dim",
                        "light_hue",
                        "light_saturation",
                        "light_temperature",
                    ]
                ):
                    device_type = "light"

                if supported_capabilities and device_type:
                    data = {
                        "type": "homey",
                        "label": device_name,
                        "item_id": device_id,
                        "device_type": device_type,
                        "capabilities": supported_capabilities,
                        "item": device,
                    }
                    return_json["items"].append(data)

        else:
            return_json["errors"].append(
                "Failed to get Homey devices, got return code: "
                + str(homey_response.status_code)
            )
            logging.error(
                "ERROR! Got status code other than 200. Got code: "
                + str(homey_response.status_code)
            )

    except Exception as e:
        return_json["errors"].append(
            "Failed to get Homey devices: " + str(traceback.format_exc())
        )
        logging.exception("Failed to get Homey devices!")

    return return_json


def get_all_homey_flows():
    """
    Fetch all Flows from Homey.
    """
    return_json = {"items": [], "errors": []}

    if (
        get_setting_with_default("homey_address") == ""
        or get_setting_with_default("homey_token") == ""
    ):
        return_json["errors"].append("Homey address and/or API token not configured")
        return return_json

    homey_request_headers = {
        "Authorization": "Bearer " + get_setting_with_default("homey_token"),
        "content-type": "application/json",
    }

    try:
        homey_api_address = (
            "http://"
            + get_setting_with_default("homey_address")
            + "/api/manager/flow/flow"
        )
        logging.debug("Trying to get Homey flows via api address: " + homey_api_address)

        homey_response = requests.get(
            homey_api_address, headers=homey_request_headers, timeout=5, verify=False
        )

        if homey_response.status_code == 200:
            flows = homey_response.json()

            # Handle both dict and list response formats
            if isinstance(flows, dict):
                flows = flows.values()

            for flow in flows:
                if not isinstance(flow, dict):
                    continue

                flow_id = flow.get("id", "")
                flow_name = flow.get("name", "Unknown Flow")

                data = {
                    "type": "homey",
                    "label": "[F] " + flow_name,  # Prefix with [F] for Flow
                    "item_id": flow_id,
                    "scene_type": "homey_flow",
                    "item": flow,
                }
                return_json["items"].append(data)

        else:
            return_json["errors"].append(
                "Failed to get Homey flows, got return code: "
                + str(homey_response.status_code)
            )
            logging.error(
                "ERROR! Got status code other than 200. Got code: "
                + str(homey_response.status_code)
            )

    except Exception as e:
        return_json["errors"].append(
            "Failed to get Homey flows: " + str(traceback.format_exc())
        )
        logging.exception("Failed to get Homey flows!")

    return return_json


def get_all_homey_moods():
    """
    Fetch all Moods from Homey.
    Moods in Homey are accessed via the insights or a specific moods endpoint.
    """
    return_json = {"items": [], "errors": []}

    if (
        get_setting_with_default("homey_address") == ""
        or get_setting_with_default("homey_token") == ""
    ):
        return_json["errors"].append("Homey address and/or API token not configured")
        return return_json

    homey_request_headers = {
        "Authorization": "Bearer " + get_setting_with_default("homey_token"),
        "content-type": "application/json",
    }

    try:
        # Try to get moods via the manager endpoint
        homey_api_address = (
            "http://" + get_setting_with_default("homey_address") + "/api/manager/mood"
        )
        logging.debug("Trying to get Homey moods via api address: " + homey_api_address)

        homey_response = requests.get(
            homey_api_address, headers=homey_request_headers, timeout=5, verify=False
        )

        if homey_response.status_code == 200:
            moods = homey_response.json()

            # Handle both dict and list response formats
            if isinstance(moods, dict):
                moods = moods.values()

            for mood in moods:
                if not isinstance(mood, dict):
                    continue

                mood_id = mood.get("id", "")
                mood_name = mood.get("name", "Unknown Mood")

                data = {
                    "type": "homey",
                    "label": "[M] " + mood_name,  # Prefix with [M] for Mood
                    "item_id": mood_id,
                    "scene_type": "homey_mood",
                    "item": mood,
                }
                return_json["items"].append(data)

        elif homey_response.status_code == 404:
            # Moods endpoint might not exist, log as debug
            logging.debug(
                "Homey moods endpoint not found (404). Moods support may not be available."
            )
        else:
            logging.warning(
                "Got status code "
                + str(homey_response.status_code)
                + " when fetching moods"
            )

    except Exception as e:
        # Moods fetching failure is non-fatal, just log it
        logging.debug("Could not fetch Homey moods: " + str(traceback.format_exc()))

    return return_json


def test_homey_connection():
    """
    Test if the Homey API connection is working.
    Returns (success: bool, message: str)
    """
    if (
        get_setting_with_default("homey_address") == ""
        or get_setting_with_default("homey_token") == ""
    ):
        return False, "Homey address and/or API token not configured"

    homey_request_headers = {
        "Authorization": "Bearer " + get_setting_with_default("homey_token"),
        "content-type": "application/json",
    }

    try:
        homey_api_address = (
            "http://"
            + get_setting_with_default("homey_address")
            + "/api/manager/devices/device"
        )
        logging.debug("Testing Homey connection to: " + homey_api_address)

        homey_response = requests.get(
            homey_api_address, headers=homey_request_headers, timeout=5, verify=False
        )

        if homey_response.status_code == 200:
            return True, "Successfully connected to Homey"
        elif homey_response.status_code == 401:
            return False, "Invalid API token. Please check your Homey API key"
        elif homey_response.status_code == 403:
            return False, "API token does not have required permissions"
        else:
            return (
                False,
                f"Connection failed with status code {homey_response.status_code}",
            )

    except requests.exceptions.Timeout:
        return False, "Connection to Homey timed out. Check the address and network"
    except requests.exceptions.ConnectionError:
        return (
            False,
            "Could not connect to Homey. Check the address and ensure Homey is online",
        )
    except Exception as e:
        return False, f"Connection test failed: {str(e)}"
