from requests import delete
from django.shortcuts import render, redirect, HttpResponse
from django.core.files.storage import FileSystemStorage
from django.views.decorators.csrf import csrf_exempt
from django.http import JsonResponse
from django.urls import reverse
from django.template import RequestContext

from .api import get_all_available_entities
import web.openhab_api
import web.home_assistant_api
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
from .views import send_mqttmanager_reload_command
from .mqttmanager_ipc import send_ipc_request

from web.components.nspanel_room_entities_pages.nspanel_room_entities_pages import NSPanelRoomEntitiesPages

from .models import NSPanel, Room, Light, RoomEntitiesPage, Settings, Scene, RelayGroup, RelayGroupBinding, Switch
from .apps import start_mqtt_manager
from web.settings_helper import delete_nspanel_setting, get_setting_with_default, set_setting_value, get_nspanel_setting_with_default, set_nspanel_setting_value
from web.views import get_file_md5sum, relay_groups

def partial_index_nspanels_section(request):
    md5_firmware = get_file_md5sum("firmware.bin")
    md5_data_file = get_file_md5sum("data_file.bin")
    md5_tft_file = get_file_md5sum("gui.tft")
    md5_us_tft_file = get_file_md5sum("gui_us.tft")

    if get_setting_with_default("use_fahrenheit") == "True":
        temperature_unit = "°F"
    else:
        temperature_unit = "°C"

    nspanels = []
    for nspanel in NSPanel.objects.filter(denied=False):
        panel_info = {}
        panel_info["data"] = nspanel
        print(F"Checking status for panel {nspanel.id}")
        panel_info["status"] = send_ipc_request(F"nspanel/{nspanel.id}/status", {"command": "get"})
        print(F"Got status for panel {nspanel.id}")
        panel_info["status"]["warnings"] = [] # TODO: Check if already array, then don't clear existin warnings
        for panel in NSPanel.objects.filter(denied=False):
            if panel == nspanel:
                continue
            elif panel.friendly_name == nspanel.friendly_name:
                panel_info["status"]["warnings"].append({
                    "level": "error",
                    "text": "Two or more panels exists with the same name. This may have unintended consequences"
                })
                break
        if nspanel.md5_firmware != md5_firmware or nspanel.md5_data_file != md5_data_file:
            panel_info["status"]["warnings"].append({
                "level": "info",
                "text": "Firmware update available."
            })
        if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "False" and nspanel.md5_tft_file != md5_tft_file:
            panel_info["status"]["warnings"].append({
                "level": "info",
                "text": "GUI update available."
            })
        if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "True" and nspanel.md5_tft_file != md5_us_tft_file:
            panel_info["status"]["warnings"].append({
                "level": "info",
                "text": "GUI update available."
            })
        # TODO: Load warnings from MQTTManager.
        nspanels.append(panel_info)

    data = {
        'nspanels': nspanels,
        'temperature_unit': temperature_unit,
    }

    return render(request, 'index_htmx_nspanels_section.html', data)

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
def unblock_nspanel(request, nspanel_id):
    if request.method == "DELETE":
        nspanel = NSPanel.objects.get(id=nspanel_id)
        nspanel.delete()

        # Successful, refresh page
        response = HttpResponse("")
        response["HX-Refresh"] = "true"
        return response


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


def select_weather_location(request):
    return render(request, 'partial/select_weather_location.html')

def select_weather_outside_temperature_sensor(request):
    return render(request, 'partial/select_weather_outside_temperature_sensor.html')

