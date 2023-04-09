import websocket
import requests
import logging
import json
from time import sleep
from threading import Thread
import mqtt_manager_libs.light_states

openhab_url = ""
openhab_token = ""
settings = {}


def init(settings_from_manager, mqtt_client_from_manager):
    global openhab_url, openhab_token, settings, mqtt_client
    settings = settings_from_manager
    mqtt_client = mqtt_client_from_manager
    openhab_url = settings["openhab_address"]
    openhab_token = settings["openhab_token"]
    # Disable logging from underlying "websocket"
    logging.getLogger("websocket").propagate = False
    # Disable logging from underlying "websocket"
    logging.getLogger("websockets").propagate = False


def on_message(ws, message):
    json_msg = json.loads(message)
    if json_msg["type"] == "ItemStateEvent":
        for light in mqtt_manager_libs.light_states.states.values():
            if light.type == "openhab":
                topic_parts = json_msg["topic"].split("/")
                item = topic_parts[2]
                payload = json.loads(json_msg["payload"])
                if light.openhab_control_mode == "dimmer" and item == light.openhab_item_name:
                    mqtt_client.publish(
                        F"nspanel/entities/light/{light.id}/state_brightness_pct", int(float(payload["value"])), retain=True)
                elif light.openhab_control_mode == "switch" and item == light.openhab_item_name:
                    if payload["value"] == "ON":
                        mqtt_client.publish(
                            F"nspanel/entities/light/{light.id}/state_brightness_pct", 100, retain=True)
                    else:
                        mqtt_client.publish(
                            F"nspanel/entities/light/{light.id}/state_brightness_pct", 0, retain=True)
                elif item == light["openhab_item_color_temp"]:
                    mqtt_client.publish(
                        F"nspanel/entities/light/{light.id}/state_kelvin", int(float(payload["value"])), retain=True)


def connect():
    Thread(target=_do_connection, daemon=True).start()
    # Update all existing states
    _update_all_light_states()


def _do_connection():
    global openhab_url, ws
    ws_url = openhab_url.replace(
        "https://", "wss://").replace("http://", "ws://")
    ws_url += "/ws"
    logging.info(F"Connecting to OpenHAB at {ws_url}")
    ws_url += "?accessToken=" + openhab_token
    ws = websocket.WebSocketApp(F"{ws_url}", on_message=on_message)
    # Open KeepAlive thread
    Thread(target=_send_keepalive, daemon=True).start()
    ws.run_forever()


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
                ws.send(json.dumps(keepalive_msg))
            except Exception as e:
                logging.error(
                    "Error! Failed to send keepalive message to OpenHAB websocket.")
                logging.error(e)
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
                    new_brightness = round(
                        new_state["attributes"]["brightness"] / 2.55)
                    mqtt_client.publish(
                        F"nspanel/entities/light/{entity_name}/state_brightness_pct", new_brightness, retain=True)
                    mqtt_manager_libs.light_states.states[entity_id]["brightness"] = new_brightness
                else:
                    if new_state["state"] == "on":
                        mqtt_client.publish(
                            F"nspanel/entities/light/{entity_name}/state_brightness_pct", 100, retain=True)
                        mqtt_manager_libs.light_states.states[entity_id]["brightness"] = 100
                    else:
                        mqtt_client.publish(
                            F"nspanel/entities/light/{entity_name}/state_brightness_pct", 0, retain=True)
                        mqtt_manager_libs.light_states.states[entity_id]["brightness"] = 0

                if "color_temp" in new_state["attributes"]:
                    # Convert from MiRed from OpenHAB to kelvin values
                    color_temp_kelvin = round(
                        1000000 / new_state["attributes"]["color_temp"])
                    mqtt_client.publish(
                        F"nspanel/entities/light/{entity_name}/state_kelvin", color_temp_kelvin, retain=True)
                    mqtt_manager_libs.light_states.states[entity_id]["color_temp"] = color_temp_kelvin
    except Exception as e:
        logging.error("Failed to send entity update!")
        logging.error(e)


