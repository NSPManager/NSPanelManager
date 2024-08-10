from re import A
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.core.files.storage import FileSystemStorage

import environ
import hashlib
import logging
import json
from .mqttmanager_ipc import send_ipc_request

from web.protobuf import mqttmanager_pb2

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

            proto = mqttmanager_pb2.MQTTManagerSettings()
            proto.color_temp_min = int(get_setting_with_default("color_temp_min"))
            proto.color_temp_max = int(get_setting_with_default("color_temp_max"))
            proto.date_format = get_setting_with_default("date_format")
            proto.outside_temp_sensor_provider = get_setting_with_default("outside_temp_sensor_provider")
            proto.outside_temp_sensor_entity_id = get_setting_with_default("outside_temp_sensor_entity_id")
            proto.weather_location_latitude = get_setting_with_default("weather_location_latitude")
            proto.weather_location_longitude = get_setting_with_default("weather_location_longitude")
            proto.max_log_buffer_size = int(get_setting_with_default("max_log_buffer_size"))
            proto.manager_port = int(get_setting_with_default("manager_port"))
            proto.manager_address = get_setting_with_default("manager_address")
            proto.is_home_assistant_addon = "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true"

            wind_speed_unit = get_setting_with_default("wind_speed_format")
            if wind_speed_unit == "kmh":
                proto.weather_wind_speed_format = mqttmanager_pb2.MQTTManagerSettings.wind_speed_format.KM_H;
            elif wind_speed_unit == "ms":
                proto.weather_wind_speed_format = mqttmanager_pb2.MQTTManagerSettings.wind_speed_format.M_S;
            elif wind_speed_unit == "mph":
                proto.weather_wind_speed_format = mqttmanager_pb2.MQTTManagerSettings.wind_speed_format.MP_H;
            elif wind_speed_unit == "kn":
                proto.weather_wind_speed_format = mqttmanager_pb2.MQTTManagerSettings.wind_speed_format.KNOTS;
            else:
                proto.weather_wind_speed_format = mqttmanager_pb2.MQTTManagerSettings.wind_speed_format.KM_H;

            if get_setting_with_default("precipitation_format") == "inch":
                proto.weather_precipitation_format = mqttmanager_pb2.MQTTManagerSettings.precipitation_unit.INCES;
            else:
                proto.weather_precipitation_format = mqttmanager_pb2.MQTTManagerSettings.precipitation_unit.MILLIMETERS;

            if get_setting_with_default("clock_us_style") == "True":
                proto.clock_format = mqttmanager_pb2.MQTTManagerSettings.time_format.AM_PM;
            else:
                proto.clock_format = mqttmanager_pb2.MQTTManagerSettings.time_format.FULL;

            if get_setting_with_default("temperature_format") == "True":
                proto.temperature_format = mqttmanager_pb2.MQTTManagerSettings.temperature_unit.FAHRENHEIT;
            else:
                proto.temperature_format = mqttmanager_pb2.MQTTManagerSettings.temperature_unit.CELCIUS;

            return HttpResponse(proto.SerializeToString(), status=200)
        else:
            return JsonResponse({"status": "error"}, status=405)
    except Exception as ex:
        logging.exception(ex)
        return JsonResponse({"status": "error"}, status=500)
