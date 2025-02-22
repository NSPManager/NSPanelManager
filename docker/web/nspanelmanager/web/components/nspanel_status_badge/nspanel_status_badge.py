from django_components import component
from django.template.context import Context

from web.mqttmanager_ipc import send_ipc_request
from web.models import NSPanel

@component.register("nspanel_status_badge")
class NSPanelStatusBadge(component.Component):
    template_view = None

    def get_context_data(self, id):
        panel = NSPanel.objects.get(id=id)
        print(F"Sending status request for panel {id}")
        panel_status = send_ipc_request(F"nspanel/{id}/status", {"command": "get"})
        if "status" not in panel_status or panel_status["status"] == "error":
            return {
                "id": id,
                "friendly_name": panel.friendly_name,
                "state": "error",
                "update_progress": 0,
            }
        else:
            return {
                "id": id,
                "friendly_name": panel.friendly_name,
                "state": panel_status["state"],
                "update_progress": panel_status["update_progress"],
            }

    def get_template_name(self, context: Context):
            if self.template_view:
                url_name = self.template_view
            else:
                url_name = context.request.resolver_match.url_name

            return "nspanel_status_badge/nspanel_status_badge_" + url_name + ".html"

    def get(self, request, view, nspanel_id):
        self.template_view = view
        args = {
            "id": nspanel_id,
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
