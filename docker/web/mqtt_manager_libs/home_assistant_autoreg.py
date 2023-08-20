import json
import logging
from paho import mqtt
import subprocess
import re


def get_machine_mac():
    pid = subprocess.Popen(["ifconfig" ], stdout=subprocess.PIPE)
    s = pid.communicate()[0].decode()
    mac = re.search(r"(([a-f\d]{1,2}\:){5}[a-f\d]{1,2})", s).groups()[0]
    return mac


def register_panel(nspanel, mqtt_client, settings):
    base_config = {
        "device": {
            "identifiers": [
                ("mac_" + nspanel["mac"].replace(":", "_").lower()).strip()
            ],
            "manufacturer": "Sonoff",
            "model": "NSPanel",
            "name": nspanel["name"].strip(),
            "sw_version": "0.0.27"
        },
        "availability": [
            {
                "topic": "nspanel/" + nspanel["name"] + "/status",
                "value_template": "{{ value_json.state }}"
            }
        ]
    }
    register_temperature_sensor(nspanel, mqtt_client, settings, base_config)
    register_relays(nspanel, mqtt_client, settings, base_config)
    register_screen_switch(nspanel, mqtt_client, settings, base_config)
    register_screen_brightness_controls(nspanel, mqtt_client, settings, base_config)
    register_scenes(settings, mqtt_client)


def register_temperature_sensor(nspanel, mqtt_client, settings, base_config):
    config = dict(base_config)
    config["device_class"] = "temperature"
    if settings["use_farenheit"] == "True":
        config["unit_of_measurement"] = "°F"
    else:
        config["unit_of_measurement"] = "°C"
    config["name"] = "Temperature"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/temperature_state"
    config["unique_id"] = nspanel["mac"].replace(":", "_").lower() + "_temperature"
    mqtt_client.publish("homeassistant/sensor/nspanelmanager/" + nspanel["mac"].replace(":", "_").lower() + "_temperature/config", json.dumps(config), retain=True)


def register_relays(nspanel, mqtt_client, settings, base_config):
    config = dict(base_config)
    config["name"] = "Relay 1"
    config["device_class"] = "switch"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/r1_state"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/r1_cmd"
    config["state_on"] = "1"
    config["state_off"] = "0"
    config["payload_on"] = "1"
    config["payload_off"] = "0"
    config["unique_id"] = nspanel["mac"].replace(":", "_").lower() + "_relay1"
    mqtt_client.publish("homeassistant/switch/nspanelmanager/" + nspanel["mac"].replace(":", "_").lower() + "_relay1/config", json.dumps(config), retain=True)
    
    config["name"] = "Relay 2"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/r2_state"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/r2_cmd"
    config["unique_id"] = nspanel["mac"].replace(":", "_").lower() + "_relay2"
    mqtt_client.publish("homeassistant/switch/nspanelmanager/" + nspanel["mac"].replace(":", "_").lower() + "_relay2/config", json.dumps(config), retain=True)


def register_screen_switch(nspanel, mqtt_client, settings, base_config):
    config = dict(base_config)
    config["name"] = "Screen power"
    config["device_class"] = "switch"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/screen_state"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/screen_cmd"
    config["state_on"] = "1"
    config["state_off"] = "0"
    config["payload_on"] = "1"
    config["payload_off"] = "0"
    config["unique_id"] = nspanel["mac"].replace(":", "_").lower() + "_screen_state"
    mqtt_client.publish("homeassistant/switch/nspanelmanager/" + nspanel["mac"].replace(":", "_").lower() + "_screen/config", json.dumps(config), retain=True)


def register_screen_brightness_controls(nspanel, mqtt_client, settings, base_config):
    config = dict(base_config)
    config["name"] = "Screen brightness"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/brightness"
    config["min"] = "1"
    config["max"] = "100"
    config["unique_id"] = nspanel["mac"].replace(":", "_").lower() + "_screen_brightness"
    mqtt_client.publish("homeassistant/number/nspanelmanager/" + nspanel["mac"].replace(":", "_").lower() + "_screen_brightness/config", json.dumps(config), retain=True)

    config["name"] = "Screensaver brightness"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/brightness_screensaver"
    config["min"] = "0"
    config["max"] = "100"
    config["unique_id"] = nspanel["mac"].replace(":", "_").lower() + "_screensaver_brightness"
    mqtt_client.publish("homeassistant/number/nspanelmanager/" + nspanel["mac"].replace(":", "_").lower() + "_screensaver_brightness/config", json.dumps(config), retain=True)

def register_scenes(settings, mqtt_client):
    for id, scene in settings["scenes"].items():
        config = {
            "availability":  [
                {
                    "topic": "nspanel/status/availability_" + get_machine_mac(),
                }
            ]
        }
        config["name"] = "NSPM Scene " + scene["name"]
        config["payload_on"] = "1"
        if "room_id" in scene and "room_name" in scene:
            config["command_topic"] = "nspanel/scenes/room/" + scene["room_name"] + "/" + scene["name"] + "/activate"
        else:
            config["command_topic"] = "nspanel/scenes/global/" + scene["name"] + "/activate"

