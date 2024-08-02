from requests import delete
from django.shortcuts import render, redirect, HttpResponse
from django.core.files.storage import FileSystemStorage
from django.views.decorators.csrf import csrf_exempt

import hashlib
import psutil
import subprocess
import logging
import environ
import os
import signal
from time import sleep

#from nspanelmanager.web.mqttmanager_ipc import send_ipc_request
from .mqttmanager_ipc import send_ipc_request

from .models import NSPanel, Room, Light, Settings, Scene, RelayGroup, RelayGroupBinding
from .apps import start_mqtt_manager
from web.settings_helper import delete_nspanel_setting, get_setting_with_default, set_setting_value, get_nspanel_setting_with_default, set_nspanel_setting_value

def restart_mqtt_manager():
    for proc in psutil.process_iter():
        if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
            print("Killing running MQTTManager")
            proc.kill()
            break
    start_mqtt_manager()


def send_mqttmanager_reload_command():
    for proc in psutil.process_iter():
        if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
            print("Found running MQTTManager. Sending reload command via SIGUSR1 signal.")
            os.kill(proc.pid, signal.SIGUSR1)
            break


def get_file_md5sum(filename):
    fs = FileSystemStorage()
    if fs.exists(filename):
        return hashlib.md5(fs.open(filename).read()).hexdigest()
    else:
        return None


def index(request):
    md5_firmware = get_file_md5sum("firmware.bin")
    md5_data_file = get_file_md5sum("data_file.bin")
    md5_tft_file = get_file_md5sum("gui.tft")
    md5_us_tft_file = get_file_md5sum("gui_us.tft")

    if get_setting_with_default("use_fahrenheit") == "True":
        temperature_unit = "°F"
    else:
        temperature_unit = "°C"

    notifications = []
    if get_setting_with_default("manager_address") == "":
        notifications.append(
            {"text": "No manager address configured in settings.", "class": "error"})

    nspanels = []
    for nspanel in NSPanel.objects.filter(denied=False):
        panel_info = {}
        panel_info["data"] = nspanel
        panel_status = send_ipc_request(F"nspanel/{nspanel.id}/status", {"command": "get"})
        panel_info["status"] = panel_status
        panel_info["status"]["warnings"] = []
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
        'dark_theme': get_setting_with_default("dark_theme"),
        'nspanels': nspanels,
        'notifications': notifications,
        'temperature_unit': temperature_unit,
        'manager_address': get_setting_with_default("manager_address")
    }

    if (data["manager_address"] == ""):
        environment = environ.Env()
        data = {**data, **{
            'manager_port': get_setting_with_default("manager_port"),
            "mqtt_server": get_setting_with_default("mqtt_server"),
            "mqtt_port": get_setting_with_default("mqtt_port"),
            "mqtt_username": get_setting_with_default("mqtt_username"),
            "mqtt_password": get_setting_with_default("mqtt_password"),
            "home_assistant_address": get_setting_with_default("home_assistant_address"),
            "home_assistant_token": get_setting_with_default("home_assistant_token"),
            "openhab_address": get_setting_with_default("openhab_address"),
            "openhab_token": get_setting_with_default("openhab_token"),
            "is_home_assistant_addon": ("IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true")
        }}

    return render(request, 'index_htmx.html', data)


def rooms(request):
    return render(request, 'rooms.html', {'dark_theme': get_setting_with_default("dark_theme"), 'rooms': Room.objects.all().order_by('displayOrder')})


def rooms_order(request):
    return render(request, 'rooms_order.html', {'dark_theme': get_setting_with_default("dark_theme"), 'rooms': Room.objects.all().order_by('displayOrder')})


def move_room_up(request, room_id: int):
    room = Room.objects.get(id=room_id)
    if room.displayOrder > 1:
        otherRoom = Room.objects.filter(displayOrder=room.displayOrder - 1)
        if otherRoom.count() > 0:
            move_up_room = otherRoom.first()
            move_up_room.displayOrder += 1
            move_up_room.save()

            room.displayOrder -= 1
            room.save()

        # Loop through all rooms and make sure they all follow a pattern
        all_rooms = Room.objects.all().order_by('displayOrder')
        i = 1
        for room in all_rooms:
            room.displayOrder = i
            room.save()
            i += 1

    return redirect('rooms_order')


