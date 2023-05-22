import logging

settings = {}

def init(settings_from_manager, mqtt_client_from_manager):
    global openhab_url, openhab_token, settings, mqtt_client
    settings = settings_from_manager
    mqtt_client = mqtt_client_from_manager

def save_scene():
    pass
