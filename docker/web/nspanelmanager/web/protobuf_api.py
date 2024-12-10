from re import A
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.core.files.storage import FileSystemStorage

import environ
import hashlib
import logging
import json
from .mqttmanager_ipc import send_ipc_request

from web.protobuf import protobuf_formats_pb2, protobuf_general_pb2, protobuf_mqttmanager_pb2

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

def mqttmanager_get_all_settings(request):
    try:
        if request.method == "GET":
            environment = environ.Env()

            proto = protobuf_mqttmanager_pb2.MQTTManagerSettings()
            proto.color_temp_min = int(get_setting_with_default("color_temp_min"))
            proto.color_temp_max = int(get_setting_with_default("color_temp_max"))
            proto.reverse_color_temperature_slider = get_setting_with_default("reverse_color_temp") == "True"
            proto.date_format = get_setting_with_default("date_format")
            proto.outside_temp_sensor_provider = get_setting_with_default("outside_temp_sensor_provider")
            proto.outside_temp_sensor_entity_id = get_setting_with_default("outside_temp_sensor_entity_id")
            proto.weather_location_latitude = get_setting_with_default("location_latitude")
            proto.weather_location_longitude = get_setting_with_default("location_longitude")
            proto.weather_update_interval_minutes = int(get_setting_with_default("weather_update_interval"))
            proto.max_log_buffer_size = int(get_setting_with_default("max_log_buffer_size"))
            proto.manager_port = int(get_setting_with_default("manager_port"))
            proto.manager_address = get_setting_with_default("manager_address")
            proto.is_home_assistant_addon = "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true"
            proto.optimistic_mode = bool(get_setting_with_default("optimistic_mode"))
            proto.mqtt_wait_time = int(get_setting_with_default("mqtt_wait_time"))

            wind_speed_unit = get_setting_with_default("wind_speed_format")
            if wind_speed_unit == "kmh":
                proto.weather_wind_speed_format = protobuf_formats_pb2.wind_speed_format.KM_H
            elif wind_speed_unit == "ms":
                proto.weather_wind_speed_format = protobuf_formats_pb2.wind_speed_format.M_S
            elif wind_speed_unit == "mph":
                proto.weather_wind_speed_format = protobuf_formats_pb2.wind_speed_format.MP_H
            elif wind_speed_unit == "kn":
                proto.weather_wind_speed_format = protobuf_formats_pb2.wind_speed_format.KNOTS
            else:
                proto.weather_wind_speed_format = protobuf_formats_pb2.wind_speed_format.KM_H

            if get_setting_with_default("precipitation_format") == "inch":
                proto.weather_precipitation_unit = protobuf_formats_pb2.precipitation_format.INCHES
            else:
                proto.weather_precipitation_unit = protobuf_formats_pb2.precipitation_format.MILLIMETERS

            if get_setting_with_default("clock_us_style") == "True":
                proto.clock_format = protobuf_formats_pb2.time_format.AM_PM
            else:
                proto.clock_format = protobuf_formats_pb2.time_format.FULL

            if get_setting_with_default("use_fahrenheit") == "True":
                proto.temperature_unit = protobuf_formats_pb2.temperature_format.FAHRENHEIT
            else:
                proto.temperature_unit = protobuf_formats_pb2.temperature_format.CELCIUS

            return HttpResponse(proto.SerializeToString(), status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)

