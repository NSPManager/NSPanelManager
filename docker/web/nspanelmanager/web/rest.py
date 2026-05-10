import hashlib
import json
import logging
import socket
from pprint import pprint
from re import A

from django.core.files.storage import FileSystemStorage
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt

import web.home_assistant_api
import web.openhab_api
from web.settings_helper import (
    get_nspanel_setting_with_default,
    get_setting_with_default,
    set_setting_value,
)

from .apps import send_mqttmanager_reload_command
from .models import Entity, LightState, NSPanel, RelayGroup, Room, RoomEntitiesPage, Scene

########################
# Get entities section #
########################


def get_home_assistant_entities(request):
    if request.method != "GET":
        return JsonResponse({"status": "error"}, status=405)

    filter_params = json.loads(request.GET.get("filter", "{}"))
    return JsonResponse(web.home_assistant_api.get_all_home_assistant_items(filter_params))


##########################
## MQTTManager section ###
##########################

# Keys that are not allowed to be pulled though the REST API beacuse of security concerns.
banned_setting_keys = [
    "MQTT_SERVER",
    "MQTT_PORT",
    "MQTT_USERNAME",
    "MQTT_PASSWORD",
    "HOME_ASSISTANT_ADDRESS",
    "HOME_ASSISTANT_TOKEN",
    "OPENHAB_ADDRESS",
    "OPENHAB_TOKEN",
    "OPENHAB_TOKEN",
]


def mqttmanager_get_setting(request, setting_key):
    if setting_key in banned_setting_keys:
        return JsonResponse({"status": "error"}, status=403)  # Return error forbidden

    try:
        settings = {}
        if request.method == "GET":
            settings[setting_key] = get_setting_with_default(setting_key)
        else:
            return JsonResponse({"status": "error"}, status=405)

        return JsonResponse(
            {
                "status": "ok",
                "settings": settings,
            }
        )
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def mqttmanager_settings_post(request):
    try:
        settings = {}
        if request.method == "POST":
            data = json.loads(request.body)
            for setting_key in data["settings"]:
                if setting_key in banned_setting_keys:
                    return JsonResponse({"status": "error"}, status=403)  # Return error forbidden
                settings[setting_key] = get_setting_with_default(setting_key)
        else:
            return JsonResponse({"status": "error"}, status=405)

        return JsonResponse(
            {
                "status": "ok",
                "settings": settings,
            }
        )
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error"}, status=500)


######################
## NSPanel section ###
######################


# Get the MD5 checksum of a file
# Return none if file not found
def get_file_md5sum(filename):
    fs = FileSystemStorage()
    if fs.exists(filename):
        return hashlib.md5(fs.open(filename).read()).hexdigest()
    else:
        return None


@csrf_exempt
def nspanels(request):
    if request.method == "GET":
        return nspanels_get(request)
    elif request.method == "POST":
        return nspanel_post(request)
    else:
        return JsonResponse({"status": "error"}, status=405)


