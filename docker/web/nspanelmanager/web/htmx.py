from requests import delete
from django.shortcuts import render, redirect, HttpResponse
from django.core.files.storage import FileSystemStorage
from django.views.decorators.csrf import csrf_exempt
from django.http import JsonResponse

from .components.nspanel_status_header.nspanel_status_header import NSPanelHeader

from .api import get_all_available_entities
import hashlib
import psutil
import subprocess
import logging
import os
import signal
import json
from time import sleep

#from nspanelmanager.web.mqttmanager_ipc import send_ipc_request
from .mqttmanager_ipc import send_ipc_request

from .models import NSPanel, Room, Light, Settings, Scene, RelayGroup, RelayGroupBinding
from .apps import start_mqtt_manager
from web.settings_helper import delete_nspanel_setting, get_setting_with_default, set_setting_value, get_nspanel_setting_with_default, set_nspanel_setting_value

def partial_nspanel_index_view(request, nspanel_id):
    try:
        if request.method == 'GET':
            data = {
                "nspanel": {
                    "status": send_ipc_request(F"nspanel/{nspanel_id}/status", {"command": "get"}),
                    "data": NSPanel.objects.get(id=nspanel_id)
                },
            }
            return render(request, 'partial/nspanel_index_view_htmx.html', data)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)

@csrf_exempt
def nspanel_reboot(request, nspanel_id):
    try:
        if request.method == 'POST':
            response = send_ipc_request(F"nspanel/{nspanel_id}/reboot", {})
            if response["status"] == "ok":
                return JsonResponse({"status": "ok"}, status=200)
            else:
                return JsonResponse({"status": "error"}, status=500)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_update_screen(request, nspanel_id):
    try:
        if request.method == 'POST':
            response = send_ipc_request(F"nspanel/{nspanel_id}/update_screen", {})
            if response["status"] == "ok":
                return JsonResponse({"status": "ok"}, status=200)
            else:
                return JsonResponse({"status": "error"}, status=500)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_update_firmware(request, nspanel_id):
    try:
        if request.method == 'POST':
            response = send_ipc_request(F"nspanel/{nspanel_id}/update_firmware", {})
            if response["status"] == "ok":
                return JsonResponse({"status": "ok"}, status=200)
            else:
                return JsonResponse({"status": "error"}, status=500)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_accept_register_request(request, nspanel_id):
    try:
        if request.method == 'POST':
            nspanel = NSPanel.objects.get(id=nspanel_id)
            nspanel.denied = False
            nspanel.accepted = True
            nspanel.save()

            response = send_ipc_request(F"nspanel/{nspanel_id}/accept_register_request", {})
            if response["status"] == "ok":
                response = HttpResponse("", status=200)
                response["HX-Redirect"] = "/"
                return response
            else:
                return JsonResponse({"status": "error"}, status=500)
            return HttpResponse("", status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_deny_register_request(request, nspanel_id):
    try:
        if request.method == 'POST':
            nspanel = NSPanel.objects.get(id=nspanel_id)
            nspanel.denied = True
            nspanel.save()
            response = send_ipc_request(F"nspanel/{nspanel_id}/deny_register_request", {})
            if response["status"] == "ok":
                response = HttpResponse("", status=200)
                response["HX-Redirect"] = "/"
                return response
            else:
                return HttpResponse("", status=500)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_delete(request, nspanel_id):
    try:
        if request.method == 'DELETE':
            nspanel = NSPanel.objects.get(id=nspanel_id)
            nspanel.delete()
            response = HttpResponse("", status=200)
            response["HX-Redirect"] = "/"
            return response
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def interface_theme(request):
    new_theme = request.POST.get('theme-dropdown')
    set_setting_value("theme", new_theme)
    return JsonResponse({"status": "OK"}, status=200)


def partial_select_new_entity_item_list(request, room_id):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entities": get_all_available_entities(request),
        "room": Room.objects.get(id=room_id)
    }
    return render(request, 'partial/select_entity/entity_list.html', data)

def partial_entity_add_light_to_room(request, room_id):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "room": Room.objects.get(id=room_id),
        "entity": json.loads(request.GET["entity"]),
        "control_mode": "",
        "can_color_temperature": "false",
        "can_color": "false",
        "openhab_channel_brightness": "",
        "openhab_channel_color_temperature": "",
        "openhab_channel_color": ""
    }

    # Check Home Assistant capabilites
    if data["entity"]["type"] == "home_assistant":
        if data["entity"]["raw_data"]["entity_id"].startswith("light."):
            data["control_mode"] = "dimmable"
        elif data["entity"]["raw_data"]["entity_id"].startswith("switch."):
            data["control_mode"] = "switch"

        if "supported_color_modes" in data["entity"]["raw_data"]["attributes"]:
            if "color_temp" in data["entity"]["raw_data"]["attributes"]["supported_color_modes"]:
                data["can_color_temperature"] = "true"
        if "rgb_color" in data["entity"]["raw_data"]["attributes"]:
            data["can_color"] = "true"

    # Check OpenHAB capabilites
    elif data["entity"]["type"] == "openhab":
        for item in data["entity"]["items"]:
            if "brightness" in item.lower() or "level" in item.lower():
                data["openhab_channel_brightness"] = item
            elif "temp" in item.lower():
                data["can_color_temperature"] = "true"
                data["openhab_channel_color_temperature"] = item
            elif "color" in item.lower() and "temp" not in item.lower():
                data["can_color"] = "true"
                data["openhab_channel_color"] = item

        if "dim" in data["entity"]["label"].lower() or data["can_color"] == "true" or data["can_color_temperature"] == "true":
            data["control_mode"] = "dimmable"
        elif "switch" in data["entity"]["label"].lower():
            data["control_mode"] = "switch"

    return render(request, 'partial/select_entity/entity_add_light_to_room.html', data)
