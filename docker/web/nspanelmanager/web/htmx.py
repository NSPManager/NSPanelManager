import base64
import hashlib
import json
import logging
import os
import signal
import subprocess
from time import sleep

import environ
import psutil
from django.core.files.storage import FileSystemStorage
from django.http import JsonResponse
from django.shortcuts import HttpResponse, redirect, render
from django.template import RequestContext
from django.urls import reverse
from django.views.decorators.csrf import csrf_exempt
from requests import delete
from requests.sessions import Request

import web.home_assistant_api
import web.openhab_api
from web.components.rooms_list.rooms_list import RoomsList
from web.settings_helper import (
    delete_nspanel_setting,
    get_nspanel_setting_with_default,
    get_setting_with_default,
    set_nspanel_setting_value,
    set_setting_value,
)
from web.views import get_base_data, get_file_md5sum, relay_groups

from .api import get_all_available_entities
from .apps import send_mqttmanager_reload_command, start_mqtt_manager
from .models import (
    Entity,
    Message,
    NSPanel,
    RelayGroup,
    RelayGroupBinding,
    Room,
    RoomEntitiesPage,
    Scene,
    Settings,
)


@csrf_exempt
def unblock_nspanel(request, nspanel_id):
    if request.method == "DELETE":
        nspanel = NSPanel.objects.get(id=nspanel_id)
        nspanel.delete()
        send_mqttmanager_reload_command()

        # Successful, refresh page
        response = HttpResponse("")
        response["HX-Refresh"] = "true"
        return response


@csrf_exempt
def nspanel_accept_register_request(request, nspanel_id):
    try:
        if request.method == "POST":
            nspanel = NSPanel.objects.get(id=nspanel_id)
            nspanel.denied = False
            nspanel.accepted = True
            nspanel.save()
            send_mqttmanager_reload_command()

            response = HttpResponse("", status=200)
            response["HX-Refresh"] = "true"
            return response
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_deny_register_request(request, nspanel_id):
    try:
        if request.method == "POST":
            nspanel = NSPanel.objects.get(id=nspanel_id)
            nspanel.accepted = False
            nspanel.denied = True
            nspanel.save()

            send_mqttmanager_reload_command()

            response = HttpResponse("", status=200)
            response["HX-Refresh"] = "true"
            return response
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def nspanel_delete(request, nspanel_id):
    try:
        if request.method == "DELETE":
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


def select_room_temperature_sensor_provider(request, room_id):
    if (get_setting_with_default("home_assistant_address") == "" or get_setting_with_default("home_assistant_token") == "") and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        # OpenHAB connection configured but not Home Assistant. Skip selecting source:
        return redirect(
            "htmx_partial_select_room_temperature_sensor_from_list",
            entity_source="openhab",
            room_id=room_id,
        )
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and (get_setting_with_default("openhab_address") == "" or get_setting_with_default("openhab_token") == ""):
        # OpenHAB connection not configured but Home Assistant is. Skip selecting source:
        return redirect(
            "htmx_partial_select_room_temperature_sensor_from_list",
            entity_source="home_assistant",
            room_id=room_id,
        )
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        return render(
            request,
            "partial/select_room_temperature_sensor_provider.html",
            {"room_id": room_id},
        )
    else:
        return JsonResponse(
            {
                "status": "error",
                "text": "Unknown sources configured. Check configuration for Home Assistant and/or OpenHAB in settings.",
            },
            status=500,
        )


def select_room_temperature_sensor_from_list(request, entity_source, room_id):
    data = {
        "entity_source": entity_source,
        "entities": [],
    }

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["sensor"]})
        if len(ha_items["errors"]) == 0:
            data["entities"] = ha_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from Home Assistant!"},
                status=500,
            )
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) == 0:
            data["entities"] = openhab_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from OpenHAB!"},
                status=500,
            )
    else:
        logging.error("Unknown entity source! Source: " + data["entity_source"])

    return render(request, "partial/select_room_temperature_sensor.html", data)


