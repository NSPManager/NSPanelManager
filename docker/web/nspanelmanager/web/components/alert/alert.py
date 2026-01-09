import hashlib

from django.core.files.storage import FileSystemStorage
from django.template.context import Context
from django_components import component


@component.register("alert")
class Alert(component.Component):
    template_name = "alert.html"
    template_view = None

    def get_context_data(self, level, text, style="alert"):
        return {"level": level, "text": text, "style": style}

    def get(self, request, level, text, style="alert"):
        args = {"level": level, "text": text, "style": style}
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
