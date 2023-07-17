#!/usr/bin/env python
import asyncio
import logging
import paho.mqtt.client as mqtt
from requests import get, post
import time
import datetime
import json
import subprocess
import mqtt_manager_libs.home_assistant
import mqtt_manager_libs.openhab
import mqtt_manager_libs.websocket_server
import mqtt_manager_libs.light_states
import mqtt_manager_libs.nspanel_states
import mqtt_manager_libs.light
import mqtt_manager_libs.scenes
import mqtt_manager_libs.home_assistant_autoreg
import re 
import threading
import os
import pytz

def get_machine_mac():
    pid = subprocess.Popen(["ifconfig" ], stdout=subprocess.PIPE)
    s = pid.communicate()[0].decode()
    mac = re.search(r"(([a-f\d]{1,2}\:){5}[a-f\d]{1,2})", s).groups()[0]
    return mac

settings = {}
last_settings_file_mtime = 0
has_sent_reload_command = False
client = mqtt.Client("NSPanelManager_" + get_machine_mac())
logging.basicConfig(level=logging.DEBUG)
logging.getLogger("urllib3").propagate = False
last_sent_time_string = ""

def get_md5_sum(url):
    req = get(url)
    if req.status_code == 200:
        return req.text
    else:
        return None

def send_time_thread():
    global last_sent_time_string
    use_timezone = pytz.utc
    if os.path.exists("/etc/timezone"):
        with open("/etc/timezone", "r") as f:
            tz = pytz.timezone(f.read().strip())
            if tz:
                use_timezone = tz

    while(True):
        if(client.is_connected()):
            if "clock_us_style" in settings and settings["clock_us_style"] == "True":
                time_format = "%-I:%M %p" 
            else:
                time_format = "%H:%M"
            time_string = datetime.datetime.now(use_timezone).strftime(time_format)
            if time_string != last_sent_time_string:
                client.publish("nspanel/status/time", time_string, retain=True)
                last_sent_time_string = time_string
        time.sleep(1)

async def send_mqttmanager_status(websocket = None):
    logging.debug("Sending mqttmanager_status.")
    status = {
        "type": "mqttmanager_status",
        "mqtt": {
            "connected": client.is_connected()
        },
        "home_assistant": {
            "configured": settings["home_assistant_token"] != "",
            "connected": mqtt_manager_libs.home_assistant.ws_connected,
            "auth_ok": mqtt_manager_libs.home_assistant.auth_ok
        },
        "openhab": {
            "configured": settings["openhab_token"] != "",
            "connected": mqtt_manager_libs.openhab.ws_connected
        }
    }
    if websocket is None:
        mqtt_manager_libs.websocket_server.send_message(json.dumps(status))
    else:
        await websocket.send(json.dumps(status))

def send_mqttmanager_status_sync():
    logging.debug("Sending MQTTManager status from sync.")
    asyncio.run(send_mqttmanager_status())
    # loop = asyncio.get_event_loop()
    # coroutine = send_mqttmanager_status()
    # loop.run_until_complete(coroutine)

def on_connect(client, userdata, flags, rc):
    logging.info("Connected to MQTT Server")
    # Listen for all events sent to and from panels to control states
    client.subscribe("nspanel/mqttmanager/command")
    client.subscribe("nspanel/+/log")
    client.subscribe("nspanel/+/status")
    client.subscribe("nspanel/+/status_report")
    client.subscribe("nspanel/scenes/room/+/+/save")
    client.subscribe("nspanel/scenes/room/+/+/activate")
    client.subscribe("nspanel/scenes/global/+/save")
    client.subscribe("nspanel/scenes/global/+/activate")
    client.subscribe("nspanel/entities/#")

def send_nspanel_command(panel_id, command_data):
    panel_id = int(panel_id)
    if panel_id in mqtt_manager_libs.nspanel_states.states:
        if "address" in mqtt_manager_libs.nspanel_states.states[panel_id]:
            nspanel = mqtt_manager_libs.nspanel_states.states[panel_id]
            client.publish("nspanel/" + nspanel["name"] + "/command", json.dumps(command_data))

async def on_websocket_client_connect(websocket):
    await send_mqttmanager_status(websocket)