def select_weather_outside_temperature_sensor_provider(request):
    if (get_setting_with_default("home_assistant_address") == "" or get_setting_with_default("home_assistant_token") == "") and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        # OpenHAB connection configured but not Home Assistant. Skip selecting source:
        return redirect('htmx_partial_select_weather_outside_temperature_sensor_from_list', entity_source="openhab")
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and (get_setting_with_default("openhab_address") == "" or get_setting_with_default("openhab_token") == ""):
        # OpenHAB connection configured but not Home Assistant. Skip selecting source:
        return redirect('htmx_partial_select_weather_outside_temperature_sensor_from_list', entity_source="home_assistant")
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        return render(request, 'partial/select_weather_outside_temperature_sensor_provider.html')
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown sources configured. Check configuration for Home Assistant and/or OpenHAB in settings."
        }, status=500)

def select_weather_outside_temperature_sensor_from_list(request, entity_source):
    data = {
        "entity_source": entity_source,
        "entities": [],
    }

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["sensor"]})
        if len(ha_items["errors"]) == 0:
            data["entities"] = ha_items["items"]
        else:
            return JsonResponse({
                "status": "error",
                "text": "Failed to get items from Home Assistant!"
            }, status=500)
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) == 0:
            data["entities"] = openhab_items["items"]
        else:
            return JsonResponse({
                "status": "error",
                "text": "Failed to get items from OpenHAB!"
            }, status=500)
    else:
        logging.error("Unknown entity source! Source: " + data["entity_source"])

    return render(request, "partial/select_weather_outside_temperature_sensor.html", data)

@csrf_exempt
def interface_theme(request):
    new_theme = request.POST.get('theme-dropdown')
    set_setting_value("theme", new_theme)
    return JsonResponse({"status": "OK"}, status=200)


def relay_group_create_new_modal(request):
    return render(request, 'modals/relay_groups/create_or_edit_relay_group_modal.html')


def relay_group_edit_modal(request, relay_group_id):
    data = {
        'relay_group': RelayGroup.objects.get(id=relay_group_id)
    }
    return render(request, 'modals/relay_groups/create_or_edit_relay_group_modal.html', data)

def relay_group_save(request):
    if request.method == "POST":
        if "relay_group_id" in request.POST:
            rg = RelayGroup.objects.get(id=request.POST['relay_group_id'])
        else:
            rg = RelayGroup()
        rg.friendly_name = request.POST["name"]
        rg.save()

        response = HttpResponse()
        response["HX-Refresh"] = "true"
        return response


@csrf_exempt
def relay_group_delete(request, relay_group_id):
    if request.method == "DELETE":
        rg = RelayGroup.objects.get(id=relay_group_id)
        rg.delete()

        response = HttpResponse()
        response["HX-Refresh"] = "true"
        return response


@csrf_exempt
def relay_group_add_relay_modal(request, relay_group_id):
    data = {
        "nspanels": NSPanel.objects.filter(accepted=True, denied=False),
        "relay_group_id": relay_group_id,
    }
    return render(request, 'modals/relay_groups/add_relay_modal.html', data)


def relay_group_add_relay(request, relay_group_id):
    if request.method == "POST":
        rg = RelayGroup.objects.get(id=relay_group_id)
        nspanel = NSPanel.objects.get(id=request.POST["nspanel_id"])
        relay_num = request.POST["relay_selection"]

        exists = RelayGroupBinding.objects.filter(nspanel=nspanel, relay_num=relay_num, relay_group=rg).count() > 0
        if not exists:
            binding = RelayGroupBinding()
            binding.relay_group = rg
            binding.nspanel = nspanel
            binding.relay_num = relay_num
            binding.save()
            send_mqttmanager_reload_command()

        response = HttpResponse()
        response["HX-Refresh"] = "true"
        return response


@csrf_exempt
def relay_group_remove_relay(request, relay_binding_id):
    if request.method == "DELETE":
        rgb = RelayGroupBinding.objects.get(id=relay_binding_id)
        rgb.delete()
        response = HttpResponse()
        response["HX-Refresh"] = "true"
        return response


@csrf_exempt
def handle_entity_modal_result(request):
    if request.session["action"] == "ADD_LIGHT_TO_ROOM":
        return create_or_update_light_entity(request)
    elif request.session["action"] == "ADD_SWITCH_TO_ROOM":
        return create_or_update_switch_entity(request)
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown action! Action: " + request.session["action"]
        }, status=500)


