#!/usr/bin/env python
import os
import paho.mqtt.client as mqtt
from requests import get, post
from time import sleep
import datetime
import json
import mqtt_manager_libs.home_assistant
import mqtt_manager_libs.openhab
import mqtt_manager_libs.websocket_server

settings = {}
last_settings_file_mtime = 0
client = mqtt.Client("NSPanelManager")

def read_settings():
    global settings
    with open("nspanelmanager/mqtt_manager.json", "r") as f:
        settings = json.loads(f.read())


def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Server")
    # Listen for all events sent to and from panels to control states
    client.subscribe("nspanel/entities/#")
    client.subscribe("nspanel/+/log")


def on_message(client, userdata, msg):
    parts = msg.topic.split('/')
    if parts[len(parts)-1] == "log": # Messages received was a status update (online/offline)
        message_parts = msg.payload.decode('utf-8').split(':')
        data = {
            "type": "log",
            "time": datetime.datetime.now().strftime("%H:%M:%S"),
            "panel": parts[1],
            "level": message_parts[0],
            "message": ':'.join(message_parts[1:])
        }
        mqtt_manager_libs.websocket_server.send_message(json.dumps(data))
    elif len(parts) >= 5:
        domain = parts[2]
        entity_id = parts[3]
        attribute = parts[4]
        if attribute.startswith("state_"):
            return

        if domain == "light":
            for light in settings["lights"]:
                if light["name"] == entity_id:
                    if light["type"] == "home_assistant":
                        mqtt_manager_libs.home_assistant.set_light_attribute(entity_id, attribute, msg.payload.decode('utf-8'))
                    elif light["type"] == "openhab":
                        mqtt_manager_libs.openhab.set_light_attribute(entity_id, attribute, msg.payload.decode('utf-8'))

def get_config():
    global settings
    while True:
        try:
            config_request = get(
                "http://127.0.0.1:8000/api/get_mqtt_manager_config", timeout=5)
            if config_request.status_code == 200:
                print("Got config, will start MQTT Manager.")
                settings = config_request.json()
                break
        except:
            print("ERROR: Failed to get config. Will try again in 5 seconds.")
            sleep(5)


def connect_and_loop():
    global settings, home_assistant
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(settings["mqtt_username"], settings["mqtt_password"])
    # Wait for connection
    connection_return_code = 0
    mqtt_server = settings["mqtt_server"]
    mqtt_port = int(settings["mqtt_port"])
    print(F"Connecting to {mqtt_server}:{mqtt_port}")
    while True:
        try:
            client.connect(mqtt_server, mqtt_port, 5)
            break  # Connection call did not raise exception, connection is sucessfull
        except:
            print(
                F"Failed to connect to MQTT {mqtt_server}:{mqtt_port}. Will try again in 10 seconds. Code: {connection_return_code}")
            sleep(10)
    
    # MQTT Connected, start APIs if configured
    if settings["home_assistant_address"] != "" and settings["home_assistant_token"] != "":
        mqtt_manager_libs.home_assistant.init(settings, client)
        mqtt_manager_libs.home_assistant.connect()
    else:
        print("Home Assistant values not configured, will not connect.")
    

    if settings["openhab_address"] != "" and settings["openhab_token"] != "":
        mqtt_manager_libs.openhab.init(settings, client)
        mqtt_manager_libs.openhab.connect()
    else:
        print("Home Assistant values not configured, will not connect.")
    
    mqtt_manager_libs.websocket_server.start_server()
    
    # Loop MQTT
    client.loop_forever()


if __name__ == '__main__':
    get_config()
    if settings["mqtt_server"] and settings["mqtt_port"]:
        connect_and_loop()
    else:
        print(
            "Settings dictate to NOT use MQTT Manager as no MQTT configuration is present.")
