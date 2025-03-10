from django.apps import AppConfig
import environ
import logging
import psutil
import subprocess
import os
import signal
import time
import signal
from web.protobuf import protobuf_mqttmanager_pb2

mqttmanager_process = None

def sigchld_handler(signum, frame):
    # Handle SIGCHLD from mqttmanager_process
    pid, status = os.waitpid(-1, os.WNOHANG)
    if pid > 0:
        if os.WIFEXITED(status):
            logging.error(F"MQTTManager binary has exited unexpectedly. Return code: {os.WEXITSTATUS(status)}")
        elif os.WIFSIGNALED(status):
            logging.error(F"MQTTManager binary has exited unexpectedly. Killed by signal: {os.WTERMSIG(status)}")
            logging.error(F"stderr: {mqttmanager_process.stderr}")


def start_mqtt_manager():
    from .settings_helper import get_setting_with_default
    global mqttmanager_process

    print("Did not find a running MQTTManager, starting MQTTManager...")
    # Restart the process
    logging.info("Starting a new mqtt_manager")

    mqttmanager_env = os.environ.copy()
    mqttmanager_env["LOG_LEVEL"] = get_setting_with_default("mqttmanager_log_level")
    mqttmanager_process = subprocess.Popen(["/MQTTManager/build/nspm_mqttmanager"], cwd="/usr/src/app/", env=mqttmanager_env)


"""
Restart the MQTT Manager process
"""
def restart_mqtt_manager_process():
    for proc in psutil.process_iter():
        try:
            if proc.status() == psutil.STATUS_ZOMBIE:
                print("Found zombie MQTTManager process. Will not try to kill it as it's already dead.")
            else:
                if "/MQTTManager/build/nspm_mqttmanager" in proc.cmdline():
                    logging.info("Killing running MQTTManager")
                    os.kill(proc.pid, signal.SIGKILL)
        except:
            print("Warning! Failed to check state or kill existing MQTTManager process. Will continue anyway.")
            continue
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
        # Setup handler to catch SIGCHLD from mqttmanager_process
        signal.signal(signal.SIGCHLD, sigchld_handler)

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
