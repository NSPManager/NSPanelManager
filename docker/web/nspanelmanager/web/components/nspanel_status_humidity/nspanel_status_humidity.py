from django.template.context import Context
from django_components import component
from web.models import NSPanel
from web.settings_helper import get_setting_with_default


@component.register("nspanel_status_humidity")
class NSPanelStatusHumidity(component.Component):
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
            return "nspanel_status_humidity/nspanel_status_humidity_edit_nspanel.html"

    def get(self, request, view, nspanel_id):
        self.template_view = view
        args = {
            "id": nspanel_id,
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
