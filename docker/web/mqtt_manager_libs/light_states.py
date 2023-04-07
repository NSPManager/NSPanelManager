import mqtt_manager_libs.light

states: dict[int, mqtt_manager_libs.light.Light] = {}

def get_id_by_name(name: str):
    for id, light in states.items():
        if light.home_assistant_name == name or light.openhab_item_name == name:
            return id
    return -1