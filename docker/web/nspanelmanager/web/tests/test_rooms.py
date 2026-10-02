import json
from unittest import expectedFailure

from django.urls import reverse

from web.models import Entity, Room, RoomEntitiesPage, Scene

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

    def test_delete_room_cascades_and_rehomes_panels(self):
        kitchen = self.make_room("Kitchen")
        lounge = self.make_room("Lounge")
        panel = self.make_panel(kitchen)
        self.make_light(kitchen)
        self.make_scene(kitchen)

        self.client.get(reverse("delete_room", kwargs={"room_id": kitchen.id}))

        self.assertFalse(Room.objects.filter(id=kitchen.id).exists())
        self.assertFalse(Entity.objects.exists())
        self.assertFalse(Scene.objects.exists())
        self.assertFalse(RoomEntitiesPage.objects.filter(room_id=kitchen.id).exists())
        # The panel is not deleted with its room; it moves to another room.
        panel.refresh_from_db()
        self.assertEqual(panel.room, lounge)
        self.assertManagerReloaded(times=1)

    def test_last_room_cannot_be_deleted(self):
        room = self.make_room("Only room")

        self.client.get(reverse("delete_room", kwargs={"room_id": room.id}))

        self.assertTrue(Room.objects.filter(id=room.id).exists())
        self.assertManagerNotReloaded()

    def test_reorder_rooms(self):
        a, b, c = self.make_room("A"), self.make_room("B"), self.make_room("C")

        response = self.client.post(reverse("htmx_partial_reorder_rooms"), {"htmx_form_save_rooms_order_field": json.dumps({"rooms": [c.id, a.id, b.id]})})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(list(Room.objects.order_by("displayOrder").values_list("friendly_name", flat=True)), ["C", "A", "B"])
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
        response = self.post_json(reverse("rest_rooms_create"), {"name": "Office"})

        self.assertEqual(response.status_code, 200)
        room_id = response.json()["room_id"]
        self.assertEqual(Room.objects.get(id=room_id).friendly_name, "Office")

    def test_create_room_rejects_bad_payloads(self):
        for body in ({}, {"friendly_name": "wrong key"}, "not json"):
            with self.subTest(body=body):
                response = self.post_json(reverse("rest_rooms_create"), body)
                self.assertEqual(response.status_code, 500)
                self.assertEqual(response.json()["status"], "error")
        self.assertFalse(Room.objects.exists())

    def test_rooms_endpoint_rejects_unsupported_methods(self):
        self.assertEqual(self.client.delete(reverse("rest_rooms_create")).status_code, 405)
        room = self.make_room()
        self.assertEqual(self.client.get(reverse("rest_room_delete", kwargs={"room_id": room.id})).status_code, 405)

    def test_delete_room(self):
        room = self.make_room("Office")
        self.make_light(room)

        response = self.client.delete(reverse("rest_room_delete", kwargs={"room_id": room.id}))

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.json(), {"status": "ok", "room_id": room.id})
        self.assertFalse(Room.objects.exists())
        self.assertFalse(Entity.objects.exists())

    def test_delete_unknown_room_is_rejected(self):
        room = self.make_room("Office")

        response = self.client.delete(reverse("rest_room_delete", kwargs={"room_id": room.id + 100}))

        self.assertRejected(response)
        self.assertTrue(Room.objects.filter(id=room.id).exists())

    def test_list_rooms(self):
        office = self.make_room("Office")
        lounge = self.make_room("Lounge")

        response = self.client.get(reverse("rest_rooms_create"))

        self.assertEqual(response.status_code, 200)
        self.assertEqual([(r["id"], r["friendly_name"]) for r in response.json()["rooms"]], [(office.id, "Office"), (lounge.id, "Lounge")])
        self.assertEqual(set(response.json()["rooms"][0]), {"id", "friendly_name", "display_order", "room_temp_provider", "room_temp_sensor"})

    def test_get_single_room(self):
        self.make_room("Office")
        lounge = self.make_room("Lounge")

        response = self.client.get(reverse("rest_rooms_create"), {"id": lounge.id})

        self.assertEqual([(r["id"], r["friendly_name"]) for r in response.json()["rooms"]], [(lounge.id, "Lounge")])

    def test_update_room(self):
        room = self.make_room("Office")

        response = self.put_json(reverse("rest_rooms_create"), {"id": room.id, "friendly_name": "Study", "room_temp_provider": "home_assistant", "room_temp_sensor": "sensor.study"})

        self.assertEqual(response.status_code, 200)
        room.refresh_from_db()
        self.assertEqual((room.friendly_name, room.room_temp_provider, room.room_temp_sensor), ("Study", "home_assistant", "sensor.study"))
        self.assertManagerReloaded(times=1)

    @expectedFailure
    def test_update_room_display_order(self):
        # KNOWN BUG: room_put checks for "displayOrder" but reads data["display_order"], so
        # sending display_order (the key GET /rest/rooms returns) is silently ignored and
        # sending displayOrder raises KeyError and returns 500.
        room = self.make_room("Office")

        response = self.put_json(reverse("rest_rooms_create"), {"id": room.id, "display_order": 7})

        self.assertEqual(response.status_code, 200)
        room.refresh_from_db()
        self.assertEqual(room.displayOrder, 7)

    def test_delete_room_reloads_manager(self):
        room = self.make_room("Office")
        self.make_room("Lounge")

        self.client.delete(reverse("rest_room_delete", kwargs={"room_id": room.id}))

        self.assertManagerReloaded(times=1)

    def test_delete_room_rehomes_panels(self):
        office = self.make_room("Office")
        lounge = self.make_room("Lounge")
        panel = self.make_panel(office)

        self.client.delete(reverse("rest_room_delete", kwargs={"room_id": office.id}))

        panel.refresh_from_db()
        self.assertEqual(panel.room, lounge)

    def test_created_room_has_entity_pages(self):
        response = self.post_json(reverse("rest_rooms_create"), {"name": "Office"})

        room_id = response.json()["room_id"]
        self.assertEqual(RoomEntitiesPage.objects.filter(room_id=room_id).count(), 2)

    def test_create_room_rejects_overlong_name(self):
        response = self.post_json(reverse("rest_rooms_create"), {"name": "x" * 100})

        self.assertRejected(response)
        self.assertFalse(Room.objects.exists())
