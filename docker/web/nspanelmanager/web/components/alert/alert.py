from django_components import component
from django.template.context import Context
from django.core.files.storage import FileSystemStorage
import hashlib

from web.mqttmanager_ipc import send_ipc_request
from web.models import NSPanel
from web.settings_helper import get_nspanel_setting_with_default

@component.register("alert")
class Alert(component.Component):
    template_name = "alert.html"
    template_view = None

    def get_context_data(self, level, text):
        return {
            "level": level,
            "text": text
        }

    def get(self, request, level, text):
        args = {
            "level": level,
            "text": text
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