def handle_entity_modal_entity_selected(request, entity):
    entity_data = json.loads(base64.b64decode(entity).decode('utf-8'))

    if request.session["action"] == "ADD_LIGHT_TO_ROOM":
        return partial_entity_add_light_entity(request, json.dumps(entity_data))
    elif request.session["action"] == "ADD_SWITCH_TO_ROOM":
        return partial_entity_add_switch_entity(request, json.dumps(entity_data))
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

def partial_entity_add_light_entity(request):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entity_source": request.session["entity_source"],
        "control_mode": "",
        "can_color_temperature": False,
        "can_color": False,
        "openhab_item_brightness": "",
        "openhab_item_color_temperature": "",
        "openhab_item_color": "",
        "home_assistant_item": "",
        "controlled_by_nspanel_main_page": True, # By default when adding a light. Make it controlled by the NSPanel main page.
        "openhab_items": [],
        "home_assistant_items": [],
    }

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["light", "switch"]})
        if len(ha_items["errors"]) == 0:
            data["home_assistant_items"] = ha_items["items"]
        else:
            return JsonResponse({
                "status": "error",
                "text": "Failed to get items from Home Assistant!"
            }, status=500)
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) == 0:
            data["openhab_items"] = openhab_items["items"]
        else:
            return JsonResponse({
                "status": "error",
                "text": "Failed to get items from OpenHAB!"
            }, status=500)
    else:
        logging.error("Unknown entity source! Source: " + data["entity_source"])

    return render(request, 'partial/select_entity/entity_add_or_edit_light_to_room.html', data)

def partial_entity_edit_light_entity(request, light_id):
    light = Light.objects.get(id=light_id)

    request.session["action"] = "ADD_LIGHT_TO_ROOM"
    request.session["action_args"] = json.dumps({
        "entity_id": light_id,
        "room_id": light.room.id,
        "page_id": light.entities_page.id,
        "page_slot": light.room_view_position,
    })

    data = {
        "light": light,
        "entity_source": light.type,
        "entity_name": light.friendly_name,
        "controlled_by_nspanel_main_page": light.controlled_by_nspanel_main_page,
        "can_color_temperature": light.can_color_temperature,
        "can_rgb": light.can_rgb,
        "home_assistant_item": light.home_assistant_name,
        "openhab_brightness_item": "", # Set below
        "openhab_color_temperature_item": light.openhab_item_color_temp,
        "openhab_rgb_item": light.openhab_item_rgb,
        "openhab_items": [],
        "home_assistant_items": [],
    }

    if light.can_dim:
        data["control_mode"] = "dimmable"
        data["openhab_brightness_item"] = light.openhab_item_dimmer
    else:
        data["control_mode"] = "switch"
        data["openhab_brightness_item"] = light.openhab_item_switch

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["light", "switch"]})
        if len(ha_items["errors"]) == 0:
            data["home_assistant_items"] = ha_items["items"]
        else:
            return JsonResponse({
                "status": "error",
                "text": "Failed to get items from Home Assistant!"
            }, status=500)
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) > 0:
            return JsonResponse({
                "status": "error",
                "text": "Failed to fetch OpenHAB items. Check logs for more information."
            }, status=500)
        else:
            data["openhab_items"] = openhab_items["items"]

    return render(request, 'partial/select_entity/entity_add_or_edit_light_to_room.html', data)

def partial_entity_add_switch_entity(request, entity):
    data = {
        "entity": json.loads(entity),
    }
    return render(request, "partial/select_entity/entity_add_or_edit_switch_to_room.html", data)


