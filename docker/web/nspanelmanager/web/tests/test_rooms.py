import json

from django.test import Client, TestCase
from django.urls import reverse

from web.models import Entity, NSPanel, Room, RoomEntitiesPage, Scene

from .harness import NSPMTestCase


class RoomUITests(NSPMTestCase):
    """Rooms created/edited/deleted through the web UI views."""

    def test_create_room(self):
        response = self.client.post(reverse("save_new_room"), {"friendly_name": "Kitchen"})

        room = Room.objects.get(friendly_name="Kitchen")
        self.assertRedirects(response, reverse("edit_room", kwargs={"room_id": room.id}), fetch_redirect_response=False)
        # A new room always gets one entities page and one scenes page.
        self.assertEqual(RoomEntitiesPage.objects.filter(room=room, is_scenes_page=False).count(), 1)
        self.assertEqual(RoomEntitiesPage.objects.filter(room=room, is_scenes_page=True).count(), 1)
        self.assertManagerReloaded(times=1)

    def test_create_room_without_name_is_rejected(self):
        response = self.client.post(reverse("save_new_room"), {})

        self.assertRejected(response)
        self.assertFalse(Room.objects.exists())
        self.assertManagerNotReloaded()

    def test_rename_room(self):
        room = self.make_room("Kitchen")

        response = self.client.post(
            reverse("update_room_form", kwargs={"room_id": room.id}),
            {"friendly_name": "Galley", "room_temp_provider": "home_assistant", "room_temp_sensor": "sensor.galley_temp"},
        )

        self.assertEqual(response.status_code, 302)
        room.refresh_from_db()
        self.assertEqual(room.friendly_name, "Galley")
        self.assertEqual(room.room_temp_sensor, "sensor.galley_temp")
        self.assertManagerReloaded(times=1)

    def test_reorder_rooms(self):
        a, b, c = self.make_room("A"), self.make_room("B"), self.make_room("C")

        response = self.client.post(reverse("htmx_partial_reorder_rooms"), {"htmx_form_save_rooms_order_field": json.dumps({"rooms": [c.id, a.id, b.id]})})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(list(Room.objects.order_by("displayOrder").values_list("friendly_name", flat=True)), ["C", "A", "B"])
        self.assertManagerReloaded(times=1)

    def test_reorder_rooms_when_a_room_has_a_panel(self):
        # Issue #337: the re-rendered room list shows each panel's status badge, which used to 500.
        a, b = self.make_room("A"), self.make_room("B")
        self.make_panel(a, accepted=True)

        response = self.client.post(reverse("htmx_partial_reorder_rooms"), {"htmx_form_save_rooms_order_field": json.dumps({"rooms": [b.id, a.id]})})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(list(Room.objects.order_by("displayOrder").values_list("friendly_name", flat=True)), ["B", "A"])
        self.assertManagerReloaded(times=1)

    def test_reorder_rooms_with_missing_or_empty_payload_is_rejected(self):
        self.make_room("A")
        for payload in ({}, {"htmx_form_save_rooms_order_field": json.dumps({})}, {"htmx_form_save_rooms_order_field": json.dumps({"rooms": []})}):
            with self.subTest(payload=payload):
                self.assertRejected(self.client.post(reverse("htmx_partial_reorder_rooms"), payload))
        self.assertManagerNotReloaded()