async def on_websocket_message(websocket, message):
    reply = {"cmd_id": message["cmd_id"]}
    if message["command"] == "get_nspanel_status":
        reply["nspanels"] = mqtt_manager_libs.nspanel_states.states
    elif message["command"] == "reboot_nspanels":
        if "nspanels" in message["args"]:
            for nspanel_id in message["args"]["nspanels"]:
                nspanel_id = int(nspanel_id)
                if nspanel_id in mqtt_manager_libs.nspanel_states.states:
                    send_nspanel_command(nspanel_id, {"command": "reboot"})
                    if "address" in mqtt_manager_libs.nspanel_states.states[nspanel_id]: # TODO: Remove old HTTP GET method after a few updates as it is not longed in us.
                        get("http://" + mqtt_manager_libs.nspanel_states.states[nspanel_id]["address"] + "/do_reboot")
    elif message["command"] == "firmware_update_nspanels":
        if "nspanels" in message["args"]:
            for nspanel_id in message["args"]["nspanels"]:
                nspanel_id = int(nspanel_id)
                if nspanel_id in mqtt_manager_libs.nspanel_states.states:
                    send_nspanel_command(nspanel_id, {"command": "firmware_update"})
                    if "address" in mqtt_manager_libs.nspanel_states.states[nspanel_id]: # TODO: Remove old HTTP GET method after a few updates as it is not longed in us.
                        post("http://" + mqtt_manager_libs.nspanel_states.states[nspanel_id]["address"] + "/start_ota_update")

    elif message["command"] == "tft_update_nspanels":
        if "nspanels" in message["args"]:
            for nspanel_id in message["args"]["nspanels"]:
                nspanel_id = int(nspanel_id)
                if nspanel_id in mqtt_manager_libs.nspanel_states.states:
                    send_nspanel_command(nspanel_id, {"command": "tft_update"})
                    if "address" in mqtt_manager_libs.nspanel_states.states[nspanel_id]: # TODO: Remove old HTTP GET method after a few updates as it is not longed in us.
                        post("http://" + mqtt_manager_libs.nspanel_states.states[nspanel_id]["address"] + "/start_tft_ota_update")

    await websocket.send(json.dumps(reply))


def on_message(client, userdata, msg):
    try:
        if msg.payload.decode() == "":
            return
        parts = msg.topic.split('/')
        # Messages received was a status update (online/offline)
        if parts[-1] == "log":
            message_parts = msg.payload.decode('utf-8').split(';')
            data = {
                "type": "log",
                "time": datetime.datetime.now().strftime("%H:%M:%S"),
                "panel": parts[1],
                "mac": message_parts[0],
                "level": message_parts[1],
                "message": ':'.join(message_parts[2:])
            }
            mqtt_manager_libs.websocket_server.send_message(json.dumps(data))
        elif parts[-1] == "status":
            panel_found = False
            for panel in settings["nspanels"].values():
                if panel["name"] == parts[-2]:
                    panel_found = True
                    break

            if panel_found:
                panel = parts[1]
                data = json.loads(msg.payload.decode('utf-8'))
                panel_id = mqtt_manager_libs.nspanel_states.get_id_by_mac(data["mac"])
                if panel_id >= 0:
                    send_online_status(panel, data)
                    mqtt_manager_libs.nspanel_states.states[panel_id].update(data)
                    ws_data = {
                        "type": "status",
                        "payload": mqtt_manager_libs.nspanel_states.states[panel_id]
                    }
                    mqtt_manager_libs.websocket_server.send_message(json.dumps(ws_data))
            else:
                logging.warning(F"Removing mqtt topic: {msg.topic} as panel does not exist any more.")
                client.publish('/'.join(parts), payload=None, qos=0, retain=True)
        elif parts[-1] == "status_report":
            panel = parts[1]
            data = json.loads(msg.payload.decode('utf-8'))
            panel_id = mqtt_manager_libs.nspanel_states.get_id_by_mac(data["mac"])
            if panel_id >= 0:
                send_online_status(panel, data)
                mqtt_manager_libs.nspanel_states.states[panel_id].update(data)
                ws_data = {
                    "type": "status",
                    "payload": mqtt_manager_libs.nspanel_states.states[panel_id]
                }
                mqtt_manager_libs.websocket_server.send_message(json.dumps(ws_data))
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
                        mqtt_manager_libs.light_states.states[entity_id].set_light_level(data["brightness"])
                elif data["method"] == "set" and data["attribute"] == "kelvin":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_color_temp(data["kelvin"])
                elif data["method"] == "set" and data["attribute"] == "saturation":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_color_saturation(data["saturation"])
                elif data["method"] == "set" and data["attribute"] == "hue":
                    for entity_id in data["entity_ids"]:
                        mqtt_manager_libs.light_states.states[entity_id].set_color_hue(data["hue"])
            else:
                logging.info("Received command but from panel not controlled by us: " + data["mac_origin"])
        elif msg.topic.startswith("nspanel/scenes/room/") and msg.topic.endswith("/activate") and msg.payload.decode('utf-8') == "1":
            mqtt_manager_libs.scenes.activate_scene(parts[3], parts[4]) # Activate scene were part[3] is room and part[4] is scene name
        elif msg.topic.startswith("nspanel/scenes/room/") and msg.topic.endswith("/save") and msg.payload.decode('utf-8') == "1":
            mqtt_manager_libs.scenes.save_scene(parts[3], parts[4]) # Save scene were part[3] is room and part[4] is scene name
        elif msg.topic.startswith("nspanel/scenes/global/") and msg.topic.endswith("/activate") and msg.payload.decode('utf-8') == "1":
            mqtt_manager_libs.scenes.activate_scene(None, parts[3]) # Activate scene were part[3] is scene name
        elif msg.topic.startswith("nspanel/scenes/global/") and msg.topic.endswith("/save") and msg.payload.decode('utf-8') == "1":
            mqtt_manager_libs.scenes.save_scene(None, parts[3]) # Save scene were part[3] is scene name
        elif msg.topic.startswith("nspanel/entities/light/"):
            light_id =int(parts[3])
            if not light_id in mqtt_manager_libs.light_states.states:
                logging.warning(F"Removing MQTT topic '{msg.topic}' for light that does not exist any more.")
                client.publish('/'.join(parts), payload=None, qos=0, retain=True)
        else:
            logging.debug(F"Received unhandled message on topic: {msg.topic}")

    except Exception as e:
        logging.exception("Something went wrong during processing of message:")
        try:
            logging.error(msg.payload.decode('utf-8'))
        except:
            logging.error("Something went wrong when processing the exception message, couldn't decode payload to utf-8.")


