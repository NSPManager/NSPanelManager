"""
Smaller endpoints: messages, Home Assistant / OpenHAB entity lookups used by the entity
modal, hostname lookup and read-only listings.
"""

import json
import socket
from unittest import mock

from django.urls import reverse

from web.models import Message

from .harness import NSPMTestCase


class MessageTests(NSPMTestCase):
    def test_unread_messages_are_flagged_and_can_be_marked_read(self):
        message = Message.objects.create(title="Update available", message="v2.1 is out")
        self.assertTrue(self.client.get(reverse("index")).context["has_unread_messages"])

        response = self.client.get(reverse("htmx_mark_message_read", kwargs={"message_id": message.id}))

        self.assertContains(response, "Update available")
        message.refresh_from_db()
        self.assertTrue(message.read)
        self.assertFalse(self.client.get(reverse("index")).context["has_unread_messages"])

    def test_mark_unknown_message_is_rejected(self):
        self.assertEqual(self.client.get(reverse("htmx_mark_message_read", kwargs={"message_id": 999})).status_code, 500)


class EntitySourceLookupTests(NSPMTestCase):
    """The lists the React entity modal offers when picking a Home Assistant / OpenHAB item."""

    def test_home_assistant_entities(self):
        self.home_assistant.add("light.kitchen", friendly_name="Kitchen")
        self.home_assistant.add("switch.fan", friendly_name="Fan")

        response = self.client.get(reverse("rest_get_home_assistant_entities"))

        self.assertEqual(response.status_code, 200)
        self.assertEqual([(i["item_id"], i["label"]) for i in response.json()["items"]], [("light.kitchen", "Kitchen"), ("switch.fan", "Fan")])

    def test_home_assistant_entities_filtered_by_domain(self):
        self.home_assistant.add("light.kitchen")
        self.home_assistant.add("switch.fan")
        self.home_assistant.add("climate.lounge")

        response = self.client.get(reverse("rest_get_home_assistant_entities"), {"filter": json.dumps({"type": ["switch", "climate"]})})

        self.assertEqual([i["item_id"] for i in response.json()["items"]], ["switch.fan", "climate.lounge"])

    def test_entity_without_friendly_name_is_labelled_by_id(self):
        self.home_assistant.add("light.unnamed")

        self.assertEqual(self.client.get(reverse("rest_get_home_assistant_entities")).json()["items"][0]["label"], "light.unnamed")

    def test_home_assistant_error_is_reported(self):
        self.home_assistant.fail = True

        response = self.client.get(reverse("rest_get_home_assistant_entities"))

        self.assertEqual(response.json()["items"], [])
        self.assertTrue(response.json()["errors"])

    def test_home_assistant_not_configured_is_reported(self):
        from web.settings_helper import set_setting_value

        set_setting_value("home_assistant_token", "")

        self.assertTrue(self.client.get(reverse("rest_get_home_assistant_entities")).json()["errors"])

    def test_openhab_items_include_scenes(self):
        self.openhab_items = [{"type": "openhab", "label": "Lamp", "item_id": "Lamp"}]

        response = self.client.get(reverse("rest_get_openhab_items"))

        # The fake returns the same list for items and for scenes (rules), so both are merged in.
        self.assertEqual([i["item_id"] for i in response.json()["items"]], ["Lamp", "Lamp"])

    def test_lookups_are_read_only(self):
        self.assertEqual(self.client.post(reverse("rest_get_home_assistant_entities")).status_code, 405)
        self.assertEqual(self.client.post(reverse("rest_get_openhab_items")).status_code, 405)


class HostnameLookupTests(NSPMTestCase):
    def test_resolves_hostname(self):
        with mock.patch("web.rest.socket.gethostbyname", return_value="192.168.1.20") as lookup:
            response = self.post_json(reverse("get_ip_by_hostname"), {"hostname": "mqtt.local"})

        lookup.assert_called_once_with("mqtt.local")
        self.assertEqual(response.json(), {"ip": "192.168.1.20"})

    def test_unknown_hostname_is_404(self):
        with mock.patch("web.rest.socket.gethostbyname", side_effect=socket.gaierror):
            self.assertEqual(self.post_json(reverse("get_ip_by_hostname"), {"hostname": "nope.invalid"}).status_code, 404)

    def test_missing_hostname_is_rejected(self):
        with mock.patch("web.rest.socket.gethostbyname") as lookup:
            self.assertEqual(self.post_json(reverse("get_ip_by_hostname"), {}).status_code, 500)
        lookup.assert_not_called()


class ReadOnlyListingTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")

    def test_room_entities(self):
        light = self.make_light(self.room, "Ceiling")
        self.make_light(self.make_room("Lounge"), "Lamp")

        response = self.client.get(reverse("rest_room_entities", kwargs={"room_id": self.room.id}))

        self.assertEqual([e["id"] for e in response.json()["entities"]], [light.id])

    def test_global_entities_pages(self):
        page = self.global_scenes_page()
        scene = self.make_scene(None, "Goodnight")

        response = self.client.get(reverse("rest_global_entities_pages"))

        pages = response.json()["entities_pages"]
        self.assertEqual([(p["id"], p["room_id"]) for p in pages], [(page.id, None)])
        self.assertEqual([s["id"] for s in pages[0]["scenes"]], [scene.id])

    def test_single_entities_page(self):
        light = self.make_light(self.room, slot=1)

        response = self.client.get(reverse("rest_room_entities_page", kwargs={"page_id": self.entities_page(self.room).id}))

        self.assertEqual([e["id"] for e in response.json()["entities"]], [light.id])

    def test_get_scene(self):
        scene = self.make_scene(self.room, "Dinner", backend_name="scene.dinner")

        result = self.client.get(reverse("rest_get_scene", kwargs={"scene_id": scene.id})).json()["result"]

        self.assertEqual((result["friendly_name"], result["backend_name"], result["room_id"]), ("Dinner", "scene.dinner", self.room.id))

    def test_scenes_listing(self):
        scene = self.make_scene(self.room, "Dinner")
        self.make_scene(None, "Goodnight")

        response = self.client.get(reverse("rest_lights"), {"room_id": self.room.id})

        self.assertEqual([s["scene_id"] for s in response.json()["scenes"]], [scene.id])

    def test_legacy_scene_and_room_config(self):
        scene = self.make_scene(self.room, "Dinner")

        self.assertEqual(self.client.get(reverse("get_scenes")).json()["scenes"][0]["scene_id"], scene.id)
        self.assertEqual(self.client.get(reverse("get_nspanel_config", kwargs={"room_id": self.room.id})).json()["name"], "Kitchen")

    def test_scenes_listing_filters_by_scene_id(self):
        scene = self.make_scene(self.room, "Dinner")
        self.make_scene(self.room, "Breakfast")

        response = self.client.get(reverse("rest_lights"), {"scene_id": scene.id})

        self.assertEqual([s["scene_id"] for s in response.json()["scenes"]], [scene.id])
