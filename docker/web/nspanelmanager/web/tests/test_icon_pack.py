"""
Custom icons: registering them, and the icon pack the panels download.
"""

import json
import struct

from django.urls import reverse

from web.icon_pack import PRIVATE_USE_FIRST, clean_icon
from web.models import Entity, IconPackIcon, Scene

from .harness import NSPMTestCase


class IconPackTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")

    def register(self, name):
        return self.client.post(reverse("rest_icons"), json.dumps({"name": name}), content_type="application/json")

    def pack_url(self, sizes="48,28,30"):
        return reverse("download_icon_pack", kwargs={"panel_id": 1}) + f"?sizes={sizes}"

    def test_register_gives_stable_private_use_code_points(self):
        lamp = self.register("lamp").json()
        fan = self.register("fan").json()

        self.assertEqual(lamp["code_point"], PRIVATE_USE_FIRST)
        self.assertEqual(fan["code_point"], PRIVATE_USE_FIRST + 1)
        self.assertEqual(lamp["character"], chr(PRIVATE_USE_FIRST))
        self.assertEqual(self.register("lamp").json()["code_point"], PRIVATE_USE_FIRST)  # Registering again keeps the code point

    def test_unknown_icon_is_rejected(self):
        self.assertEqual(self.register("not-an-icon").status_code, 400)
        self.assertFalse(IconPackIcon.objects.exists())

    def test_list_icons(self):
        self.register("lamp")

        icons = self.client.get(reverse("rest_icons")).json()["icons"]

        self.assertEqual([icon["name"] for icon in icons], ["lamp"])

    def test_only_registered_custom_icons_are_kept(self):
        lamp = chr(self.register("lamp").json()["code_point"])

        self.assertEqual(clean_icon(lamp), lamp)
        self.assertEqual(clean_icon(chr(PRIVATE_USE_FIRST + 100)), "")  # Not registered
        self.assertEqual(clean_icon("s"), "")  # Built-in characters are only for thermostat modes
        self.assertEqual(clean_icon("s", allow_builtin=True), "s")

    def test_no_pack_without_custom_icons(self):
        self.register("lamp")  # Registered but not used anywhere

        self.assertEqual(self.client.get(self.pack_url()).status_code, 404)

    def test_pack_holds_icons_of_entities_scenes_and_thermostat_modes(self):
        lamp = chr(self.register("lamp").json()["code_point"])
        snowflake = chr(self.register("snowflake").json()["code_point"])
        palette = chr(self.register("palette").json()["code_point"])
        self.register("fan")  # Not used
        Entity.objects.create(room=self.room, entity_type="light", entity_data={"controller": "home_assistant", "icon": lamp})
        Entity.objects.create(
            room=self.room, entity_type="thermostat", entity_data={"controller": "home_assistant", "hvac_modes": [{"icon": snowflake, "label": "Cool", "value": "cool"}]}
        )
        Scene.objects.create(friendly_name="Party", scene_type="nspm_scene", room=self.room, room_view_position=0, icon=palette)

        response = self.client.get(self.pack_url("48,28"))

        self.assertEqual(response.status_code, 200)
        data = response.content
        magic, version, font_count, glyph_count = struct.unpack_from("<4sBBH", data)
        self.assertEqual((magic, version, font_count, glyph_count), (b"NSIP", 1, 2, 3))
        first_font_size = struct.unpack_from("<H", data, 12)[0]
        self.assertEqual(first_font_size, 48)
        code_points = [struct.unpack_from("<I", data, 12 + 8 + i * 16)[0] for i in range(glyph_count)]
        self.assertEqual(code_points, sorted(ord(icon) for icon in (lamp, snowflake, palette)))

    def test_pack_holds_the_off_icon_of_lights_and_switches(self):
        on = chr(self.register("lightbulb-on").json()["code_point"])
        off = chr(self.register("lightbulb-off").json()["code_point"])
        Entity.objects.create(room=self.room, entity_type="light", entity_data={"controller": "home_assistant", "icon": on, "icon_off": off})

        data = self.client.get(self.pack_url("48")).content

        glyph_count = struct.unpack_from("<H", data, 6)[0]
        code_points = [struct.unpack_from("<I", data, 12 + 8 + i * 16)[0] for i in range(glyph_count)]
        self.assertEqual(code_points, [ord(on), ord(off)])

    def test_unchanged_pack_is_not_sent_again(self):
        lamp = chr(self.register("lamp").json()["code_point"])
        Entity.objects.create(room=self.room, entity_type="light", entity_data={"controller": "home_assistant", "icon": lamp})
        etag = self.client.get(self.pack_url())["ETag"]

        response = self.client.get(self.pack_url(), headers={"If-None-Match": etag})

        self.assertEqual(response.status_code, 304)

    def test_bad_sizes_are_rejected(self):
        self.assertEqual(self.client.get(self.pack_url("0")).status_code, 400)
        self.assertEqual(self.client.get(self.pack_url("48,abc")).status_code, 400)
