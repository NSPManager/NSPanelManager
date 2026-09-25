from django_components import component
from django.template.context import Context
from django.core.files.storage import FileSystemStorage
import hashlib

from web.models import NSPanel
from web.settings_helper import get_nspanel_setting_with_default

@component.register("nspanel_warnings")
class NSPanelWarnings(component.Component):
    template_view = None

    def get_context_data(self, id):
        nspanel = NSPanel.objects.get(id=id)
        return {
            "nspanel": nspanel,
        }

    def get_template_name(self, context: Context):
        if self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name
        if url_name == "htmx_partial_index_nspanels_section" or url_name == "index":
            return "nspanel_warnings/nspanel_warnings_index.html"
        elif url_name == "edit_nspanel":
            return "nspanel_warnings/nspanel_warnings_edit_nspanel.html"


    # TODO: Move md5 checksum to DB instead of always calculating them on reqest.
    def get_file_md5sum(self, filename):
        fs = FileSystemStorage()
        if fs.exists(filename):
            return hashlib.md5(fs.open(filename).read()).hexdigest()
        else:
            return None

    def get(self, request, view, nspanel_id):
        self.template_view = view
        args = {
            "id": nspanel_id,
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