def move_room_down(request, room_id: int):
    room = Room.objects.get(id=room_id)
    otherRoom = Room.objects.filter(displayOrder=room.displayOrder + 1)
    if otherRoom.count() > 0:
        move_up_room = otherRoom.first()
        move_up_room.displayOrder -= 1
        move_up_room.save()

        room.displayOrder += 1
        room.save()

    # Loop through all rooms and make sure they all follow a pattern
    all_rooms = Room.objects.all().order_by('displayOrder')
    i = 1
    for room in all_rooms:
        room.displayOrder = i
        room.save()
        i += 1

    return redirect('rooms_order')


def edit_room(request, room_id: int):
    total_num_rooms = Room.objects.all().count()
    room = Room.objects.filter(id=room_id).first()
    data = {
        'dark_theme': get_setting_with_default("dark_theme"),
        'room': room,
        'total_num_rooms': total_num_rooms
    }
    lights = Light.objects.filter(
        room=room, room_view_position__gte=1, room_view_position__lte=12)
    for light in lights:
        data["light" + str(light.room_view_position)] = light
    return render(request, 'edit_room.html', data)


def save_new_room(request):
    new_room = Room()
    new_room.friendly_name = request.POST['friendly_name']
    new_room.save()
    send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=new_room.id)


def delete_room(request, room_id: int):
    num_rooms = Room.objects.all().count()
    if num_rooms > 1:
        room = Room.objects.filter(id=room_id).first()
        if num_rooms >= 2:
            nspanels = NSPanel.objects.filter(room=room)
            new_room = Room.objects.all().exclude(id=room.id).first()
            nspanels.update(room=new_room)
        room.delete()

        send_mqttmanager_reload_command()
    return redirect('rooms')


def update_room_form(request, room_id: int):
    room = Room.objects.filter(id=room_id).first()
    room.friendly_name = request.POST['friendly_name']
    room.save()
    send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=room_id)


def edit_nspanel(request, panel_id: int):
    if get_setting_with_default("use_fahrenheit") == "True":
        temperature_unit = "°F"
    else:
        temperature_unit = "°C"
    settings = {
        "lock_to_default_room": get_nspanel_setting_with_default(panel_id, "lock_to_default_room", "False"),
        "screen_dim_level": get_nspanel_setting_with_default(panel_id, "screen_dim_level", ""),
        "screensaver_dim_level": get_nspanel_setting_with_default(panel_id, "screensaver_dim_level", ""),
        "is_us_panel": get_nspanel_setting_with_default(panel_id, "is_us_panel", "False"),
        "screensaver_activation_timeout": get_nspanel_setting_with_default(panel_id, "screensaver_activation_timeout", ""),
        "screensaver_mode": get_nspanel_setting_with_default(panel_id, "screensaver_mode", "global"),
        "reverse_relays": get_nspanel_setting_with_default(panel_id, "reverse_relays", "False"),
        "relay1_default_mode": get_nspanel_setting_with_default(panel_id, "relay1_default_mode", "False"),
        "relay1_is_light": get_nspanel_setting_with_default(panel_id, "relay1_is_light", "False"),
        "relay2_default_mode": get_nspanel_setting_with_default(panel_id, "relay2_default_mode", "False"),
        "relay2_is_light": get_nspanel_setting_with_default(panel_id, "relay2_is_light", "False"),
        "temperature_calibration": get_nspanel_setting_with_default(panel_id, "temperature_calibration", 0),
        "button1_custom_mqtt_topic": get_nspanel_setting_with_default(panel_id, "button1_mqtt_topic", ""),
        "button1_custom_mqtt_payload": get_nspanel_setting_with_default(panel_id, "button1_mqtt_payload", ""),
        "button2_custom_mqtt_topic": get_nspanel_setting_with_default(panel_id, "button2_mqtt_topic", ""),
        "button2_custom_mqtt_payload": get_nspanel_setting_with_default(panel_id, "button2_mqtt_payload", ""),
        "default_page": get_nspanel_setting_with_default(panel_id, "default_page", "0"),
    }

    return render(request, 'edit_nspanel.html', {
        'dark_theme': get_setting_with_default("dark_theme"),
        'panel': NSPanel.objects.get(id=panel_id),
        'rooms': Room.objects.all(),
        'settings': settings,
        "temperature_unit": temperature_unit,
        "multiple": [1, 2, 3, 4],
        "max_live_log_messages": get_setting_with_default("max_live_log_messages"),
    })


