from requests import delete
from django.shortcuts import render, redirect, HttpResponse
from django.core.files.storage import FileSystemStorage
from django.views.decorators.csrf import csrf_exempt
from django.http import JsonResponse
from django.urls import reverse

from .components.nspanel_status_header.nspanel_status_header import NSPanelHeader

from .api import get_all_available_entities
import hashlib
import psutil
import subprocess
import logging
import os
import signal
import json
import base64
from time import sleep

#from nspanelmanager.web.mqttmanager_ipc import send_ipc_request
from .mqttmanager_ipc import send_ipc_request

from .models import NSPanel, Room, Light, RoomEntitiesPage, Settings, Scene, RelayGroup, RelayGroupBinding
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

@csrf_exempt
def handle_entity_modal_result(request):
    if request.session["action"] == "ADD_LIGHT_TO_ROOM" or request.session["action"] == "ADD_ENTITY_TO_NSPANEL_ENTITY_PAGE":
        return create_or_update_light_entity(request)
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown action! Action: " + request.session["action"]
        }, status=500)

def handle_entity_modal_entity_selected(request, entity):
    entity_data = json.loads(base64.b64decode(entity).decode('utf-8'))

    if entity_data["entity_type"] == "light":
        return partial_entity_add_light_entity(request, json.dumps(entity_data))
    elif entity_data["entity_type"] == "switch":
        if request.session["action"] == "ADD_LIGHT_TO_ROOM":
            return partial_entity_add_light_entity(request, json.dumps(entity_data))
        elif request.session["action"] == "ADD_ENTITY_TO_NSPANEL_ENTITY_PAGE":
            return partial_entity_add_switch_entity(request, json.dumps(entity_data))
        else:
            return JsonResponse({
                "status": "error",
                "text": "Unknown 'action' for light! Action: " + request.session["action"]
            }, status=500)
    else:
        return JsonResponse({"status": "error", "text": "Unknown entity type! Type: " + entity_data["entity_type"]}, status=500)

def partial_select_new_entity_item_list(request, action, action_args):
    # This is used in the last step of adding an entity to call the correct
    # "add function" so that it get added to the correct room, page and so on.
    request.session["action"] = action
    request.session["action_args"] = action_args

    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entities": get_all_available_entities(request)
    }
    return render(request, 'partial/select_entity/entity_list.html', data)

def partial_entity_add_light_entity(request, entity):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entity": json.loads(entity),
        "control_mode": "",
        "can_color_temperature": "false",
        "can_color": "false",
        "openhab_channel_brightness": "",
        "openhab_channel_color_temperature": "",
        "openhab_channel_color": "",
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

    if request.session["action"] == "ADD_LIGHT_TO_ROOM":
        return render(request, 'partial/select_entity/entity_add_light_to_room.html', data)
    elif request.session["action"] == "ADD_ENTITY_TO_NSPANEL_ENTITY_PAGE":
        return render(request, 'partial/select_entity/entity_add_light_to_entity_page.html', data)
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown 'action' for light! Action: " + request.session["action"]
        }, status=500)


def partial_entity_add_switch_entity(request, entity):
    data = {
        "entity": json.loads(entity),
    }
    return render(request, "partial/select_entity/entity_add_switch_to_entity_page.html", data)

@csrf_exempt
def partial_remove_entity_from_page_slot(request, page_id, slot_id):
    page = RoomEntitiesPage.objects.get(id=page_id)

    # Check for light in given slot
    lights = page.light_set.filter(room_view_position=slot_id).all()
    if lights.count() > 0:
        lights.delete();

    response = HttpResponse("", status=200)
    response["HX-Refresh"] = "true" # Tell client to refresh whole page
    return response

@csrf_exempt
def partial_add_entities_page_to_room(request, room_id):
    data = {
        "room_id": room_id
    }
    return render(request, 'partial/add_entities_page_to_room.html', data)


