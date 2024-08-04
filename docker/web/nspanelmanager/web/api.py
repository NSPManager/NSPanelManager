from django.http import HttpResponse, JsonResponse
from django.shortcuts import render, redirect
from datetime import datetime
from django.views.decorators.csrf import csrf_exempt
from django.core.files.storage import FileSystemStorage
import json
import requests
import logging
import traceback

import hashlib
import psutil
import signal
import subprocess
import environ
import os

from .models import NSPanel, Room, Light, LightState, Scene, RelayGroup
from .apps import start_mqtt_manager
from web.settings_helper import get_setting_with_default, get_nspanel_setting_with_default, set_setting_value


def restart_mqtt_manager_process():
    for proc in psutil.process_iter():
        if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
            print("Killing running MQTTManager")
            proc.kill()
    start_mqtt_manager()


def send_mqttmanager_reload_command():
    for proc in psutil.process_iter():
        if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
            print("Found running MQTTManager. Sending reload command via SIGUSR1 signal.")
            os.kill(proc.pid, signal.SIGUSR1)


def get_file_md5sum(filename):
    fs = FileSystemStorage()
    if fs.exists(filename):
        return hashlib.md5(fs.open(filename).read()).hexdigest()
    else:
        return None


def get_mqtt_manager_config(request):
    environment = environ.Env()

    return_json = {
        "color_temp_min": int(get_setting_with_default("color_temp_min", "2000")),
        "color_temp_max": int(get_setting_with_default("color_temp_max", "6000")),
        "openhab_brightness_channel_name": get_setting_with_default("openhab_brightness_channel_name", ""),
        "openhab_brightness_channel_min": get_setting_with_default("openhab_brightness_channel_min", 0),
        "openhab_brightness_channel_max": get_setting_with_default("openhab_brightness_channel_max", 255),
        "openhab_color_temp_channel_name": get_setting_with_default("openhab_color_temp_channel_name", ""),
        "openhab_rgb_channel_name": get_setting_with_default("openhab_rgb_channel_name", ""),
        "clock_us_style": get_setting_with_default("clock_us_style", False) == "True",
        "use_fahrenheit": get_setting_with_default("use_fahrenheit", False) == "True",
        "turn_on_behavior": get_setting_with_default("turn_on_behavior", "color_temp"),
        "max_log_buffer_size": get_setting_with_default("max_log_buffer_size", "10"),
        "manager_address": get_setting_with_default("manager_address", ""),
        "manager_port": get_setting_with_default("manager_port", ""),
        "date_format": get_setting_with_default("date_format", "%a %d/%m %Y"),
        "outside_temp_sensor_provider": get_setting_with_default("outside_temp_sensor_provider", ""),
        "outside_temp_sensor_entity_id": get_setting_with_default("outside_temp_sensor_entity_id", ""),
        "weather_location_latitude": get_setting_with_default("location_latitude", ""),
        "weather_location_longitude": get_setting_with_default("location_longitude", ""),
        "weather_wind_speed_format": get_setting_with_default("wind_speed_format", "kmh"),
        "weather_precipitation_format": get_setting_with_default("precipitation_format", "mm"),
        "weather_update_interval": int(get_setting_with_default("weather_update_interval", 10)),
    }

    if "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true":
        return_json["is_home_assistant_addon"] = True
    else:
        return_json["is_home_assistant_addon"] = False
    fs = FileSystemStorage()
    return_json["icon_mapping"] = json.loads(
        fs.open("icon_mapping.json").read())

    return_json["lights"] = {}
    for light in Light.objects.all():
        lightConfig = {}
        lightConfig["type"] = "light"
        lightConfig["id"] = light.id
        lightConfig["name"] = light.friendly_name
        lightConfig["room_name"] = light.room.friendly_name
        lightConfig["room_id"] = light.room.id
        lightConfig["light_type"] = light.type
        lightConfig["can_dim"] = light.can_dim
        lightConfig["can_color_temperature"] = light.can_color_temperature
        lightConfig["can_rgb"] = light.can_rgb
        lightConfig["home_assistant_name"] = light.home_assistant_name
        lightConfig["openhab_name"] = light.openhab_name
        lightConfig["openhab_control_mode"] = light.openhab_control_mode
        lightConfig["openhab_item_switch"] = light.openhab_item_switch
        lightConfig["openhab_item_dimmer"] = light.openhab_item_dimmer
        lightConfig["openhab_item_color_temp"] = light.openhab_item_color_temp
        lightConfig["openhab_item_rgb"] = light.openhab_item_rgb
        return_json["lights"][light.id] = lightConfig

    return_json["nspanels"] = {}
    for panel in NSPanel.objects.all():
        return_json["nspanels"][panel.id] = get_nspanel_json_representation(
            panel)

    return_json["scenes"] = []
    for scene in Scene.objects.all():
        scene_info = {
            "id": scene.id,
            "type": "scene",
            "scene_type": scene.scene_type,
            "scene_name": scene.friendly_name,
            "entity_name": scene.backend_name,
            "room_name": scene.room.friendly_name if scene.room != None else None,
            "room_id": scene.room.id if scene.room != None else None,
            "light_states": []
        }
        for state in scene.lightstate_set.all():
            scene_info["light_states"].append({
                "type": "light_state",
                "light_id": state.light.id,
                "light_type": state.light.type,
                "color_mode": state.color_mode,
                "light_level": state.light_level,
                "color_temp": state.color_temperature,
                "hue": state.hue,
                "saturation": state.saturation
            })
        return_json["scenes"].append(scene_info)

    return_json["rooms"] = []
    for room in Room.objects.all():
        room_info = {
            "type": "room",
            "id": room.id,
            "name": room.friendly_name
        }
        return_json["rooms"].append(room_info)

    return_json["nspanel_relay_groups"] = []
    for relay_group in RelayGroup.objects.all():
        rg_info = {
            "type": "nspanel_relay_group",
            "id": relay_group.id,
            "name": relay_group.friendly_name,
            "relays": []
        }
        for relay_binding in relay_group.relaygroupbinding_set.all():
            rg_info["relays"].append(
                {"nspanel_id": relay_binding.nspanel.id, "relay_num": relay_binding.relay_num})
        return_json["nspanel_relay_groups"].append(rg_info)

    return JsonResponse(return_json)