def select_room_temperature_sensor(request):
    return render(request, "partial/select_room_temperature_sensor.html")


def select_weather_location(request):
    return render(request, "partial/select_weather_location.html")


def select_weather_outside_temperature_sensor(request):
    return render(request, "partial/select_weather_outside_temperature_sensor.html")


def select_weather_outside_temperature_sensor_provider(request):
    if (get_setting_with_default("home_assistant_address") == "" or get_setting_with_default("home_assistant_token") == "") and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        # OpenHAB connection configured but not Home Assistant. Skip selecting source:
        return redirect(
            "htmx_partial_select_weather_outside_temperature_sensor_from_list",
            entity_source="openhab",
        )
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and (get_setting_with_default("openhab_address") == "" or get_setting_with_default("openhab_token") == ""):
        # OpenHAB connection not configured but Home Assistant is. Skip selecting source:
        return redirect(
            "htmx_partial_select_weather_outside_temperature_sensor_from_list",
            entity_source="home_assistant",
        )
    elif get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "" and get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        return render(request, "partial/select_weather_outside_temperature_sensor_provider.html")
    else:
        return JsonResponse(
            {
                "status": "error",
                "text": "Unknown sources configured. Check configuration for Home Assistant and/or OpenHAB in settings.",
            },
            status=500,
        )


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
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from Home Assistant!"},
                status=500,
            )
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) == 0:
            data["entities"] = openhab_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from OpenHAB!"},
                status=500,
            )
    else:
        logging.error("Unknown entity source! Source: " + data["entity_source"])

    return render(request, "partial/select_weather_outside_temperature_sensor.html", data)


@csrf_exempt
def interface_theme(request):
    new_theme = request.POST.get("theme-dropdown")
    set_setting_value("theme", new_theme)
    return JsonResponse({"status": "OK"}, status=200)


def relay_group_create_new_modal(request):
    return render(request, "modals/relay_groups/create_or_edit_relay_group_modal.html")


def relay_group_edit_modal(request, relay_group_id):
    data = {"relay_group": RelayGroup.objects.get(id=relay_group_id)}
    return render(request, "modals/relay_groups/create_or_edit_relay_group_modal.html", data)


def relay_group_save(request):
    if request.method == "POST":
        if "relay_group_id" in request.POST:
            rg = RelayGroup.objects.get(id=request.POST["relay_group_id"])
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
    return render(request, "modals/relay_groups/add_relay_modal.html", data)


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
        send_mqttmanager_reload_command()

        response = HttpResponse()
        response["HX-Refresh"] = "true"
        return response


def partial_select_new_entity_item_list(request, action, action_args):
    # This is used in the last step of adding an entity to call the correct
    # "add function" so that it get added to the correct room, page and so on.
    request.session["action"] = action
    request.session["action_args"] = action_args
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {"entities": get_all_available_entities(request)}
    return render(request, "partial/select_entity/entity_list.html", data)


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
        "controlled_by_nspanel_main_page": True,  # By default when adding a light. Make it controlled by the NSPanel main page.
        "openhab_items": [],
        "home_assistant_items": [],
    }

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["light", "switch"]})
        if len(ha_items["errors"]) == 0:
            data["home_assistant_items"] = ha_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from Home Assistant!"},
                status=500,
            )
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) == 0:
            data["openhab_items"] = openhab_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from OpenHAB!"},
                status=500,
            )
    else:
        logging.error("Unknown entity source! Source: " + data["entity_source"])

    return render(request, "partial/select_entity/entity_add_or_edit_light_to_room.html", data)


