from django.http import HttpResponse, JsonResponse
from django.shortcuts import render, redirect
from datetime import datetime
from django.views.decorators.csrf import csrf_exempt
import json
import requests

from .models import NSPanel, Room, Light
from web.settings_helper import get_setting_with_default


def get_mqtt_manager_config(request):
    return_json = {}
    return_json["color_temp_min"] = get_setting_with_default(
        "color_temp_min", 2000)
    return_json["color_temp_max"] = get_setting_with_default(
        "color_temp_max", 6000)
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
    return_json["openhab_rgb_channel_name"] = get_setting_with_default(
        "openhab_rgb_channel_name", "")
    
    return_json["lights"] = []

    for light in Light.objects.all():
        lightConfig = {}
        lightConfig["name"] = light.friendly_name
        lightConfig["type"] = light.type
        lightConfig["can_dim"] = light.can_dim
        lightConfig["can_color_temperature"] = light.can_color_temperature
        lightConfig["can_rgb"] = light.can_rgb
        lightConfig["openhab_item_dimmer"] = light.openhab_item_dimmer
        lightConfig["openhab_item_color_temp"] = light.openhab_item_color_temp
        return_json["lights"].append(lightConfig)

    return JsonResponse(return_json)


def get_all_available_light_entities(request):
    # TODO: Implement OpenHAB and manually entered entities
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
                if (entity["entity_id"].startswith("light.")):
                    return_json["home_assistant_lights"].append({
                        "label": entity["entity_id"].replace("light.", ""),
                        "items": []
                    })
        except:
            print("Failed to get Home Assistant lights!")

    # OpenHAB
    if get_setting_with_default("openhab_token", "") != "":
        # TODO: Sort out how to map channels from items to the correct POST request when MQTT is received
        openhab_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("openhab_token", ""),
            "content-type": "application/json",
        }
        openhab_response = requests.get(get_setting_with_default("openhab_address", "") + "/rest/things", headers=openhab_request_headers)

        for entity in openhab_response.json():
            if "channels" in entity:
                add_entity = False
                items = []
                for channel in entity["channels"]:
                    # Check if this thing has a channel that indicates that it might be a light
                    if "itemType" in channel and (channel["itemType"] == "Dimmer" or channel["itemType"] == "Number" or channel["itemType"] == "Color"):
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
    new_panel = NSPanel.objects.filter(
        mac_address=data['mac_address']).first()

    if not new_panel:
        new_panel = NSPanel()

    new_panel.friendly_name = data['friendly_name']
    new_panel.mac_address = data['mac_address']
    new_panel.version = data["version"]
    new_panel.last_seen = datetime.now()
    new_panel.ip_address = get_client_ip(request)

    # If no room is set, select the first one as default
    if not new_panel.room:
        new_panel.room = Room.objects.first()

    # Save the update/Create new panel
    new_panel.save()
    return HttpResponse('OK', status=200)


def get_nspanel_config(request):
    nspanel = NSPanel.objects.get(mac_address=request.GET["mac"])
    base = {}
    base["home"] = nspanel.room.displayOrder
    base["rooms"] = {}
    for room in Room.objects.all().order_by('displayOrder'):
        base["rooms"][str(room.displayOrder)] = {}
        base["rooms"][str(room.displayOrder)]["name"] = room.friendly_name
        base["rooms"][str(room.displayOrder)]["lights"] = {}
        for light in room.light_set.all():
            base["rooms"][str(room.displayOrder)
                          ]["lights"][light.id] = {}
            base["rooms"][str(
                room.displayOrder)]["lights"][light.id]["name"] = light.friendly_name
            base["rooms"][str(
                room.displayOrder)]["lights"][light.id]["ceiling"] = light.is_ceiling_light
            base["rooms"][str(
                room.displayOrder)]["lights"][light.id]["can_dim"] = light.can_dim
            base["rooms"][str(
                room.displayOrder)]["lights"][light.id]["can_temperature"] = light.can_color_temperature
            base["rooms"][str(
                room.displayOrder)]["lights"][light.id]["can_rgb"] = light.can_rgb

    return JsonResponse(base)
