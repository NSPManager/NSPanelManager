from django_components import component
from django.template.context import Context
from django.template import RequestContext
from web.models import RoomEntitiesPage, Room

@component.register("nspanel_room_entities_pages")
class NSPanelRoomEntitiesPages(component.Component):
    template_view = None

    def get_context_data(self, room_id, is_scenes_pages, is_global_scenes_page):
        print("Is global scenes page:", is_global_scenes_page)
        if is_global_scenes_page:
            room = None
        else:
            room = Room.objects.get(id=room_id)

        data = {
            "room": room,
            "is_scenes_pages": is_scenes_pages,
            "entity_pages": RoomEntitiesPage.objects.filter(room=room, is_scenes_page=is_scenes_pages).order_by('display_order'),
        }
        return data

    def get_template_name(self, context: Context):
        return "nspanel_room_entities_pages/nspanel_room_entities_pages_edit_room.html"

    def get(self, request, view, room_id, is_scenes_pages, is_global_scenes_page):
        self.template_view = view
        args = {
            "room_id": room_id,
            "is_scenes_pages": is_scenes_pages,
            "is_global_scenes_page": is_global_scenes_page,
        }
        context = RequestContext(request, {"view": view})
        return self.render_to_response(context=context, kwargs=args)

    class Media:
        css = "css/main.css"
