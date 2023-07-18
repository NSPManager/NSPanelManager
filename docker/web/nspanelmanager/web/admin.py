from django.contrib import admin
from .models import Room, NSPanel, Light, Scene, LightState, Settings, NSPanelSettings

admin.site.register(Room)
admin.site.register(NSPanel)
admin.site.register(Light)
admin.site.register(Scene)
admin.site.register(LightState)
admin.site.register(Settings)
admin.site.register(NSPanelSettings)