def save_panel_settings(request, panel_id: int):
    panel = NSPanel.objects.get(id=panel_id)
    panel.room = Room.objects.get(id=request.POST["room_id"])
    panel.friendly_name = request.POST["name"]
    panel.button1_mode = request.POST["button1_mode"]
    if request.POST["button1_mode"] == "1":  # Detached mode
        panel.button1_detached_mode_light = Light.objects.get(
            id=request.POST["button1_detached_mode_light"])
    else:
        panel.button1_detached_mode_light = None

    if request.POST["button1_mode"] == "2":  # Custom MQTT Mode
        set_nspanel_setting_value(
            panel_id, "button1_mqtt_topic", request.POST["button1_custom_mqtt_topic"])
        set_nspanel_setting_value(
            panel_id, "button1_mqtt_payload", request.POST["button1_custom_mqtt_payload"])
    else:
        delete_nspanel_setting(panel_id, "button1_mqtt_topic")
        delete_nspanel_setting(panel_id, "button1_mqtt_payload")

    panel.button2_mode = request.POST["button2_mode"]
    if request.POST["button2_mode"] == "1":  # Detached mode
        panel.button2_detached_mode_light = Light.objects.get(
            id=request.POST["button2_detached_mode_light"])
    else:
        panel.button2_detached_mode_light = None

    if request.POST["button1_mode"] == "2":  # Custom MQTT Mode
        set_nspanel_setting_value(
            panel_id, "button2_mqtt_topic", request.POST["button2_custom_mqtt_topic"])
        set_nspanel_setting_value(
            panel_id, "button2_mqtt_payload", request.POST["button2_custom_mqtt_payload"])
    else:
        delete_nspanel_setting(panel_id, "button2_mqtt_topic")
        delete_nspanel_setting(panel_id, "button2_mqtt_payload")

    if request.POST["screen_dim_level"].strip():
        set_nspanel_setting_value(
            panel_id, "screen_dim_level", request.POST["screen_dim_level"])
    else:
        delete_nspanel_setting(panel_id, "screen_dim_level")

    if request.POST["screensaver_dim_level"].strip():
        set_nspanel_setting_value(
            panel_id, "screensaver_dim_level", request.POST["screensaver_dim_level"])
    else:
        delete_nspanel_setting(panel_id, "screensaver_dim_level")

    if request.POST["screensaver_activation_timeout"].strip():
        set_nspanel_setting_value(
            panel_id, "screensaver_activation_timeout", request.POST["screensaver_activation_timeout"])
    else:
        delete_nspanel_setting(panel_id, "screensaver_activation_timeout")

    if request.POST["screensaver_mode"] == "global":
        delete_nspanel_setting(panel_id, "screensaver_mode")
    else:
        set_nspanel_setting_value(
            panel_id, "screensaver_mode", request.POST["screensaver_mode"])

    set_nspanel_setting_value(
        panel_id, "relay1_default_mode", request.POST["relay1_default_mode"])
    set_nspanel_setting_value(
        panel_id, "relay2_default_mode", request.POST["relay2_default_mode"])
    set_nspanel_setting_value(panel_id, "temperature_calibration", float(
        request.POST["temperature_calibration"]))
    set_nspanel_setting_value(panel_id, "default_page",
                              request.POST["default_page"])
    set_nspanel_setting_value(
        panel_id, "lock_to_default_room", request.POST["lock_to_default_room"])
    set_nspanel_setting_value(
        panel_id, "reverse_relays", request.POST["reverse_relays"])
    set_nspanel_setting_value(panel_id, "is_us_panel",
                              request.POST["is_us_panel"])
    set_nspanel_setting_value(panel_id, "relay1_is_light",
                              request.POST["relay1_is_light"])
    set_nspanel_setting_value(panel_id, "relay2_is_light",
                              request.POST["relay2_is_light"])

    panel.save()
    send_mqttmanager_reload_command()
    return redirect('edit_nspanel', panel_id)


