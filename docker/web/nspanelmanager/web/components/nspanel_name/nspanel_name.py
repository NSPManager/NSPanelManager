from django_components import component
from django.template.context import Context

@component.register("nspanel_name")
class NSPanelName(component.Component):
    template_view = None

    def get_context_data(self, id, name):
        return {
            "id": id,
            "name": name,
        }

    def get_template_name(self, context: Context):
        if self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name
        if url_name == "htmx_partial_index_nspanels_section" or url_name == "index":
            return "nspanel_name/nspanel_name_index.html"

    class Media:
        css = "css/main.css"