def send_status_report(panel, new_status):
    post("http://127.0.0.1:8000/api/set_panel_status/" + new_status["mac"] + "/", json=new_status)


def send_online_status(panel, new_status):
    post("http://127.0.0.1:8000/api/set_panel_online_status/" + new_status["mac"] + "/", json=new_status)


def get_config():
    global settings
    while True:
        try:
            config_request = get("http://127.0.0.1:8000/api/get_mqtt_manager_config", timeout=5)
            if config_request.status_code == 200:
                logging.info("Got config, will start MQTT Manager.")
                settings = config_request.json()

                for id, light in settings["lights"].items():
                    int_id = int(id)
                    mqtt_manager_libs.light_states.states[int_id] = mqtt_manager_libs.light.Light.from_dict(light, settings)

                for id, nspanel in settings["nspanels"].items():
                    mqtt_manager_libs.nspanel_states.states[int(id)] = nspanel

                # All light-data sucessfully loaded into light_states, clear own register
                settings.pop("lights")
                break
        except Exception as e:
            logging.error(
                "ERROR: Failed to get config. Will try again in 5 seconds.")
            logging.error(e)
            time.sleep(5)


def connect_and_loop():
    global settings, home_assistant
    mqtt_manager_libs.websocket_server.register_message_handler(on_websocket_message)
    mqtt_manager_libs.websocket_server.register_on_connect_handler(on_websocket_client_connect)
    mqtt_manager_libs.websocket_server.start_server()  # Start websocket server
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(settings["mqtt_username"], settings["mqtt_password"])
    # Wait for connection
    connection_return_code = 0
    mqtt_server = settings["mqtt_server"]
    mqtt_port = int(settings["mqtt_port"])
    logging.info(F"Connecting to {mqtt_server}:{mqtt_port} as 'NSPanelManager_" + get_machine_mac() + "'")
    while True:
        try:
            client.connect(mqtt_server, mqtt_port, 5)
            break  # Connection call did not raise exception, connection is sucessfull
        except:
            logging.exception(F"Failed to connect to MQTT {mqtt_server}:{mqtt_port}. Will try again in 10 seconds. Code: {connection_return_code}")
            time.sleep(10)

    # Send reload command to panels for them to reload config as MQTT manager JUST restarted (probably because of config change)
    if has_sent_reload_command == False:
        client.publish(F"nspanel/config/reload", 1, retain=True)
    
    send_clock_thread = threading.Thread(target=send_time_thread)
    send_clock_thread.daemon = True
    send_clock_thread.start()

    # MQTT Connected, start APIs if configured
    if settings["home_assistant_address"] != "" and settings["home_assistant_token"] != "":
        mqtt_manager_libs.home_assistant.init(settings, client)
        mqtt_manager_libs.home_assistant.register_on_connect_handler(send_mqttmanager_status_sync)
        mqtt_manager_libs.home_assistant.register_on_disconnect_handler(send_mqttmanager_status_sync)
        mqtt_manager_libs.home_assistant.connect()
    else:
        logging.info("Home Assistant values not configured, will not connect.")

    if settings["openhab_address"] != "" and settings["openhab_token"] != "":
        mqtt_manager_libs.openhab.init(settings, client)
        mqtt_manager_libs.openhab.register_on_connect_handler(send_mqttmanager_status_sync)
        mqtt_manager_libs.openhab.register_on_disconnect_handler(send_mqttmanager_status_sync)
        mqtt_manager_libs.openhab.connect()
    else:
        logging.info("OpenHAB values not configured, will not connect.")

    for id, nspanel in settings["nspanels"].items():
        mqtt_manager_libs.home_assistant_autoreg.register_panel(nspanel, client, settings)

    # Loop MQTT
    client.loop_forever()


if __name__ == '__main__':
    get_config()
    if settings["mqtt_server"] and settings["mqtt_port"]:
        connect_and_loop()
    else:
        logging.error("Settings dictate to NOT use MQTT Manager as no MQTT configuration is present.")
