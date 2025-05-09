from django_components import component
from django.template.context import Context

from web.mqttmanager_ipc import send_ipc_request

@component.register("nspanel_reboot_button")
class NSPanelRebootButton(component.Component):
    template_view = None

    def get_context_data(self, id, state):
        return {
            "id": id,
            "state": state,
        }

    def get_template_name(self, context: Context):
        if self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name
        if url_name == "htmx_partial_index_nspanels_section" or url_name == "index":
            return "nspanel_reboot_button/nspanel_reboot_button_index.html"
        elif url_name == "edit_nspanel":
            return "nspanel_reboot_button/nspanel_reboot_button_edit_nspanel.html"

    def get(self, request, view, nspanel_id):
        self.template_view = view
        panel_status = send_ipc_request(F"nspanel/{nspanel_id}/status", {"command": "get"})
        args = {
            "id": nspanel_id,
            "state": panel_status["state"],
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
