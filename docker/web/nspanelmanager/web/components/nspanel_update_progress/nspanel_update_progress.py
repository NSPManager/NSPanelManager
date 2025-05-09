from django_components import component
from django.template.context import Context

from web.mqttmanager_ipc import send_ipc_request

@component.register("nspanel_update_progress")
class NSPanelUpdateProgress(component.Component):
    template_view = None

    def get_context_data(self, id, state, progress):
        return {
            "id": id,
            "state": state,
            "progress": progress,
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
        panel_status = send_ipc_request(F"nspanel/{nspanel_id}/status", {"command": "get"})
        args = {
            "id": nspanel_id,
            "state": panel_status["state"],
            "progress": panel_status["update_progress"],
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