def mqttmanager_get_all_nspanels(request):
    try:
        if request.method == "GET":
            proto = protobuf_mqttmanager_pb2.MultipleNSPanelsSettings()
            for nspanel in NSPanel.objects.all():
                proto_panel = proto.nspanels.add()
                proto_panel.id = nspanel.id
                proto_panel.default_room = nspanel.room.id
                proto_panel.mac_address = nspanel.mac_address
                proto_panel.name = nspanel.friendly_name
                proto_panel.home = nspanel.room.id
                proto_panel.default_page = int(get_nspanel_setting_with_default(nspanel.id, "default_page", 0))
                proto_panel.raise_to_100_light_level = int(get_setting_with_default("raise_to_100_light_level"))
                proto_panel.min_button_push_time = int(get_setting_with_default("min_button_push_time"))
                proto_panel.button_long_press_time = int(get_setting_with_default("button_long_press_time"))
                proto_panel.special_mode_trigger_time = int(get_setting_with_default("special_mode_trigger_time"))
                proto_panel.special_mode_release_time = int(get_setting_with_default("special_mode_release_time"))
                proto_panel.mqtt_ignore_time = int(get_nspanel_setting_with_default(nspanel.id, "mqtt_ignore_time", 3000))
                proto_panel.screen_dim_level = int(get_nspanel_setting_with_default(nspanel.id, "screen_dim_level", get_setting_with_default("screen_dim_level")))
                proto_panel.screensaver_dim_level = int(get_nspanel_setting_with_default(nspanel.id, "screensaver_dim_level", get_setting_with_default("screensaver_dim_level")))
                proto_panel.is_us_panel = bool(get_nspanel_setting_with_default(nspanel.id, "is_us_panel", False))
                proto_panel.lock_to_default_room = bool(get_nspanel_setting_with_default(nspanel.id, "lock_to_default_room", False))
                proto_panel.reverse_relays = bool(get_nspanel_setting_with_default(nspanel.id, "reverse_relays", False))
                proto_panel.relay1_default_mode = bool(get_nspanel_setting_with_default(nspanel.id, "relay1_default_mode", False))
                proto_panel.relay2_default_mode = bool(get_nspanel_setting_with_default(nspanel.id, "relay2_default_mode", False))
                proto_panel.denied = nspanel.denied
                proto_panel.accepted = nspanel.accepted
                for room in Room.objects.all().order_by('displayOrder'):
                    proto_panel.rooms.append(room.id)
                for scene in Scene.objects.all():
                    proto_panel.scenes.append(scene.id)

                screensaver_mode = get_nspanel_setting_with_default(nspanel.id, "screensaver_mode", get_setting_with_default("screensaver_mode"))
                if screensaver_mode == "with_background":
                    proto_panel.screensaver_mode = protobuf_mqttmanager_pb2.NSPanelSettings.WEATHER_WITH_BACKGROUND
                elif screensaver_mode == "without_background":
                    proto_panel.screensaver_mode = protobuf_mqttmanager_pb2.NSPanelSettings.WEATHER_WITHOUT_BACKGROUND
                elif screensaver_mode == "datetime_with_background":
                    proto_panel.screensaver_mode = protobuf_mqttmanager_pb2.NSPanelSettings.DATETIME_WITH_BACKGROUND
                elif screensaver_mode == "datetime_without_background":
                    proto_panel.screensaver_mode = protobuf_mqttmanager_pb2.NSPanelSettings.DATETIME_WITHOUT_BACKGROUND
                elif screensaver_mode == "no_screensaver":
                    proto_panel.screensaver_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NO_SCREENSAVER

                if bool(get_setting_with_default("clock_us_style")):
                    proto_panel.clock_format = protobuf_formats_pb2.time_format.AM_PM
                else:
                    proto_panel.clock_format = protobuf_formats_pb2.time_format.FULL

                if bool(get_setting_with_default("use_fahrenheit")):
                    proto_panel.temperature_unit = protobuf_formats_pb2.temperature_format.FAHRENHEIT
                else:
                    proto_panel.temperature_unit = protobuf_formats_pb2.temperature_format.CELSIUS

                if nspanel.button1_mode == 0:
                    proto_panel.button1_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.DIRECT_MODE
                elif nspanel.button1_mode == 1:
                    proto_panel.button1_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.DETACHED_MODE
                    proto_panel.detached_light_id = nspanel.button1_detached_mode_light.id
                elif nspanel.button1_mode == 2:
                    proto_panel.button1_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.CUSTOM_MQTT
                    proto_panel.button1_mqtt_topic = get_nspanel_setting_with_default(nspanel.id, "button1_mqtt_topic", "")
                    proto_panel.button1_mqtt_payload = get_nspanel_setting_with_default(nspanel.id, "button1_mqtt_payload", "")
                elif nspanel.button1_mode == 3:
                    proto_panel.button1_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.FOLLOW_MODE

                if nspanel.button2_mode == 0:
                    proto_panel.button2_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.DIRECT_MODE
                elif nspanel.button2_mode == 1:
                    proto_panel.button2_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.DETACHED_MODE
                    proto_panel.detached_light_id = nspanel.button2_detached_mode_light.id
                elif nspanel.button2_mode == 2:
                    proto_panel.button2_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.CUSTOM_MQTT
                    proto_panel.button2_mqtt_topic = get_nspanel_setting_with_default(nspanel.id, "button2_mqtt_topic", "")
                    proto_panel.button2_mqtt_payload = get_nspanel_setting_with_default(nspanel.id, "button2_mqtt_payload", "")
                elif nspanel.button2_mode == 3:
                    proto_panel.button2_mode = protobuf_mqttmanager_pb2.NSPanelSettings.NSPanelButtonMode.FOLLOW_MODE

            return HttpResponse(proto.SerializeToString(), status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)

def mqttmanager_get_all_lights(request):
    try:
        if request.method == "GET":
            proto = protobuf_general_pb2.MultipleLightsSettings()
            for light in Light.objects.all():
                proto.lights.extend([light.get_protobuf_object()])

            return HttpResponse(proto.SerializeToString(), status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)

def mqttmanager_get_all_rooms(request):
    try:
        if request.method == "GET":
            proto = protobuf_general_pb2.MultipleRoomsSettings()
            for room in Room.objects.all():
                proto.rooms.extend([room.get_protobuf_object()])

            return HttpResponse(proto.SerializeToString(), status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
