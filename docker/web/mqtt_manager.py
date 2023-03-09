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
    domain = parts[2].split('.')[0]
    entity_id = parts[2]
    attribute = parts[3]
    if attribute.startswith("state_"):
        return

    service = ""
    if domain == "light":
        service = "turn_on"

    setHomeassistantState(domain=domain, service=service, entity_id=entity_id,
                          attribute=attribute, state=msg.payload.decode('utf-8'))


def setHomeassistantState(domain, service, entity_id, attribute, state):
    global settings
    url = settings["sources"]["homeassistant"]["url"] + \
        "api/services/" + domain + "/" + service

    headers = {
        "Authorization": "Bearer " + settings["sources"]["homeassistant"]["token"],
        "content-type": "application/json",
    }
    body = {
        "entity_id": entity_id,
        attribute: state
    }
    # print("Posting to " + url)
    # print(body)
    # print("----------------")
    response = post(url, headers=headers, json=body)
    if response.status_code == 200:
        send_new_status(domain, service, entity_id, attribute, state)


def send_new_status(domain, service, entity_id, attribute, state):
    client.publish("nspanel/entities/" + entity_id +
                   "/state_" + attribute, state, retain=True)


def connect_and_loop():
    global settings
    print("Connecting to " +
          settings["mqtt_server"] + ":" + str(settings["mqtt_port"]))
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(
        settings["mqtt_username"], settings["mqtt_password"])
    client.connect(settings["mqtt_server"], settings["mqtt_port"], 60)
    client.loop_start()


if __name__ == '__main__':
    while True:
        if last_settings_file_mtime != os.path.getmtime("nspanelmanager/mqtt_manager.json"):
            client.loop_stop()
            client.disconnect()
            read_settings()
            if settings["mqtt_server"] and settings["mqtt_port"]:
                connect_and_loop()

            else:
                print("Settings dictate to NOT use MQTT Manager.")
            last_settings_file_mtime = os.path.getmtime(
                "nspanelmanager/mqtt_manager.json")
        sleep(5)
