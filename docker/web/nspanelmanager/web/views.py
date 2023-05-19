from django.shortcuts import render, redirect, HttpResponse
from django.core.files.storage import FileSystemStorage
from django.views.decorators.csrf import csrf_exempt

from ranged_response import RangedFileResponse

import hashlib
import psutil
import subprocess

from .models import NSPanel, Room, Light, Settings
from web.settings_helper import get_setting_with_default, set_setting_value


def restart_mqtt_manager():
    for proc in psutil.process_iter():
        if "./mqtt_manager.py" in proc.cmdline():
            print("Killing existing mqtt_manager")
            proc.kill()
    # Restart the process
    print("Starting a new mqtt_manager")
    subprocess.Popen(
        ["/usr/local/bin/python", "./mqtt_manager.py"], cwd="/usr/src/app/")


def index(request):
    print(len(Room.objects.all()))
    return render(request, 'index.html', {'nspanels': NSPanel.objects.all()})


def rooms(request):
    return render(request, 'rooms.html', {'rooms': Room.objects.all().order_by('displayOrder')})


def rooms_order(request):
    return render(request, 'rooms_order.html', {'rooms': Room.objects.all().order_by('displayOrder')})


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
    room = Room.objects.filter(id=room_id).first()
    data = {
        'room': room,
        'light1': Light.objects.filter(room=room, room_view_position=1).first(),
        'light2': Light.objects.filter(room=room, room_view_position=2).first(),
        'light3': Light.objects.filter(room=room, room_view_position=3).first(),
        'light4': Light.objects.filter(room=room, room_view_position=4).first(),
        'light5': Light.objects.filter(room=room, room_view_position=5).first(),
        'light6': Light.objects.filter(room=room, room_view_position=6).first(),
        'light7': Light.objects.filter(room=room, room_view_position=7).first(),
        'light8': Light.objects.filter(room=room, room_view_position=8).first(),
        'light9': Light.objects.filter(room=room, room_view_position=9).first(),
        'light10': Light.objects.filter(room=room, room_view_position=10).first(),
        'light11': Light.objects.filter(room=room, room_view_position=11).first(),
        'light12': Light.objects.filter(room=room, room_view_position=12).first(),
    }
    return render(request, 'edit_room.html', data)


def save_new_room(request):
    new_room = Room()
    new_room.friendly_name = request.POST['friendly_name']
    new_room.save()
    return redirect('edit_room', room_id=new_room.id)


def delete_room(request, room_id: int):
    Room.objects.filter(id=room_id).delete()
    return redirect('rooms')


def update_room_form(request, room_id: int):
    room = Room.objects.filter(id=room_id).first()
    room.friendly_name = request.POST['friendly_name']
    room.save()
    return redirect('edit_room', room_id=room_id)


def edit_nspanel(request, panel_id: int):
    return render(request, 'edit_nspanel.html', {
        'panel': NSPanel.objects.get(id=panel_id),
        'rooms': Room.objects.all()
    })


def save_panel_settings(request, panel_id: int):
    panel = NSPanel.objects.get(id=panel_id)
    panel.room = Room.objects.get(id=request.POST["room_id"])
    panel.button1_mode = request.POST["button1_mode"]
    if request.POST["button1_mode"] == "1":
        panel.button1_detached_mode_light = Light.objects.get(id=request.POST["button1_detached_mode_light"])
    else:
        panel.button1_detached_mode_light = None
    panel.button2_mode = request.POST["button2_mode"]
    if request.POST["button2_mode"] == "1":
        panel.button2_detached_mode_light = Light.objects.get(id=request.POST["button2_detached_mode_light"])
    else:
        panel.button2_detached_mode_light = None
    panel.save()
    return redirect('edit_nspanel', panel_id)


def remove_light_from_room(request, room_id: int, light_id: int):
    Light.objects.filter(id=light_id).delete()
    restart_mqtt_manager()
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

    if newLight.room_view_position == 0:
        for i in range(1, 13):
            if not Light.objects.filter(room=room, room_view_position=i).exists():
                newLight.room_view_position = i
                break

    newLight.save()
    restart_mqtt_manager()
    return redirect('edit_room', room_id=room_id)


