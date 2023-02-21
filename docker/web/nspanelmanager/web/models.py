from django.db import models
from datetime import datetime
import django.utils


class Room(models.Model):
    friendly_name = models.CharField(max_length=30)


class NSPanel(models.Model):
    mac_address = models.CharField(max_length=17)
    friendly_name = models.CharField(max_length=100)
    last_seen = models.DateTimeField(default=django.utils.timezone.now)
    room = models.ForeignKey(Room, on_delete=models.CASCADE)


class Light(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    can_dim = models.BooleanField(default=False)
    can_rgb = models.BooleanField(default=False)
    can_color_temperature = models.BooleanField(default=False)
    cmd_topic = models.CharField(max_length=255)
    state_topic = models.CharField(max_length=255)
