from django.apps import AppConfig
import environ
import logging
import psutil
import subprocess
import os
import signal
import time
from web.protobuf import protobuf_mqttmanager_pb2

mqttmanager_process = None

def start_mqtt_manager():
    from .settings_helper import get_setting_with_default

    print("Did not find a running MQTTManager, starting MQTTManager...")
    # Restart the process
    logging.info("Starting a new mqtt_manager")
    settings = protobuf_mqttmanager_pb2.MQTTManagerPrivateSettings()
    settings.home_assistant_address = get_setting_with_default("home_assistant_address")
    settings.home_assistant_token = get_setting_with_default("home_assistant_token")
    settings.openhab_address = get_setting_with_default("openhab_address")
    settings.openhab_token = get_setting_with_default("openhab_token")
    settings.mqtt_server = get_setting_with_default("mqtt_server")
    settings.mqtt_server_port = int(get_setting_with_default("mqtt_port"))
    settings.mqtt_username = get_setting_with_default("mqtt_username")
    settings.mqtt_password = get_setting_with_default("mqtt_password")

    mqttmanager_env = os.environ.copy()
    mqttmanager_env["SETTINGS"] = settings.SerializeToString()
    mqttmanager_env["LOG_LEVEL"] = get_setting_with_default("mqttmanager_log_level")

    global mqttmanager_process
    mqttmanager_process = subprocess.Popen(["/MQTTManager/build/nspm_mqttmanager"], cwd="/usr/src/app/", env=mqttmanager_env)


"""
Restart the MQTT Manager process
"""
def restart_mqtt_manager_process():
    global mqttmanager_process
    if mqttmanager_process != None:
        mqttmanager_process.poll()
        if mqttmanager_process.returncode != 0:
            if mqttmanager_process.returncode > 0:
                logging.error(F"MQTTManager binary has exited unexpectedly. Return code: {mqttmanager_process.returncode}")
            else:
                logging.error(F"MQTTManager binary has exited unexpectedly. Killed by signal: {mqttmanager_process.returncode}")
            logging.error(F"stderr: {mqttmanager_process.stderr}")
            mqttmanager_process.kill()

    for proc in psutil.process_iter():
        if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
            logging.info("Killing running MQTTManager")
            os.kill(proc.pid, signal.SIGKILL)
    start_mqtt_manager()


"""
Send command to MQTT Manager to reload config
"""
def send_mqttmanager_reload_command():
    for proc in psutil.process_iter():
        if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
            logging.info("Found running MQTTManager. Sending reload command via SIGUSR1 signal.")
            os.kill(proc.pid, signal.SIGUSR1)


class WebConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'web'

    def ready(self):
        try:
            environment = environ.Env()
            if "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true":
                if "SUPERVISOR_TOKEN" in environment:
                    from .settings_helper import get_setting_with_default, set_setting_value
                    if get_setting_with_default("home_assistant_token") == "" and get_setting_with_default("home_assistant_address") == "":
                        print(
                            "No home assistant address or token stored, setting according to addon environment.")
                        set_setting_value(
                            "home_assistant_token", environment("SUPERVISOR_TOKEN"))
                        set_setting_value(
                            "home_assistant_address", "http://supervisor")
                    elif get_setting_with_default("home_assistant_token") != environment("SUPERVISOR_TOKEN"):
                        print(
                            "Home Assistant token has changed. Will update database.")
                        set_setting_value(
                            "home_assistant_token", environment("SUPERVISOR_TOKEN"))
                    # from .models import Settings
                    # objects = Settings.objects.filter(name=name)
            restart_mqtt_manager_process()
        except:
            logging.exception(
                "Failed to populate Home Assistant addon settings.")
