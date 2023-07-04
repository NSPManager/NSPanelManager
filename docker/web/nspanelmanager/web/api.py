from django.http import HttpResponse, JsonResponse
from django.shortcuts import render, redirect
from datetime import datetime
from django.views.decorators.csrf import csrf_exempt
from django.core.files.storage import FileSystemStorage
import json
import requests
import logging

import hashlib
import psutil
import subprocess

from .models import NSPanel, Room, Light, LightState, Scene
from web.settings_helper import get_setting_with_default, get_nspanel_setting_with_default


def restart_mqtt_manager():
    for proc in psutil.process_iter():
        if "./mqtt_manager.py" in proc.cmdline():
            logging.info("Killing existing mqtt_manager")
            proc.kill()
    # Restart the process
    logging.info("Starting a new mqtt_manager")
    subprocess.Popen(["/usr/local/bin/python", "./mqtt_manager.py"], cwd="/usr/src/app/")


def get_mqtt_manager_config(request):
    return_json = {}
    return_json["color_temp_min"] = int(
        get_setting_with_default("color_temp_min", 2000))
    return_json["color_temp_max"] = int(
        get_setting_with_default("color_temp_max", 6000))
    return_json["mqtt_server"] = get_setting_with_default("mqtt_server", "")
    return_json["mqtt_port"] = int(get_setting_with_default("mqtt_port", 1883))
    return_json["mqtt_username"] = get_setting_with_default(
        "mqtt_username", "")
    return_json["mqtt_password"] = get_setting_with_default(
        "mqtt_password", "")
    return_json["home_assistant_address"] = get_setting_with_default(
        "home_assistant_address", "")
    return_json["home_assistant_token"] = get_setting_with_default(
        "home_assistant_token", "")
    return_json["openhab_address"] = get_setting_with_default(
        "openhab_address", "")
    return_json["openhab_token"] = get_setting_with_default(
        "openhab_token", "")
    return_json["openhab_brightness_channel_name"] = get_setting_with_default(
        "openhab_brightness_channel_name", "")
    return_json["openhab_brightness_channel_min"] = get_setting_with_default(
        "openhab_brightness_channel_min", 0)
    return_json["openhab_brightness_channel_max"] = get_setting_with_default(
        "openhab_brightness_channel_max", 255)
    return_json["openhab_color_temp_channel_name"] = get_setting_with_default(
        "openhab_color_temp_channel_name", "")
    return_json["openhab_rgb_channel_name"] = get_setting_with_default("openhab_rgb_channel_name", "")
    return_json["clock_us_style"] = get_setting_with_default("clock_us_style", False)
    return_json["use_farenheit"] = get_setting_with_default("use_farenheit", False)

    return_json["lights"] = {}
    for light in Light.objects.all():
        lightConfig = {}
        lightConfig["id"] = light.id
        lightConfig["name"] = light.friendly_name
        lightConfig["room_name"] = light.room.friendly_name
        lightConfig["type"] = light.type
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
        panel_config = {
            "id": panel.id,
            "mac": panel.mac_address,
            "name": panel.friendly_name
        }
        return_json["nspanels"][panel.id] = panel_config

    return JsonResponse(return_json)


def get_all_available_light_entities(request):
    # TODO: Implement manually entered entities
    # Get Home Assistant lights
    return_json = {}
    return_json["home_assistant_lights"] = []
    return_json["openhab_lights"] = []
    return_json["manual_lights"] = []

    # Home Assistant
    if get_setting_with_default("home_assistant_token", "") != "":
        home_assistant_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("home_assistant_token", ""),
            "content-type": "application/json",
        }
        try:
            home_assistant_response = requests.get(
                get_setting_with_default("home_assistant_address", "") + "/api/states", headers=home_assistant_request_headers, timeout=5)
            for entity in home_assistant_response.json():
                if (entity["entity_id"].startswith("light.") or entity["entity_id"].startswith("switch.")):
                    return_json["home_assistant_lights"].append({
                        "label": entity["entity_id"],
                        "items": []
                    })
        except:
            logging.exception("Failed to get Home Assistant lights!")

    # OpenHAB
    if get_setting_with_default("openhab_token", "") != "":
        # TODO: Sort out how to map channels from items to the correct POST request when MQTT is received
        openhab_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("openhab_token", ""),
            "content-type": "application/json",
        }
        openhab_response = requests.get(get_setting_with_default(
            "openhab_address", "") + "/rest/things", headers=openhab_request_headers)

        for entity in openhab_response.json():
            if "channels" in entity:
                add_entity = False
                items = []
                for channel in entity["channels"]:
                    # Check if this thing has a channel that indicates that it might be a light
                    if "itemType" in channel and (channel["itemType"] == "Dimmer" or channel["itemType"] == "Number" or channel["itemType"] == "Color" or channel["itemType"] == "Switch"):
                        add_entity = True
                    if "linkedItems" in channel:
                        # Add all available items to the list of items for this thing
                        for linkedItem in channel["linkedItems"]:
                            if linkedItem not in items:
                                items.append(linkedItem)
                if add_entity:
                    # return_json["openhab_lights"].append(entity["label"])
                    return_json["openhab_lights"].append({
                        "label": entity["label"],
                        "items": items
                    })

    return JsonResponse(return_json)


