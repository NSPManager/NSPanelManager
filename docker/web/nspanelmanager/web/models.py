from django.db import models
from datetime import datetime
import django.utils


class Settings(models.Model):
    name = models.CharField(max_length=255)
    value = models.CharField(max_length=255)


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


def _default_nspanel_status_data():
    return {
        "rssi": 0,
        "mac": "??:??:??:??:??:??",
        "free_heap": 0
    };

class NSPanel(models.Model):
    mac_address = models.CharField(max_length=17)
    friendly_name = models.CharField(max_length=100)
    last_seen = models.DateTimeField(default=django.utils.timezone.now)
    ip_address = models.CharField(max_length=15, default="")
    version = models.CharField(max_length=15, default="")
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    status_data = models.JSONField(default=_default_nspanel_status_data)
    online_state = models.BooleanField(default=False)

class RoomViewPageLight(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    display_position = models.IntegerField(default=0)

class Light(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    friendly_name = models.CharField(max_length=255, default="")
    type = models.CharField(max_length=16, default="manual") # "home_assistant", "openhab" or "manual"
    is_ceiling_light = models.BooleanField(default=False)
    can_dim = models.BooleanField(default=False)
    can_rgb = models.BooleanField(default=False)
    can_color_temperature = models.BooleanField(default=False)
    openhab_control_mode =models.CharField(max_length=32, default="dimmer") 
    openhab_item_switch = models.CharField(max_length=255, default="")
    openhab_item_dimmer = models.CharField(max_length=255, default="")
    openhab_item_color_temp = models.CharField(max_length=255, default="")
    room_view_page = models.ForeignKey(RoomViewPageLight, on_delete=models.CASCADE, null=True)