class RoomRESTTests(NSPMTestCase):
    """The /rest/rooms JSON API used by external tools."""

    def test_create_room(self):
        response = self.post_json(reverse("rest_v2_rooms"), {"friendly_name": "Office"})

        self.assertEqual(response.status_code, 201)
        room_id = response.json()["id"]
        self.assertEqual(Room.objects.get(id=room_id).friendly_name, "Office")

    def test_create_room_rejects_bad_payloads(self):
        print("BAD PAYLOADS")
        for body in ({}, {"name": "wrong key"}, "not json"):
            with self.subTest(body=body):
                print(f"Testing: {body}")
                response = self.post_json(reverse("rest_v2_rooms"), body)
                print(response.json())
                self.assertEqual(response.status_code, 400)
                self.assertEqual(response.json()["status"], "error")
        self.assertFalse(Room.objects.exists())
        print("END BAD PAYLOADS")

    def test_rooms_endpoint_rejects_unsupported_methods(self):
        self.assertEqual(self.client.delete(reverse("rest_v2_rooms")).status_code, 405)
        # room = self.make_room()
        # self.assertEqual(self.client.get(reverse("rest_v2_rooms", kwargs={"room_id": room.id})).status_code, 405)

    def test_delete_room(self):
        room = self.make_room("Office")
        self.make_room("Livingroom")
        self.make_light(room)

        response = self.client.delete(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id}))
        self.assertEqual(response.status_code, 204)
        self.assertEqual(Room.objects.count(), 1)

    def test_delete_last_room_without_panels_is_rejected(self):
        room = self.make_room("Office")

        response = self.client.delete(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id}))

        self.assertEqual(response.status_code, 400)
        self.assertTrue(Room.objects.exists())
        self.assertManagerReloaded(times=0)

    def test_delete_last_room_with_panels_is_rejected(self):
        room = self.make_room("Office")
        panel = self.make_panel(room)

        response = self.client.delete(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id}))

        self.assertEqual(response.status_code, 400)
        self.assertTrue(Room.objects.filter(id=room.id).exists())
        self.assertTrue(NSPanel.objects.filter(id=panel.id).exists())
        self.assertManagerNotReloaded()

    def test_delete_unknown_room_is_rejected(self):
        room = self.make_room("Office")

        response = self.client.delete(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id + 100}))

        self.assertRejected(response)
        self.assertTrue(Room.objects.filter(id=room.id).exists())

    def test_list_rooms(self):
        office = self.make_room("Office")
        lounge = self.make_room("Lounge")

        response = self.client.get(reverse("rest_v2_rooms"))

        self.assertEqual(response.status_code, 200)
        self.assertEqual([(r["id"], r["friendly_name"]) for r in response.json()], [(office.id, "Office"), (lounge.id, "Lounge")])
        self.assertEqual(set(response.json()[0]), {"id", "friendly_name", "displayOrder", "room_temp_provider", "room_temp_sensor"})

    def test_get_single_room(self):
        self.make_room("Office")
        lounge = self.make_room("Lounge")

        response = self.client.get(reverse("rest_v2_rooms_detail", kwargs={"pk": lounge.id}))

        self.assertEqual([(response.json()["id"], response.json()["friendly_name"])], [(lounge.id, "Lounge")])

    def test_update_room(self):
        room = self.make_room("Office")

        response = self.patch_json(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id}), {"friendly_name": "Study", "room_temp_provider": "home_assistant", "room_temp_sensor": "sensor.study"})

        self.assertEqual(response.status_code, 200)
        room.refresh_from_db()
        self.assertEqual((room.friendly_name, room.room_temp_provider, room.room_temp_sensor), ("Study", "home_assistant", "sensor.study"))
        self.assertManagerReloaded(times=1)

    def test_update_room_display_order(self):
        # display_order is the key GET /rest/rooms returns. room_put used to check for "displayOrder" instead, so it was ignored.
        room = self.make_room("Office")

        response = self.patch_json(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id}), {"displayOrder": 7})

        self.assertEqual(response.status_code, 200)
        room.refresh_from_db()
        self.assertEqual(room.displayOrder, 7)

    def test_delete_room_reloads_manager(self):
        room = self.make_room("Office")
        self.make_room("Lounge")

        self.client.delete(reverse("rest_v2_rooms_detail", kwargs={"pk": room.id}))

        self.assertManagerReloaded(times=1)

    def test_delete_room_cascades_and_rehomes_panels(self):
        kitchen = self.make_room("Kitchen")
        lounge = self.make_room("Lounge")
        panel = self.make_panel(kitchen)
        self.make_light(kitchen)
        self.make_scene(kitchen)

        self.client.delete(reverse("rest_v2_rooms_detail", kwargs={"pk": kitchen.id}))

        self.assertFalse(Room.objects.filter(id=kitchen.id).exists())
        self.assertFalse(Entity.objects.exists())
        self.assertFalse(Scene.objects.exists())
        self.assertFalse(RoomEntitiesPage.objects.filter(room_id=kitchen.id).exists())
        # The panel is not deleted with its room; it moves to another room.
        panel.refresh_from_db()
        self.assertEqual(panel.room, lounge)
        self.assertManagerReloaded(times=1)

    def test_created_room_has_entity_pages(self):
        response = self.post_json(reverse("rest_v2_rooms"), {"friendly_name": "Office"})
        self.assertEqual(response.status_code, 201)

        room_id = response.json()["id"]
        self.assertEqual(RoomEntitiesPage.objects.filter(room_id=room_id).count(), 2)

    def test_create_room_rejects_overlong_name(self):
        response = self.post_json(reverse("rest_v2_rooms"), {"friendly_name": "x" * 100})

        self.assertRejected(response)
        self.assertFalse(Room.objects.exists())