def get_nspanel_json_representation(panel):
    panel_config = {
        "type": "nspanel",
        "id": panel.id,
        "mac": panel.mac_address,
        "name": panel.friendly_name,
        "is_us_panel": get_nspanel_setting_with_default(panel.id, "is_us_panel", "False") == "True",
        "address": panel.ip_address,
        "relay1_is_light": get_nspanel_setting_with_default(panel.id, "relay1_is_light", "False") == "True",
        "relay2_is_light": get_nspanel_setting_with_default(panel.id, "relay2_is_light", "False") == "True",
        "denied": "True" if panel.denied else "False"
    }
    return panel_config


def get_nspanels_warnings(request):
    md5_firmware = get_file_md5sum("firmware.bin")
    md5_data_file = get_file_md5sum("data_file.bin")
    md5_tft_file = get_file_md5sum("gui.tft")
    md5_us_tft_file = get_file_md5sum("gui_us.tft")
    nspanels = []

    for nspanel in NSPanel.objects.all():
        panel_info = {}
        panel_info["nspanel"] = {
            "name": nspanel.friendly_name,
            "mac": nspanel.mac_address
        }
        panel_info["warnings"] = ""
        for panel in NSPanel.objects.all():
            if panel == nspanel:
                continue
            elif panel.friendly_name == nspanel.friendly_name:
                panel_info["warnings"] += "Two or more panels exists with the same name. This may have unintended consequences\n"
                break
        if nspanel.md5_firmware != md5_firmware or nspanel.md5_data_file != md5_data_file:
            panel_info["warnings"] += "Firmware update available.\n"
        if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "False" and nspanel.md5_tft_file != md5_tft_file:
            panel_info["warnings"] += "GUI update available.\n"
        if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "True" and nspanel.md5_tft_file != md5_us_tft_file:
            panel_info["warnings"] += "GUI update available.\n"
        nspanels.append(panel_info)
    return JsonResponse({"panels": nspanels})


