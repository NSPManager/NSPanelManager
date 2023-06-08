import websocket
import requests
import logging
import json
from time import sleep
from threading import Thread
import traceback
import mqtt_manager_libs.light_states

openhab_url = ""
openhab_token = ""
keepalive_thread = None
stop_keepalive = False
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
        topic_parts = json_msg["topic"].split("/")
        item = topic_parts[2]
        payload = json.loads(json_msg["payload"])
        #logging.info(F"Got ItemStateEvent on topic: " + json_msg["topic"])
        for light in mqtt_manager_libs.light_states.states.values():
            try:
                if light.type == "openhab":
                    if light.openhab_control_mode == "dimmer" and item == light.openhab_item_name:
                        logging.info("Item matches light: " + light.friendly_name + " dimmer item.")
                        light_level_pct = int(float(payload["value"]))
                        mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_brightness_pct", light_level_pct, retain=True)
                        light.light_level = light_level_pct
                        return None
                    elif light.openhab_control_mode == "switch" and item == light.openhab_item_name:
                        logging.info("Item matches light: " + light.friendly_name + " switch item.")
                        if payload["value"] == "ON":
                            mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_brightness_pct", 100, retain=True)
                            light.light_level = 100
                            return None
                        else:
                            mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_brightness_pct", 0, retain=True)
                            light.light_level = 0
                            return None
                    elif item == light.openhab_item_color_temp:
                        logging.info("Item matches light: " + light.friendly_name + " color temp item.")
                        received_color_temp_percent = 100 - int(float(payload["value"]))
                        # logging.debug(F"Recevied color temp from OpenHAB: {received_color_temp_percent}%")
                        kelvin_max_floored = settings["color_temp_max"] - settings["color_temp_min"]
                        send_color_temp = settings["color_temp_min"] + int((received_color_temp_percent / 100) * kelvin_max_floored)
                        mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_kelvin", send_color_temp, retain=True)
                        light.color_temp = send_color_temp
                        light.last_command_sent = "color_temp"
                        return None
                    elif item == light.openhab_item_rgb:
                        #hue, sat, brightness = payload["value"]
                        #logging.info("Item matches light: " + light.friendly_name + " RGB item.")
                        #logging.info("RGB Payload: " + payload["value"])
                        hue, sat, brightness = payload["value"].split(",")
                        light.color_hue = int(float(hue))
                        light.color_saturation = int(float(sat))
                        light.light_level = int(float(brightness))
                        mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_hue", light.color_hue, retain=True)
                        mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_sat", light.color_saturation, retain=True)
                        mqtt_client.publish(F"nspanel/entities/light/{light.id}/state_brightness_pct", light.light_level, retain=True)
                        light.last_command_sent = "rgb"
                        return None

                logging.info(F"Got state update event for light not managed by NSPanelManager. Topic: " + json_msg["topic"])
            except Exception as e:
                traceback.print_exc()

def connect():
    Thread(target=_do_connection, daemon=True).start()
    # Update all existing states
    _update_all_light_states()


def _ws_connection_open(ws):
    global keepalive_thread
    logging.info("WebSocket connection to OpenHAB opened.")
    if keepalive_thread == None:
        keepalive_thread = Thread(target=_send_keepalive, daemon=True)
        keepalive_thread.start()


def _ws_connection_close(ws, close_status_code, close_msg):
    global keepalive_thread, stop_keepalive
    if keepalive_thread != None:
        stop_keepalive = True
        keepalive_thread.join()
        logging.info("Keep-alive thread stopped.")
        stop_keepalive = False
        keepalive_thread = None
    logging.error("WebSocket connection closed!")


def _do_connection():
    global openhab_url, ws
    ws_url = openhab_url.replace(
        "https://", "wss://").replace("http://", "ws://")
    ws_url += "/ws"
    logging.info(F"Connecting to OpenHAB at {ws_url}")
    ws_url += "?accessToken=" + openhab_token
    ws = websocket.WebSocketApp(
        F"{ws_url}", on_message=on_message, on_open=_ws_connection_open, on_close=_ws_connection_close)
    ws.run_forever(reconnect=5)