def partial_entity_edit_light_entity(request, light_id):
    light = Entity.objects.get(id=light_id)

    request.session["action"] = "ADD_LIGHT_TO_ROOM"
    request.session["action_args"] = json.dumps(
        {
            "entity_id": light_id,
            "room_id": light.room.id,
            "page_id": light.entities_page.id,
            "page_slot": light.room_view_position,
        }
    )

    entity_data = light.entity_data
    data = {
        "light": light,
        "entity_name": light.friendly_name,
        "entity_source": entity_data["controller"],
        "controlled_by_nspanel_main_page": entity_data.get("controlled_by_nspanel_main_page", True),
        "can_color_temperature": entity_data.get("can_color_temperature", False),
        "can_rgb": entity_data.get("can_rgb", False),
        "home_assistant_item": entity_data.get("home_assistant_name", ""),
        "openhab_brightness_item": "",  # Set below
        "openhab_color_temperature_item": entity_data.get("openhab_item_color_temp", ""),
        "openhab_rgb_item": entity_data.get("openhab_item_rgb", ""),
        "openhab_items": [],
        "home_assistant_items": [],
    }

    if entity_data.get("can_dim", False):
        data["control_mode"] = "dimmable"
        data["openhab_brightness_item"] = entity_data.get("openhab_item_dimmer", "")
    else:
        data["control_mode"] = "switch"
        data["openhab_brightness_item"] = entity_data.get("openhab_item_switch", "")

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["light", "switch"]})
        if len(ha_items["errors"]) == 0:
            data["home_assistant_items"] = ha_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from Home Assistant!"},
                status=500,
            )
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_items()
        if len(openhab_items["errors"]) > 0:
            return JsonResponse(
                {
                    "status": "error",
                    "text": "Failed to fetch OpenHAB items. Check logs for more information.",
                },
                status=500,
            )
        else:
            data["openhab_items"] = openhab_items["items"]

    return render(request, "partial/select_entity/entity_add_or_edit_light_to_room.html", data)


def partial_entity_edit_scene_entity(request, scene_id):
    scene = Scene.objects.get(id=scene_id)

    action_args = {
        "entity_id": scene.id,
        "page_id": scene.entities_page.id,
        "page_slot": scene.room_view_position,
    }
    # If scene is not a global scene (ie. room is set) then also add room_id
    if scene.room:
        action_args["room_id"] = scene.room.id

    request.session["action"] = "ADD_SCENE_TO_NSPANEL_ENTITY_PAGE"
    request.session["action_args"] = json.dumps(action_args)

    data = {
        "scene": scene,
        "edit_scene_id": scene.id,
        "entity": {
            "name": scene.friendly_name,
        },
    }
    return render(request, "partial/select_entity/entity_add_or_edit_scene.html", data)


def partial_entity_add_scene_entity(request):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entity_source": request.session["entity_source"],
        "openhab_item": "",
        "home_assistant_item": "",
        "openhab_items": [],
        "home_assistant_items": [],
    }

    if data["entity_source"] == "home_assistant":
        ha_items = web.home_assistant_api.get_all_home_assistant_items({"type": ["scene", "script"]})
        if len(ha_items["errors"]) == 0:
            data["home_assistant_items"] = ha_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from Home Assistant!"},
                status=500,
            )
    elif data["entity_source"] == "openhab":
        openhab_items = web.openhab_api.get_all_openhab_scenes()
        if len(openhab_items["errors"]) == 0:
            data["openhab_items"] = openhab_items["items"]
        else:
            return JsonResponse(
                {"status": "error", "text": "Failed to get items from OpenHAB!"},
                status=500,
            )
    else:
        logging.error("Unknown entity source! Source: " + data["entity_source"])

    return render(request, "partial/select_entity/entity_add_or_edit_scene.html", data)


def partial_edit_entities_page(request, page_id):
    data = {
        "page_id": page_id,
    }
    return render(request, "partial/edit_entities_page.html", data)


def get_entity_in_page_slot(page_id, slot_id):
    page = RoomEntitiesPage.objects.get(id=page_id)
    entities = Entity.objects.filter(entities_page=page, room_view_position=slot_id)
    if entities.count() > 0:
        return entities[0]

    entities = Scene.objects.filter(entities_page=page, room_view_position=slot_id)
    if entities.count() > 0:
        return entities[0]

    return None


