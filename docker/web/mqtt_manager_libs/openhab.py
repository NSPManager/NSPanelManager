import websocket
import asyncio
import json
from time import sleep
from threading import Thread

openhab_url = ""
openhab_token = ""
settings = {}

def init(settings_from_manager, mqtt_client_from_manager):
    global openhab_url, openhab_token, settings, mqtt_client
    settings = settings_from_manager
    mqtt_client = mqtt_client_from_manager
    openhab_url = settings["openhab_address"]
    openhab_token = settings["openhab_token"]

def on_message(ws, message):
    json_msg = json.loads(message)
    if json_msg["type"] == "ItemStateEvent":
        for light in settings["lights"]:
            if light["type"] == "openhab":
                topic_parts = json_msg["topic"].split("/")
                item = topic_parts[2]
                payload = json.loads(json_msg["payload"])
                entity_name = light["name"]
                if item == light["openhab_item_dimmer"]:
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", payload["value"], retain=True)
                elif item == light["openhab_item_color_temp"]:
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_kelvin", payload["value"], retain=True)
                    


def connect():
    Thread(target=_do_connection, daemon=True).start()
    Thread(target=_send_keepalive, daemon=True).start()

def _do_connection():
    global openhab_url, ws
    ws_url = openhab_url.replace("https://", "wss://").replace("http://", "ws://")
    ws_url += "/ws"
    print(F"Connecting to OpenHAB at {ws_url}")
    ws_url += "?accessToken=" + openhab_token
    ws = websocket.WebSocketApp(F"{ws_url}", on_message=on_message)
    ws.run_forever()
    # Open KeepAlive thread

def _send_keepalive():
    while True:
        if ws:
            keepalive_msg = {
                "type": "WebSocketEvent",
                "topic": "openhab/websocket/heartbeat",
                "payload": "PING",
                "source": "WebSocketTestInstance"
            }
            try:
                ws.send(json.dumps(keepalive_msg));
            except Exception as e:
                print("Error! Failed to send keepalive message to OpenHAB websocket.")
                print(e)
        sleep(5)

# Got new value from OpenHAB, publish to MQTT
def send_entity_update(json_msg, item):
    global mqtt_client
    # Check if the light is used on any nspanel and if so, send MQTT state update
    try:
        entity_id = json_msg["event"]["data"]["entity_id"]
        entity_name = entity_id.replace("light.", "")
        new_state = json_msg["event"]["data"]["new_state"]
        for light in settings["lights"]:
            if light["name"] == entity_name:
                if "brightness" in new_state["attributes"]:
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", round(new_state["attributes"]["brightness"] / 2.55), retain=True)
                else:
                    if new_state["state"] == "on":
                        mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", 100, retain=True)
                    else:
                        mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", 0, retain=True)
                
                if light["can_color_temperature"]:
                    # Convert from MiRed from OpenHAB to kelvin values
                    color_temp_kelvin = round(1000000 / new_state["attributes"]["color_temp"])
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_kelvin", color_temp_kelvin, retain=True)
    except Exception as e:
        print("Failed to send entity update!")
        print(e)

def get_light_by_name(entity_id):
    for light in settings["lights"]:
        if light["name"] == entity_id:
            return light
    return None

# Set an attribute for a light in OpenHAB
def set_light_attribute(entity_id, attribute, value):
    light = get_light_by_name(entity_id)
    if light:
        item = ""
        payload_attr = ""
        if attribute == "brightness_pct":
            payload_attr = "Percent"
            item = light["openhab_item_dimmer"]
        elif attribute == "kelvin":
            payload_attr = "Decimal"
            item = light["openhab_item_color_temp"]

        msg = {
            "type": "ItemCommandEvent",
            "topic": "openhab/items/" + item + "/command",
            "payload": "{\"type\":\"" + payload_attr + "\",\"value\":\"" + str(value) + "\"}",
            "source": "WebSocketNSPanelManager"
        }
        ws.send(json.dumps(msg))