from datetime import datetime
from logging import NullHandler

import django.utils
from django.db import models
from requests.models import default_hooks


class Settings(models.Model):
    name = models.CharField(max_length=255, unique=True)
    value = models.CharField(max_length=255)

    def __str__(self) -> str:
        return self.name


class NSPanelSettings(models.Model):
    nspanel = models.ForeignKey("NSPanel", on_delete=models.CASCADE)
    name = models.CharField(max_length=255)
    value = models.CharField(max_length=255)

    def __str__(self) -> str:
        return f"{self.nspanel.friendly_name}::{self.name}"


class Room(models.Model):
    # Get the next free number in the ordering
    def number():
        no = Room.objects.all().count()
        if no == None:
            return 1
        else:
            return no + 1

    friendly_name = models.CharField(max_length=30)
    displayOrder = models.IntegerField(default=number)
    room_temp_provider = models.CharField(max_length=255, default="")
    room_temp_sensor = models.CharField(max_length=255, default="")

    def __str__(self) -> str:
        return "room_obj: " + str(self.id) + "::" + self.friendly_name


class RoomEntitiesPage(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE, null=True)
    display_order = models.IntegerField()
    page_type = models.IntegerField()  # Is this page displaying 4, 8 or 12 entities?
    is_scenes_page = models.BooleanField(default=False)


class NSPanel(models.Model):
    class NSPanelModel(models.TextChoices):
        SONOFF = "sonoff"
        CUSTOM = "custom"
        WEB = "web"

    mac_address = models.CharField(max_length=17)
    friendly_name = models.CharField(max_length=100)
    version = models.CharField(max_length=15, default="")
    model = models.CharField(max_length=32, choices=NSPanelModel.choices)
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    button1_mode = models.IntegerField(default=0)
    button1_detached_mode_entity = models.ForeignKey("Entity", on_delete=models.SET_NULL, blank=True, null=True, related_name="+")
    button2_mode = models.IntegerField(default=0)
    button2_detached_mode_entity = models.ForeignKey("Entity", on_delete=models.SET_NULL, blank=True, null=True, related_name="+")
    md5_firmware = models.CharField(max_length=64, default="")
    md5_data_file = models.CharField(max_length=64, default="")
    md5_tft_file = models.CharField(max_length=64, default="")
    denied = models.BooleanField(default=False)
    accepted = models.BooleanField(default=False)

    def __str__(self) -> str:
        return self.friendly_name


def _default_nspanel_status_data():
    pass


class RelayGroup(models.Model):
    friendly_name = models.CharField(max_length=255, default="")
    register_as_light = models.BooleanField(default=False)


class RelayGroupBinding(models.Model):
    relay_group = models.ForeignKey(RelayGroup, on_delete=models.CASCADE, null=True, default=None)
    nspanel = models.ForeignKey(NSPanel, on_delete=models.CASCADE)
    relay_num = models.IntegerField(default=1)


class Entity(models.Model):
    class EntityType(models.TextChoices):
        LIGHT = "light"
        SWITCH = "switch"
        BUTTON = "button"
        THERMOSTAT = "thermostat"
        MEDIA_PLAYER = "media_player"
        COVER = "cover"

    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    friendly_name = models.CharField(max_length=255, default="")
    entities_page = models.ForeignKey(RoomEntitiesPage, on_delete=models.CASCADE, null=True)
    room_view_position = models.IntegerField(default=0)
    entity_type = models.CharField(max_length=64, choices=EntityType)
    entity_data = models.JSONField(default=dict)


class Scene(models.Model):
    friendly_name = models.CharField(max_length=32)
    # The name of the scene in Home Assistant or OpenHAB
    backend_name = models.CharField(max_length=32, null=True, default=None)
    scene_type = models.CharField(max_length=64)
    room = models.ForeignKey(Room, null=True, blank=True, on_delete=models.CASCADE)
    room_view_position = models.IntegerField()
    entities_page = models.ForeignKey(RoomEntitiesPage, on_delete=models.CASCADE, null=True)
    # Custom icon: the private use character of an IconPackIcon, or empty for the default scene icon.
    icon = models.CharField(max_length=8, default="", blank=True)


class IconPackIcon(models.Model):
    """
    A custom icon chosen for an entity, scene or thermostat mode: a Material Design Icon, and the
    code point in Unicode's private use area (U+E000-U+F8FF) it is sent to the panels as. The
    panels draw it from the icon pack, see web/icon_pack.py. Code points never change once given.
    """

    name = models.CharField(max_length=128, unique=True)  # MDI name, e.g. "lamp"
    code_point = models.IntegerField(unique=True)


class LightState(models.Model):
    light = models.ForeignKey(Entity, on_delete=models.CASCADE)
    scene = models.ForeignKey(Scene, on_delete=models.CASCADE)
    color_mode = models.CharField(max_length=32, default="dimmer")
    light_level = models.IntegerField(default=0)
    color_temperature = models.IntegerField(default=4000)
    hue = models.IntegerField(default=0)
    saturation = models.IntegerField(default=0)


class Message(models.Model):
    title = models.CharField(max_length=255)
    message = models.TextField()
    read = models.BooleanField(default=False)
