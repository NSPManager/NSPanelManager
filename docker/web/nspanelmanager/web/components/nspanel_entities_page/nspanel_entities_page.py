from django_components import component
from django.template.context import Context
from web.models import RoomEntitiesPage

@component.register("nspanel_entities_page")
class NSPanelEntitiesPage(component.Component):
    template_view = None

    def get_context_data(self, id, page):
        data = {
            "id": id,
            "page": page,
            "range": range(0, page.page_type),
            "entities": {}
        }

        for entity in page.light_set.all():
            data["entities"][entity.room_view_position] = entity
            data["entities"][entity.room_view_position].type = entity.__class__.__name__
        for entity in page.switch_set.all():
            data["entities"][entity.room_view_position] = entity
            data["entities"][entity.room_view_position].type = entity.__class__.__name__
        return data

    def get_template_name(self, context: Context):
        if "view" in context:
            url_name = context["view"]
        elif self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name

        if url_name == "edit_room":
            return "nspanel_entities_page/nspanel_entities_page_edit_room.html"
        else:
            print("ERROR! Could not determine URL name for component nspanel_entities_page")

    def get(self, request, view, id):
        self.template_view = view
        page = RoomEntitiesPage.objects.get(id=id)
        args = {
            "id": id,
            "page": page,
            "range": range(0, page.page_type),
            "entities": {}
        }
        for entity in page.light_set.all():
            args["entities"][entity.room_view_position] = entity
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