def nspanels_get(request):
    if request.method == "GET":
        nspanels = list()
        if request.GET.get("id"):
            nspanel_objects = NSPanel.objects.filter(id=request.GET.get("id"))
        elif request.GET.get("mac_address"):
            nspanel_objects = NSPanel.objects.filter(mac_address=request.GET.get("mac_address"))
        else:
            nspanel_objects = NSPanel.objects.all()

        for nspanel in nspanel_objects:
            nspanels.append(
                {
                    "nspanel_id": nspanel.id,
                    "mac_address": nspanel.mac_address,
                    "name": nspanel.friendly_name,
                    "home": nspanel.room.id,
                    "default_page": get_nspanel_setting_with_default(nspanel.id, "default_page", 0),
                    "raise_to_100_light_level": get_setting_with_default("raise_to_100_light_level"),
                    "color_temp_min": get_setting_with_default("color_temp_min"),
                    "color_temp_max": get_setting_with_default("color_temp_max"),
                    "reverse_color_temp": get_setting_with_default("reverse_color_temp"),
                    "min_button_push_time": get_setting_with_default("min_button_push_time"),
                    "button_long_press_time": get_setting_with_default("button_long_press_time"),
                    "special_mode_trigger_time": get_setting_with_default("special_mode_trigger_time"),
                    "special_mode_release_time": get_setting_with_default("special_mode_release_time"),
                    "screen_dim_level": get_nspanel_setting_with_default(
                        nspanel.id,
                        "screen_dim_level",
                        get_setting_with_default("screen_dim_level"),
                    ),
                    "screensaver_dim_level": get_nspanel_setting_with_default(
                        nspanel.id,
                        "screensaver_dim_level",
                        get_setting_with_default("screensaver_dim_level"),
                    ),
                    "screensaver_activation_timeout": get_nspanel_setting_with_default(
                        nspanel.id,
                        "screensaver_activation_timeout",
                        get_setting_with_default("screensaver_activation_timeout"),
                    ),
                    "screensaver_mode": get_nspanel_setting_with_default(
                        nspanel.id,
                        "screensaver_mode",
                        get_setting_with_default("screensaver_mode"),
                    ),
                    "clock_us_style": get_setting_with_default("clock_us_style"),
                    "use_fahrenheit": get_setting_with_default("use_fahrenheit"),
                    "is_us_panel": get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False"),
                    "lock_to_default_room": get_nspanel_setting_with_default(nspanel.id, "lock_to_default_room", "False"),
                    "reverse_relays": get_nspanel_setting_with_default(nspanel.id, "reverse_relays", False),
                    "relay1_default_mode": get_nspanel_setting_with_default(nspanel.id, "relay1_default_mode", "False"),
                    "relay2_default_mode": get_nspanel_setting_with_default(nspanel.id, "relay2_default_mode", "False"),
                    "temperature_calibration": get_nspanel_setting_with_default(nspanel.id, "temperature_calibration", 0),
                    "button1_mode": nspanel.button1_mode,
                    "button2_mode": nspanel.button2_mode,
                    "button1_mqtt_topic": get_nspanel_setting_with_default(nspanel.id, "button1_mqtt_topic", ""),
                    "button2_mqtt_topic": get_nspanel_setting_with_default(nspanel.id, "button2_mqtt_topic", ""),
                    "button1_mqtt_payload": get_nspanel_setting_with_default(nspanel.id, "button1_mqtt_payload", ""),
                    "button2_mqtt_payload": get_nspanel_setting_with_default(nspanel.id, "button2_mqtt_payload", ""),
                    "button1_detached_light": nspanel.button1_detached_mode_light.id if nspanel.button1_detached_mode_light else -1,
                    "button2_detached_light": nspanel.button2_detached_mode_light.id if nspanel.button2_detached_mode_light else -1,
                    "denied": nspanel.denied,
                    "accepted": nspanel.accepted,
                    "rooms": [room.id for room in Room.objects.all().order_by("displayOrder")],
                    "scenes": [scene.id for scene in Scene.objects.all()],
                }
            )
        return JsonResponse({"status": "ok", "nspanels": nspanels})