@csrf_exempt
def partial_delete_entities_page(request, page_id):
    RoomEntitiesPage.objects.get(id=page_id).delete()
    response = HttpResponse("", status=200)
    response["HX-Refresh"] = "true" # Tell client to refresh whole page
    return response

def create_entities_page_in_room(self, room_id, page_type):
    room = Room.objects.get(id=room_id)
    entity_page = RoomEntitiesPage()
    entity_page.room = room
    entity_page.display_order = RoomEntitiesPage.objects.filter(room=room).count() + 1
    if page_type == 4:
        entity_page.page_type = 4
        entity_page.save()
    elif page_type == 8:
        entity_page.page_type = 8
        entity_page.save()
    elif page_type == 12:
        entity_page.page_type = 12
        entity_page.save()
    else:
        print(F"ERROR! Unknown page type {page_type}")
    # TODO: Return HTMX data to append to entities view instead of reloading page.
    return redirect("edit_room", room_id=room_id)

def partial_select_new_outside_temperature_sensor(request):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entities": get_all_available_entities(request),
    }
    return render(request, 'partial/select_entity/entity_list_select_outside_temperature_sensor.html', data)


# When creating a new or updating an existing light entity this will take care of the actual creation/updating of the model
# in the database.
def create_or_update_light_entity(request):
    room = None
    entity_page = None
    action_args = json.loads(request.session["action_args"]) # Loads arguments set when first starting process of adding/updating entity

    if "light_id" in request.session and int(request.session["light_id"]) >= 0:
        newLight = Light.objects.get(id=int(request.session["light_id"]))
    else:
        newLight = Light()

    if request.session["action"] == "ADD_LIGHT_TO_ROOM":
        room = Room.objects.get(id=int(action_args["room_id"]))
        newLight.room = room

        # This is only applicable when adding light to main page. Not as a stand alone entity
        if request.POST["light_type"] == "ceiling":
            newLight.is_ceiling_light = True
        else:
            newLight.is_ceiling_light = False
    elif request.session["action"] == "ADD_ENTITY_TO_NSPANEL_ENTITY_PAGE":
        entity_page = RoomEntitiesPage.objects.get(id=int(action_args["page_id"]))
        newLight.entities_page = entity_page
        newLight.room_view_position = int(action_args["page_slot"])
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown action mode! Action: " + request.session["action"]
        }, status=500)


    newLight.type = request.POST["add_new_light_type"]
    newLight.friendly_name = request.POST["add_new_light_name"]

    if newLight.type == "home_assistant":
        newLight.home_assistant_name = request.POST["entity_id"]
    elif newLight.type == "openhab":
        newLight.openhab_name = request.POST["entity_id"]

    if request.POST["light_control_mode"] == "dimmer":
        newLight.can_dim = True
        newLight.openhab_control_mode = "dimmer"
        if newLight.type == "openhab":
            newLight.openhab_item_dimmer = request.POST["openhab_dimming_channel_name"]
    else:
        newLight.openhab_control_mode = "switch"
        newLight.can_dim = False
        if newLight.type == "openhab":
            newLight.openhab_item_switch = request.POST["openhab_switch_channel_name"]

    if "color_temperature" in request.POST:
        newLight.can_color_temperature = True
        if newLight.type == "openhab":
            newLight.openhab_item_color_temp = request.POST["openhab_color_temperature_channel_name"]
    else:
        newLight.can_color_temperature = False
        newLight.openhab_item_color_temp = ""

    if "rgb" in request.POST:
        newLight.can_rgb = True
        if newLight.type == "openhab":
            newLight.openhab_item_rgb = request.POST["openhab_RGB_channel_name"]
    else:
        newLight.can_rgb = False
        newLight.openhab_item_rgb = ""

    newLight.save()
    #send_mqttmanager_reload_command()
    command_data = {
        # TODO: Base64 Encode data.
        "data": newLight.get_protobuf_object().SerializeToString()
    }
    send_ipc_request("entity_manager/add_light", command_data)
    return redirect('edit_room', room_id=action_args["room_id"])
