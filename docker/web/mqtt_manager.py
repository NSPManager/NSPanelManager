#!/usr/bin/env python
import logging
import paho.mqtt.client as mqtt
from requests import get, post
from time import sleep
import datetime
import json
import mqtt_manager_libs.home_assistant
import mqtt_manager_libs.openhab
import mqtt_manager_libs.websocket_server
import mqtt_manager_libs.light_states
import mqtt_manager_libs.light

settings = {}
last_settings_file_mtime = 0
client = mqtt.Client("NSPanelManager")
logging.basicConfig(level=logging.DEBUG)
logging.getLogger("urllib3").propagate = False


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
        # Messages received was a status update (online/offline)
        if parts[-1] == "log":
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
            data = json.loads(msg.payload.decode('utf-8'))
            send_online_status(panel, data)
            ws_data = {
                "type": "status",
                "payload": data
            }
            mqtt_manager_libs.websocket_server.send_message(
                json.dumps(ws_data))
        elif parts[-1] == "status_report":
            panel = parts[1]
            data = json.loads(msg.payload.decode('utf-8'))
            send_status_report(panel, data)
            ws_data = {
                "type": "status_report",
                "payload": data
            }
            mqtt_manager_libs.websocket_server.send_message(
                json.dumps(ws_data))
        elif msg.topic == "nspanel/mqttmanager/command":
            data = json.loads(msg.payload.decode('utf-8'))
            # Verify that the mac_origin is off a panel that is controlled by this instance
            origin_panel = None
            for nspanel in settings["nspanels"].values():
                if nspanel["mac"] == data["mac_origin"]:
                    origin_panel = nspanel
                    break
            if origin_panel:
                if data["method"] == "set" and data["attribute"] == "brightness":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_light_level(
                            data["brightness"])
                elif data["method"] == "set" and data["attribute"] == "kelvin":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_color_temp(
                            data["kelvin"])
                elif data["method"] == "set" and data["attribute"] == "saturation":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_color_saturation(
                            data["saturation"])
                elif data["method"] == "set" and data["attribute"] == "hue":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_color_hue(
                            data["hue"])
            else:
                logging.info(
                    "Received command but from panel not controlled by us: " + data["mac_origin"])

    except Exception as e:
        logging.error("Something went wrong during processing of message:")
        logging.error(e)
        try:
            logging.error(msg.payload.decode('utf-8'))
        except:
            logging.error(
                "Something went wrong when processing the exception message, couldn't decode payload to utf-8.")


def send_status_report(panel, new_status):
    post("http://127.0.0.1:8000/api/set_panel_status/" +
         new_status["mac"] + "/", json=new_status)


def send_online_status(panel, new_status):
    post("http://127.0.0.1:8000/api/set_panel_online_status/" +
         new_status["mac"] + "/", json=new_status)


def get_config():
    global settings
    while True:
        try:
            config_request = get(
                "http://127.0.0.1:8000/api/get_mqtt_manager_config", timeout=5)
            if config_request.status_code == 200:
                logging.info("Got config, will start MQTT Manager.")
                settings = config_request.json()

                for id, light in settings["lights"].items():
                    int_id = int(id)
                    mqtt_manager_libs.light_states.states[int_id] = mqtt_manager_libs.light.Light.from_dict(
                        light)
                # All light-data sucessfully loaded into light_states, clear own register
                settings.pop("lights")
                break
        except Exception as e:
            logging.error(
                "ERROR: Failed to get config. Will try again in 5 seconds.")
            logging.error(e)
            sleep(5)


def connect_and_loop():
    global settings, home_assistant
    mqtt_manager_libs.websocket_server.start_server()  # Start websocket server
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(
        settings["mqtt_username"], settings["mqtt_password"])
    # Wait for connection
    connection_return_code = 0
    mqtt_server = settings["mqtt_server"]
    mqtt_port = int(settings["mqtt_port"])
    logging.info(F"Connecting to {mqtt_server}:{mqtt_port}")
    while True:
        try:
            client.connect(mqtt_server, mqtt_port, 5)
            break  # Connection call did not raise exception, connection is sucessfull
        except:
            logging.error(
                F"Failed to connect to MQTT {mqtt_server}:{mqtt_port}. Will try again in 10 seconds. Code: {connection_return_code}")
            sleep(10)

    # MQTT Connected, start APIs if configured
    if settings["home_assistant_address"] != "" and settings["home_assistant_token"] != "":
        mqtt_manager_libs.home_assistant.init(settings, client)
        mqtt_manager_libs.home_assistant.connect()
    else:
        logging.info("Home Assistant values not configured, will not connect.")

    if settings["openhab_address"] != "" and settings["openhab_token"] != "":
        mqtt_manager_libs.openhab.init(settings, client)
        mqtt_manager_libs.openhab.connect()
    else:
        logging.info("OpenHABA values not configured, will not connect.")

    # Loop MQTT
    client.loop_forever()


if __name__ == '__main__':
    get_config()
    if settings["mqtt_server"] and settings["mqtt_port"]:
        connect_and_loop()
    else:
        logging.error(
            "Settings dictate to NOT use MQTT Manager as no MQTT configuration is present.")