def _send_keepalive():
    num_error = 0
    while True and num_error < 5 and not stop_keepalive:
        if ws:
            keepalive_msg = {
                "type": "WebSocketEvent",
                "topic": "openhab/websocket/heartbeat",
                "payload": "PING",
                "source": "WebSocketTestInstance"
            }
            try:
                ws.send(json.dumps(keepalive_msg))
                num_error = 0
            except Exception as e:
                num_error += 1
                logging.error(
                    "Error! Failed to send keepalive message to OpenHAB websocket.")
                logging.error(e)
        sleep(5)
    logging.error(
        "More than 5 keep-alive messages have failed. Stopping keep-alive thread.")


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
            if light_level > 0:
                onoff = "ON"
            if light_level <= 0:
                onoff = "OFF"

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
        traceback.print_exc()
        return False


def set_entity_color_temp(openhab_item_name: str, color_temp: int) -> bool:
    try:
        # Convert kelvin to %
        kelvin_max_floored = settings["color_temp_max"] - settings["color_temp_min"]
        color_temp_floored = color_temp - settings["color_temp_min"]
        send_color_temp = 100 - int((color_temp_floored / kelvin_max_floored) * 100)
        if send_color_temp < 0:
            send_color_temp = 0
        elif send_color_temp > 100:
            send_color_temp = 100
        # Format OpenHAB state update
        msg = {
            "type": "ItemCommandEvent",
            "topic": F"openhab/items/{openhab_item_name}/command",
            "payload": "{\"type\":\"Percent\",\"value\":\"" + str(send_color_temp) + "\"}",
            "source": "WebSocketNSPanelManager"
        }
        ws.send(json.dumps(msg))
        return True
    except Exception as e:
        logging.error("Failed to send entity update to OpenHAB.")
        traceback.print_exc()
        return False


def set_entity_color_saturation(openhab_item_name: str, light_level: int, color_saturation: int, color_hue: int) -> bool:
    try:
        # Format OpenHAB state update
        msg = {
            "type": "ItemCommandEvent",
            "topic": F"openhab/items/{openhab_item_name}/command",
            "payload": "{\"type\":\"HSB\",\"value\":\"" + str(color_hue) + "," + str(color_saturation) + "," + str(light_level) + "\"}",
            "source": "WebSocketNSPanelManager"
        }
        ws.send(json.dumps(msg))
        return True
    except Exception as e:
        logging.error("Failed to send entity update to OpenHAB.")
        traceback.print_exc()
        return False


def _update_all_light_states():
    for light_id, light in mqtt_manager_libs.light_states.states.items():
        if light.type == "openhab":
            try:
                if light.openhab_control_mode == "dimmer":
                    item_state = _get_item_state(light.openhab_item_name)
                    if item_state != None:
                        light.light_level = int(float(item_state["state"]))
                        mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_brightness_pct", int(float(item_state["state"])), retain=True)
                    else:
                        logging.error("Failed to get item state for OppenHAB item: " + light.openhab_item_name)
                elif light.openhab_control_mode == "switch":
                    item_state = _get_item_state(light.openhab_item_name)
                    if item_state != None:
                        if item_state["state"] == "ON":
                            light.light_level = 100
                            mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_brightness_pct", 100, retain=True)
                        else:
                            light.light_level = 0
                            mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_brightness_pct", 0, retain=True)
                    else:
                        logging.error(F"Failed to get item state for OppenHAB item: {light.openhab_item_name}")

                if light.can_color_temperature:
                    item_state = _get_item_state(light.openhab_item_color_temp)
                    if item_state != None:
                        light.color_temp = int(float(item_state["state"]))
                        mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_kelvin", int(float(item_state["state"])), retain=True)
                    else:
                        logging.error("Failed to get item state for OppenHAB item: " + light.openhab_item_color_temp)

                if light.can_rgb:
                    item_state = _get_item_state(light.openhab_item_rgb)
                    if item_state != None:
                        #hue, sat, brightness = item_state["state"].split(",")
                        hue, sat, brightness = item_state["state"].split(",")
                        mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_hue", int(float(hue)), retain=True)
                        mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_sat", int(float(sat)), retain=True)
                        mqtt_client.publish(F"nspanel/entities/light/{light_id}/state_brightness_pct", brightness, retain=True)
                        light.color_hue = int(float(hue))
                        light.color_saturation = int(float(sat))
                        light.light_level = int(float(brightness))
                    else:
                        logging.error("Failed to get item state for OppenHAB item: " + light.openhab_item_color_temp)
            except Exception as e:
                logging.error(F"Failed to process light ID:{light_id}. The following error occured:")
                traceback.print_exc()


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
        logging.error("Something went wrong when trying to get state for item " + item)
