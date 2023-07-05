import websocket
import logging
import json
from threading import Thread
import mqtt_manager_libs.light_states

home_assistant_url = ""
home_assistant_token = ""
settings = {}
auth_ok = False
next_id = 0
request_all_states_id = 0


def init(settings_from_manager, mqtt_client_from_manager):
    global home_assistant_url, home_assistant_token, settings, mqtt_client
    settings = settings_from_manager
    mqtt_client = mqtt_client_from_manager
    home_assistant_url = settings["home_assistant_address"]
    home_assistant_token = settings["home_assistant_token"]
    # Disable logging from underlying "websocket"
    logging.getLogger("websocket").propagate = False
    # Disable logging from underlying "websocket"
    logging.getLogger("websockets").propagate = False


def on_message(ws, message):
    global auth_ok, request_all_states_id
    json_msg = json.loads(message)
    if json_msg["type"] == "auth_required":
        authenticate_client()
    elif json_msg["type"] == "auth_ok":
        logging.info("Home Assistant auth OK. Requesting existing states.")
        subscribe_to_events()
        _get_all_states()
        auth_ok = True
    elif json_msg["type"] == "event" and json_msg["event"]["event_type"] == "state_changed":
        entity_id = json_msg["event"]["data"]["entity_id"]
        if entity_id.startswith("light.") or entity_id.startswith("switch."):
            send_entity_update(json_msg)
    elif json_msg["type"] == "result" and not json_msg["success"]:
        logging.error("Failed result: ")
        logging.error(json_msg)
    elif json_msg["type"] == "result" and json_msg["success"]:
        if json_msg["id"] == request_all_states_id:
            for entity in json_msg["result"]:
                for id, light in mqtt_manager_libs.light_states.states.items():
                    if light.home_assistant_name == entity["entity_id"]:
                        try:
                            if entity["state"] == "off":
                                light.light_level = 0
                            else:
                                if "brightness" in entity["attributes"]:
                                    light.light_level = round(
                                        entity["attributes"]["brightness"] / 2.55)
                                else:
                                    light.light_level = 100  # Type is a switch and is ON, regard it as 100% on

                            mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_brightness_pct", light.light_level, retain=True)
                            if light.can_rgb and "color_mode" in entity["attributes"] and entity["attributes"]["color_mode"] != "color_temp":
                                light.last_command_sent = "rgb"
                                mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_hue", entity["attributes"]["hs_color"][0], retain=True)
                                mqtt_manager_libs.light_states.states[light.id].color_hue = entity["attributes"]["hs_color"][0]
                                mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_sat", entity["attributes"]["hs_color"][1], retain=True)
                                mqtt_manager_libs.light_states.states[light.id].color_saturation = entity["attributes"]["hs_color"][1]
                            elif light.can_color_temperature and "color_temp_kelvin" in entity["attributes"]:
                                light.last_command_sent = "color_temp"
                                light.color_temp = entity["attributes"]["color_temp_kelvin"]
                                mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_kelvin", light.color_temp, retain=True)
                        except:
                            logging.exception("Something went wrong while trying to load current states.")
        return None # Ignore success result messages
    else:
        logging.debug(message)


def connect():
    Thread(target=_do_connection, daemon=True).start()


def _do_connection():
    global home_assistant_url, ws
    ws_url = home_assistant_url.replace(
        "https://", "wss://").replace("http://", "ws://")
    ws_url += "/api/websocket"
    logging.info(F"Connecting to Home Assistant at {ws_url}")
    ws = websocket.WebSocketApp(F"{ws_url}", on_message=on_message)
    ws.run_forever(reconnect=5)


def authenticate_client():
    global home_assistant_token
    logging.info("Sending auth to Home Assistant")
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


def _get_all_states():
    global next_id, request_all_states_id
    msg = {
        "id": next_id,
        "type": "get_states",
    }
    request_all_states_id = next_id
    send_message(json.dumps(msg))

# Got new value from Home Assistant, publish to MQTT