def remove_light_from_room(request, room_id: int, light_id: int):
    Light.objects.filter(id=light_id).delete()
    send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=room_id)


def add_light_to_room(request, room_id: int):
    room = Room.objects.filter(id=room_id).first()
    if int(request.POST["edit_light_id"]) >= 0:
        newLight = Light.objects.get(id=int(request.POST["edit_light_id"]))
    else:
        newLight = Light()
    newLight.room = room
    newLight.type = request.POST["add_new_light_type"]
    newLight.friendly_name = request.POST["add_new_light_name"]
    if request.POST["light_type"] == "ceiling":
        newLight.is_ceiling_light = True
    else:
        newLight.is_ceiling_light = False

    if newLight.type == "home_assistant":
        newLight.home_assistant_name = request.POST["home_assistant_name"]
    elif newLight.type == "openhab":
        newLight.openhab_name = request.POST["openhab_name"]

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

    if "add_to_room_view" in request.POST:
        if newLight.room_view_position == 0:
            all_lights = Light.objects.filter(
                room=room, room_view_position__gte=1, room_view_position__lte=12)
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
    send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=room_id)


def add_scene_to_room(request, room_id: int):
    room = Room.objects.filter(id=room_id).first()
    if request.POST["edit_scene_id"].strip() != "" and int(request.POST["edit_scene_id"]) >= 0:
        new_scene = Scene.objects.get(id=int(request.POST["edit_scene_id"]))
    else:
        new_scene = Scene()
        new_scene.scene_type = "nspm_scene"
    new_scene.friendly_name = request.POST["scene_name"]
    new_scene.room = room
    new_scene.save()
    send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=room_id)


def add_existing_scene_to_room(request, room_id: int):
    room = Room.objects.filter(id=room_id).first()
    new_scene = Scene()
    new_scene.friendly_name = request.POST["scene_name"]
    new_scene.backend_name = request.POST["scene_entity_name"]
    new_scene.scene_type = request.POST["scene_type"]
    new_scene.room = room
    new_scene.save()
    send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=room_id)


def delete_scene(request, scene_id: int):
    scene = Scene.objects.get(id=scene_id)
    if scene:
        scene.delete()
        send_mqttmanager_reload_command()
    return redirect('edit_room', room_id=scene.room.id)


def delete_global_scene(request, scene_id: int):
    scene = Scene.objects.get(id=scene_id)
    if scene:
        scene.delete()
        send_mqttmanager_reload_command()
    return redirect('global_scenes')


def add_scene_to_global(request):
    if request.POST["edit_scene_id"].strip() != "" and int(request.POST["edit_scene_id"]) >= 0:
        new_scene = Scene.objects.get(id=int(request.POST["edit_scene_id"]))
    else:
        new_scene = Scene()
        new_scene.scene_type = "nspm_scene"
    new_scene.friendly_name = request.POST["scene_name"]
    new_scene.room = None
    new_scene.save()
    send_mqttmanager_reload_command()
    return redirect('global_scenes')


def add_light_to_room_view(request, room_id: int):
    if "light_id" not in request.POST:
        return redirect('edit_room', room_id=room_id)
    room = Room.objects.filter(id=room_id).first()
    light_position = int(request.POST["position"])
    existing_light_at_position = Light.objects.filter(
        room=room, room_view_position=light_position).first()
    if existing_light_at_position != None:
        existing_light_at_position.room_view_position = 0
        existing_light_at_position.save()
    new_light = Light.objects.filter(id=int(request.POST["light_id"])).first()
    new_light.room_view_position = light_position
    new_light.save()
    return redirect('edit_room', room_id=room_id)


def remove_light_from_room_view(request, room_id: int):
    room = Room.objects.filter(id=room_id).first()
    light_position = int(request.POST["position"])
    existing_light_at_position = Light.objects.filter(
        room=room, room_view_position=light_position).first()
    if existing_light_at_position != None:
        existing_light_at_position.room_view_position = 0
        existing_light_at_position.save()
    return redirect('edit_room', room_id=room_id)


