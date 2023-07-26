import mqtt_manager_libs.home_assistant
import mqtt_manager_libs.openhab
import logging
import time


class Light:
    id: int = 0
    friendly_name: str = ""
    room_name: str = ""
    type: str = ""
    can_dim: bool = False
    can_color_temperature: bool = False
    can_rgb: bool = False
    home_assistant_name: str = ""
    openhab_control_mode: str = ""
    openhab_item_name: str = ""
    openhab_item_color_temp: str = ""
    openhab_item_rgb: str = ""

    light_level: int = 0
    color_temp: int = 0
    color_hue: int = 0
    color_saturation: int = 0
    # "color_temp" or "rgb". Used to restore correct state when using scenes
    last_command_sent: str = "color_temp"
    last_mode_change = 0
    last_light_level_change = 0
    settings = {} # This is ALL the settings data gathered from NSPanel Manager web

    @staticmethod
    def from_dict(dict_data, settings):
        newLight = Light()
        newLight.settings = settings
        # Set default color temp to be 50%
        newLight.color_temp = settings["color_temp_min"] + ((settings["color_temp_max"] - settings["color_temp_min"]) * 0.5)
        newLight.id = dict_data["id"]
        newLight.friendly_name = dict_data["name"]
        newLight.room_name = dict_data["room_name"]
        newLight.type = dict_data["type"]
        newLight.can_dim = dict_data["can_dim"]
        newLight.can_color_temperature = dict_data["can_color_temperature"]
        newLight.can_rgb = dict_data["can_rgb"]

        if newLight.type == "home_assistant":
            newLight.home_assistant_name = dict_data["home_assistant_name"]
        elif newLight.type == "openhab":
            newLight.openhab_control_mode = dict_data["openhab_control_mode"]
            if newLight.openhab_control_mode == "switch":
                newLight.openhab_item_name = dict_data["openhab_item_switch"]
            elif newLight.openhab_control_mode == "dimmer":
                newLight.openhab_item_name = dict_data["openhab_item_dimmer"]

            if newLight.can_color_temperature:
                newLight.openhab_item_color_temp = dict_data["openhab_item_color_temp"]
            if newLight.can_rgb:
                newLight.openhab_item_rgb = dict_data["openhab_item_rgb"]

        logging.info(F"Loaded light '{newLight.type}::{newLight.friendly_name}'")
        logging.info(F"Loaded light '{newLight.type}::{newLight.friendly_name}', can dim? " + ("Yes" if newLight.can_dim else "No"))
        logging.info(F"Loaded light '{newLight.type}::{newLight.friendly_name}', can color temp? " + ("Yes" if newLight.can_color_temperature else "No"))
        logging.info(F"Loaded light '{newLight.type}::{newLight.friendly_name}', can rgb? " + ("Yes" if newLight.can_rgb else "No"))
        return newLight

    def get_light_level(self) -> int:
        return self.light_level

    def set_light_level(self, light_level: int):
        if self.type == "home_assistant":
            send_color_temp = 0
            if self.last_command_sent == "color_temp" or self.settings["turn_on_behavior"] == "color_temp":
                send_color_temp = self.color_temp
            if mqtt_manager_libs.home_assistant.set_entity_brightness(self.home_assistant_name, light_level, send_color_temp):
                self.light_level = int(light_level)
        elif self.type == "openhab":
            if self.last_command_sent == "color_temp":
                logging.info(F"Sending light update to OpenHAB for light {self.friendly_name}. Sending brightness value: {light_level}")
                if mqtt_manager_libs.openhab.set_entity_brightness(self.openhab_item_name, self.openhab_control_mode, light_level):
                    if self.light_level == 0:
                        logging.info(F"Sending kelvin update to OpenHAB for light {self.friendly_name}. Light was off previously.")
                        mqtt_manager_libs.openhab.set_entity_color_temp(self.openhab_item_color_temp, self.color_temp)
                    self.light_level = int(light_level)
                    self.last_mode_change = time.time()*1000
            elif self.last_command_sent == "rgb":
                logging.info(F"Sending light update to OpenHAB for light {self.friendly_name}. Sending RGB value: {light_level}")
                if mqtt_manager_libs.openhab.set_entity_color_saturation(self.openhab_item_rgb, int(light_level), self.color_saturation, self.color_hue):
                    self.light_level = int(light_level)
                    self.last_mode_change = time.time()*1000
            else:
                logging.error("Unknown OpenHAB light state. Last command sent: " + self.last_command_sent)

    def get_color_temp(self) -> int:
        return self.color_temp

    def set_color_temp(self, color_temp: int):
        # If the light is currently on, send out new value to light
        if self.can_color_temperature and self.light_level > 0:
            if self.type == "home_assistant":
                mqtt_manager_libs.home_assistant.set_entity_color_temp(
                    self.home_assistant_name, color_temp)
            elif self.type == "openhab":
                mqtt_manager_libs.openhab.set_entity_color_temp(
                    self.openhab_item_color_temp, color_temp)
        self.color_temp = int(color_temp)
        self.last_command_sent = "color_temp"
        self.last_mode_change = time.time()*1000

    def set_color_saturation(self, color_saturation: int):
        if self.type == "home_assistant":
            mqtt_manager_libs.home_assistant.set_entity_color_saturation(
                self.home_assistant_name, self.light_level, color_saturation, self.color_hue)
        elif self.type == "openhab":
            mqtt_manager_libs.openhab.set_entity_color_saturation(
                self.openhab_item_rgb, self.light_level, color_saturation, self.color_hue)
        self.color_saturation = int(color_saturation)
        self.last_command_sent = "rgb"
        self.last_mode_change = time.time()*1000

    def get_color_saturation(self):
        return self.color_saturation

    def set_color_hue(self, color_hue: int):
        if self.type == "home_assistant":
            mqtt_manager_libs.home_assistant.set_entity_color_saturation(
                self.home_assistant_name, self.light_level, self.color_saturation, color_hue)
        elif self.type == "openhab":
            mqtt_manager_libs.openhab.set_entity_color_saturation(
                self.openhab_item_rgb, self.light_level, self.color_saturation, color_hue)
        self.color_hue = int(color_hue)
        self.last_command_sent = "rgb"
        self.last_mode_change = time.time()*1000

    def get_color_hue(self):
        return self.color_hue

    def set_from_scene_data(self, scene_data: dict):
        if int(scene_data["light_level"]) == 0:
            self.set_light_level(0)
        elif scene_data["color_mode"] == "dimmer":
            self.set_color_temp(scene_data["color_temp"])
            self.set_light_level(scene_data["light_level"])
        elif scene_data["color_mode"] == "color":
            self.set_light_level(scene_data["light_level"])
            self.set_color_hue(scene_data["hue"])
            self.set_color_saturation(scene_data["saturation"])

    def to_scene_data_dict(self):
        return_dict = {
            "light_id": self.id,
            "level": self.get_light_level(),
            "mode": "dimmer"
        }
        if self.last_command_sent == "color_temp":
            return_dict["color_temp"] = self.get_color_temp()
        elif self.last_command_sent == "rgb":
            return_dict["mode"] = "color"
            return_dict["hue"] = self.get_color_hue()
            return_dict["saturation"] = self.get_color_saturation()

        return return_dict
