import logging
from requests import get, post
import mqtt_manager_libs.light_states
import traceback

settings = {}

def init(settings_from_manager, mqtt_client_from_manager):
    global openhab_url, openhab_token, settings, mqtt_client
    settings = settings_from_manager
    mqtt_client = mqtt_client_from_manager

def save_scene(room_name, scene_name: str):
    logging.debug(F"Trying to save scene {scene_name}")
    try:
        scenes_request = get("http://127.0.0.1:8000/api/get_scenes", timeout=5)
        if scenes_request.status_code == 200:
            logging.debug(F"Got scenes, will try to find {room_name}->{scene_name}")
            scenes = scenes_request.json()

            for scene in scenes["scenes"]:
                if scene["scene_name"] == scene_name and scene["room_name"] == room_name:
                    logging.debug("Found scene matching room and scene name: scene_id::" + str(scene["scene_id"]))
                    # We now know the ID of the scene we sant to save to, proceed to build post body

                    scene_save_data = {
                        "scene_id": scene["scene_id"],
                        "light_states": []
                    }

                    for light in mqtt_manager_libs.light_states.states.values():
                        if room_name == None or light.room_name == scene["room_name"]:
                            scene_save_data["light_states"].append(light.to_scene_data_dict())

                    post("http://127.0.0.1:8000/api/save_scene", json=scene_save_data)
                    break # We found the matching scene, no need to keep looping
    except:
        logging.exception("ERROR: Failed to save scene, giving up.")

def activate_scene(room_name, scene_name: str):
    logging.debug(F"Trying to turn on scene {room_name}->{scene_name}")
    try:
        scenes_request = get("http://127.0.0.1:8000/api/get_scenes", timeout=5)
        if scenes_request.status_code == 200:
            logging.debug(F"Got scenes, will try to find {room_name}->{scene_name}")
            scenes = scenes_request.json()

            for scene in scenes["scenes"]:
                if scene["scene_name"] == scene_name and scene["room_name"] == None:
                    logging.debug("Found scene matching room and scene name: scene_id::" + str(scene["scene_id"]))
                    for light_state in scene["light_states"]:
                        if light_state["light_id"] in mqtt_manager_libs.light_states.states:
                            light = mqtt_manager_libs.light_states.states[light_state["light_id"]]
                            logging.debug(F"Setting light '{light.friendly_name}' according to saved scene.")
                            light.set_from_scene_data(light_state)
                    break # We found the matching scene, no need to keep looping
    except:
        logging.exception("ERROR: Failed to get scenes, giving up.")