def settings_page(request):
    environment = environ.Env()

    data = {
        'dark_theme': get_setting_with_default("dark_theme"),
        'mqttmanager_log_level': get_setting_with_default("mqttmanager_log_level"),
    }
    data["color_temp_min"] = get_setting_with_default("color_temp_min")
    data["color_temp_max"] = get_setting_with_default("color_temp_max")
    data["reverse_color_temp"] = get_setting_with_default(
        "reverse_color_temp")
    data["mqtt_server"] = get_setting_with_default("mqtt_server")
    data["mqtt_port"] = get_setting_with_default("mqtt_port")
    data["mqtt_username"] = get_setting_with_default("mqtt_username")
    data["mqtt_password"] = get_setting_with_default("mqtt_password")
    data["home_assistant_address"] = get_setting_with_default("home_assistant_address")
    data["home_assistant_token"] = get_setting_with_default("home_assistant_token")
    data["openhab_address"] = get_setting_with_default("openhab_address")
    data["openhab_token"] = get_setting_with_default("openhab_token")
    data["openhab_brightness_channel_name"] = get_setting_with_default("openhab_brightness_channel_name")
    data["openhab_brightness_channel_min"] = get_setting_with_default("openhab_brightness_channel_min")
    data["openhab_brightness_channel_max"] = get_setting_with_default("openhab_brightness_channel_max")
    data["openhab_color_temp_channel_name"] = get_setting_with_default("openhab_color_temp_channel_name")
    data["openhab_rgb_channel_name"] = get_setting_with_default("openhab_rgb_channel_name")
    data["raise_to_100_light_level"] = get_setting_with_default("raise_to_100_light_level")
    data["min_button_push_time"] = get_setting_with_default("min_button_push_time")
    data["button_long_press_time"] = get_setting_with_default("button_long_press_time")
    data["special_mode_trigger_time"] = get_setting_with_default("special_mode_trigger_time")
    data["special_mode_release_time"] = get_setting_with_default("special_mode_release_time")
    data["mqtt_ignore_time"] = get_setting_with_default("mqtt_ignore_time")
    data["screensaver_activation_timeout"] = get_setting_with_default("screensaver_activation_timeout")
    data["screen_dim_level"] = get_setting_with_default("screen_dim_level")
    data["screensaver_dim_level"] = get_setting_with_default("screensaver_dim_level")
    data["screensaver_mode"] = get_setting_with_default("screensaver_mode")
    data["turn_on_behavior"] = get_setting_with_default("turn_on_behavior")
    data["max_live_log_messages"] = get_setting_with_default("max_live_log_messages")
    data["max_log_buffer_size"] = get_setting_with_default("max_log_buffer_size")
    data["is_home_assistant_addon"] = ("IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true")
    data["manager_address"] = get_setting_with_default("manager_address")
    data["manager_port"] = get_setting_with_default("manager_port")
    return render(request, 'settings.html', data)


