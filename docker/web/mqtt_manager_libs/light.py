import mqtt_manager_libs.home_assistant
import mqtt_manager_libs.openhab
import logging


class Light:
    id: int = 0
    friendly_name: str = ""
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
    color_saturation: int = 0
    color_hue: int = 0
    # "color_temp" or "rgb". Used to restore correct state when using scenes
    last_command_sent: str = "color_temp"

    @staticmethod
    def from_dict(dict_data):
        newLight = Light()
        newLight.id = dict_data["id"]
        newLight.friendly_name = dict_data["name"]
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

        logging.info(
            F"Loaded light '{newLight.type}::{newLight.friendly_name}'")
        return newLight

    def get_light_level(self) -> int:
        return self.light_level

    def set_light_level(self, light_level: int):
        if self.type == "home_assistant":
            send_color_temp = 0
            if self.last_command_sent == "color_temp":
                send_color_temp = self.color_temp
            if mqtt_manager_libs.home_assistant.set_entity_brightness(self.home_assistant_name, light_level, send_color_temp):
                self.light_level = light_level
        elif self.type == "openhab":
            if mqtt_manager_libs.openhab.set_entity_brightness(self.openhab_item_name, self.openhab_control_mode, light_level):
                if self.can_color_temperature and self.light_level == 0 and self.last_command_sent == "rgb":
                    mqtt_manager_libs.openhab.set_entity_color_temp(
                        self.openhab_item_color_temp, self.color_temp)
                self.light_level = light_level

    def get_color_temp(self) -> int:
        return self.color_temp

    def set_color_temp(self, color_temp: int):
        # If the light is currently on, send out new value to light
        if self.can_color_temperature and self.light_level > 0:
            if self.type == "home_assistant":
                mqtt_manager_libs.home_assistant.set_entity_color_temp(
                    self.home_assistant_name, color_temp)
            elif self.type == "openhab":
                mqtt_manager_libs.home_assistant.set_entity_color_temp(
                    self.openhab_item_color_temp, color_temp)
        self.color_temp = color_temp
        self.last_command_sent = "color_temp"

    def set_color_saturation(self, color_saturation: int):
        if self.type == "home_assistant":
            mqtt_manager_libs.home_assistant.set_entity_color_saturation(
                self.home_assistant_name, self.light_level, color_saturation, self.color_hue)
        elif self.type == "openhab":
            mqtt_manager_libs.home_assistant.set_entity_color_saturation(
                self.openhab_item_rgb, self.light_level, color_saturation, self.color_hue)
        self.color_saturation = color_saturation
        self.last_command_sent = "rgb"

    def set_color_hue(self, color_hue: int):
        if self.type == "home_assistant":
            mqtt_manager_libs.home_assistant.set_entity_color_saturation(
                self.home_assistant_name, self.light_level, self.color_saturation, color_hue)
        elif self.type == "openhab":
            mqtt_manager_libs.home_assistant.set_entity_color_saturation(
                self.openhab_item_rgb, self.light_level, self.color_saturation, color_hue)
        self.color_hue = color_hue
        self.last_command_sent = "rgb"
