from django.contrib import admin
from .models import Room, NSPanel, Light, Scene, LightState

admin.site.register(Room)
admin.site.register(NSPanel)
admin.site.register(Light)
admin.site.register(Scene)
admin.site.register(LightState)