def save_settings(request):
    set_setting_value(name="mqtt_server", value=request.POST["mqtt_server"])
    set_setting_value(name="mqtt_port", value=request.POST["mqtt_port"])
    set_setting_value(name="mqtt_username",
                      value=request.POST["mqtt_username"])
    set_setting_value(name="mqtt_password",
                      value=request.POST["mqtt_password"])
    if "home_assistant_address" in request.POST:
        home_assistant_address = request.POST["home_assistant_address"]
        if home_assistant_address.endswith("/"):
            home_assistant_address = home_assistant_address[:-1]
        set_setting_value(name="home_assistant_address",
                          value=home_assistant_address)
    if "home_assistant_token" in request.POST:
        set_setting_value(name="home_assistant_token",
                          value=request.POST["home_assistant_token"])
    openhab_address = request.POST["openhab_address"]
    if openhab_address.endswith("/"):
        openhab_address = openhab_address[:-1]
    set_setting_value(name="openhab_address", value=openhab_address)
    set_setting_value(name="openhab_token",
                      value=request.POST["openhab_token"])
    set_setting_value(name="raise_to_100_light_level",
                      value=request.POST["raise_to_100_light_level"])
    set_setting_value(name="color_temp_min",
                      value=request.POST["color_temp_min"])
    set_setting_value(name="color_temp_max",
                      value=request.POST["color_temp_max"])
    set_setting_value(name="reverse_color_temp",
                      value=request.POST["reverse_color_temp"])
    set_setting_value(name="min_button_push_time",
                      value=request.POST["min_button_push_time"])
    set_setting_value(name="button_long_press_time",
                      value=request.POST["button_long_press_time"])
    set_setting_value(name="special_mode_trigger_time",
                      value=request.POST["special_mode_trigger_time"])
    set_setting_value(name="special_mode_release_time",
                      value=request.POST["special_mode_release_time"])
    set_setting_value(name="mqtt_ignore_time",
                      value=request.POST["mqtt_ignore_time"])

    set_setting_value(name="screensaver_activation_timeout",
                      value=request.POST["screensaver_activation_timeout"])
    set_setting_value(name="screen_dim_level",
                      value=request.POST["screen_dim_level"])
    set_setting_value(name="screensaver_dim_level",
                      value=request.POST["screensaver_dim_level"])
    set_setting_value(name="screensaver_mode",
                      value=request.POST["screensaver_mode"])
    set_setting_value(name="turn_on_behavior",
                      value=request.POST["turn_on_behavior"])
    set_setting_value(name="max_live_log_messages",
                      value=request.POST["max_live_log_messages"])
    set_setting_value(name="max_log_buffer_size",
                      value=request.POST["max_log_buffer_size"])
    set_setting_value(name="mqttmanager_log_level",
                      value=request.POST["mqttmanager_log_level"])
    set_setting_value(name="manager_address",
                      value=request.POST["manager_address"])
    set_setting_value(name="manager_port", value=request.POST["manager_port"])
    # Settings saved, restart mqtt_manager
    restart_mqtt_manager()
    return redirect('settings')


@csrf_exempt
def initial_setup_manager_config(request):
    set_setting_value(name="manager_address",
                      value=request.POST["manager_address"])
    set_setting_value(name="manager_port", value=request.POST["manager_port"])
    restart_mqtt_manager()
    return HttpResponse('OK', 200)


@csrf_exempt
def initial_setup_mqtt_config(request):
    set_setting_value(name="mqtt_server", value=request.POST["mqtt_server"])
    set_setting_value(name="mqtt_port", value=request.POST["mqtt_port"])
    set_setting_value(name="mqtt_username",
                      value=request.POST["mqtt_username"])
    set_setting_value(name="mqtt_password",
                      value=request.POST["mqtt_password"])
    restart_mqtt_manager()
    return HttpResponse('OK', 200)


@csrf_exempt
def initial_setup_home_assistant_config(request):
    if "home_assistant_address" in request.POST:
        home_assistant_address = request.POST["home_assistant_address"]
        if home_assistant_address.endswith("/"):
            home_assistant_address = home_assistant_address[:-1]
        set_setting_value(name="home_assistant_address",
                          value=home_assistant_address)
    if "home_assistant_token" in request.POST:
        set_setting_value(name="home_assistant_token",
                          value=request.POST["home_assistant_token"])
    restart_mqtt_manager()
    return HttpResponse('OK', 200)


@csrf_exempt
def initial_setup_openhab_config(request):
    openhab_address = request.POST["openhab_address"]
    if openhab_address.endswith("/"):
        openhab_address = openhab_address[:-1]
    set_setting_value(name="openhab_address", value=openhab_address)
    set_setting_value(name="openhab_token",
                      value=request.POST["openhab_token"])
    restart_mqtt_manager()
    return HttpResponse('OK', 200)


@csrf_exempt
def save_new_firmware(request):
    if request.method == 'POST':
        uploaded_file = request.FILES['firmware']
        fs = FileSystemStorage()
        fs.delete("firmware.bin")
        fs.save("firmware.bin", uploaded_file)
    return redirect('/')


# TODO: Make exempt only when Debug = true
@csrf_exempt
def save_new_data_file(request):
    if request.method == 'POST':
        uploaded_file = request.FILES['data_upload_file_name']
        fs = FileSystemStorage()
        fs.delete("data_file.bin")
        fs.save("data_file.bin", uploaded_file)
    return redirect('/')


@csrf_exempt
def save_new_merged_flash(request):
    if request.method == 'POST':
        uploaded_file = request.FILES['bin']
        fs = FileSystemStorage()
        fs.delete("merged_flash.bin")
        fs.save("merged_flash.bin", uploaded_file)
    return redirect('/')
