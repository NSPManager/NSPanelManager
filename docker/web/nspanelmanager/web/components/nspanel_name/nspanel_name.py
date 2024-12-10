from django_components import component
from django.template.context import Context

@component.register("nspanel_name")
class NSPanelName(component.Component):
    # Dict of path_name: template_file. This is used to define different templates depending on actual page.
    template_name_dict = {
        "index": "nspanel_name/nspanel_name_index.html"
    }

    def get_context_data(self, id, name):
        return {
            "id": id,
            "name": name,
        }

    def get_template_name(self, context: Context):
        if context.request.resolver_match.url_name in self.template_name_dict:
            return self.template_name_dict[context.request.resolver_match.url_name]
        else:
            return None

    class Media:
        css = "css/main.css"
