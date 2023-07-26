import logging

states: dict[int, dict] = {}

def get_id_by_name(name: str):
    for id, nspanel in states.items():
        if nspanel["name"] == name:
            return int(id)
    return -1


def get_id_by_mac(mac: str):
    for id, nspanel in states.items():
        if nspanel["mac"] == mac:
            return int(id)
    return -1
