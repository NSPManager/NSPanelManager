import websocket
import asyncio
import json
from threading import Thread

home_assistant_url = ""
home_assistant_token = ""
settings = {}
auth_ok = False
next_id = 0

def init(settings_from_manager, mqtt_client_from_manager):
    global home_assistant_url, home_assistant_token, settings, mqtt_client
    settings = settings_from_manager
    mqtt_client = mqtt_client_from_manager
    home_assistant_url = settings["home_assistant_address"]
    home_assistant_token = settings["home_assistant_token"]

def on_message(ws, message):
    global auth_ok
    json_msg = json.loads(message)
    if json_msg["type"] == "auth_required":
        authenticate_client()
    elif json_msg["type"] == "auth_ok":
        print("Home Assistant auth OK.")
        subscribe_to_events()
        auth_ok = True
    elif json_msg["type"] == "event" and json_msg["event"]["event_type"] == "state_changed":
        entity_id = json_msg["event"]["data"]["entity_id"]
        if entity_id.startswith("light."):
            #print(json_msg)
            send_entity_update(json_msg)
    elif json_msg["type"] == "result" and not json_msg["success"]:
        print("Failed result: ")
        print(json_msg)
    elif json_msg["type"] == "result" and json_msg["success"]:
        pass # Ignore success result messages
    else:
        print(message)

def connect():
    Thread(target=_do_connection, daemon=True).start()

def _do_connection():
    global home_assistant_url, ws
    ws_url = home_assistant_url.replace("https://", "wss://").replace("http://", "ws://")
    ws_url += "/api/websocket"
    print(F"Connecting to Home Assistant at {ws_url}")
    ws = websocket.WebSocketApp(F"{ws_url}", on_message=on_message)
    ws.run_forever()

def authenticate_client():
    global home_assistant_token
    print("Sending auth to Home Assistant")
    msg = {
        "type": "auth",
        "access_token": home_assistant_token
    }
    send_message(json.dumps(msg))

def subscribe_to_events():
    global next_id
    msg = {
        "id": next_id,
        "type": "subscribe_events",
        "event_type": "state_changed"
    }
    send_message(json.dumps(msg))

# Got new value from Home Assistant, publish to MQTT
def send_entity_update(json_msg):
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
                    # Convert from MiRed from Home Assistant to kelvin values
                    color_temp_kelvin = round(1000000 / new_state["attributes"]["color_temp"])
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_kelvin", color_temp_kelvin, retain=True)
    except Exception as e:
        print("Failed to send entity update!")
        print(e)

# Set an attribute for a light in Home Assistant
def set_light_attribute(light, attribute, value):
    global next_id
    msg = {
        "id": next_id,
        "type": "call_service",
        "domain": "light",
        "service": "turn_on",
        "service_data": {
            attribute: value
        },
        "target": {
            "entity_id": F"light.{light}"
        }
    }
    send_message(json.dumps(msg))

def send_message(message):
    global ws, next_id
    next_id += 1
    ws.send(message)