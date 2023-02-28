#!/usr/bin/env python
import os
import paho.mqtt.client as mqtt
from requests import get, post
from time import sleep
import json

use_mqtt_manager = False
mqtt_server = ""
mqtt_port = 1883
mqtt_username = ""
mqtt_password = ""
homeassistant_url = ""
homeassistant_token = ""
last_settings_file_mtime = ""
client = mqtt.Client("NSPanelManager")


def read_settings():
    global mqtt_server, mqtt_port, mqtt_username, mqtt_password, homeassistant_url,  homeassistant_token, use_mqtt_manager
    with open("nspanelmanager/mqtt_manager.json", "r") as f:
        data = json.loads(f.read())
        mqtt_server = data["mqtt_server"]
        mqtt_port = data["mqtt_port"]
        mqtt_username = data["mqtt_username"]
        mqtt_password = data["mqtt_password"]
        homeassistant_url = data["homeassistant_url"]
        homeassistant_token = data["homeassistant_token"]
        use_mqtt_manager = data["use_mqtt_manager"]


def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Server")
    # Listen for all events sent to and from panels to control states
    client.subscribe("nspanel/entities/#")


def on_message(client, userdata, msg):
    parts = msg.topic.split('/')
    domain = parts[2].split('.')[0]
    entity_id = parts[2]
    attribute = parts[3]
    service = ""
    if domain == "light":
        service = "turn_on"

    setHomeassistantState(domain=domain, service=service, entity_id=entity_id,
                          attribute=attribute, state=msg.payload.decode('utf-8'))


def setHomeassistantState(domain, service, entity_id, attribute, state):
    url = homeassistant_url + "api/services/" + domain + "/" + service
    headers = {
        "Authorization": "Bearer " + homeassistant_token,
        "content-type": "application/json",
    }
    body = {
        "entity_id": entity_id,
        attribute: state
    }
    print("Posting to " + url)
    print(body)
    response = post(url, headers=headers, json=body)
    if response.status_code == 200:
        send_new_status(domain, service, entity_id, attribute, state)


def send_new_status(domain, service, entity_id, attribute, state):
    client.publish("nspanel/entities/" + entity_id +
                   "/state_" + attribute, state, retain=True)


def connect_and_loop():
    global client, mqtt_server, mqtt_port, mqtt_username, mqtt_password
    print("Connecting to " + mqtt_server + ":" + str(mqtt_port))
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(mqtt_username, mqtt_password)
    client.connect(mqtt_server, mqtt_port, 60)
    client.loop_start()


if __name__ == '__main__':
    while True:
        if last_settings_file_mtime != os.path.getmtime("nspanelmanager/mqtt_manager.json"):
            client.loop_stop()
            client.disconnect()
            read_settings()
            if use_mqtt_manager:
                connect_and_loop()
            else:
                print("Settings dictate to NOT use MQTT Manager.")
            last_settings_file_mtime = os.path.getmtime(
                "nspanelmanager/mqtt_manager.json")
        sleep(5)
