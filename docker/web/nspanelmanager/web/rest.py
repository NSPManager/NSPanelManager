from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.core.files.storage import FileSystemStorage

import hashlib
import logging
import json
from .mqttmanager_ipc import send_ipc_request

from .models import NSPanel, Room, Light, LightState, Scene, RelayGroup
from .apps import start_mqtt_manager
from web.settings_helper import get_setting_with_default, get_nspanel_setting_with_default, set_setting_value

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
        return JsonResponse({"status": "error"}, status=403) # Return error forbidden

    try:
        settings = {}
        if request.method == 'GET':
            settings[setting_key] = get_setting_with_default(setting_key)
        else:
            return JsonResponse({"status": "error"}, status=405)

        return JsonResponse({
            "status": "ok",
            "settings": settings,
        })
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error"}, status=500)


@csrf_exempt
def mqttmanager_settings_post(request):
    try:
        settings = {}
        if request.method == 'POST':
            data = json.loads(request.body)

            for setting_key in data["settings"]:
                if setting_key in banned_setting_keys:
                    return JsonResponse({"status": "error"}, status=403) # Return error forbidden
                settings[setting_key] = get_setting_with_default(setting_key)
        else:
            return JsonResponse({"status": "error"}, status=405)

        return JsonResponse({
            "status": "ok",
            "settings": settings,
        })
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

# Retreive warnings for a NSPanel. Only GET-requests are valid for this resource
def nspanel_warnings(request):
    try:
        if(request.method == "GET"):
            md5_firmware = get_file_md5sum("firmware.bin")
            md5_data_file = get_file_md5sum("data_file.bin")
            md5_tft_file = get_file_md5sum("gui.tft")
            md5_us_tft_file = get_file_md5sum("gui_us.tft")

            if request.GET.get('id'):
                nspanel_objects = NSPanel.objects.filter(id=request.GET.get('id'))
            elif request.GET.get('mac_address'):
                nspanel_objects = NSPanel.objects.filter(mac_address=request.GET.get('mac_address'))
            else:
                nspanel_objects = NSPanel.objects.all()

            panels = []
            for nspanel in nspanel_objects:

                panel_info = {
                    "id": nspanel.id,
                    "mac_address": nspanel.mac_address,
                    "warnings": []
                }
                for panel in NSPanel.objects.all():
                    if panel == nspanel:
                        continue
                    elif panel.friendly_name == nspanel.friendly_name:
                        panel_info["warnings"].append("Two or more panels exists with the same name. This may have unintended consequences")
                        break
                if nspanel.md5_firmware != md5_firmware or nspanel.md5_data_file != md5_data_file:
                    panel_info["warnings"].append("Firmware update available.")
                if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "False" and nspanel.md5_tft_file != md5_tft_file:
                    panel_info["warnings"].append("GUI update available.")
                if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "True" and nspanel.md5_tft_file != md5_us_tft_file:
                    panel_info["warnings"].append("GUI update available.")
                panels.append(panel_info)

            return JsonResponse({
                "status": "ok",
                "nspanels": panels
            }, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
    return JsonResponse({"status": "error"}, status=405)

@csrf_exempt
def nspanels(request):
    if request.method == "GET":
        return nspanels_get(request)
    elif request.method == "POST":
        return nspanel_post(request)
    else:
        return JsonResponse({
            "status": "error"
        }, status=405)

def nspanels_get(request):
    if request.method == "GET":
        nspanels = list()
        if request.GET.get('id'):
            nspanel_objects = NSPanel.objects.filter(id=request.GET.get('id'))
        elif request.GET.get('mac_address'):
            nspanel_objects = NSPanel.objects.filter(mac_address=request.GET.get('mac_address'))
        else:
            nspanel_objects = NSPanel.objects.all()

        for nspanel in nspanel_objects:
            nspanels.append({
                "id": nspanel.id,
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
                "mqtt_ignore_time": get_nspanel_setting_with_default(nspanel.id, "mqtt_ignore_time", 3000),
                "screen_dim_level": get_nspanel_setting_with_default(nspanel.id, "screen_dim_level", get_setting_with_default("screen_dim_level")),
                "screensaver_dim_level": get_nspanel_setting_with_default(nspanel.id, "screensaver_dim_level", get_setting_with_default("screensaver_dim_level")),
                "screensaver_activation_timeout": get_nspanel_setting_with_default(nspanel.id, "screensaver_activation_timeout", get_setting_with_default("screensaver_activation_timeout")),
                "screensaver_mode": get_nspanel_setting_with_default(nspanel.id, "screensaver_mode", get_setting_with_default("screensaver_mode")),
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
                "rooms": [room.id for room in Room.objects.all().order_by('displayOrder')],
                "scenes": [scene.id for scene in Scene.objects.all()],
            })
        return JsonResponse({
            "status": "ok",
            "nspanels": nspanels
        })

# Handle DELETE request to "nspanel" endpoint.
@csrf_exempt
def nspanel_delete(request, panel_id):
    if request.method == "DELETE":
        try:
            logging.info(F"Deleting NSPanel with ID {panel_id}.")
            nspanel = NSPanel.objects.get(id=panel_id)
            nspanel.delete()
            return JsonResponse({
                "status": "ok",
                "nspanel_id": panel_id
            }, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)

# Handle POST request to "nspanel" endpoint.
@csrf_exempt
def nspanel_post(request):
    try:
        data = json.loads(request.body)

        if "mac" in data:
            data["mac_address"] = data["mac"]
        elif "mac_origin" in data:
            data["mac_address"] = data["mac_origin"]
        new_panel = NSPanel.objects.filter(mac_address=data['mac_address']).first()

        # new_panel is none ie. we didn't find a known panel with that MAC. Create a new one.
        if not new_panel:
            new_panel = NSPanel()
            new_panel.friendly_name = data['friendly_name']

        new_panel.mac_address = data['mac_address']
        new_panel.version = data["version"] if "version" in data else ""
        new_panel.ip_address = ""  # TODO: Remove ip_address from DB

        fs = FileSystemStorage()
        if "md5_firmware" in data:
            if data["md5_firmware"] == "":
                new_panel.md5_firmware = hashlib.md5(
                    fs.open("firmware.bin").read()).hexdigest()
            else:
                new_panel.md5_firmware = data["md5_firmware"]

        if "md5_data_file" in data:
            if data["md5_data_file"] == "":
                new_panel.md5_data_file = hashlib.md5(
                    fs.open("data_file.bin").read()).hexdigest()
            else:
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
        }
        return JsonResponse(json_response, status=200)
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
        if request.GET.get('id'):
            room_objects = Room.objects.filter(id=request.GET.get('id'))
        else:
            room_objects = Room.objects.all()
        for room in room_objects:
            rooms.append({
                "id": room.id,
                "name": room.friendly_name,
                "lights": [light.id for light in room.light_set.all()],
                "scenes": [scene.id for scene in room.scene_set.all()],
            })
        return JsonResponse({
            "status": "ok",
            "rooms": rooms
        }, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)

