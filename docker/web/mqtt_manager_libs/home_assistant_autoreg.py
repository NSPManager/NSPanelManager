import json
import logging

from paho import mqtt

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
