from django_components import component
from django.template.context import Context

from web.mqttmanager_ipc import send_ipc_request

@component.register("nspanel_status_ram_usage")
class NSPanelStatusRamUsage(component.Component):
    template_view = None

    def get_context_data(self, id, state, ram_usage):
        return {
            "id": id,
            "state": state,
            "ram_usage": ram_usage,
        }

    def get_template_name(self, context: Context):
            if self.template_view:
                url_name = self.template_view
            else:
                url_name = context.request.resolver_match.url_name
            if url_name == "htmx_partial_index_nspanels_section" or url_name == "index":
                return "nspanel_status_ram_usage/nspanel_status_ram_usage_index.html"
            elif url_name == "edit_nspanel":
                return "nspanel_status_ram_usage/nspanel_status_ram_usage_edit_nspanel.html"

    def get(self, request, view, nspanel_id):
        panel_status = send_ipc_request(F"nspanel/{nspanel_id}/status", {"command": "get"})
        self.template_view = view
        args = {
            "id": nspanel_id,
            "state": panel_status["state"],
            "ram_usage": panel_status["ram_usage"],
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
