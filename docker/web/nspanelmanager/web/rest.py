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
from .models import Entity, LightState, NSPanel, RelayGroup, Room, RoomEntitiesPage, Scene, Settings

########################
# Get entities section #
########################


def get_home_assistant_entities(request):
    if request.method != "GET":
        return JsonResponse({"status": "error"}, status=405)

    filter_params = json.loads(request.GET.get("filter", "{}"))
    return JsonResponse(web.home_assistant_api.get_all_home_assistant_items(filter_params))


def get_openhab_items(request):
    if request.method != "GET":
        return JsonResponse({"status": "error"}, status=405)

    filter_params = json.loads(request.GET.get("filter", "{}"))
    return JsonResponse(web.openhab_api.get_all_openhab_items(filter_params))


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


def settings(request):
    if request.method == "GET":
        return settings_get(request)
    else:
        return JsonResponse({"status": "error"}, status=405)


def settings_get(request):
    settings = {}
    for setting in Settings.objects.all():
        settings[setting.name] = setting.value
    settings["home_assistant_token_set"] = settings.get("home_assistant_token", "") != ""
    del settings["home_assistant_token"]
    settings["openhab_token_set"] = settings.get("openhab_token", "") != ""
    del settings["openhab_token"]
    settings["mqtt_password_set"] = settings.get("mqtt_password", "") != ""
    del settings["mqtt_password"]
    return JsonResponse({"status": "ok", "settings": settings}, status=200)


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
                    "id": room.id,
                    "name": room.friendly_name,
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
                db_entity = Entity.objects.get(id=entity["id"])
                db_entity.room_view_position = entity["room_view_position"]
                db_entity.entities_page_id = entity["entities_page_id"]
                db_entity.save()
            for scene in data["scenes"]:
                db_scene = Scene.objects.get(id=scene["id"])
                db_scene.room_view_position = scene["room_view_position"]
                db_scene.entities_page_id = scene["entities_page_id"]
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
                        "entities": [get_rest_entitiy_representation(entity.id) for entity in page.entity_set.all().order_by("room_view_position")],
                        "scenes": [get_rest_scene_representation(entity.id) for entity in page.scene_set.all().order_by("room_view_position")],
                    }
                )
            return JsonResponse({"status": "ok", "entities_pages": response}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    elif request.method == "PUT":
        required_fields = ["is_scenes_page", "type"]
        data = json.loads(request.body)

        for field in required_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        room = Room.objects.get(id=room_id)
        pages = RoomEntitiesPage.objects.filter(room=room).order_by("display_order")
        new_display_order = 0  # Default to zero of no pages exists
        if len(pages) > 0:
            new_display_order = pages[len(pages) - 1].display_order + 1

        new_page = RoomEntitiesPage()
        new_page.page_type = data["type"]
        new_page.display_order = new_display_order
        new_page.is_scenes_page = data["is_scenes_page"]
        new_page.room = room
        new_page.save()
        send_mqttmanager_reload_command()
        return JsonResponse({"status": "ok"}, status=200)
    else:
        return JsonResponse({"status": "error"}, status=405)


def room_entities_page(request, page_id):
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
    elif request.method == "PUT":
        try:
            data = json.loads(request.body)
            db_page = RoomEntitiesPage.objects.get(id=page_id)
            db_page.page_type = data.get("number_of_entities", db_page.page_type)
            db_page.display_order = data.get("display_order", db_page.display_order)
            db_page.save()
            return JsonResponse({"status": "ok"}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    elif request.method == "DELETE":
        try:
            db_page = RoomEntitiesPage.objects.get(id=page_id)
            db_page.delete()
            send_mqttmanager_reload_command()
            return JsonResponse({"status": "ok"}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


def room_entities_pages_order(request):
    if request.method == "PUT":
        try:
            json_data = json.loads(request.body)
            if "order" not in json_data:
                return JsonResponse({"status": "error", "message": "order field is required"}, status=400)
            for page_id, display_order in json_data["order"]:
                db_page = RoomEntitiesPage.objects.get(id=page_id)
                db_page.display_order = display_order
                db_page.save()
            send_mqttmanager_reload_command()
            return JsonResponse({"status": "ok"}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


########################
### Global functions ###
########################


def global_entities_pages(request):
    if request.method == "GET":
        try:
            pages = RoomEntitiesPage.objects.filter(room=None).order_by("display_order")
            response = []
            for page in pages:
                response.append(
                    {
                        "id": page.id,
                        "display_order": page.display_order,
                        "number_of_entities": page.page_type,
                        "type": "scene" if page.is_scenes_page else "entity",
                        "room_id": None,
                        "entities": [get_rest_entitiy_representation(entity.id) for entity in page.entity_set.all().order_by("room_view_position")],
                        "scenes": [get_rest_scene_representation(entity.id) for entity in page.scene_set.all().order_by("room_view_position")],
                    }
                )
            return JsonResponse({"status": "ok", "entities_pages": response}, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    elif request.method == "PUT":
        required_fields = ["is_scenes_page", "type"]
        data = json.loads(request.body)

        for field in required_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        pages = RoomEntitiesPage.objects.filter(room=None).order_by("display_order")
        new_display_order = 0  # Default to zero of no pages exists
        if len(pages) > 0:
            new_display_order = pages[len(pages) - 1].display_order + 1

        new_page = RoomEntitiesPage()
        new_page.page_type = data["type"]
        new_page.display_order = new_display_order
        new_page.is_scenes_page = data["is_scenes_page"]
        new_page.room = None
        new_page.save()
        send_mqttmanager_reload_command()
        return JsonResponse({"status": "ok"}, status=200)
    else:
        return JsonResponse({"status": "error"}, status=405)


######################
### Room functions ###
######################


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


def room_entities(request, room_id):
    if request.method == "GET":
        try:
            entities = Entity.objects.filter(room_id=room_id)
            return JsonResponse({"status": "ok", "entities": [get_rest_entitiy_representation(entity.id) for entity in entities]}, status=200)
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
        "id": scene.id,
        "friendly_name": scene.friendly_name,
        "type": "scene",
        "room_id": scene.room.id if scene.room != None else None,
        "entities_page_id": scene.entities_page.id if scene.entities_page != None else None,
        "room_view_position": scene.room_view_position,
        "controller": scene.scene_type,
        "scene_type": scene.scene_type,
        "backend_name": scene.backend_name,  # Name for OpenHAB or Home Assistant entity to activate
        "light_states": [],
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
        elif request.method == "DELETE":
            scene = Scene.objects.get(id=scene_id)
            scene.delete()
            return JsonResponse({"status": "success"})
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"error": "Internal server error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


### Generic Entity section ###
def get_rest_entitiy_representation(entity_id):
    entity = Entity.objects.get(id=entity_id)
    return {
        "id": entity.id,
        "friendly_name": entity.friendly_name,
        "type": "entity",
        "entity_type": entity.entity_type,
        "room_id": entity.room_id,
        "entities_page_id": entity.entities_page_id,
        "room_view_position": entity.room_view_position,
        "controller": entity.entity_data["controller"],
        **entity.entity_data,
    }


def get_entity(request, entity_id):
    try:
        if request.method == "GET":
            return JsonResponse(
                get_rest_entitiy_representation(entity_id),
            )
        elif request.method == "DELETE":
            Entity.objects.get(id=entity_id).delete()
            send_mqttmanager_reload_command()
            return JsonResponse(
                {
                    "status": "success",
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
        required_light_fields = [  # Fields required for light entities
            "room_id",
            "entities_page_id",
            "room_view_position",
            "controller",
            "type",
            "friendly_name",
            "can_color_temperature",
            "can_dim",
            "can_rgb",
            "controlled_by_nspanel_main_page",
            "home_assistant_name",
            "is_ceiling_light",
            "openhab_item_color_temp",
            "openhab_item_dimmer",
            "openhab_item_rgb",
        ]
        data = json.loads(request.body)["values"]
        for field in required_light_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        entity_data = {
            "controller": data["controller"],
            "home_assistant_name": data.get("home_assistant_name", ""),
            "openhab_control_mode": "dimmer" if data.get("can_dim", False) else "switch",
            "openhab_item_dimmer": data.get("openhab_item_dimmer", ""),
            "openhab_item_color_temp": data.get("openhab_item_color_temp", ""),
            "openhab_item_rgb": data.get("openhab_item_rgb", ""),
            "can_dim": str(data["can_dim"]).lower() == "true",
            "can_color_temperature": str(data["can_color_temperature"]).lower() == "true",
            "can_rgb": str(data["can_rgb"]).lower() == "true",
            "is_ceiling_light": str(data["is_ceiling_light"]).lower() == "true",
            "controlled_by_nspanel_main_page": str(data["controlled_by_nspanel_main_page"]).lower() == "true",
        }
        if "id" in data and data["id"]:
            new_light = Entity.objects.get(id=int(data["id"]))
        else:
            new_light = Entity()
            new_light.entity_type = Entity.EntityType.LIGHT

        new_light.friendly_name = data["friendly_name"]
        new_light.room = Room.objects.get(id=int(data["room_id"]))
        new_light.entities_page = RoomEntitiesPage.objects.get(id=int(data["entities_page_id"]))
        new_light.room_view_position = int(data["room_view_position"])

        new_light.entity_data = entity_data
        new_light.save()
        send_mqttmanager_reload_command()

        return JsonResponse({"status": "ok"}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


####################
# Switches section #
####################


def entities_switches(request):
    try:
        if request.method == "PUT":
            return put_switch_entity(request)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


def put_switch_entity(request):
    try:
        required_fields = [  # Fields required for light entities
            "room_id",
            "entities_page_id",
            "room_view_position",
            "controller",
            "type",
            "friendly_name",
            "home_assistant_name",
            "openhab_item_switch",
        ]
        data = json.loads(request.body)
        for field in required_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        entity_data = {
            "openhab_item_switch": data.get("openhab_item_switch", ""),
            "home_assistant_name": data.get("home_assistant_name", ""),
            "controller": data["controller"],
        }
        if "id" in data and data["id"]:
            new_switch = Entity.objects.get(id=int(data["id"]))
        else:
            new_switch = Entity()
            new_switch.entity_type = Entity.EntityType.SWITCH

        new_switch.friendly_name = data["friendly_name"]
        new_switch.room = Room.objects.get(id=int(data["room_id"]))
        new_switch.entities_page = RoomEntitiesPage.objects.get(id=int(data["entities_page_id"]))
        new_switch.room_view_position = int(data["room_view_position"])

        new_switch.entity_data = entity_data
        new_switch.save()
        send_mqttmanager_reload_command()

        return JsonResponse({"status": "ok"}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


###################
# Buttons section #
###################


def entities_buttons(request):
    try:
        if request.method == "PUT":
            return put_button_entity(request)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


def put_button_entity(request):
    try:
        required_fields = [  # Fields required for button entities
            "room_id",
            "entities_page_id",
            "room_view_position",
            "controller",
            "type",
            "friendly_name",
            "home_assistant_name",
            "mqtt_topic",
            "mqtt_payload",
        ]
        data = json.loads(request.body)
        for field in required_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        entity_data = {
            "mqtt_topic": data.get("mqtt_topic", ""),
            "mqtt_payload": data.get("mqtt_payload", ""),
            "home_assistant_name": data.get("home_assistant_name", ""),
            "controller": data["controller"],
        }
        if "id" in data and data["id"]:
            new_button = Entity.objects.get(id=int(data["id"]))
        else:
            new_button = Entity()
            new_button.entity_type = Entity.EntityType.BUTTON

        new_button.friendly_name = data["friendly_name"]
        new_button.room = Room.objects.get(id=int(data["room_id"]))
        new_button.entities_page = RoomEntitiesPage.objects.get(id=int(data["entities_page_id"]))
        new_button.room_view_position = int(data["room_view_position"])

        new_button.entity_data = entity_data
        new_button.save()
        send_mqttmanager_reload_command()

        return JsonResponse({"status": "ok"}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


###################
# Thermostat section #
###################


def entities_thermostats(request):
    try:
        # if request.method == "PUT":
        return put_thermostat_entity(request)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


def put_thermostat_entity(request):
    try:
        required_fields = [  # Fields required for thermostat entities
            "room_id",
            "entities_page_id",
            "room_view_position",
            "controller",
            "type",
            "friendly_name",
            "step_size",
            "home_assistant_name",
            "openhab_fan_mode_item",
            "openhab_hvac_mode_item",
            "openhab_preset_mode_item",
            "openhab_swing_mode_item",
            "openhab_swingh_mode_item",
            "openhab_temperature_item",
            "fan_modes",
            "hvac_modes",
            "preset_modes",
            "swing_modes",
            "swingh_modes",
        ]
        required_mode_fields = ["icon", "label", "value"]  # Fields required in each ..._modes items

        data = json.loads(request.body)
        for field in required_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        for mode in ["fan_modes", "hvac_modes", "preset_modes", "swing_modes", "swingh_modes"]:
            if mode in data:
                for item in data[mode]:
                    for field in required_mode_fields:
                        if field not in item:
                            return JsonResponse({"status": "error", "message": f"Missing required field in {mode}. Missing field: {field}"}, status=400)

        entity_data = {
            "controller": data.get("controller", ""),
            "fan_modes": data.get("fan_modes", []),
            "hvac_modes": data.get("hvac_modes", []),
            "preset_modes": data.get("preset_modes", []),
            "swing_modes": data.get("swing_modes", []),
            "swingh_modes": data.get("swingh_modes", []),
            "home_assistant_name": data.get("home_assistant_name", ""),
            "openhab_fan_mode_item": data.get("openhab_fan_mode_item", ""),
            "openhab_hvac_mode_item": data.get("openhab_hvac_mode_item", ""),
            "openhab_preset_mode_item": data.get("openhab_preset_mode_item", ""),
            "openhab_swing_mode_item": data.get("openhab_swing_mode_item", ""),
            "openhab_swingh_mode_item": data.get("openhab_swingh_mode_item", ""),
            "openhab_temperature_item": data.get("openhab_temperature_item", ""),
            "step_size": data.get("step_size", 1),
        }
        if "id" in data and data["id"]:
            new_thermostat = Entity.objects.get(id=int(data["id"]))
        else:
            new_thermostat = Entity()
            new_thermostat.entity_type = Entity.EntityType.THERMOSTAT

        new_thermostat.friendly_name = data["friendly_name"]
        new_thermostat.room = Room.objects.get(id=int(data["room_id"]))
        new_thermostat.entities_page = RoomEntitiesPage.objects.get(id=int(data["entities_page_id"]))
        new_thermostat.room_view_position = int(data["room_view_position"])

        new_thermostat.entity_data = entity_data
        new_thermostat.save()
        send_mqttmanager_reload_command()

        return JsonResponse({"status": "ok"}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)


#################
# Scene section #
#################


def entities_scenes(request):
    try:
        # if request.method == "PUT":
        return put_scene_entity(request)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=403)


def put_scene_entity(request):
    try:
        required_fields = [  # Fields required for scene entities
            "room_id",
            "entities_page_id",
            "room_view_position",
            "controller",
            "type",
            "friendly_name",
            "scene_type",
            "backend_name",
        ]

        data = json.loads(request.body)
        for field in required_fields:
            if field not in data:
                return JsonResponse({"status": "error", "message": f"Missing required field: {field}"}, status=400)

        if "id" in data and data["id"]:
            new_scene = Scene.objects.get(id=int(data["id"]))
        else:
            new_scene = Scene()

        new_scene.friendly_name = data["friendly_name"]
        new_scene.room = Room.objects.get(id=int(data["room_id"])) if data["room_id"] else None
        new_scene.entities_page = RoomEntitiesPage.objects.get(id=int(data["entities_page_id"]))
        new_scene.room_view_position = int(data["room_view_position"])

        new_scene.scene_type = data.get("scene_type", "")
        new_scene.backend_name = data.get("backend_name", "")
        new_scene.save()
        send_mqttmanager_reload_command()

        return JsonResponse({"status": "ok"}, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