def set_entity_brightness(openhab_item_name: str, openhab_control_mode: str, light_level: int) -> bool:
    try:
        # Get light from state list
        if openhab_control_mode == "dimmer":
            # Format OpenHAB state update
            msg = {
                "type": "ItemCommandEvent",
                "topic": F"openhab/items/{openhab_item_name}/command",
                "payload": "{\"type\":\"Percent\",\"value\":\"" + str(light_level) + "\"}",
                "source": "WebSocketNSPanelManager"
            }
            ws.send(json.dumps(msg))
        elif openhab_control_mode == "switch":
            # Format OpenHAB state update
            if new_brightness > 0:
                onoff = "ON"
                new_brightness = 100
            if new_brightness <= 0:
                onoff = "OFF"
                new_brightness = 0

            msg = {
                "type": "ItemCommandEvent",
                "topic": F"openhab/items/{openhab_item_name}/command",
                "payload": "{\"type\":\"OnOff\",\"value\":\"" + onoff + "\"}",
                "source": "WebSocketNSPanelManager"
            }
            ws.send(json.dumps(msg))
            return True
    except Exception as e:
        logging.error("Failed to send entity update to OpenHAB.")
        logging.error(e)
        return False


def set_entity_color_temp(openhab_item_name: str, color_temp: int) -> bool:
    try:
        # Format OpenHAB state update
        msg = {
            "type": "ItemCommandEvent",
            "topic": F"openhab/items/{openhab_item_name}/command",
            "payload": "{\"type\":\"Decimal\",\"value\":\"" + str(color_temp) + "\"}",
            "source": "WebSocketNSPanelManager"
        }
        ws.send(json.dumps(msg))
        return True
    except Exception as e:
        logging.error("Failed to send entity update to OpenHAB.")
        logging.error(e)
        return False


def set_entity_color_saturation(entity_name: str, light_level: int, color_saturation: int, color_hue: int) -> bool:
    logging.error("Color not implemented for OpenHAB yet.")


def _update_all_light_states():
    for light_id, light in mqtt_manager_libs.light_states.states.items():
        if light.type == "openhab":
            try:
                if light.openhab_control_mode == "dimmer":
                    item_state = _get_item_state(light.openhab_item_name)
                    if item_state != None:
                        light.light_level = int(float(item_state["state"]))
                        mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_brightness_pct", int(
                            float(item_state["state"])), retain=True)
                    else:
                        logging.error(
                            "Failed to get item state for OppenHAB item: " + light.openhab_item_name)
                elif light.openhab_control_mode == "switch":
                    item_state = _get_item_state(light.openhab_item_name)
                    if item_state != None:
                        if item_state["state"] == "ON":
                            light.light_level = 100
                            mqtt_client.publish(
                                F"nspanel/entities/light/{light_id}/state_brightness_pct", 100, retain=True)
                        else:
                            light.light_level = 0
                            mqtt_client.publish(
                                F"nspanel/entities/light/{light_id}/state_brightness_pct", 0, retain=True)
                    else:
                        logging.error(
                            F"Failed to get item state for OppenHAB item: {light.openhab_item_name}")

                if light.can_color_temperature:
                    item_state = _get_item_state(light.openhab_item_color_temp)
                    if item_state != None:
                        light.color_temp = int(float(item_state["state"]))
                        mqtt_client.publish(
                            F"nspanel/entities/light/{light_id}/state_kelvin", int(float(item_state["state"])), retain=True)
                    else:
                        logging.error(
                            "Failed to get item state for OppenHAB item: " + light.openhab_item_color_temp)
            except Exception as e:
                logging.error(
                    F"Failed to process light ID:{light_id}. The following error occured:")
                logging.error(e)


def _get_item_state(item):
    global openhab_url, openhab_token
    url = openhab_url + "/rest/items/" + item
    headers = {
        "Authorization": "Bearer " + openhab_token,
        "content-type": "application/json",
    }
    request_result = requests.get(url, headers=headers)
    if (request_result.status_code == 200):
        return json.loads(request_result.text)
    else:
        logging.error(
            "Something went wrong when trying to get state for item " + item)