def send_entity_update(json_msg):
    global mqtt_client
    # Check if the light is used on any nspanel and if so, send MQTT state update
    try:
        entity_id = json_msg["event"]["data"]["entity_id"]
        entity_name = entity_id
        new_state = json_msg["event"]["data"]["new_state"]
        entity_id = mqtt_manager_libs.light_states.get_id_by_name(entity_name)
        if entity_id >= 0:
            if "brightness" in new_state["attributes"]:
                new_brightness = round(
                    new_state["attributes"]["brightness"] / 2.55)
                mqtt_client.publish(
                    F"nspanel/entities/light/{entity_id}/state_brightness_pct", new_brightness, retain=True)
                mqtt_manager_libs.light_states.states[entity_id].light_level = new_brightness
            else:
                if new_state["state"] == "on":
                    mqtt_client.publish(
                        F"nspanel/entities/light/{entity_id}/state_brightness_pct", 100, retain=True)
                    mqtt_manager_libs.light_states.states[entity_id].light_level = 100
                else:
                    mqtt_client.publish(
                        F"nspanel/entities/light/{entity_id}/state_brightness_pct", 0, retain=True)
                    mqtt_manager_libs.light_states.states[entity_id].light_level = 0

            if mqtt_manager_libs.light_states.states[entity_id].can_rgb and "color_mode" in new_state["attributes"] and new_state["attributes"]["color_mode"] != "color_temp":
                mqtt_manager_libs.light_states.states[entity_id].last_command_sent = "rgb"
                mqtt_client.publish(
                    F"nspanel/entities/light/{entity_id}/state_hue", new_state["attributes"]["hs_color"][0], retain=True)
                mqtt_manager_libs.light_states.states[entity_id].color_hue = new_state["attributes"]["hs_color"][0]
                mqtt_client.publish(
                    F"nspanel/entities/light/{entity_id}/state_sat", new_state["attributes"]["hs_color"][1], retain=True)
                mqtt_manager_libs.light_states.states[
                    entity_id].color_saturation = new_state["attributes"]["hs_color"][1]
            elif "color_temp_kelvin" in new_state["attributes"]:
                mqtt_manager_libs.light_states.states[entity_id].last_command_sent = "color_temp"
                mqtt_client.publish(
                    F"nspanel/entities/light/{entity_id}/state_kelvin", new_state["attributes"]["color_temp_kelvin"], retain=True)
                mqtt_manager_libs.light_states.states[entity_id].color_temp = new_state["attributes"]["color_temp_kelvin"]

    except:
        logging.exception("Failed to send entity update!")


def set_entity_brightness(home_assistant_name: str, light_level: int, color_temp: int) -> bool:
    """Set entity brightness"""
    global next_id
    try:
        # Format Home Assistant state update
        msg = None
        if home_assistant_name.startswith("light."):
            msg = {
                "id": next_id,
                "type": "call_service",
                "domain": "light",
                "service": "turn_on",
                "service_data": {
                    "brightness_pct": light_level,
                },
                "target": {
                    "entity_id": home_assistant_name
                }
            }
            if color_temp > 0:
                msg["service_data"]["kelvin"] = color_temp
        elif home_assistant_name.startswith("switch."):
            msg = {
                "id": next_id,
                "type": "call_service",
                "domain": "switch",
                "service": "turn_on" if light_level > 0 else "turn_off",
                "target": {
                    "entity_id": home_assistant_name
                }
            }

        if msg:
            send_message(json.dumps(msg))
            return True
        else:
            logging.error(F"{home_assistant_name} is now a recognized domain.")
            return False
    except:
        logging.exception("Failed to send entity update to Home Assisatant.")
        return False


def set_entity_color_temp(entity_name: str, color_temp: int) -> bool:
    """Set entity brightness"""
    global next_id
    try:
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
                "entity_id": entity_name
            }
        }
        send_message(json.dumps(msg))
        return True
    except:
        logging.exception("Failed to send entity update to Home Assisatant.")
        return False


def set_entity_color_saturation(entity_name: str, light_level: int, color_saturation: int, color_hue: int) -> bool:
    """Set entity brightness"""
    global next_id
    try:
        # Format Home Assistant state update
        msg = {
            "id": next_id,
            "type": "call_service",
            "domain": "light",
            "service": "turn_on",
            "service_data": {
                "hs_color": [
                    color_hue,
                    color_saturation
                ],
                "brightness_pct": light_level
            },
            "target": {
                "entity_id": entity_name
            }
        }
        send_message(json.dumps(msg))
        return True
    except Exception as e:
        logging.exception("Failed to send entity update to Home Assisatant.")
        return False


def send_message(message):
    global ws, next_id
    next_id += 1
    ws.send(message)
