states = {}

def get_id_by_name(name: str):
    for id, state in states.items():
        if state["name"] == name:
            return id
    return -1