@csrf_exempt
def room_delete(request, room_id):
    if request.method == "DELETE":
        try:
            room = Room.objects.get(id=room_id)
            room.delete()
            return JsonResponse({
                "status": "ok",
                "room_id": room_id
            }, status=200)
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
            return JsonResponse({
                "status": "ok",
                "room_id": new_room.id
            }, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)


#####################
### Light section ###
#####################

@csrf_exempt
def lights(request):
    if request.method == "GET":
        return lights_get(request)
    elif request.method == "POST":
        return lights_post(request)
    else:
        return JsonResponse({"status": "error"}, status=405)

def lights_get(request):
    send_ipc_request("test_topic", {})

    try:
        lights = []
        if request.GET.get('light_id'):
            light_objects = Light.objects.filter(id=request.GET.get('light_id'))
        elif request.GET.get('room_id'):
                light_objects = Light.objects.filter(room__id=request.GET.get('room_id'))
        else:
            light_objects = Light.objects.all()

        for light in light_objects:
            lights.append({
                "id": light.id,
                "name": light.friendly_name,
                "type": light.type,
                "ceiling": light.is_ceiling_light,
                "can_dim": light.can_dim,
                "can_color_temperature": light.can_color_temperature,
                "can_rgb": light.can_rgb,
                "home_assistant_name": light.home_assistant_name,
                "openhab_name": light.openhab_name,
                "openhab_control_mode": light.openhab_control_mode,
                "openhab_item_switch": light.openhab_item_switch,
                "openhab_item_dimmer": light.openhab_item_dimmer,
                "openhab_item_color_temp": light.openhab_item_color_temp,
                "openhab_item_rgb": light.openhab_item_rgb,
            })
        return JsonResponse({
            "status": "ok",
            "lights": lights
        }, status=200)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)

def lights_post(request):
    if request.method == "POST":
        try:
            data = json.loads(request.body)
            room = Room.objects.get(id=data['room_id'])
            if 'light_id' in data:
                newLight = Light.objects.get(id=data['light_id'])
            else:
                newLight = Light()
            newLight.room = room
            newLight.type = data['type']
            newLight.friendly_name = data['friendly_name']
            if data["light_type"] == "ceiling":
                newLight.is_ceiling_light = True
            else:
                newLight.is_ceiling_light = False

            if newLight.type == "home_assistant":
                newLight.home_assistant_name = data["home_assistant_name"]
            elif newLight.type == "openhab":
                newLight.openhab_name = data["openhab_name"]

            if data["light_control_mode"] == "dimmer":
                newLight.can_dim = True
                newLight.openhab_control_mode = "dimmer"
                if newLight.type == "openhab":
                    newLight.openhab_item_dimmer = data["openhab_dimming_channel_name"]
            else:
                newLight.openhab_control_mode = "switch"
                newLight.can_dim = False
                if newLight.type == "openhab":
                    newLight.openhab_item_switch = data["openhab_switch_channel_name"]

            if data["color_temperature"]:
                newLight.can_color_temperature = True
                if newLight.type == "openhab":
                    newLight.openhab_item_color_temp = data["openhab_color_temperature_channel_name"]
            else:
                newLight.can_color_temperature = False
                newLight.openhab_item_color_temp = ""

            if data["rgb"]:
                newLight.can_rgb = True
                if newLight.type == "openhab":
                    newLight.openhab_item_rgb = data["openhab_rgb_channel_name"]
            else:
                newLight.can_rgb = False
                newLight.openhab_item_rgb = ""

            if data["add_to_room_view"]:
                if newLight.room_view_position == 0:
                    all_lights = Light.objects.filter(room=room, room_view_position__gte=1, room_view_position__lte=12)
                    # Find the first available free position on the room view screen to place the light at
                    for i in range(1, 13):
                        position_free = True
                        for light in all_lights:
                            if light.room_view_position == i:
                                position_free = False
                                break
                        if position_free:
                            newLight.room_view_position = i
                            break

            newLight.save()
            return JsonResponse({
                "status": "ok",
                "light_id": newLight.id
            }, status=200)

        except BaseException as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)

@csrf_exempt
def light_delete(request, light_id):
    if(request.method == "DELETE"):
        try:
            Light.objects.get(id=light_id).delete()
            return JsonResponse({
                "status": "ok",
                "light_id": light_id
            }, status=200)
        except Exception as ex:
            logging.exception(ex)
            return JsonResponse({"status": "error"}, status=500)
    else:
        return JsonResponse({"status": "error"}, status=405)
