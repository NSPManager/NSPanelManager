from .models import Settings, NSPanelSettings, NSPanel
import logging

default_settings = {
    "button_long_press_time": 5000,
    "clock_us_style": False,
    "color_temp_max": 6000,
    "color_temp_min": 2000,
    "dark_theme": False,
    "date_format": "%a %d/%m %Y",
    "home_assistant_address": "",
    "home_assistant_token": "",
    "location_latitude": "",
    "location_longitude": "",
    "manager_address": "",
    "manager_port": "",
    "max_live_log_messages": 10,
    "max_log_buffer_size": "10",
    "min_button_push_time": 50,
    "mqttmanager_log_level": "debug",
    "mqtt_password": "",
    "mqtt_port": 1883,
    "mqtt_server": "",
    "mqtt_username": "",
    "openhab_address": "",
    "openhab_token": "",
    "openhab_brightness_channel_max": 255,
    "openhab_brightness_channel_min": 0,
    "openhab_brightness_channel_name": "",
    "openhab_color_temp_channel_name": "",
    "openhab_rgb_channel_name": "",
    "outside_temp_sensor_entity_id": "",
    "outside_temp_sensor_provider": "",
    "weather_precipitation_format": "mm",
    "raise_to_100_light_level": 95,
    "reverse_color_temp": False,
    "screen_dim_level": 100,
    "screensaver_activation_timeout": 30000,
    "screensaver_dim_level": 0,
    "screensaver_mode": "with_background",
    "special_mode_release_time": 5000,
    "special_mode_trigger_time": 300,
    "turn_on_behavior": "color_temp",
    "use_fahrenheit": False,
    "weather_update_interval": 10,
    "weather_wind_speed_format": "kmh",
    "weather_location_latitude": "",
    "weather_location_longitude": "",
}

def get_setting_with_default(name):
    objects = Settings.objects.filter(name=name)
    if objects.count() > 0:
        return objects.first().value
    elif name in default_settings:
        return default_settings[name]
    else:
        logging.error(F"Failed to get default setting for key '{name}'. No default value for setting exists.")
        return None

def does_setting_exist(name):
    objects = Settings.objects.filter(name=name)
    return objects.count() > 0

def set_setting_value(name, value):
    Settings.objects.update_or_create(name=name, defaults={
        "value": value
    })


def get_nspanel_setting_with_default(nspanel_id, name, default):
    panel = NSPanel.objects.filter(id=nspanel_id)
    if panel.count() > 0:
        objects = NSPanelSettings.objects.filter(nspanel=panel.first(), name=name)
        if objects.count() > 0:
            return objects.first().value
        else:
            return default
    else:
        return default


def delete_nspanel_setting(nspanel_id, name):
    panel = NSPanel.objects.filter(id=nspanel_id)
    if panel.count() > 0:
        objects = NSPanelSettings.objects.filter(nspanel=panel.first(), name=name)
        if objects.count() > 0:
            return objects.first().delete()


def set_nspanel_setting_value(nspanel_id, name, value):
    panel = NSPanel.objects.filter(id=nspanel_id)
    if panel.count() > 0:
        NSPanelSettings.objects.update_or_create(nspanel=panel.first(), name=name, defaults={
            "value": value
        })