# TODO: Make exempt only when Debug = true


def get_client_ip(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[-1]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip


@csrf_exempt
def save_new_tft_file(request):
    if request.method == 'POST':
        if request.POST["file_version"] == "Europe":
            filename = "gui.tft"
        else:
            filename = "gui_us.tft"

        uploaded_file = request.FILES['tft_file']
        fs = FileSystemStorage()
        fs.delete(filename)
        fs.save(filename, uploaded_file)
    return redirect('/')


def download_firmware(request):
    fs = FileSystemStorage()
    if "Range" in request.headers and request.headers["Range"].startswith("bytes="):
        parts = request.headers["Range"][6:].split('-')
        range_start = int(parts[0])
        range_end = int(parts[1])
        if range_end == 255:  # Workaround for copy-paste error in firmware
            return HttpResponse(fs.open("firmware.bin").read(), content_type="application/octet-stream")
        data = fs.open("firmware.bin").read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open("firmware.bin").read(), content_type="application/octet-stream")


def download_data_file(request):
    fs = FileSystemStorage()
    if "Range" in request.headers and request.headers["Range"].startswith("bytes="):
        parts = request.headers["Range"][6:].split('-')
        range_start = int(parts[0])
        range_end = int(parts[1])
        data = fs.open("data_file.bin").read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open("data_file.bin").read(), content_type="application/octet-stream")


def download_tft(request):
    fs = FileSystemStorage()
    panel_ip = get_client_ip(request)
    nspanel = NSPanel.objects.filter(ip_address=panel_ip).first()
    if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "True":
        filename = "gui_us.tft"
    else:
        filename = "gui.tft"

    if "Range" in request.headers and request.headers["Range"].startswith("bytes="):
        parts = request.headers["Range"][6:].split('-')
        range_start = int(parts[0])
        range_end = int(parts[1])
        data = fs.open(filename).read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open(filename).read(), content_type="application/octet-stream")


def download_tft_eu(request):
    fs = FileSystemStorage()
    filename = "gui.tft"
    if "Range" in request.headers and request.headers["Range"].startswith("bytes="):
        parts = request.headers["Range"][6:].split('-')
        range_start = int(parts[0])
        range_end = int(parts[1])
        data = fs.open(filename).read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open(filename).read(), content_type="application/octet-stream")


def download_tft_us(request):
    fs = FileSystemStorage()
    filename = "gui_us.tft"
    if "Range" in request.headers and request.headers["Range"].startswith("bytes="):
        parts = request.headers["Range"][6:].split('-')
        range_start = int(parts[0])
        range_end = int(parts[1])
        data = fs.open(filename).read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open(filename).read(), content_type="application/octet-stream")


def checksum_firmware(request):
    return HttpResponse(get_file_md5sum("firmware.bin"))


def checksum_data_file(request):
    return HttpResponse(get_file_md5sum("data_file.bin"))


def checksum_tft_file(request):
    panel_ip = get_client_ip(request)
    nspanel = NSPanel.objects.filter(ip_address=panel_ip).first()
    if get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False") == "True":
        filename = "gui_us.tft"
    else:
        filename = "gui.tft"
    return HttpResponse(get_file_md5sum(filename))


def checksum_tft_file_eu(request):
    filename = "gui.tft"
    return HttpResponse(get_file_md5sum(filename))


def checksum_tft_file_us(request):
    filename = "gui_us.tft"
    return HttpResponse(get_file_md5sum(filename))


def get_manual(request):
    fs = FileSystemStorage()
    response = HttpResponse(fs.open("manual.pdf").read(),
                            content_type='application/pdf')
    response['Content-Disposition'] = 'attachment; filename="manual.pdf"'
    return response


def global_scenes(request):
    data = {'dark_theme': get_setting_with_default("dark_theme"), }
    data["global_scenes"] = Scene.objects.filter(room__isnull=True)
    return render(request, 'global_scenes.html', data)


def relay_groups(request):
    data = {'dark_theme': get_setting_with_default("dark_theme"), }
    data["nspanels"] = NSPanel.objects.all()
    data["relay_groups"] = RelayGroup.objects.all()
    return render(request, 'relay_groups.html', data)