def partial_entity_edit_switch_entity(request, switch_id):
    switch = Switch.objects.get(id=switch_id)

    request.session["action"] = "ADD_SWITCH_TO_ROOM"
    request.session["action_args"] = json.dumps({
        "entity_id": switch_id,
        "room_id": switch.room.id,
        "page_id": switch.entities_page.id,
        "page_slot": switch.room_view_position,
    })

    data = {
        "light": switch,
        "edit_light_id": switch_id,
        "entity": {
            "label": switch.friendly_name,
            "entity_id": switch.home_assistant_name,
        },
    }
    return render(request, "partial/select_entity/entity_add_or_edit_switch_to_room.html", data)


@csrf_exempt
def partial_remove_entity_from_page_slot(request, page_id, slot_id):
    page = RoomEntitiesPage.objects.get(id=page_id)

    # Check for light in given slot
    entities = page.light_set.filter(room_view_position=slot_id).all()
    if entities.count() > 0:
        entities.delete();
        send_mqttmanager_reload_command()

    entities = page.switch_set.filter(room_view_position=slot_id).all()
    if entities.count() > 0:
        entities.delete();
        send_mqttmanager_reload_command()

    entities_pages = NSPanelRoomEntitiesPages()
    return entities_pages.get(request=request, view="edit_room", room_id=page.room.id)

@csrf_exempt
def partial_add_entities_page_to_room(request, room_id, is_scenes_page):
    data = {
        "room_id": room_id,
        "is_scenes_page": is_scenes_page,
    }
    return render(request, 'partial/add_entities_page_to_room.html', data)

def get_entity_in_page_slot(page_id, slot_id):
    page = RoomEntitiesPage.objects.get(id=page_id)
    entities = Light.objects.filter(entities_page=page, room_view_position=slot_id)
    if entities.count() > 0:
        return entities[0]

    entities = Switch.objects.filter(entities_page=page, room_view_position=slot_id)
    if entities.count() > 0:
        return entities[0]

    return None

@csrf_exempt
def partial_move_entity(request):
    existing_entity_in_slot = get_entity_in_page_slot(request.POST["page_id"], request.POST["slot_id"])
    new_entity_in_slot = None
    if request.POST["new_entity_type"] == "Light":
        new_entity_in_slot = Light.objects.get(id=request.POST["new_entity_id"])
    elif request.POST["new_entity_type"] == "Switch":
        new_entity_in_slot = Switch.objects.get(id=request.POST["new_entity_id"])
    else:
        return JsonResponse({
            "status": "error",
            "text": "Did not find existing entity to move!"
        })

    move_from_page = None
    move_from_slot = None
    return_html = ""
    if existing_entity_in_slot:
        # Swap the existing entity place with the new entity to be put on that slot
        existing_entity_in_slot.entities_page = new_entity_in_slot.entities_page
        existing_entity_in_slot.room_view_position = new_entity_in_slot.room_view_position
        existing_entity_in_slot.save()

    new_entity_in_slot.entities_page = RoomEntitiesPage.objects.get(id=request.POST["page_id"])
    new_entity_in_slot.room_view_position = request.POST["slot_id"]
    new_entity_in_slot.save()
    send_mqttmanager_reload_command()

    entities_pages = NSPanelRoomEntitiesPages()
    return entities_pages.get(request, view='edit_room', room_id=new_entity_in_slot.room.id)


@csrf_exempt
def partial_move_entities_pages(request):
    print(request.POST)
    if "htmx_form_save_entities_pages_order_field" in request.POST:
        json_data = json.loads(request.POST["htmx_form_save_entities_pages_order_field"])
        if "pages" in json_data:
            if len(json_data["pages"]) > 0:
                room_id = RoomEntitiesPage.objects.get(id=json_data["pages"][0]).room.id
                for index, page_id in enumerate(json_data["pages"]):
                    page = RoomEntitiesPage.objects.get(id=page_id)
                    page.display_order = index
                    page.save()
                send_mqttmanager_reload_command()
                entities_pages = NSPanelRoomEntitiesPages()
                return entities_pages.get(request, view='edit_room', room_id=room_id)
            else:
                return JsonResponse({
                    "status": "error",
                    "text": "'pages' field empty in request POST-data."
                }, status=500)
        else:
            return JsonResponse({
                "status": "error",
                "text": "'pages' field not available in JSON-data."
            }, status=500)
    else:
        return JsonResponse({
            "status": "error",
            "text": "'htmx_form_save_entities_pages_order_field' field not available in request POST-data."
        }, status=500)



