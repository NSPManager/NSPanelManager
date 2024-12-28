from django_components import component
from django.template.context import Context
from django.template import RequestContext
from web.models import RoomEntitiesPage, Room

@component.register("nspanel_room_entities_pages")
class NSPanelRoomEntitiesPages(component.Component):
    template_view = None

    def get_context_data(self, room_id):
        room = Room.objects.get(id=room_id)
        data = {
            "room": room,
            "entity_pages": RoomEntitiesPage.objects.filter(room=room)
        }
        return data

    def get_template_name(self, context: Context):
        if "view" in context:
            url_name = context["view"]
        elif self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name

        if url_name == "edit_room":
            return "nspanel_room_entities_pages/nspanel_room_entities_pages_edit_room.html"
        else:
            print("ERROR! Could not determine URL name for component nspanel_entities_page. url_name: " + url_name)

    def get(self, request, view, room_id):
        self.template_view = view
        args = {
            "room_id": room_id,
        }
        context = RequestContext(request, {"view": view})
        return self.render_to_response(context=context, kwargs=args)

    class Media:
        css = "css/main.css"
