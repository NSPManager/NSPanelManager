import json
import logging

from paho import mqtt

def register_panel(nspanel, mqtt_client, settings):
    base_config = {
        "device": {
            "identifiers": [
                "NSPanel-" + nspanel["mac"]
            ],
            "name": nspanel["name"],
            "model": "NSPanel",
            "manufacturer": "Sonoff",
        },
        "availability": {
            "topic": "nspanel/" + nspanel["name"] + "/status",
            "value_template": "{{ value_json.state }}"
        }
    }
    register_temperature_sensor(nspanel, mqtt_client, settings, base_config)
    register_relays(nspanel, mqtt_client, settings, base_config)
    register_screen_switch(nspanel, mqtt_client, settings, base_config)


def register_temperature_sensor(nspanel, mqtt_client, settings, base_config):
    config = base_config
    config["device_class"] = "temperature"
    if settings["use_farenheit"] == "True":
        config["unit_of_measurement"] = "°F"
    else:
        config["unit_of_measurement"] = "°C"
    config["name"] = nspanel["name"] + " temperature"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/temperature_state"
    mqtt_client.publish("homeassistant/sensor/nspanelmanager/" + nspanel["name"] + "_temperature/config", json.dumps(config), retain=True)


def register_relays(nspanel, mqtt_client, settings, base_config):
    config = base_config
    config["name"] = nspanel["name"] + " relay 1"
    config["device_class"] = "switch"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/r1_state"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/r1_cmd"
    config["state_on"] = "1"
    config["state_off"] = "0"
    config["payload_on"] = "1"
    config["payload_off"] = "0"
    mqtt_client.publish("homeassistant/switch/nspanelmanager/" + nspanel["name"] + "_relay1/config", json.dumps(config), retain=True)
    
    config["name"] = nspanel["name"] + " relay 2"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/r2_state"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/r2_cmd"
    mqtt_client.publish("homeassistant/switch/nspanelmanager/" + nspanel["name"] + "_relay2/config", json.dumps(config), retain=True)

    
def register_screen_switch(nspanel, mqtt_client, settings, base_config):
    config = base_config
    config["name"] = nspanel["name"] + " screen"
    config["device_class"] = "switch"
    config["state_topic"] = "nspanel/" + nspanel["name"] + "/screen_state"
    config["command_topic"] = "nspanel/" + nspanel["name"] + "/screen_cmd"
    config["state_on"] = "1"
    config["state_off"] = "0"
    config["payload_on"] = "1"
    config["payload_off"] = "0"
    mqtt_client.publish("homeassistant/switch/nspanelmanager/" + nspanel["name"] + "_screen/config", json.dumps(config), retain=True)