@csrf_exempt
def partial_add_entity_to_entities_page_select_entity_type(request, action, action_args):
    data = {
        "action": action,
        "action_args": action_args
    }
    return render(request, 'partial/add_entity_to_entities_page_select_entity_type.html', data)


@csrf_exempt
def partial_add_entity_to_entities_page_select_entity_source(request, action, action_args):
    data = {
        "action": action,
        "action_args": action_args
    }
    request.session["action"] = action
    request.session["action_args"] = action_args
    if (get_setting_with_default("home_assistant_address") == "" or get_setting_with_default("home_assistant_token") == "") and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        # OpenHAB connection configured but not Home Assistant. Skip selecting source:
        return redirect('htmx_partial_select_new_entity_config_modal', entity_source="openhab")
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and (get_setting_with_default("openhab_address") == "" or get_setting_with_default("openhab_token") == ""):
        # OpenHAB connection configured but not Home Assistant. Skip selecting source:
        return redirect('htmx_partial_select_new_entity_config_modal', entity_source="home_assistant")
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        return render(request, 'partial/add_entity_to_entities_page_select_entity_source.html', data)
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown sources configured. Check configuration for Home Assistant and/or OpenHAB in settings."
        }, status=500)


@csrf_exempt
def partial_add_entity_to_entities_page_config_modal(request, entity_source):
    request.session["entity_source"] = entity_source
    if request.session["action"] == "ADD_LIGHT_TO_ROOM":
        return partial_entity_add_light_entity(request)
    elif request.session["action"] == "ADD_SWITCH_TO_ROOM":
        return create_or_update_switch_entity(request)
    else:
        return JsonResponse({
            "status": "error",
            "text": "Unknown action! Action: " + request.session["action"]
        }, status=500)


@csrf_exempt
def partial_delete_entities_page(request, page_id):
    page = RoomEntitiesPage.objects.get(id=page_id)
    room_id = page.room.id
    page.delete()
    send_mqttmanager_reload_command()

    entities_pages = NSPanelRoomEntitiesPages()
    return entities_pages.get(request=request, view="edit_room", room_id=page.room.id)


def create_entities_page_in_room(request, room_id, page_type, is_scenes_page):
    room = Room.objects.get(id=room_id)
    entity_page = RoomEntitiesPage()
    entity_page.room = room
    entity_page.display_order = RoomEntitiesPage.objects.filter(room=room).count() + 1
    entity_page.is_scenes_page = is_scenes_page == "True"
    if page_type == 4:
        entity_page.page_type = 4
        entity_page.save()
        send_mqttmanager_reload_command()
    elif page_type == 8:
        entity_page.page_type = 8
        entity_page.save()
        send_mqttmanager_reload_command()
    elif page_type == 12:
        entity_page.page_type = 12
        entity_page.save()
        send_mqttmanager_reload_command()
    else:
        print(F"ERROR! Unknown page type {page_type}")
    # Return new partial HTMX update of all entities pages in this room
    entities_pages = NSPanelRoomEntitiesPages()
    return entities_pages.get(request=request, view="edit_room", room_id=room_id)


def partial_select_new_outside_temperature_sensor(request):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entities": get_all_available_entities(request),
    }
    return render(request, 'partial/select_entity/entity_list_select_outside_temperature_sensor.html', data)