def add_light_to_room_view(request, room_id: int):
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
    data = {}
    data["color_temp_min"] = get_setting_with_default("color_temp_min", 2000)
    data["color_temp_max"] = get_setting_with_default("color_temp_max", 6000)
    data["reverse_color_temp"] = get_setting_with_default(
        "reverse_color_temp", False)
    data["mqtt_server"] = get_setting_with_default("mqtt_server", "")
    data["mqtt_port"] = get_setting_with_default("mqtt_port", 1883)
    data["mqtt_username"] = get_setting_with_default("mqtt_username", "")
    data["mqtt_password"] = get_setting_with_default("mqtt_password", "")
    data["home_assistant_address"] = get_setting_with_default(
        "home_assistant_address", "")
    data["home_assistant_token"] = get_setting_with_default(
        "home_assistant_token", "")
    data["openhab_address"] = get_setting_with_default("openhab_address", "")
    data["openhab_token"] = get_setting_with_default("openhab_token", "")
    data["openhab_brightness_channel_name"] = get_setting_with_default(
        "openhab_brightness_channel_name", "")
    data["openhab_brightness_channel_min"] = get_setting_with_default(
        "openhab_brightness_channel_min", 0)
    data["openhab_brightness_channel_max"] = get_setting_with_default(
        "openhab_brightness_channel_max", 255)
    data["openhab_color_temp_channel_name"] = get_setting_with_default(
        "openhab_color_temp_channel_name", "")
    data["openhab_rgb_channel_name"] = get_setting_with_default(
        "openhab_rgb_channel_name", "")
    data["raise_to_100_light_level"] = get_setting_with_default(
        "raise_to_100_light_level", 95)
    data["min_button_push_time"] = get_setting_with_default("min_button_push_time", 50)
    data["button_long_press_time"] = get_setting_with_default("button_long_press_time", 300)
    data["special_mode_trigger_time"] = get_setting_with_default("special_mode_trigger_time", 300)
    data["special_mode_release_time"] = get_setting_with_default("special_mode_release_time", 5000)
    data["mqtt_ignore_time"] = get_setting_with_default("mqtt_ignore_time", 3000)
    return render(request, 'settings.html', data)


def save_settings(request):
    set_setting_value(name="mqtt_server", value=request.POST["mqtt_server"])
    set_setting_value(name="mqtt_port", value=request.POST["mqtt_port"])
    set_setting_value(name="mqtt_username",
                      value=request.POST["mqtt_username"])
    set_setting_value(name="mqtt_password",
                      value=request.POST["mqtt_password"])
    set_setting_value(name="home_assistant_address",
                      value=request.POST["home_assistant_address"])
    set_setting_value(name="home_assistant_token",
                      value=request.POST["home_assistant_token"])
    set_setting_value(name="openhab_address",
                      value=request.POST["openhab_address"])
    set_setting_value(name="openhab_token",
                      value=request.POST["openhab_token"])
    set_setting_value(name="raise_to_100_light_level",
                      value=request.POST["raise_to_100_light_level"])
    set_setting_value(name="color_temp_min",
                      value=request.POST["color_temp_min"])
    set_setting_value(name="color_temp_max",
                      value=request.POST["color_temp_max"])
    set_setting_value(name="reverse_color_temp", value=(
        "reverse_color_temp" in request.POST))
    set_setting_value(name="min_button_push_time", value=request.POST["min_button_push_time"])
    set_setting_value(name="button_long_press_time", value=request.POST["button_long_press_time"])
    set_setting_value(name="special_mode_trigger_time", value=request.POST["special_mode_trigger_time"])
    set_setting_value(name="special_mode_release_time", value=request.POST["special_mode_release_time"])
    set_setting_value(name="mqtt_ignore_time", value=request.POST["mqtt_ignore_time"])

    # Settings saved, restart mqtt_manager
    restart_mqtt_manager()
    return redirect('settings')

    # TODO: Make exempt only when Debug = true


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
        uploaded_file = request.FILES['data_file']
        fs = FileSystemStorage()
        fs.delete("data_file.bin")
        fs.save("data_file.bin", uploaded_file)
    return redirect('/')

# TODO: Make exempt only when Debug = true


@csrf_exempt
def save_new_tft_file(request):
    if request.method == 'POST':
        uploaded_file = request.FILES['tft_file']
        fs = FileSystemStorage()
        fs.delete("gui.tft")
        fs.save("gui.tft", uploaded_file)
        print("Saved new GUI tft file.")
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
        if range_end == 255:  # Workaround for copy-paste error in firmware
            return HttpResponse(fs.open("data_file.bin").read(), content_type="application/octet-stream")
        data = fs.open("data_file.bin").read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open("data_file.bin").read(), content_type="application/octet-stream")


def download_tft(request):
    fs = FileSystemStorage()
    if "Range" in request.headers and request.headers["Range"].startswith("bytes="):
        parts = request.headers["Range"][6:].split('-')
        range_start = int(parts[0])
        range_end = int(parts[1])
        data = fs.open("gui.tft").read()
        return HttpResponse(data[range_start:range_end], content_type="application/octet-stream")
    else:
        return HttpResponse(fs.open("gui.tft").read(), content_type="application/octet-stream")


def checksum_firmware(request):
    fs = FileSystemStorage()
    return HttpResponse(hashlib.md5(fs.open("firmware.bin").read()).hexdigest())


def checksum_data_file(request):
    fs = FileSystemStorage()
    return HttpResponse(hashlib.md5(fs.open("data_file.bin").read()).hexdigest())
