"""
Moving and deleting entities and managing entities pages, through the REST endpoints the
React entities-page view calls.
"""

from unittest import expectedFailure

from django.urls import reverse

from web.models import Entity, RoomEntitiesPage, Scene

from .harness import NSPMTestCase


def position(obj, page, slot):
    return {"id": obj.id, "entities_page_id": page.id, "room_view_position": slot}


class ReadEntityTests(NSPMTestCase):
    def test_get_entity(self):
        room = self.make_room("Kitchen")
        light = self.make_light(room, "Ceiling", slot=2, home_assistant_name="light.ceiling")

        response = self.client.get(reverse("rest_get_entity", kwargs={"entity_id": light.id}))

        self.assertEqual(response.status_code, 200)
        body = response.json()
        self.assertEqual((body["id"], body["friendly_name"], body["room_view_position"]), (light.id, "Ceiling", 2))
        self.assertEqual((body["controller"], body["home_assistant_name"]), ("home_assistant", "light.ceiling"))

    def test_get_room_pages_lists_entities_and_scenes(self):
        room = self.make_room("Kitchen")
        light = self.make_light(room)
        scene = self.make_scene(room)

        response = self.client.get(reverse("rest_room_entities_pages", kwargs={"room_id": room.id}))

        self.assertEqual(response.status_code, 200)
        pages = {page["type"]: page for page in response.json()["entities_pages"]}
        self.assertEqual([e["id"] for e in pages["entity"]["entities"]], [light.id])
        self.assertEqual([s["id"] for s in pages["scene"]["scenes"]], [scene.id])


class MoveEntityTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")
        self.page = self.entities_page(self.room)
        self.second_page = RoomEntitiesPage.objects.create(room=self.room, page_type=8, is_scenes_page=False, display_order=1)

    def save_order(self, entities=(), scenes=()):
        return self.put_json(reverse("rest_put_room_entities_order", kwargs={"room_id": self.room.id}), {"entities": list(entities), "scenes": list(scenes)})

    def assertAt(self, obj, page, slot):
        obj.refresh_from_db()
        self.assertEqual((obj.entities_page_id, obj.room_view_position), (page.id, slot))

    def test_move_entity(self):
        light = self.make_light(self.room, slot=0)

        response = self.save_order([position(light, self.page, 3)])

        self.assertEqual(response.status_code, 200)
        self.assertAt(light, self.page, 3)
        self.assertManagerReloaded(times=1)

    def test_swap_entities_across_pages(self):
        ceiling = self.make_light(self.room, "Ceiling", slot=0)
        lamp = self.make_light(self.room, "Lamp", page=self.second_page, slot=5)

        response = self.save_order([position(ceiling, self.second_page, 5), position(lamp, self.page, 0)])

        self.assertEqual(response.status_code, 200)
        self.assertAt(ceiling, self.second_page, 5)
        self.assertAt(lamp, self.page, 0)

    def test_move_scenes(self):
        scenes_page = self.scenes_page(self.room)
        movie = self.make_scene(self.room, "Movie", slot=0)
        dinner = self.make_scene(self.room, "Dinner", slot=1)

        response = self.save_order(scenes=[position(movie, scenes_page, 1), position(dinner, scenes_page, 0)])

        self.assertEqual(response.status_code, 200)
        self.assertAt(movie, scenes_page, 1)
        self.assertAt(dinner, scenes_page, 0)
        self.assertManagerReloaded(times=1)

    def test_bad_payloads_are_rejected(self):
        light = self.make_light(self.room, slot=0)
        url = reverse("rest_put_room_entities_order", kwargs={"room_id": self.room.id})
        for body in ({"entities": []}, {"scenes": []}, {"entities": [{"id": light.id}], "scenes": []}, "not json"):
            with self.subTest(body=body):
                self.assertRejected(self.put_json(url, body))
        self.assertAt(light, self.page, 0)
        self.assertManagerNotReloaded()

    def test_unknown_entity_is_rejected(self):
        self.assertRejected(self.save_order([{"id": 999, "entities_page_id": self.page.id, "room_view_position": 0}]))
        self.assertManagerNotReloaded()

    def test_other_methods_are_rejected(self):
        self.assertEqual(self.client.get(reverse("rest_put_room_entities_order", kwargs={"room_id": self.room.id})).status_code, 405)

    @expectedFailure
    def test_failed_reorder_changes_nothing(self):
        # KNOWN GAP: the reorder is not atomic. Entities before the bad entry in the list are
        # already saved when it fails, so the DB is left half-reordered, and MQTTManager is
        # not told to reload.
        light = self.make_light(self.room, slot=0)

        response = self.save_order([position(light, self.page, 3), {"id": 999, "entities_page_id": self.page.id, "room_view_position": 0}])

        self.assertRejected(response)
        self.assertAt(light, self.page, 0)

    @expectedFailure
    def test_move_to_slot_outside_page_is_rejected(self):
        # KNOWN GAP: room_view_position is not checked against the page size.
        light = self.make_light(self.room, slot=0)

        self.assertRejected(self.save_order([position(light, self.page, 11)]))
        self.assertAt(light, self.page, 0)


class DeleteEntityTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")

    def test_delete_entity(self):
        doomed = self.make_light(self.room, "Ceiling", slot=1)
        kept = self.make_light(self.room, "Lamp", slot=2)

        response = self.client.delete(reverse("rest_get_entity", kwargs={"entity_id": doomed.id}))

        self.assertEqual(response.status_code, 200)
        self.assertEqual(list(Entity.objects.values_list("id", flat=True)), [kept.id])
        self.assertManagerReloaded(times=1)

    def test_delete_unknown_entity_is_rejected(self):
        self.make_light(self.room)

        self.assertRejected(self.client.delete(reverse("rest_get_entity", kwargs={"entity_id": 999})))
        self.assertEqual(Entity.objects.count(), 1)
        self.assertManagerNotReloaded()

    def test_delete_scene(self):
        scene = self.make_scene(self.room)

        response = self.client.delete(reverse("rest_get_scene", kwargs={"scene_id": scene.id}))

        self.assertEqual(response.status_code, 200)
        self.assertFalse(Scene.objects.exists())

    def test_delete_scene_reloads_manager(self):
        scene = self.make_scene(self.room)

        self.client.delete(reverse("rest_get_scene", kwargs={"scene_id": scene.id}))

        self.assertManagerReloaded(times=1)

    def test_delete_unknown_scene_is_rejected(self):
        self.assertRejected(self.client.delete(reverse("rest_get_scene", kwargs={"scene_id": 999})))


class EntitiesPageTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")

    def pages(self, room, is_scenes_page=False):
        return list(RoomEntitiesPage.objects.filter(room=room, is_scenes_page=is_scenes_page).order_by("display_order"))

    def create_page(self, page_type, is_scenes_page=False, room=None):
        room = room or self.room
        return self.put_json(reverse("rest_room_entities_pages", kwargs={"room_id": room.id}), {"type": page_type, "is_scenes_page": is_scenes_page})

    def test_create_pages(self):
        for page_type in (8, 12):
            self.assertEqual(self.create_page(page_type).status_code, 200)

        pages = self.pages(self.room)
        self.assertEqual([p.page_type for p in pages], [4, 8, 12])
        self.assertManagerReloaded(times=2)

    def test_create_global_scenes_page(self):
        self.global_scenes_page()

        response = self.put_json(reverse("rest_global_entities_pages"), {"type": 4, "is_scenes_page": True})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(self.pages(None, is_scenes_page=True)), 2)

    def test_create_page_missing_field_is_rejected(self):
        before = RoomEntitiesPage.objects.count()
        for body in ({"type": 4}, {"is_scenes_page": False}):
            with self.subTest(body=body):
                self.assertEqual(self.put_json(reverse("rest_room_entities_pages", kwargs={"room_id": self.room.id}), body).status_code, 400)
        self.assertEqual(RoomEntitiesPage.objects.count(), before)
        self.assertManagerNotReloaded()

    def test_create_page_in_unknown_room_is_rejected(self):
        before = RoomEntitiesPage.objects.count()

        self.assertRejected(self.put_json(reverse("rest_room_entities_pages", kwargs={"room_id": 999}), {"type": 4, "is_scenes_page": False}))

        self.assertEqual(RoomEntitiesPage.objects.count(), before)
        self.assertManagerNotReloaded()

    @expectedFailure
    def test_create_page_with_unsupported_size_is_rejected(self):
        # KNOWN GAP: panels only support 4, 8 or 12 entity pages but any number is stored.
        before = RoomEntitiesPage.objects.count()

        self.assertRejected(self.create_page(6))
        self.assertEqual(RoomEntitiesPage.objects.count(), before)

    def test_delete_page_removes_its_entities(self):
        second = RoomEntitiesPage.objects.create(room=self.room, page_type=8, is_scenes_page=False, display_order=2)
        on_first = self.make_light(self.room, slot=0)
        on_second = self.make_light(self.room, page=second, slot=0)

        response = self.client.delete(reverse("rest_room_entities_page", kwargs={"page_id": second.id}))

        self.assertEqual(response.status_code, 200)
        self.assertFalse(RoomEntitiesPage.objects.filter(id=second.id).exists())
        self.assertEqual(list(Entity.objects.values_list("id", flat=True)), [on_first.id])
        self.assertNotEqual(on_second.id, on_first.id)
        self.assertManagerReloaded(times=1)

    def test_delete_unknown_page_is_rejected(self):
        self.assertRejected(self.client.delete(reverse("rest_room_entities_page", kwargs={"page_id": 999})))
        self.assertManagerNotReloaded()

    def test_reorder_pages(self):
        first = self.entities_page(self.room)
        second = RoomEntitiesPage.objects.create(room=self.room, page_type=8, is_scenes_page=False, display_order=2)

        response = self.put_json(reverse("rest_room_entities_pages_order"), {"order": [[second.id, 0], [first.id, 1]]})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(self.pages(self.room), [second, first])
        self.assertManagerReloaded(times=1)

    def test_reorder_pages_bad_payload_is_rejected(self):
        url = reverse("rest_room_entities_pages_order")
        self.assertEqual(self.put_json(url, {}).status_code, 400)
        self.assertRejected(self.put_json(url, {"order": [[999, 0]]}))
        self.assertRejected(self.put_json(url, "not json"))
        self.assertManagerNotReloaded()

    def test_resize_page(self):
        page = self.entities_page(self.room)

        response = self.put_json(reverse("rest_room_entities_page", kwargs={"page_id": page.id}), {"id": page.id, "number_of_entities": 12})

        self.assertEqual(response.status_code, 200)
        page.refresh_from_db()
        self.assertEqual(page.page_type, 12)

    def test_resize_page_reloads_manager(self):
        page = self.entities_page(self.room)

        self.put_json(reverse("rest_room_entities_page", kwargs={"page_id": page.id}), {"number_of_entities": 12})

        self.assertManagerReloaded(times=1)

    @expectedFailure
    def test_shrinking_page_does_not_strand_entities(self):
        # KNOWN GAP: shrinking a 12-slot page to 4 leaves entities in slots 4-11 attached to
        # the page but never shown on the panel. Either reject the change or relocate them.
        page = RoomEntitiesPage.objects.create(room=self.room, page_type=12, is_scenes_page=False, display_order=2)
        light = self.make_light(self.room, page=page, slot=9)

        self.put_json(reverse("rest_room_entities_page", kwargs={"page_id": page.id}), {"number_of_entities": 4})

        self.assertLoadableByManager(light)