# When creating a new or updating an existing light entity this will take care of the actual creation/updating of the model
# in the database.
def create_or_update_light_entity(request):
    action_args = json.loads(request.session["action_args"]) # Loads arguments set when first starting process of adding/updating entity
    entity_source = request.session["entity_source"]

    if "entity_id" in action_args and int(action_args["entity_id"]) >= 0:
        newLight = Light.objects.get(id=int(action_args["entity_id"]))
    else:
        newLight = Light()
        # Only set once, when first created:
        newLight.type = request.session["entity_source"]
        if newLight.type == "home_assistant":
            newLight.home_assistant_name = request.POST["home_assistant_item"]

    newLight.room = Room.objects.get(id=int(action_args["room_id"]))
    newLight.entities_page = RoomEntitiesPage.objects.get(id=int(action_args["page_id"]))
    newLight.room_view_position = int(action_args["page_slot"])

    newLight.controlled_by_nspanel_main_page = "controlled_by_nspanel_main_page" in request.POST
    newLight.is_ceiling_light = request.POST["light_type"] == "ceiling"
    newLight.friendly_name = request.POST["add_new_light_name"]


    if request.POST["light_control_mode"] == "dimmer":
        newLight.can_dim = True
        newLight.openhab_control_mode = "dimmer"
        if newLight.type == "openhab":
            newLight.openhab_item_dimmer = request.POST["openhab_dimming_item"]
    else:
        newLight.openhab_control_mode = "switch"
        newLight.can_dim = False
        if newLight.type == "openhab":
            newLight.openhab_item_switch = request.POST["openhab_dimming_item"]

    if "color_temperature" in request.POST:
        newLight.can_color_temperature = True
        if newLight.type == "openhab":
            newLight.openhab_item_color_temp = request.POST["openhab_color_temperature_item"]
    else:
        newLight.can_color_temperature = False
        newLight.openhab_item_color_temp = ""

    if "rgb" in request.POST:
        newLight.can_rgb = True
        if newLight.type == "openhab":
            newLight.openhab_item_rgb = request.POST["openhab_rgb_item"]
    else:
        newLight.can_rgb = False
        newLight.openhab_item_rgb = ""

    newLight.save()
    send_mqttmanager_reload_command()

    entities_pages = NSPanelRoomEntitiesPages()
    return entities_pages.get(request=request, view="edit_room", room_id=newLight.room.id)


def create_or_update_switch_entity(request):
    action_args = json.loads(request.session["action_args"]) # Loads arguments set when first starting process of adding/updating entity

    if "entity_id" in action_args and int(action_args["entity_id"]) >= 0:
        new_switch = Switch.objects.get(id=int(action_args["entity_id"]))
    else:
        new_switch = Switch()
        # Only set once, during initial creation:
        new_switch.room = Room.objects.get(id=int(action_args["room_id"]))
        new_switch.entities_page = RoomEntitiesPage.objects.get(id=int(action_args["page_id"]))
        new_switch.room_view_position = int(action_args["page_slot"])
        new_switch.type = request.POST["entity_type"]
        if new_switch.type == "home_assistant":
            new_switch.home_assistant_name = request.POST["entity_id"]
        elif new_switch.type == "openhab":
            new_switch.openhab_name = request.POST["entity_id"]
            new_switch.openhab_item_switch = request.POST["openhab_switch_channel_name"]

    new_switch.friendly_name = request.POST["add_new_light_name"]
    new_switch.save()
    send_mqttmanager_reload_command()

    entities_pages = NSPanelRoomEntitiesPages()
    return entities_pages.get(request=request, view="edit_room", room_id=new_switch.room.id)
    #return redirect('edit_room', room_id=action_args["room_id"])


def initial_setup_welcome(request):
    return render(request, 'modals/initial_setup/welcome.html')


