from django_components import component
from django.template.context import Context
from django.core.files.storage import FileSystemStorage
import hashlib

from web.mqttmanager_ipc import send_ipc_request
from web.models import NSPanel
from web.settings_helper import get_nspanel_setting_with_default

@component.register("item_select")
class ItemSelect(component.Component):
    template_view = None

    def get_context_data(self, items, name, selected, label):
        return {
            "items": items,
            "name": name,
            "label": label,
            "selected": selected,
        }

    def get_template_name(self, context: Context):
        return "item_select/item_select.html"

    def get(self, request, items, name, label, selected):
        args = {
            "items": items,
            "name": name,
            "label": label,
            "selected": selected
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
