import websocket
import asyncio
import json
from threading import Thread
import mqtt_manager_libs.light_states

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
            # print(json_msg) # Dump light update message
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
        entity_id = mqtt_manager_libs.light_states.get_id_by_name(entity_name)
        if entity_id > 0:
            if "brightness" in new_state["attributes"]:
                new_brightness = round(new_state["attributes"]["brightness"] / 2.55)
                mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", new_brightness, retain=True)
                mqtt_manager_libs.light_states.states[entity_id]["brightness"] = new_brightness
            else:
                if new_state["state"] == "on":
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", 100, retain=True)
                    mqtt_manager_libs.light_states.states[entity_id]["brightness"] = 100
                else:
                    mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_brightness_pct", 0, retain=True)
                    mqtt_manager_libs.light_states.states[entity_id]["brightness"] = 0
            
            if "color_temp_kelvin" in new_state["attributes"]:
                mqtt_client.publish(F"nspanel/entities/light/{entity_name}/state_kelvin", new_state["attributes"]["color_temp_kelvin"], retain=True)
                mqtt_manager_libs.light_states.states[entity_id]["color_temp"] = new_state["attributes"]["color_temp_kelvin"]

    except Exception as e:
        print("Failed to send entity update!")
        print(e)

def set_entity_brightness(entity_id: int, new_brightness: int):
    """Set entity brightness"""
    global next_id
    try:
        # Get light from state list
        light = mqtt_manager_libs.light_states.states[entity_id]
        entity_name = light["name"]
        # Format Home Assistant state update
        msg = {
            "id": next_id,
            "type": "call_service",
            "domain": "light",
            "service": "turn_on",
            "service_data": {
                "brightness_pct": new_brightness,
                "kelvin": light["color_temp"]
            },
            "target": {
                "entity_id": F"light.{entity_name}"
            }
        }
        send_message(json.dumps(msg))
        # Update the stored value
        mqtt_manager_libs.light_states.states[entity_id]["brightness"] = new_brightness
    except Exception as e:
        print("Failed to send entity update to Home Assisatant.")
        print(e)

def set_entity_color_temp(entity_id: int, color_temp: int):
    """Set entity brightness"""
    global next_id
    try:
        # Get light from state list
        light = mqtt_manager_libs.light_states.states[entity_id]
        if light["brightness"] > 0:
            entity_name = light["name"]
            # Format Home Assistant state update
            msg = {
                "id": next_id,
                "type": "call_service",
                "domain": "light",
                "service": "turn_on",
                "service_data": {
                    "kelvin": color_temp
                },
                "target": {
                    "entity_id": F"light.{entity_name}"
                }
            }
            send_message(json.dumps(msg))
        # Update the stored value
        mqtt_manager_libs.light_states.states[entity_id]["color_temp"] = color_temp
    except Exception as e:
        print("Failed to send entity update to Home Assisatant.")
        print(e)

def send_message(message):
    global ws, next_id
    next_id += 1
    ws.send(message)