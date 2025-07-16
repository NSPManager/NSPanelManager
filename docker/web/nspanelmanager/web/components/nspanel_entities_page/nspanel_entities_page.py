from django_components import component
from django.template.context import Context
from web.models import RoomEntitiesPage

@component.register("nspanel_entities_page")
class NSPanelEntitiesPage(component.Component):
    template_view = None

    def get_context_data(self, id, page, total_num_entity_pages):
        data = {
            "id": id,
            "page": page,
            "is_scenes_page": page.is_scenes_page,
            "is_global_scenes_page": page.room == None,
            "range": range(0, page.page_type),
            "entities": {},
            "total_num_entity_pages": total_num_entity_pages
        }

        for entity in page.entity_set.all():
            data["entities"][entity.room_view_position] = entity
            data["entities"][entity.room_view_position].controller = entity.entity_data.get("controller", "")
            data["entities"][entity.room_view_position].type = entity.entity_type
        for scene in page.scene_set.all():
            data["entities"][scene.room_view_position] = scene
            data["entities"][scene.room_view_position].controller = scene.scene_type
            data["entities"][scene.room_view_position].type = "scene"
        return data

    def get_template_name(self, context: Context):
        return "nspanel_entities_page/nspanel_entities_page_edit_room.html"

    def get(self, request, view, id, total_num_entity_pages):
        self.template_view = view
        page = RoomEntitiesPage.objects.get(id=id)
        args = {
            "id": id,
            "page": page,
            "range": range(0, page.page_type),
            "entities": {},
            "total_num_entity_pages": total_num_entity_pages
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