@csrf_exempt
def partial_add_entity_to_entities_page_select_entity_type_react(request, entities_page_id, room_view_position):
    entities_page = RoomEntitiesPage.objects.get(id=entities_page_id)
    return render(
        request,
        "partial/react_add_edit_entity.html",
        {
            "room_id": entities_page.room.id,
            "entities_page_id": entities_page_id,
            "room_view_position": room_view_position,
        },
    )


@csrf_exempt
def partial_add_entity_to_entities_page_select_entity_type(request, action, action_args):
    data = {"action": action, "action_args": action_args}
    return render(request, "partial/add_entity_to_entities_page_select_entity_type.html", data)


@csrf_exempt
def partial_add_entity_to_entities_page_select_entity_source(request, action, action_args):
    request.session["action"] = action
    request.session["action_args"] = action_args

    is_home_assistant_configured = False
    is_openhab_configured = False
    if get_setting_with_default("home_assistant_address") != "" and get_setting_with_default("home_assistant_token") != "":
        is_home_assistant_configured = True
    if get_setting_with_default("openhab_address") != "" and get_setting_with_default("openhab_token") != "":
        is_openhab_configured = True

    data = {
        "action": action,
        "action_args": action_args,
        "is_home_assistant_configured": is_home_assistant_configured,
        "is_openhab_configured": is_openhab_configured,
        "home_assistant_supported_entity_types": [
            "ADD_SCENE_TO_NSPANEL_ENTITY_PAGE",
        ],
        "openhab_supported_entity_types": [
            "ADD_SCENE_TO_NSPANEL_ENTITY_PAGE",
        ],
        "manual_supported_entity_types": ["ADD_BUTTON_TO_ROOM"],
    }

    return render(request, "partial/add_entity_to_entities_page_select_entity_source.html", data)


@csrf_exempt
def partial_add_entity_to_entities_page_config_modal(request, entity_source):
    request.session["entity_source"] = entity_source
    if request.session["action"] == "ADD_SCENE_TO_NSPANEL_ENTITY_PAGE":
        return partial_entity_add_scene_entity(request)
    else:
        return JsonResponse(
            {
                "status": "error",
                "text": "Unknown action! Action: " + request.session["action"],
            },
            status=500,
        )


@csrf_exempt
def partial_reorder_rooms(request):
    if "htmx_form_save_rooms_order_field" in request.POST:
        json_data = json.loads(request.POST["htmx_form_save_rooms_order_field"])
        if "rooms" in json_data:
            if len(json_data["rooms"]) > 0:
                for key, room in enumerate(json_data["rooms"]):
                    room = Room.objects.get(id=room)
                    room.displayOrder = key
                    room.save()
                send_mqttmanager_reload_command()
                return JsonResponse({"status": "success"}, status=200)
            else:
                return JsonResponse(
                    {
                        "status": "error",
                        "text": "'pages' field empty in request POST-data.",
                    },
                    status=500,
                )
        else:
            return JsonResponse(
                {
                    "status": "error",
                    "text": "'rooms' field not available in JSON-data.",
                },
                status=500,
            )
    else:
        return JsonResponse(
            {
                "status": "error",
                "text": "'htmx_form_save_rooms_order_field' field not available in request POST-data.",
            },
            status=500,
        )


def partial_select_new_outside_temperature_sensor(request):
    # TODO: Move "get_all_available_entities" from api.py to seperate files
    data = {
        "entities": get_all_available_entities(request),
    }
    return render(
        request,
        "partial/select_entity/entity_list_select_outside_temperature_sensor.html",
        data,
    )


# When creating a new or updating an existing light entity this will take care of the actual creation/updating of the model
# in the database.
def show_messages(request):
    data = get_base_data(request)
    data["messages"] = Message.objects.all()
    return render(request, "partial/show_messages.html", data)


def mark_message_read(request, message_id):
    try:
        message = Message.objects.get(id=message_id)
        message.read = True
        message.save()
        return show_messages(request)
    except Exception as e:
        return JsonResponse({"status": "error", "text": str(e)}, status=500)