def get_all_available_entities(request):
    # TODO: Implement manually entered entities
    home_assistant_type_filter = []
    if "home_assistant_type_filter" in request.GET:
        home_assistant_type_filter = json.loads(
            request.GET["home_assistant_type_filter"])

    openhab_type_filter = []
    if "openhab_type_filter" in request.GET:
        openhab_type_filter = json.loads(
            request.GET["openhab_type_filter"])

    # Get Home Assistant lights
    return_json = {}
    return_json["home_assistant_entities"] = []
    return_json["openhab_entities"] = []
    return_json["manual_entities"] = []
    return_json["errors"] = []

    # Home Assistant
    if get_setting_with_default("home_assistant_token", "") != "" and get_setting_with_default("home_assistant_address", "") != "":
        home_assistant_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("home_assistant_token", ""),
            "content-type": "application/json",
        }
        try:
            environment = environ.Env()
            if "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true":
                home_assistant_api_address = get_setting_with_default(
                    "home_assistant_address", "") + "/core/api/states"
            else:
                home_assistant_api_address = get_setting_with_default(
                    "home_assistant_address", "") + "/api/states"
            print("Trying to get Home Assistant entities via api address: " +
                  home_assistant_api_address)
            home_assistant_response = requests.get(
                home_assistant_api_address, headers=home_assistant_request_headers, timeout=5, verify=False)
            if home_assistant_response.status_code == 200:
                for entity in home_assistant_response.json():
                    entity_type = entity["entity_id"].split(".")[0]
                    if (len(home_assistant_type_filter) > 0 and entity_type in home_assistant_type_filter) or len(home_assistant_type_filter) == 0:
                        data = {
                            "label": entity["entity_id"],
                            "entity_id": entity["entity_id"],
                            "items": []
                        }
                        if "friendly_name" in entity["attributes"]:
                            data["label"] = entity["attributes"]["friendly_name"]

                        return_json["home_assistant_entities"].append(data)
            else:
                return_json["errors"].append(
                    "Failed to get Home Assistant lights, got return code: " + str(home_assistant_response.status_code))
                print("ERROR! Got status code other than 200. Got code: " +
                      str(home_assistant_response.status_code))
        except Exception as e:
            return_json["errors"].append(
                "Failed to get Home Assistant lights: " + str(traceback.format_exc()))
            logging.exception("Failed to get Home Assistant lights!")
    else:
        print("No home assistant configuration values. Will not gather Home Assistant entities.")

    # OpenHAB
    if get_setting_with_default("openhab_token", "") != "" and get_setting_with_default("openhab_address", "") != "":
        # TODO: Sort out how to map channels from items to the correct POST request when MQTT is received
        openhab_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("openhab_token", ""),
            "content-type": "application/json",
        }
        try:
            if "things" in openhab_type_filter:
                openhab_response = requests.get(get_setting_with_default(
                    "openhab_address", "") + "/rest/things", headers=openhab_request_headers, verify=False)

                if openhab_response.status_code == 200:
                    for entity in openhab_response.json():
                        if "channels" in entity:
                            add_entity = False
                            items = []
                            for channel in entity["channels"]:
                                # Check if this thing has a channel that indicates that it might be a light
                                add_items_with_channels_of_type = [
                                    "Dimmer", "Number", "Color", "Switch", "String"]
                                if "itemType" in channel and (channel["itemType"] in add_items_with_channels_of_type):
                                    add_entity = True
                                if "linkedItems" in channel:
                                    # Add all available items to the list of items for this thing
                                    for linkedItem in channel["linkedItems"]:
                                        if linkedItem not in items:
                                            items.append(linkedItem)
                            if add_entity:
                                # return_json["openhab_lights"].append(entity["label"])
                                return_json["openhab_entities"].append({
                                    "label": entity["label"],
                                    "entity_id": entity["label"],
                                    "items": items
                                })
                else:
                    return_json["errors"].append(
                        "Failed to get OpenHAB lights, got return code: " + str(openhab_response.status_code))
                    print("ERROR! Got status code other than 200. Got code: " +
                          str(openhab_response.status_code))
            elif "rules" in openhab_type_filter:
                openhab_response = requests.get(get_setting_with_default(
                    "openhab_address", "") + "/rest/rules", headers=openhab_request_headers, verify=False)

                if openhab_response.status_code == 200:
                    for entity in openhab_response.json():
                        if "name" in entity:
                            return_json["openhab_entities"].append({
                                "label": entity["name"],
                                "entity_id": entity["uid"],
                                "items": []
                            })
                else:
                    return_json["errors"].append(
                        "Failed to get OpenHAB lights, got return code: " + str(openhab_response.status_code))
                    print("ERROR! Got status code other than 200. Got code: " +
                          str(openhab_response.status_code))
        except Exception as e:
            return_json["errors"].append(
                "Failed to get OpenHAB lights: " + str(traceback.format_exc()))
            logging.exception("Failed to get OpenHAB lights!")
    else:
        print("No OpenHAB configuration values. Will not gather OpenHAB entities.")

    return JsonResponse(return_json)


