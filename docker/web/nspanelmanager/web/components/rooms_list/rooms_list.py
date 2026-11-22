from django_components import component
from django.template.context import Context
from django.core.files.storage import FileSystemStorage
from django.template import RequestContext
import hashlib

from web.models import Room

@component.register("rooms_list")
class RoomsList(component.Component):
    template_name = "rooms_list.html"
    template_view = None

    def get_context_data(self):
        data = {
            "rooms": []
        }
        # Build dict of online and accepted NSPanels for each room
        for room in Room.objects.all().order_by("displayOrder"):
            data["rooms"].append({
                "room": room,
                "nspanels": room.nspanel_set.filter(accepted=True, denied=False),
            })
        return data

    def get(self, request):
        context = RequestContext(request, {})
        return self.render_to_response(context=context, kwargs={})

    class Media:
        css = "css/main.css"
