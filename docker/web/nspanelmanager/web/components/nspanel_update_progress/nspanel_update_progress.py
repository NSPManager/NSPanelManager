from django_components import component
from django.template.context import Context

from web.models import NSPanel
from web.mqttmanager_ipc import send_ipc_request

@component.register("nspanel_update_progress")
class NSPanelUpdateProgress(component.Component):
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
            if url_name == "edit_nspanel":
                return "nspanel_update_progress/nspanel_update_progress_edit_nspanel.html"

    def get(self, request, view, nspanel_id):
        self.template_view = view
        args = {
            "id": nspanel_id,
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