def create_or_update_relay_group(request):
    if request.POST["relay_group_id"]:
        new_rg = RelayGroup.objects.get(id=request.POST["relay_group_id"])
    else:
        new_rg = RelayGroup()
    new_rg.friendly_name = request.POST['relay_group_name']
    new_rg.save()
    send_mqttmanager_reload_command()
    return redirect("relay_groups")


def delete_relay_group(request, relay_group_id):
    rg = RelayGroup.objects.get(id=relay_group_id)
    if rg:
        rg.delete()
    send_mqttmanager_reload_command()
    return redirect("relay_groups")


def add_nspanel_relay_to_group(request):
    rg = RelayGroup.objects.get(id=request.POST["relay_group_id"])
    nspanel = NSPanel.objects.get(id=request.POST["nspanel_id"])
    relay_num = request.POST["relay_num"]

    exists = RelayGroupBinding.objects.filter(
        nspanel=nspanel, relay_num=relay_num, relay_group=rg).count() > 0
    if not exists:
        binding = RelayGroupBinding()
        binding.relay_group = rg
        binding.nspanel = nspanel
        binding.relay_num = relay_num
        binding.save()
        send_mqttmanager_reload_command()
    return redirect("relay_groups")


def delete_relay_group_binding(request, relay_binding_id):
    binding = RelayGroupBinding.objects.get(id=relay_binding_id)
    if binding:
        binding.delete()
    send_mqttmanager_reload_command()
    return redirect("relay_groups")


def weather_and_time(request):
    if request.method == "POST":
        set_setting_value("location_latitude",
                          request.POST["location_latitude"])
        set_setting_value("location_longitude",
                          request.POST["location_longitude"])
        set_setting_value("wind_speed_format",
                          request.POST["wind_speed_format"])
        set_setting_value("precipitation_format",
                          request.POST["precipitation_format"])
        set_setting_value("outside_temp_sensor_provider",
                          request.POST["outside_temp_provider"])
        set_setting_value("outside_temp_sensor_entity_id",
                          request.POST["outside_temp_sensor"])
        set_setting_value("weather_update_interval",
                          request.POST["weather_update_interval"])
        set_setting_value("date_format", request.POST["date_format"])
        set_setting_value("clock_us_style", request.POST["clock_us_style"])
        set_setting_value("use_fahrenheit", request.POST["use_fahrenheit"])
        restart_mqtt_manager()
        return redirect("weather_and_time")
    else:
        data = {
            'dark_theme': get_setting_with_default("dark_theme"),
            'date_format': get_setting_with_default("date_format"),
            'clock_us_style': get_setting_with_default("clock_us_style"),
            'use_fahrenheit': get_setting_with_default("use_fahrenheit"),
            'outside_temp_provider': get_setting_with_default("outside_temp_sensor_provider"),
            'outside_temp_sensor': get_setting_with_default("outside_temp_sensor_entity_id"),
            'location_latitude': get_setting_with_default("location_latitude"),
            'location_longitude': get_setting_with_default("location_longitude"),
            'wind_speed_format': get_setting_with_default("wind_speed_format"),
            'precipitation_format': get_setting_with_default("precipitation_format"),
            'weather_update_interval': get_setting_with_default("weather_update_interval"),
        }

        return render(request, "weather_and_time.html", data)


def denied_nspanels(request):
    data = {
        'dark_theme': get_setting_with_default("dark_theme"),
        "nspanels": NSPanel.objects.all(),
    }
    return render(request, 'denied_nspanels.html', data)


def unblock_nspanel(request, nspanel_id):
    panel = NSPanel.objects.filter(id=nspanel_id).first()
    if panel:
        panel.delete()
        restart_mqtt_manager()
    return redirect("denied_nspanels")


def download_mqttmanager_log(request):
    if os.path.exists("/dev/shm/mqttmanager.log"):
        with open("/dev/shm/mqttmanager.log", 'rb') as fh:
            response = HttpResponse(
                fh.read(), content_type="application/vnd.ms-excel")
            response['Content-Disposition'] = 'inline; filename=' + \
                os.path.basename("mqttmanager_log.txt")
            return response
    raise Http404
