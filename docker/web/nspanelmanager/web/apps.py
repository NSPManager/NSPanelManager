from django.apps import AppConfig
import environ
import logging
import psutil
import subprocess
import os


def start_mqtt_manager():
    from .settings_helper import get_setting_with_default
    for proc in psutil.process_iter():
        if "./mqtt_manager.py" in proc.cmdline():
            return None # MQTT Manager already running
    # Restart the process
    logging.info("Starting a new mqtt_manager")
    mqttmanager_env = os.environ.copy()
    mqttmanager_env["MQTT_SERVER"] = get_setting_with_default("mqtt_server", "")
    mqttmanager_env["MQTT_PORT"] = get_setting_with_default("mqtt_port", "1883")
    mqttmanager_env["MQTT_USERNAME"] = get_setting_with_default("mqtt_username", "")
    mqttmanager_env["MQTT_PASSWORD"] = get_setting_with_default("mqtt_password", "")
    mqttmanager_env["HOME_ASSISTANT_ADDRESS"] = get_setting_with_default("home_assistant_address", "")
    mqttmanager_env["HOME_ASSISTANT_TOKEN"] = get_setting_with_default("home_assistant_token", "")
    mqttmanager_env["OPENHAB_ADDRESS"] = get_setting_with_default("openhab_address", "")
    mqttmanager_env["OPENHAB_TOKEN"] = get_setting_with_default("openhab_token", "")
    subprocess.Popen(["/usr/local/bin/python", "./mqtt_manager.py"], cwd="/usr/src/app/", env=mqttmanager_env)


class WebConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'web'

    def ready(self):
        try:
            environment = environ.Env()
            if "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true":
                if "SUPERVISOR_TOKEN" in environment:
                    from .settings_helper import get_setting_with_default, set_setting_value
                    if get_setting_with_default("home_assistant_token", "") == "" and get_setting_with_default("home_assistant_address", "") == "":
                        print("No home assistant address or token stored, setting according to addon environment.")
                        set_setting_value("home_assistant_token", environment("SUPERVISOR_TOKEN"))
                        set_setting_value("home_assistant_address", "http://supervisor")
                    elif get_setting_with_default("home_assistant_token", "") != environment("SUPERVISOR_TOKEN"):
                        print("Home Assistant token has changed. Will update database.")
                        set_setting_value("home_assistant_token", environment("SUPERVISOR_TOKEN"))
                    #from .models import Settings
                    #objects = Settings.objects.filter(name=name)
            start_mqtt_manager()
        except:
            logging.exception("Failed to populate Home Assistant addon settings.")
