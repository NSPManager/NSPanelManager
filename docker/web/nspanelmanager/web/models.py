from django.db import models
from datetime import datetime
import django.utils


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


class NSPanel(models.Model):
    mac_address = models.CharField(max_length=17)
    friendly_name = models.CharField(max_length=100)
    last_seen = models.DateTimeField(default=django.utils.timezone.now)
    ip_address = models.CharField(max_length=15, default="")
    version = models.CharField(max_length=15, default="")
    room = models.ForeignKey(Room, on_delete=models.CASCADE)


class Light(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    can_dim = models.BooleanField(default=False)
    can_rgb = models.BooleanField(default=False)
    can_color_temperature = models.BooleanField(default=False)
    cmd_topic = models.CharField(max_length=255)
    state_topic = models.CharField(max_length=255)