@csrf_exempt
def initial_setup_manager_settings(request):
    if request.method == "POST":
        if "manager_address" in request.POST:
            set_setting_value("manager_address", request.POST["manager_address"])
        if "manager_port" in request.POST:
            set_setting_value("manager_port", request.POST["manager_port"])
        send_mqttmanager_reload_command()

        # Save settings succesfully, return the next view in the setup guide. MQTT:
        data = {
            "mqtt_server": get_setting_with_default("mqtt_server"),
            "mqtt_port": get_setting_with_default("mqtt_port"),
            "mqtt_username": get_setting_with_default("mqtt_username"),
            "mqtt_password": get_setting_with_default("mqtt_password"),
        }
        return render(request, 'modals/initial_setup/mqtt.html', data)
    elif request.method == "GET":
        data = {
            "manager_address": get_setting_with_default("manager_address"),
            "manager_port": get_setting_with_default("manager_port"),
        }
        return render(request, 'modals/initial_setup/manager_settings.html', data)


@csrf_exempt
def initial_setup_mqtt_settings(request):
    if request.method == "POST":
        if "mqtt_server" in request.POST:
            set_setting_value("mqtt_server", request.POST["mqtt_server"])
        if "mqtt_port" in request.POST:
            set_setting_value("mqtt_port", request.POST["mqtt_port"])
        if "mqtt_username" in request.POST:
            set_setting_value("mqtt_username", request.POST["mqtt_username"])
        if "mqtt_password" in request.POST:
            set_setting_value("mqtt_password", request.POST["mqtt_password"])
        send_mqttmanager_reload_command()

        # Save settings succesfully, return the next view in the setup guide. Home Assistant:
        data = {
            "home_assistant_address": get_setting_with_default("home_assistant_address"),
            "home_assistant_token": get_setting_with_default("home_assistant_token"),
        }
        return render(request, 'modals/initial_setup/home_assistant.html', data)
    elif request.method == "GET":
        data = {
            "mqtt_server": get_setting_with_default("mqtt_server"),
            "mqtt_port": get_setting_with_default("mqtt_port"),
            "mqtt_username": get_setting_with_default("mqtt_username"),
            "mqtt_password": get_setting_with_default("mqtt_password"),
        }
        return render(request, 'modals/initial_setup/mqtt.html', data)


@csrf_exempt
def initial_setup_home_assistant_settings(request):
    if request.method == "POST":
        if "home_assistant_address" in request.POST:
            set_setting_value("home_assistant_address", request.POST["home_assistant_address"])
        if "home_assistant_token" in request.POST:
            set_setting_value("home_assistant_token", request.POST["home_assistant_token"])
        send_mqttmanager_reload_command()

        # Save settings succesfully, return the next view in the setup guide. OpenHAB:
        data = {
            "openhab_address": get_setting_with_default("openhab_address"),
            "openhab_token": get_setting_with_default("openhab_token"),
        }
        return render(request, 'modals/initial_setup/openhab.html', data)
    elif request.method == "GET":
        data = {
            "home_assistant_address": get_setting_with_default("home_assistant_address"),
            "home_assistant_token": get_setting_with_default("home_assistant_token"),
        }
        return render(request, 'modals/initial_setup/home_assistant.html', data)


@csrf_exempt
def initial_setup_openhab_settings(request):
    if request.method == "POST":
        if "openhab_address" in request.POST:
            set_setting_value("openhab_address", request.POST["openhab_address"])
        if "openhab_token" in request.POST:
            set_setting_value("openhab_token", request.POST["openhab_token"])
        send_mqttmanager_reload_command()

        # Save settings succesfully, return the next view in the setup guide. Finished:
        return render(request, 'modals/initial_setup/finished.html')
    elif request.method == "GET":
        data = {
            "openhab_address": get_setting_with_default("openhab_address"),
            "openhab_token": get_setting_with_default("openhab_token"),
        }
        return render(request, 'modals/initial_setup/openhab.html', data)


def initial_setup_finished(request):
    return render(request, 'modals/initial_setup/finished.html')