# Handle POST request to "nspanel" endpoint.
@csrf_exempt
def nspanel_post(request):
    try:
        data = json.loads(request.body)

        if "mac" in data:
            data["mac_address"] = data["mac"]
        elif "mac_origin" in data:
            data["mac_address"] = data["mac_origin"]
        new_panel = NSPanel.objects.filter(mac_address=data["mac_address"]).first()

        # new_panel is none ie. we didn't find a known panel with that MAC. Create a new one.
        if not new_panel:
            new_panel = NSPanel()
            new_panel.friendly_name = data["friendly_name"]

        new_panel.mac_address = data["mac_address"]
        new_panel.version = data["version"] if "version" in data else ""
        new_panel.ip_address = ""  # TODO: Remove ip_address from DB

        if "md5_firmware" in data:
            new_panel.md5_firmware = data["md5_firmware"]

        if "md5_data_file" in data:
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
        json_response = {
            "status": "ok",
            "nspanel_id": new_panel.id,
            "denied": new_panel.denied,
            "accepted": new_panel.accepted,
        }
        return JsonResponse(json_response, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


##################################
## NSPanel Relay Group section ###
##################################
def relay_groups(request):
    try:
        if request.method == "GET":
            relay_groups = []
            for relay_group in RelayGroup.objects.all():
                rg_info = {
                    "relay_group_id": relay_group.id,
                    "name": relay_group.friendly_name,
                    "relays": [],
                }
                for relay_binding in relay_group.relaygroupbinding_set.all():
                    rg_info["relays"].append(
                        {
                            "nspanel_id": relay_binding.nspanel.id,
                            "relay_num": relay_binding.relay_num,
                        }
                    )
                relay_groups.append(rg_info)
            return JsonResponse({"status": "ok", "relay_groups": relay_groups}, status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


####################
### Room section ###
####################


@csrf_exempt
def rooms(request):
    if request.method == "GET":
        return rooms_get(request)
    elif request.method == "POST":
        return room_create(request)
    else:
        return JsonResponse({"status": "error"}, status=405)


def rooms_get(request):
    try:
        rooms = list()
        if request.GET.get("id"):
            room_objects = Room.objects.filter(id=request.GET.get("id"))
        else:
            room_objects = Room.objects.all()
        for room in room_objects:
            rooms.append(
                {
                    "room_id": room.id,
                    "name": room.friendly_name,
                    "lights": [light.id for light in room.light_set.all()],
                    "scenes": [scene.id for scene in room.scene_set.all()],
                }
            )
        return JsonResponse({"status": "ok", "rooms": rooms}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


def put_room_entities_order(request, room_id):
    if request.method == "PUT":
        try:
            data = json.loads(request.body)
            for entity in data["entities"]:
                db_entity = Entity.objects.get(id=entity["base"]["id"])
                db_entity.room_view_position = entity["base"]["room_view_position"]
                db_entity.entities_page_id = entity["base"]["entities_page_id"]
                db_entity.save()
            for scene in data["scenes"]:
                db_scene = Scene.objects.get(id=scene["base"]["id"])
                db_scene.room_view_position = scene["base"]["room_view_position"]
                db_scene.entities_page_id = scene["base"]["entities_page_id"]
                db_scene.save()
            send_mqttmanager_reload_command()
            return JsonResponse({"status": "ok"}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


def room_entities_pages(request, room_id):
    if request.method == "GET":
        try:
            room = Room.objects.get(id=room_id)
            pages = RoomEntitiesPage.objects.filter(room=room).order_by("display_order")
            response = []
            for page in pages:
                response.append(
                    {
                        "id": page.id,
                        "display_order": page.display_order,
                        "number_of_entities": page.page_type,
                        "type": "scene" if page.is_scenes_page else "entity",
                        "room_id": room_id,
                    }
                )
            return JsonResponse({"status": "ok", "entities_pages": response}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


def room_entities_page(request, room_id, page_id):
    if request.method == "GET":
        try:
            page = RoomEntitiesPage.objects.get(id=page_id)
            response = {
                "status": "ok",
                "entities": [],
                "scenes": [],
            }
            for entity in page.entity_set.all().order_by("room_view_position"):
                response["entities"].append(get_rest_entitiy_representation(entity.id))
            for entity in page.scene_set.all().order_by("room_view_position"):
                response["scenes"].append(get_rest_scene_representation(entity.id))
            return JsonResponse(response, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


@csrf_exempt
def room_delete(request, room_id):
    if request.method == "DELETE":
        try:
            room = Room.objects.get(id=room_id)
            room.delete()
            return JsonResponse({"status": "ok", "room_id": room_id}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


@csrf_exempt
def room_create(request):
    if request.method == "POST":
        try:
            data = json.loads(request.body)
            new_room = Room()
            new_room.friendly_name = data["name"]
            new_room.save()
            return JsonResponse({"status": "ok", "room_id": new_room.id}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


#####################
### Scene section ###
#####################
def get_rest_scene_representation(scene_id):
    scene = Scene.objects.get(id=scene_id)
    scene_info = {
        "base": {
            "id": scene.id,
            "friendly_name": scene.friendly_name,
            "type": "scene",
            "room_id": scene.room.id if scene.room != None else None,
            "entities_page_id": scene.entities_page.id if scene.entities_page != None else None,
            "room_view_position": scene.room_view_position,
            "controller": scene.scene_type,
        },
        "scene": {
            "scene_type": scene.scene_type,
            "backend_name": scene.backend_name,  # Name for OpenHAB or Home Assistant entity to activate
            "light_states": [],
        },
    }
    for state in scene.lightstate_set.all():
        scene_info["scene"]["light_states"].append(
            {
                "light_id": state.light.id,
                "light_type": state.light.type,
                "color_mode": state.color_mode,
                "light_level": state.light_level,
                "color_temp": state.color_temperature,
                "hue": state.hue,
                "saturation": state.saturation,
            }
        )
    return scene_info


def scenes(request):
    if request.method == "GET":
        return scenes_get(request)
    elif request.method == "POST":
        return scenes_post(request)
    else:
        return JsonResponse({"status": "error"}, status=405)


def scenes_get(request):
    try:
        scenes = []
        if request.GET.get("light_id"):
            scenes_objects = Scene.objects.filter(id=request.GET.get("scene_id"))
        elif request.GET.get("room_id"):
            scenes_objects = Scene.objects.filter(room_id=request.GET.get("room_id"))
        else:
            scenes_objects = Scene.objects.all()

        if scenes_objects.count() > 0:
            for scene in scenes_objects:
                scene_info = {
                    "scene_id": scene.id,
                    "scene_type": scene.scene_type,
                    "entity_name": scene.backend_name,  # Name for OpenHAB or Home Assistant entity to activate
                    "scene_name": scene.friendly_name,
                    "room_id": scene.room.id if scene.room != None else None,
                    "light_states": [],
                }
                for state in scene.lightstate_set.all():
                    scene_info["light_states"].append(
                        {
                            "light_id": state.light.id,
                            "light_type": state.light.type,
                            "color_mode": state.color_mode,
                            "light_level": state.light_level,
                            "color_temp": state.color_temperature,
                            "hue": state.hue,
                            "saturation": state.saturation,
                        }
                    )
                scenes.append(scene_info)
            return JsonResponse({"status": "ok", "scenes": scenes}, status=200)
        else:
            return JsonResponse({"status": "error", "scenes": scenes}, status=404)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


# TODO: This method currently handles both "create new" and "update old".
# This functionality should be split into two calls as in best practive with REST API.
def scenes_post(request):
    try:
        data = json.loads(request.body)
        scene = Scene.objects.filter(id=data["scene_id"]).first()
        if scene:
            scene.lightstate_set.all().delete()  # Remove all old states
            # TODO: Convert light states to JSON fields in DB.
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
                    logging.warning("ERROR: Couldn't find a light with ID " + light_state["light_id"] + ". Will skip light!")
            return JsonResponse({"status": "ok"}, status=200)
        else:
            logging.error("Could not find scene with that id.")
            return JsonResponse({"status": "error"}, status=404)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


####################
### Misc section ###
####################
@csrf_exempt
def get_ip_by_hostname(request):
    try:
        data = json.loads(request.body)
        ip_address = socket.gethostbyname(data["hostname"])
        return JsonResponse({"ip": ip_address}, status=200)
    except socket.gaierror:
        return JsonResponse({"error": "Hostname not found"}, status=404)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"error": "Internal server error"}, status=500)


### Generic scene URLs ###
def get_scene(request, scene_id):
    try:
        if request.method == "GET":
            scene = Scene.objects.get(id=scene_id)
            return JsonResponse(
                {
                    "status": "success",
                    "result": {"id": scene.id, "room_id": scene.room_id, "friendly_name": scene.friendly_name, "type": "scene", "controller": scene.scene_type, "backend_name": scene.backend_name, "entities_page_id": scene.entities_page_id, "room_view_position": scene.room_view_position},
                }
            )
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"error": "Internal server error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


### Generic Entity section ###
def get_rest_entitiy_representation(entity_id):
    entity = Entity.objects.get(id=entity_id)
    return {
        "base": {
            "id": entity.id,
            "friendly_name": entity.friendly_name,
            "type": "entity",
            "room_id": entity.room_id,
            "entities_page_id": entity.entities_page_id,
            "room_view_position": entity.room_view_position,
        },
        "entity": entity.entity_data,
    }


def get_entity(request, entity_id):
    try:
        if request.method == "GET":
            return JsonResponse(
                {
                    "status": "success",
                    "result": get_rest_entitiy_representation(entity_id),
                }
            )
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"error": "Internal server error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


##################
# Lights section #
##################


def entities_lights(request):
    try:
        if request.method == "PUT":
            return put_light_entity(request)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


def put_light_entity(request):
    try:
        required_base_fields = ["room_id", "entities_page_id", "room_view_position", "controller", "type", "friendly_name"]  # Fields required for all entities
        required_light_fields = [  # Fields required for light entities
            "can_color_temperature",
            "can_dim",
            "can_rgb",
            "controlled_by_nspanel_main_page",
            "home_assistant_name",
            "is_ceiling_light",
            "openhab_control_mode",
            "openhab_item_color_temp",
            "openhab_item_dimmer",
            "openhab_item_rgb",
            "openhab_item_switch",
            "openhab_name",
        ]
        data = json.loads(request.body)
        for field in required_base_fields:
            if field not in data["base"]:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)
        for field in required_light_fields:
            if field not in data["entity"]:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        print(data)
        entity_data = {
            "controller": data["base"]["controller"],
            "home_assistant_name": data["entity"]["home_assistant_name"],
            "openhab_control_mode": data["entity"]["openhab_control_mode"],
            "openhab_item_switch": data["entity"]["openhab_item_switch"],
            "openhab_item_dimmer": data["entity"]["openhab_item_dimmer"],
            "openhab_item_color_temp": data["entity"]["openhab_item_color_temp"],
            "openhab_item_rgb": data["entity"]["openhab_item_rgb"],
            "can_dim": data["entity"]["can_dim"] == "true",
            "can_color_temperature": data["entity"]["can_color_temperature"] == "true",
            "can_rgb": data["entity"]["can_rgb"] == "true",
            "is_ceiling_light": data["entity"]["is_ceiling_light"] == "true",
            "controlled_by_nspanel_main_page": data["entity"]["controlled_by_nspanel_main_page"] == "true",
        }
        if "id" in data["base"] and data["base"]["id"]:
            new_light = Entity.objects.get(id=int(data["base"]["id"]))
            entity_data = new_light.entity_data
        else:
            new_light = Entity()
            new_light.entity_type = Entity.EntityType.LIGHT

        new_light.friendly_name = data["base"]["friendly_name"]
        new_light.room = Room.objects.get(id=int(data["base"]["room_id"]))
        new_light.entities_page = RoomEntitiesPage.objects.get(id=int(data["base"]["entities_page_id"]))
        new_light.room_view_position = int(data["base"]["room_view_position"])

        new_light.entity_data = entity_data
        new_light.save()
        # send_mqttmanager_reload_command()

        return JsonResponse({"status": "ok"}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
