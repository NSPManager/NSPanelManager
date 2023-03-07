from django.http import HttpResponse, JsonResponse
from django.shortcuts import render, redirect
from datetime import datetime
from django.views.decorators.csrf import csrf_exempt
import json
import requests

from .models import NSPanel, Room


def get_all_available_light_entities(request):
    # TODO: Implement OpenHAB and manually entered entities
    # Get Home Assistant lights
    return_json = {}
    return_json["home_assistant_lights"] = []
    return_json["openhab_lights"] = []
    return_json["manual_lights"] = []

    home_assistant_request_headers = {
        "Authorization": "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI0ZTEwM2QwODkxMmI0NTNiYWZhZWY4ZmI0NjgwM2NmZSIsImlhdCI6MTY3NzQyNDU2NSwiZXhwIjoxOTkyNzg0NTY1fQ.alIIkTQl-HxjVSFPOViLMOGgkHYeijH8o9cQ60aMqRw",
        "content-type": "application/json",
    }
    home_assistant_response = requests.get(
        "http://10.2.0.6:8123/api/states", headers=home_assistant_request_headers)

    for entity in home_assistant_response.json():
        if (entity["entity_id"].startswith("light.")):
            return_json["home_assistant_lights"].append(entity["entity_id"])

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
