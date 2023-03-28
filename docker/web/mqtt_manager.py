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
import mqtt_manager_libs.light_states

settings = {}
last_settings_file_mtime = 0
client = mqtt.Client("NSPanelManager")


def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Server")
    # Listen for all events sent to and from panels to control states
    client.subscribe("nspanel/mqttmanager/command")
    client.subscribe("nspanel/+/log")
    client.subscribe("nspanel/+/status")
    client.subscribe("nspanel/+/status_report")


def on_message(client, userdata, msg):
    try:
        parts = msg.topic.split('/')
        if parts[-1] == "log": # Messages received was a status update (online/offline)
            message_parts = msg.payload.decode('utf-8').split(':')
            data = {
                "type": "log",
                "time": datetime.datetime.now().strftime("%H:%M:%S"),
                "panel": parts[1],
                "level": message_parts[0],
                "message": ':'.join(message_parts[1:])
            }
            mqtt_manager_libs.websocket_server.send_message(json.dumps(data))
        elif parts[-1] == "status":
            panel = parts[1]
            if msg.payload.decode('utf-8') == "online":
                data = {
                    "state": "online"
                }
            else:
                {
                    "state": "offline"
                }
        elif parts[-1] == "status_report":
            panel = parts[1]
            data = json.loads(msg.payload.decode('utf-8'))
            data["state"] = "online"
            send_status_report(panel, data)
        elif msg.topic == "nspanel/mqttmanager/command":
            data = json.loads(msg.payload.decode('utf-8'))
            if data["method"] == "set" and data["attribute"] == "brightness": # Got new brightness value
                for entity_id in data["entity_ids"]:
                    if mqtt_manager_libs.light_states.states[entity_id]["type"] == "home_assistant":
                        mqtt_manager_libs.home_assistant.set_entity_brightness(entity_id, data["brightness"])
                    elif mqtt_manager_libs.light_states.states[entity_id]["type"] == "openhab":
                        mqtt_manager_libs.openhab.set_entity_brightness(entity_id, data["brightness"])
                    sleep(20/1000) # Wait 20ms between each light event
            elif data["method"] == "set" and data["attribute"] == "kelvin": # Got new brightness value
                for entity_id in data["entity_ids"]:
                    if mqtt_manager_libs.light_states.states[entity_id]["type"] == "home_assistant":
                        mqtt_manager_libs.home_assistant.set_entity_color_temp(entity_id, data["kelvin"])
                    elif mqtt_manager_libs.light_states.states[entity_id]["type"] == "openhab":
                        mqtt_manager_libs.openhab.set_entity_color_temp(entity_id, data["kelvin"])

    except:
        print("Something went wrong during processing of message:")
        try:
            print(msg.payload.decode('utf-8'))
        except:
            print("Something went wrong when processing the exception message, couldn't decode payload to utf-8.")

def send_status_report(panel, new_status):
    post("http://127.0.0.1:8000/api/set_panel_status/" + new_status["mac"] + "/", json=new_status)

def get_config():
    global settings
    while True:
        try:
            config_request = get(
                "http://127.0.0.1:8000/api/get_mqtt_manager_config", timeout=5)
            if config_request.status_code == 200:
                print("Got config, will start MQTT Manager.")
                settings = config_request.json()

                for id, light in settings["lights"].items():
                    int_id = int(id)
                    mqtt_manager_libs.light_states.states[int_id] = light
                    # Set default values
                    mqtt_manager_libs.light_states.states[int_id]["color_temp"] = 3000
                    mqtt_manager_libs.light_states.states[int_id]["brightness"] = 0
                # All light-data sucessfully loaded into light_states, clear own register
                settings.pop("lights")
                break
        except Exception as e:
            print("ERROR: Failed to get config. Will try again in 5 seconds.")
            print(e)
            sleep(5)


def connect_and_loop():
    global settings, home_assistant
    mqtt_manager_libs.websocket_server.start_server() # Start websocket server
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
    
    # Loop MQTT
    client.loop_forever()


if __name__ == '__main__':
    get_config()
    if settings["mqtt_server"] and settings["mqtt_port"]:
        connect_and_loop()
    else:
        print(
            "Settings dictate to NOT use MQTT Manager as no MQTT configuration is present.")
