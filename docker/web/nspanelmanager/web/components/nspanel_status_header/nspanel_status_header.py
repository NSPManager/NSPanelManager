from django_components import component
from django.template.context import Context

from web.mqttmanager_ipc import send_ipc_request

@component.register("nspanel_status_header")
class NSPanelHeader(component.Component):
    template_view = None

    def get_context_data(self, id, state, update_progress):
        return {
            "id": id,
            "state": state,
            "progress": update_progress
        }

    def get_template_name(self, context: Context):
        if self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name
        if url_name == "index":
            return "nspanel_status_header/nspanel_status_header_index.html"

    def get(self, request, view, nspanel_id):
        panel_status = send_ipc_request(F"nspanel/{nspanel_id}/status", {"command": "get"})
        self.template_view = view
        args = {
            "id": nspanel_id,
            "state": panel_status["state"],
            "update_progress": panel_status["update_progress"]
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
