import json
import traceback
import environ
import logging
import requests
from web.settings_helper import get_setting_with_default

def get_all_openhab_items(filter = {}):
    return_json = {
        "items": [],
        "errors": []
    }

    # OpenHAB
    if get_setting_with_default("openhab_token") != "" and get_setting_with_default("openhab_address") != "":
        item_type_filter = []
        if "types" in filter:
            item_type_filter = filter["types"]

        # TODO: Sort out how to map channels from items to the correct POST request when MQTT is received
        openhab_request_headers = {
            "Authorization": "Bearer " + get_setting_with_default("openhab_token"),
            "content-type": "application/json",
        }
        try:
            openhab_response = requests.get(get_setting_with_default("openhab_address") + "/rest/items", headers=openhab_request_headers, verify=False)
            if openhab_response.status_code == 200:
                for item in openhab_response.json():
                    add_entity = False
                    if len(item_type_filter) > 0:
                        if "type" in item and item["type"] in item_type_filter:
                            add_entity = True
                    else:
                        # Not type filter applied, return all items
                        add_entity = True

                    if add_entity:
                        return_json["items"].append({
                            "type": "openhab",
                            "openhab_type": "item",
                            "label": item["label"],
                            "item_id": item["name"],
                            "item": item,
                        })
            else:
                return_json["errors"].append("Failed to get OpenHAB items, got return code: " + str(openhab_response.status_code))
                print("ERROR! Got status code other than 200. Got code: " + str(openhab_response.status_code))
        except Exception as e:
            return_json["errors"].append("Failed to get OpenHAB items: " + str(traceback.format_exc()))
            logging.exception("Failed to get OpenHAB items!")
    else:
        print("No OpenHAB configuration values. Will not gather OpenHAB entities.")

    return return_json
