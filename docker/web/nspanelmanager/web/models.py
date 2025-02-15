from requests.models import default_hooks
from django.db import models
from datetime import datetime
import django.utils


class Settings(models.Model):
    name = models.CharField(max_length=255)
    value = models.CharField(max_length=255)

    def __str__(self) -> str:
        return self.name


class NSPanelSettings(models.Model):
    nspanel = models.ForeignKey("NSPanel", on_delete=models.CASCADE)
    name = models.CharField(max_length=255)
    value = models.CharField(max_length=255)

    def __str__(self) -> str:
        return F"{self.nspanel.friendly_name}::{self.name}"


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

    def __str__(self) -> str:
        return self.friendly_name

    """Get a protobuf_general_pb2.RoomSettings object populated with settings"""
    def get_protobuf_object(self):
        from web.protobuf import protobuf_formats_pb2, protobuf_general_pb2, protobuf_mqttmanager_pb2
        room = protobuf_general_pb2.RoomSettings()
        room.id = self.id
        room.name = self.friendly_name
        for light in self.light_set.all():
            room.light_ids.append(light.id)
        for scene in self.scene_set.all():
            room.scene_ids.append(scene.id)
        for page in self.roomentitiespage_set.all():
            room.entity_page_ids.append(page.id)
        return room

class RoomEntitiesPage(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    display_order = models.IntegerField()
    page_type = models.IntegerField() # Is this page displaying 4, 8 or 12 entities?
    is_scenes_page = models.BooleanField(default=False)

    def get_protobuf_object(self):
        from web.protobuf import protobuf_formats_pb2, protobuf_general_pb2, protobuf_mqttmanager_pb2
        ret_buf = protobuf_general_pb2.RoomEntitiesPageSettings()
        ret_buf.id = self.id
        ret_buf.page_type = self.page_type
        ret_buf.display_order = self.display_order
        ret_buf.room_id = self.room.id
        ret_buf.is_scenes_page = self.is_scenes_page

        # Add assigned lights
        for light in self.light_set.all():
            wrapper = ret_buf.entities.add()
            wrapper.light.id = light.id
            wrapper.light.name = light.friendly_name
            wrapper.light.room_view_position = light.room_view_position
        return ret_buf


class NSPanel(models.Model):
    mac_address = models.CharField(max_length=17)
    friendly_name = models.CharField(max_length=100)
    version = models.CharField(max_length=15, default="")
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    button1_mode = models.IntegerField(default=0)
    button1_detached_mode_light = models.ForeignKey(
        "Light", on_delete=models.SET_NULL, blank=True, null=True, related_name="+")
    register_relay1_as_light = models.BooleanField(default=False)
    button2_mode = models.IntegerField(default=0)
    button2_detached_mode_light = models.ForeignKey(
        "Light", on_delete=models.SET_NULL, blank=True, null=True, related_name="+")
    register_relay2_as_light = models.BooleanField(default=False)
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


class Light(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    friendly_name = models.CharField(max_length=255, default="")
    # "home_assistant", "openhab" or "manual":
    type = models.CharField(max_length=16, default="manual")
    is_ceiling_light = models.BooleanField(default=False)
    controlled_by_nspanel_main_page = models.BooleanField(default=True)
    can_dim = models.BooleanField(default=False)
    can_rgb = models.BooleanField(default=False)
    can_color_temperature = models.BooleanField(default=False)
    home_assistant_name = models.CharField(max_length=255, default="")
    openhab_name = models.CharField(max_length=255, default="")
    openhab_control_mode = models.CharField(max_length=32, default="dimmer")
    openhab_item_switch = models.CharField(max_length=255, default="")
    openhab_item_dimmer = models.CharField(max_length=255, default="")
    openhab_item_color_temp = models.CharField(max_length=255, default="")
    openhab_item_rgb = models.CharField(max_length=255, default="")
    room_view_position = models.IntegerField()
    entities_page = models.ForeignKey(RoomEntitiesPage, on_delete=models.CASCADE)

    def __str__(self) -> str:
        if self.room:
            return F"{self.room.friendly_name} -> {self.friendly_name}"
        else:
            return F"??? -> {self.friendly_name}"

    """Get a protobuf_general_pb2.LightSettings object populated with settings."""
    def get_protobuf_object(self):
        from web.protobuf import protobuf_formats_pb2, protobuf_general_pb2, protobuf_mqttmanager_pb2
        proto_light = protobuf_general_pb2.LightSettings()
        proto_light.id = self.id
        if self.room:
            proto_light.room_id = self.room.id
        else:
            proto_light.room_id = -1
        proto_light.name = self.friendly_name
        proto_light.type = self.type
        proto_light.is_ceiling_light = self.is_ceiling_light
        proto_light.can_dim = self.can_dim
        proto_light.can_color_temperature = self.can_color_temperature
        proto_light.can_rgb = self.can_rgb
        proto_light.entities_page_id = self.entities_page.id
        proto_light.entities_page_room_view_position = self.room_view_position
        proto_light.controlled_from_main_page = self.controlled_by_nspanel_main_page

        if proto_light.type == "home_assistant":
            proto_light.home_assistant_name = self.home_assistant_name
        elif proto_light.type == "openhab":
            proto_light.openhab_name = self.openhab_name
            proto_light.openhab_control_mode = self.openhab_control_mode
            proto_light.openhab_item_switch = self.openhab_item_switch
            proto_light.openhab_item_dimmer = self.openhab_item_dimmer
            proto_light.openhab_item_color_temp = self.openhab_item_color_temp
            proto_light.openhab_item_rgb = self.openhab_item_rgb

        return proto_light

class Switch(models.Model):
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    entities_page = models.ForeignKey(RoomEntitiesPage, on_delete=models.CASCADE)
    room_view_position = models.IntegerField()
    friendly_name= models.CharField(max_length=32)

    # "home_assistant", "openhab" or "manual":
    type = models.CharField(max_length=16, default="manual")
    home_assistant_name = models.CharField(max_length=128, default="")
    openhab_name = models.CharField(max_length=255, default="")
    openhab_item_switch = models.CharField(max_length=255, default="")



class Scene(models.Model):
    friendly_name = models.CharField(max_length=32)
    # The name of the scene in Home Assistant or OpenHAB
    backend_name = models.CharField(max_length=32, null=True, default=None)
    scene_type = models.CharField(max_length=64)
    room = models.ForeignKey(
        Room, null=True, blank=True, on_delete=models.CASCADE)


class LightState(models.Model):
    light = models.ForeignKey(Light, on_delete=models.CASCADE)
    scene = models.ForeignKey(Scene, on_delete=models.CASCADE)
    color_mode = models.CharField(max_length=32, default="dimmer")
    light_level = models.IntegerField(default=0)
    color_temperature = models.IntegerField(default=4000)
    hue = models.IntegerField(default=0)
    saturation = models.IntegerField(default=0)
