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
import logging

from .apps import restart_mqtt_manager_process
from .models import NSPanel, Room, LightState, Scene, RelayGroup
from web.settings_helper import get_setting_with_default, get_nspanel_setting_with_default, set_setting_value

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

# TODO: Rework how available entities are gathered
def get_all_available_entities(request):
    # TODO: Implement manually entered entities
    home_assistant_type_filter = []
    openhab_type_filter = []
    if "filter" in request.GET:
        filter_data = json.loads(request.GET["filter"])
        if "home_assistant_type_filter" in filter_data:
            home_assistant_type_filter = filter_data["home_assistant_type_filter"]

        if "openhab_type_filter" in filter_data:
            openhab_type_filter = filter_data["openhab_type_filter"]

    # Get Home Assistant lights
    return_json = {
        "entities": [],
        "errors": []
    }

    # Home Assistant
    if get_setting_with_default("home_assistant_token") != "" and get_setting_with_default("home_assistant_address") != "":
        home_assistant_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("home_assistant_token"),
            "content-type": "application/json",
        }
        try:
            environment = environ.Env()
            if "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true":
                home_assistant_api_address = get_setting_with_default("home_assistant_address") + "/core/api/states"
            else:
                home_assistant_api_address = get_setting_with_default("home_assistant_address") + "/api/states"
            logging.debug("Trying to get Home Assistant entities via api address: " + home_assistant_api_address)
            home_assistant_response = requests.get(home_assistant_api_address, headers=home_assistant_request_headers, timeout=5, verify=False)
            if home_assistant_response.status_code == 200:
                for entity in home_assistant_response.json():
                    entity_type = entity["entity_id"].split(".")[0]
                    if (len(home_assistant_type_filter) > 0 and entity_type in home_assistant_type_filter) or len(home_assistant_type_filter) == 0:
                        data = {
                            "type": "home_assistant",
                            "label": entity["entity_id"],
                            "entity_id": entity["entity_id"],
                            "raw_data": entity,
                        }
                        if "friendly_name" in entity["attributes"]:
                            data["label"] = entity["attributes"]["friendly_name"]

                        if data["entity_id"].startswith("light."):
                            data["entity_type"] = "light"
                        elif data["entity_id"].startswith("switch."):
                            data["entity_type"] = "switch"
                        else:
                            logging.warn("Unknown entity type for entity: " + data["entity_id"])

                        return_json["entities"].append(data)
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
    if get_setting_with_default("openhab_token") != "" and get_setting_with_default("openhab_address") != "":
        # TODO: Sort out how to map channels from items to the correct POST request when MQTT is received
        openhab_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("openhab_token"),
            "content-type": "application/json",
        }
        try:
            if "things" in openhab_type_filter:
                openhab_response = requests.get(get_setting_with_default(
                    "openhab_address") + "/rest/things", headers=openhab_request_headers, verify=False)

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
                                return_json["entities"].append({
                                    "type": "openhab",
                                    "openhab_type": "thing",
                                    "label": entity["label"],
                                    "entity_id": entity["label"],
                                    "items": items,
                                    "raw_data": entity,
                                })
                else:
                    return_json["errors"].append(
                        "Failed to get OpenHAB lights, got return code: " + str(openhab_response.status_code))
                    print("ERROR! Got status code other than 200. Got code: " +
                          str(openhab_response.status_code))
            elif "rules" in openhab_type_filter:
                openhab_response = requests.get(get_setting_with_default(
                    "openhab_address") + "/rest/rules", headers=openhab_request_headers, verify=False)

                if openhab_response.status_code == 200:
                    for entity in openhab_response.json():
                        if "name" in entity:
                            return_json["entities"].append({
                                "type": "openhab",
                                "openhab_type": "rule",
                                "label": entity["name"],
                                "entity_id": entity["uid"],
                                "raw_data": entity,
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

    return return_json



def get_nspanel_config(request):
    try:
        logging.info("Trying to load config for NSPanel with MAC " + request.GET['mac'])
        nspanel = NSPanel.objects.get(mac_address=request.GET["mac"])
        base = {}
        base["name"] = nspanel.friendly_name
        base["home"] = nspanel.room.id
        base["default_page"] = get_nspanel_setting_with_default(
            nspanel.id, "default_page", "0")
        base["raise_to_100_light_level"] = get_setting_with_default(
            "raise_to_100_light_level")
        base["color_temp_min"] = get_setting_with_default(
            "color_temp_min")
        base["color_temp_max"] = get_setting_with_default(
            "color_temp_max")
        base["reverse_color_temp"] = get_setting_with_default(
            "reverse_color_temp")
        base["min_button_push_time"] = get_setting_with_default(
            "min_button_push_time")
        base["button_long_press_time"] = get_setting_with_default(
            "button_long_press_time")
        base["special_mode_trigger_time"] = get_setting_with_default(
            "special_mode_trigger_time")
        base["special_mode_release_time"] = get_setting_with_default(
            "special_mode_release_time")
        base["screen_dim_level"] = get_nspanel_setting_with_default(
            nspanel.id, "screen_dim_level", get_setting_with_default("screen_dim_level"))
        base["screensaver_dim_level"] = get_nspanel_setting_with_default(
            nspanel.id, "screensaver_dim_level", get_setting_with_default("screensaver_dim_level"))
        base["screensaver_activation_timeout"] = get_nspanel_setting_with_default(
            nspanel.id, "screensaver_activation_timeout", get_setting_with_default("screensaver_activation_timeout"))
        base["screensaver_mode"] = get_nspanel_setting_with_default(
            nspanel.id, "screensaver_mode", get_setting_with_default("screensaver_mode"))
        base["clock_us_style"] = get_setting_with_default(
            "clock_us_style")
        base["use_fahrenheit"] = get_setting_with_default(
            "use_fahrenheit")
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


# TODO: Remove set_panel_status as it is no longer used. All this data is stored in memory in the MQTTManager application.
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
# TODO: Remove set_panel_status as it is no longer used. All this data is stored in memory in the MQTTManager application.
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
def restart_mqtt_manager(request):
    restart_mqtt_manager_process()
    return JsonResponse({"result": "OK"})


@csrf_exempt
def save_theme(request):
    set_setting_value("dark_theme", request.POST["dark"])
    return HttpResponse("OK", status=200)