def get_client_ip(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[0]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip


@csrf_exempt
def register_nspanel(request):
    """Update the already existing NSPanel OR create a new one"""
    data = json.loads(request.body)
    new_panel = NSPanel.objects.filter(mac_address=data['mac_address']).first()
    panel_already_exists = True

    if not new_panel:
        new_panel = NSPanel()
        new_panel.friendly_name = data['friendly_name']
        panel_already_exists = False

    new_panel.mac_address = data['mac_address']
    new_panel.version = data["version"]
    new_panel.last_seen = datetime.now()
    new_panel.ip_address = get_client_ip(request)
    fs = FileSystemStorage()
    if "md5_firmware" in data:
        if data["md5_firmware"] == "":
            new_panel.md5_firmware = hashlib.md5(fs.open("firmware.bin").read()).hexdigest()
        else:
            new_panel.md5_firmware = data["md5_firmware"]
    if "md5_data_file" in data:
        if data["md5_data_file"] == "":
            new_panel.md5_data_file = hashlib.md5(fs.open("data_file.bin").read()).hexdigest()
        else:
            new_panel.md5_data_file = data["md5_data_file"]
    # TFT file will never be flashed by default with a new panel, always set the MD5 from registration
    if "md5_tft_file" in data:
        new_panel.md5_tft_file = data["md5_tft_file"]

    # If no room is set, select the first one as default
    try:
        if not new_panel.room:
            new_panel.room = Room.objects.first()
    except NSPanel.room.RelatedObjectDoesNotExist:
        new_panel.room = Room.objects.first()

    # Save the update/Create new panel
    new_panel.save()
    if not panel_already_exists:
        restart_mqtt_manager()
    return HttpResponse('OK', status=200)


def delete_panel(request, panel_id: int):
    NSPanel.objects.get(id=panel_id).delete()
    restart_mqtt_manager()
    return redirect('/')


def get_nspanel_config(request):
    nspanel = NSPanel.objects.get(mac_address=request.GET["mac"])
    base = {}
    base["name"] = nspanel.friendly_name
    base["home"] = nspanel.room.id
    base["raise_to_100_light_level"] = get_setting_with_default(
        "raise_to_100_light_level", 95)
    base["color_temp_min"] = get_setting_with_default("color_temp_min", 2000)
    base["color_temp_max"] = get_setting_with_default("color_temp_max", 6000)
    base["reverse_color_temp"] = get_setting_with_default("reverse_color_temp", False)
    base["min_button_push_time"] = get_setting_with_default("min_button_push_time", 50)
    base["button_long_press_time"] = get_setting_with_default("button_long_press_time", 5000)
    base["special_mode_trigger_time"] = get_setting_with_default("special_mode_trigger_time", 300)
    base["special_mode_release_time"] = get_setting_with_default("special_mode_release_time", 5000)
    base["mqtt_ignore_time"] = get_setting_with_default("mqtt_ignore_time", 3000)
    base["screen_dim_level"] = get_nspanel_setting_with_default(nspanel.id, "screen_dim_level", get_setting_with_default("screen_dim_level", 100))
    base["screensaver_dim_level"] = get_nspanel_setting_with_default(nspanel.id, "screensaver_dim_level", get_setting_with_default("screensaver_dim_level", 0))
    base["screensaver_activation_timeout"] = get_nspanel_setting_with_default(nspanel.id, "screensaver_activation_timeout", get_setting_with_default("screensaver_activation_timeout", 30000))
    base["show_screensaver_clock"] = get_nspanel_setting_with_default(nspanel.id, "show_screensaver_clock", get_setting_with_default("show_screensaver_clock", False))
    base["clock_us_style"] = get_setting_with_default("clock_us_style", False)
    base["button1_mode"] = nspanel.button1_mode
    base["use_farenheit"] = get_setting_with_default("use_farenheit", False)
    base["lock_to_default_room"] = get_nspanel_setting_with_default(nspanel.id, "lock_to_default_room", "False")
    base["relay1_default_mode"] = get_nspanel_setting_with_default(nspanel.id, "relay1_default_mode", False)
    base["relay2_default_mode"] = get_nspanel_setting_with_default(nspanel.id, "relay2_default_mode", False)
    if nspanel.button1_detached_mode_light:
        base["button1_detached_light"] = nspanel.button1_detached_mode_light.id
    else:
        base["button1_detached_mode_light"] = -1
    base["button2_mode"] = nspanel.button2_mode
    if nspanel.button2_detached_mode_light:
        base["button2_detached_light"] = nspanel.button2_detached_mode_light.id
    else:
        base["button2_detached_light"] = -1
    base["rooms"] = []
    for room in Room.objects.all().order_by('displayOrder'):
        base["rooms"].append(room.id)
    return JsonResponse(base)


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


def reboot_nspanel(request):
    address = request.GET["address"]
    try:
        requests.get(F"http://{address}/do_reboot")
    except:
        pass
    return redirect("/")


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
            "room_name": scene.room.friendly_name,
            "room_id": scene.room.id,
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
        scene.lightstate_set.all().delete() # Remove all old states
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
                logging.error("ERROR: Couldn't find a light with ID " + light_state["light_id"] + ". Will skip light!")
        return HttpResponse("OK", status=200)
    else:
        return HttpResponse("Scene does not exist!", status=500)