def get_client_ip(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[-1]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip


@csrf_exempt
def register_nspanel(request):
    """Update the already existing NSPanel OR create a new one"""
    data = json.loads(request.body)
    if "mac" in data:
        data["mac_address"] = data["mac"]
    if "mac_origin" in data:
        data["mac_address"] = data["mac_origin"]
    new_panel = NSPanel.objects.filter(mac_address=data['mac_address']).first()
    panel_already_exists = True

    if not new_panel:
        new_panel = NSPanel()
        new_panel.friendly_name = data['friendly_name']
        panel_already_exists = False

    new_panel.mac_address = data['mac_address']
    new_panel.version = data["version"] if "version" in data else ""
    new_panel.ip_address = ""  # TODO: Remove ip_address from DB
    fs = FileSystemStorage()
    if "md5_firmware" in data:
        if data["md5_firmware"] == "":
            new_panel.md5_firmware = hashlib.md5(
                fs.open("firmware.bin").read()).hexdigest()
        else:
            new_panel.md5_firmware = data["md5_firmware"]
    if "md5_data_file" in data:
        if data["md5_data_file"] == "":
            new_panel.md5_data_file = hashlib.md5(
                fs.open("data_file.bin").read()).hexdigest()
        else:
            new_panel.md5_data_file = data["md5_data_file"]
    # TFT file will never be flashed by default with a new panel, always set the MD5 from registration
    if "md5_tft_file" in data:
        new_panel.md5_tft_file = data["md5_tft_file"]

    if "denied" in data:
        if str(data["denied"]).lower() == "true":
            new_panel.denied = True
        else:
            new_panel.denied = False

    # If no room is set, select the first one as default
    try:
        if not new_panel.room:
            new_panel.room = Room.objects.first()
    except NSPanel.room.RelatedObjectDoesNotExist:
        new_panel.room = Room.objects.first()

    # Save the update/Create new panel
    new_panel.save()
    json_response = get_nspanel_json_representation(new_panel)
    return JsonResponse(json_response)


def delete_panel(request, panel_id: int):
    NSPanel.objects.get(id=panel_id).delete()
    # restart_mqtt_manager()
    send_mqttmanager_reload_command()
    return HttpResponse("OK", status=200)


def get_nspanel_config(request):
    print(request)
    print(request.GET)
    try:
        logging.info("Trying to load config for NSPanel with MAC " + request.GET['mac'])
        nspanel = NSPanel.objects.get(mac_address=request.GET["mac"])
        base = {}
        base["name"] = nspanel.friendly_name
        base["home"] = nspanel.room.id
        base["default_page"] = get_nspanel_setting_with_default(
            nspanel.id, "default_page", "0")
        base["raise_to_100_light_level"] = get_setting_with_default(
            "raise_to_100_light_level", 95)
        base["color_temp_min"] = get_setting_with_default(
            "color_temp_min", 2000)
        base["color_temp_max"] = get_setting_with_default(
            "color_temp_max", 6000)
        base["reverse_color_temp"] = get_setting_with_default(
            "reverse_color_temp", False)
        base["min_button_push_time"] = get_setting_with_default(
            "min_button_push_time", 50)
        base["button_long_press_time"] = get_setting_with_default(
            "button_long_press_time", 5000)
        base["special_mode_trigger_time"] = get_setting_with_default(
            "special_mode_trigger_time", 300)
        base["special_mode_release_time"] = get_setting_with_default(
            "special_mode_release_time", 5000)
        base["mqtt_ignore_time"] = get_setting_with_default(
            "mqtt_ignore_time", 3000)
        base["screen_dim_level"] = get_nspanel_setting_with_default(
            nspanel.id, "screen_dim_level", get_setting_with_default("screen_dim_level", 100))
        base["screensaver_dim_level"] = get_nspanel_setting_with_default(
            nspanel.id, "screensaver_dim_level", get_setting_with_default("screensaver_dim_level", 0))
        base["screensaver_activation_timeout"] = get_nspanel_setting_with_default(
            nspanel.id, "screensaver_activation_timeout", get_setting_with_default("screensaver_activation_timeout", 30000))
        base["screensaver_mode"] = get_nspanel_setting_with_default(
            nspanel.id, "screensaver_mode", get_setting_with_default("screensaver_mode", "with_background"))
        base["clock_us_style"] = get_setting_with_default(
            "clock_us_style", "False")
        base["use_fahrenheit"] = get_setting_with_default(
            "use_fahrenheit", "False")
        base["is_us_panel"] = get_nspanel_setting_with_default(
            nspanel.id, "is_us_panel", "False")
        base["lock_to_default_room"] = get_nspanel_setting_with_default(
            nspanel.id, "lock_to_default_room", "False")
        base["reverse_relays"] = get_nspanel_setting_with_default(
            nspanel.id, "reverse_relays", False)
        base["relay1_default_mode"] = get_nspanel_setting_with_default(
            nspanel.id, "relay1_default_mode", "False")
        base["relay2_default_mode"] = get_nspanel_setting_with_default(
            nspanel.id, "relay2_default_mode", "False")
        base["temperature_calibration"] = float(
            get_nspanel_setting_with_default(nspanel.id, "temperature_calibration", 0))
        base["button1_mode"] = nspanel.button1_mode
        base["button1_mqtt_topic"] = get_nspanel_setting_with_default(
            nspanel.id, "button1_mqtt_topic", "")
        base["button1_mqtt_payload"] = get_nspanel_setting_with_default(
            nspanel.id, "button1_mqtt_payload", "")
        base["button2_mode"] = nspanel.button2_mode
        base["button2_mqtt_topic"] = get_nspanel_setting_with_default(
            nspanel.id, "button2_mqtt_topic", "")
        base["button2_mqtt_payload"] = get_nspanel_setting_with_default(
            nspanel.id, "button2_mqtt_payload", "")

        if nspanel.button1_detached_mode_light:
            base["button1_detached_light"] = nspanel.button1_detached_mode_light.id
        else:
            base["button1_detached_mode_light"] = -1

        if nspanel.button2_detached_mode_light:
            base["button2_detached_light"] = nspanel.button2_detached_mode_light.id
        else:
            base["button2_detached_light"] = -1
        base["rooms"] = []
        for room in Room.objects.all().order_by('displayOrder'):
            base["rooms"].append(room.id)
        base["scenes"] = {}
        for scene in Scene.objects.filter(room__isnull=True):
            base["scenes"][scene.id] = {}
            base["scenes"][scene.id]["name"] = scene.friendly_name
            if scene.scene_type == "nspm_scene":
                base["scenes"][scene.id]["can_save"] = True
            else:
                base["scenes"][scene.id]["can_save"] = False
        return JsonResponse(base)
    except Exception as ex:
        logging.exception(ex)
        print("Tried to get NSPanel config for panel that was not registered.")
        return HttpResponse("", status=500)


def get_room_config(request, room_id: int):
    room = Room.objects.get(id=room_id)
    return_json = {}
    return_json["name"] = room.friendly_name
    return_json["lights"] = {}
    for light in room.light_set.all():
        return_json["lights"][light.id] = {}
        return_json["lights"][light.id]["name"] = light.friendly_name
        return_json["lights"][light.id]["ceiling"] = light.is_ceiling_light
        return_json["lights"][light.id]["can_dim"] = light.can_dim
        return_json["lights"][light.id]["can_temperature"] = light.can_color_temperature
        return_json["lights"][light.id]["can_rgb"] = light.can_rgb
        return_json["lights"][light.id]["view_position"] = light.room_view_position
    return_json["scenes"] = {}
    for scene in room.scene_set.all():
        return_json["scenes"][scene.id] = {}
        return_json["scenes"][scene.id]["name"] = scene.friendly_name
        if scene.scene_type == "nspm_scene":
            return_json["scenes"][scene.id]["can_save"] = True
        else:
            return_json["scenes"][scene.id]["can_save"] = False
    return JsonResponse(return_json)


def get_light_config(request, light_id: int):
    light = Light.objects.get(id=light_id)
    return_json = {}
    return_json["id"] = light.id
    return_json["name"] = light.friendly_name
    return_json["type"] = light.type
    return_json["ceiling"] = light.is_ceiling_light
    return_json["can_dim"] = light.can_dim
    return_json["can_color_temperature"] = light.can_color_temperature
    return_json["can_rgb"] = light.can_rgb
    return_json["home_assistant_name"] = light.home_assistant_name
    return_json["openhab_name"] = light.openhab_name
    return_json["openhab_control_mode"] = light.openhab_control_mode
    return_json["openhab_item_switch"] = light.openhab_item_switch
    return_json["openhab_item_dimmer"] = light.openhab_item_dimmer
    return_json["openhab_item_color_temp"] = light.openhab_item_color_temp
    return_json["openhab_item_rgb"] = light.openhab_item_rgb
    return JsonResponse(return_json)


@csrf_exempt
def set_panel_status(request, panel_mac: str):
    nspanels = NSPanel.objects.filter(mac_address=panel_mac)
    if nspanels.exists():
        nspanel = nspanels.first()
        # We got a match
        json_payload = json.loads(request.body.decode('utf-8'))
        nspanel.wifi_rssi = int(json_payload["rssi"])
        nspanel.heap_used_pct = int(json_payload["heap_used_pct"])
        nspanel.temperature = round(json_payload["temperature"], 2)
        nspanel.save()
        return HttpResponse("", status=200)

    return HttpResponse("", status=500)


@csrf_exempt
def set_panel_online_status(request, panel_mac: str):
    nspanels = NSPanel.objects.filter(mac_address=panel_mac)
    if nspanels.exists():
        nspanel = nspanels.first()
        # We got a match
        payload = json.loads(request.body.decode('utf-8'))
        nspanel.online_state = (payload["state"] == "online")
        nspanel.save()
        return HttpResponse("", status=200)

    return HttpResponse("Panel is not registered", status=500)


def get_scenes(request):
    return_json = {}
    return_json["scenes"] = []
    for scene in Scene.objects.all():
        scene_info = {
            "scene_id": scene.id,
            "scene_name": scene.friendly_name,
            "room_name": scene.room.friendly_name if scene.room != None else None,
            "room_id": scene.room.id if scene.room != None else None,
            "light_states": []
        }
        for state in scene.lightstate_set.all():
            scene_info["light_states"].append({
                "light_id": state.light.id,
                "light_type": state.light.type,
                "color_mode": state.color_mode,
                "light_level": state.light_level,
                "color_temp": state.color_temperature,
                "hue": state.hue,
                "saturation": state.saturation
            })
        return_json["scenes"].append(scene_info)
    return JsonResponse(return_json)


@csrf_exempt
def save_scene(request):
    data = json.loads(request.body)
    scene = Scene.objects.filter(id=data["scene_id"]).first()
    if scene:
        scene.lightstate_set.all().delete()  # Remove all old states
        for light_state in data["light_states"]:
            light = Light.objects.filter(id=light_state["light_id"]).first()
            if light:
                new_state = LightState()
                new_state.light = light
                new_state.scene = scene
                if light_state["mode"] == "dimmer":
                    new_state.color_mode = "dimmer"
                    new_state.light_level = light_state["level"]
                    new_state.color_temperature = light_state["color_temp"]
                elif light_state["mode"] == "color":
                    new_state.color_mode = "color"
                    new_state.light_level = light_state["level"]
                    new_state.hue = light_state["hue"]
                    new_state.saturation = light_state["saturation"]
                new_state.save()
            else:
                logging.error("ERROR: Couldn't find a light with ID " +
                              light_state["light_id"] + ". Will skip light!")
        return HttpResponse("OK", status=200)
    else:
        return HttpResponse("Scene does not exist!", status=500)


@csrf_exempt
def restart_mqtt_manager(request):
    restart_mqtt_manager_process()
    return JsonResponse({"result": "OK"})


@csrf_exempt
def save_theme(request):
    set_setting_value("dark_theme", request.POST["dark"])
    return HttpResponse("OK", status=200)
