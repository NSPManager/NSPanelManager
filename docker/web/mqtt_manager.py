#!/usr/bin/env python
import os
import paho.mqtt.client as mqtt
from requests import get, post
from time import sleep
import json

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


def on_message(client, userdata, msg):
    parts = msg.topic.split('/')
    if len(parts) >= 5:
        domain = parts[2]
        entity_id = parts[3]
        attribute = parts[4]
        if attribute.startswith("state_"):
            return

        service = ""
        if domain == "light":
            service = "turn_on"

        setHomeassistantState(domain=domain, service=service, entity_id=entity_id, attribute=attribute, state=msg.payload.decode('utf-8'))


def setHomeassistantState(domain, service, entity_id, attribute, state):
    global settings
    url = settings["home_assistant_address"] + "/api/services/" + domain + "/" + service

    headers = {
        "Authorization": "Bearer " + settings["home_assistant_token"],
        "content-type": "application/json",
    }
    body = {
        "entity_id": F"light.{entity_id}",
        attribute: state
    }
    response = post(url, headers=headers, json=body)
    if response.status_code == 200:
        send_new_status(domain, service, entity_id, attribute, state)
    else:
        print(F"ERROR: Setting {entity_id}.{attribute} = {state} reeturned code: {response.status_code}")


def send_new_status(domain, service, entity_id, attribute, state):
    client.publish(F"nspanel/entities/{domain}/{entity_id}/state_{attribute}", state, retain=True)


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
    global settings
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(
        settings["mqtt_username"], settings["mqtt_password"])
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
    client.loop_forever()


if __name__ == '__main__':
    get_config()
    if settings["mqtt_server"] and settings["mqtt_port"]:
        connect_and_loop()
    else:
        print(
            "Settings dictate to NOT use MQTT Manager as no MQTT configuration is present.")
