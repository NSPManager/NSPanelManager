from django.apps import AppConfig
import environ


class WebConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'web'

    def ready(self):
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

