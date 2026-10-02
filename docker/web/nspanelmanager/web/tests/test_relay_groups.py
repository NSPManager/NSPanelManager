"""
Relay groups: tie panel relays together so switching one switches them all.
Covered through both the form views and the htmx endpoints, plus the REST listing
MQTTManager reads.
"""

from django.urls import reverse

from web.models import NSPanel, RelayGroup, RelayGroupBinding

from .harness import NSPMTestCase


class RelayGroupTestCase(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Hall")
        self.panel = self.make_panel(self.room, "Hall panel", accepted=True)
        self.other_panel = self.make_panel(self.room, "Stairs panel", mac="AA:BB:CC:DD:EE:01", accepted=True)
        self.group = RelayGroup.objects.create(friendly_name="Hall lights")

    def bind(self, panel, relay_num):
        return RelayGroupBinding.objects.create(relay_group=self.group, nspanel=panel, relay_num=relay_num)


class RelayGroupViewTests(RelayGroupTestCase):
    def test_create_group(self):
        response = self.client.post(reverse("create_or_update_relay_group"), {"relay_group_id": "", "relay_group_name": "Porch"})

        self.assertRedirects(response, reverse("relay_groups"), fetch_redirect_response=False)
        self.assertTrue(RelayGroup.objects.filter(friendly_name="Porch").exists())
        self.assertManagerReloaded(times=1)

    def test_rename_group(self):
        self.client.post(reverse("create_or_update_relay_group"), {"relay_group_id": self.group.id, "relay_group_name": "Landing"})

        self.group.refresh_from_db()
        self.assertEqual(self.group.friendly_name, "Landing")
        self.assertEqual(RelayGroup.objects.count(), 1)

    def test_add_relay_to_group(self):
        response = self.client.post(reverse("add_nspanel_relay_to_group"), {"relay_group_id": self.group.id, "nspanel_id": self.panel.id, "relay_num": "2"})

        self.assertEqual(response.status_code, 302)
        binding = RelayGroupBinding.objects.get()
        self.assertEqual((binding.relay_group, binding.nspanel, binding.relay_num), (self.group, self.panel, 2))
        self.assertManagerReloaded(times=1)

    def test_adding_the_same_relay_twice_is_a_no_op(self):
        self.bind(self.panel, 1)

        self.client.post(reverse("add_nspanel_relay_to_group"), {"relay_group_id": self.group.id, "nspanel_id": self.panel.id, "relay_num": "1"})

        self.assertEqual(RelayGroupBinding.objects.count(), 1)
        self.assertManagerNotReloaded()

    def test_add_relay_with_unknown_group_or_panel_is_rejected(self):
        for form in ({"relay_group_id": 999, "nspanel_id": self.panel.id, "relay_num": "1"}, {"relay_group_id": self.group.id, "nspanel_id": 999, "relay_num": "1"}):
            with self.subTest(form=form):
                self.assertRejected(self.client.post(reverse("add_nspanel_relay_to_group"), form))
        self.assertFalse(RelayGroupBinding.objects.exists())
        self.assertManagerNotReloaded()

    def test_remove_relay_from_group(self):
        binding = self.bind(self.panel, 1)
        kept = self.bind(self.other_panel, 1)

        self.client.get(reverse("delete_relay_group_binding", kwargs={"relay_binding_id": binding.id}))

        self.assertEqual(list(RelayGroupBinding.objects.all()), [kept])
        self.assertManagerReloaded(times=1)

    def test_delete_group_removes_its_bindings(self):
        self.bind(self.panel, 1)

        self.client.get(reverse("delete_relay_group", kwargs={"relay_group_id": self.group.id}))

        self.assertFalse(RelayGroup.objects.exists())
        self.assertFalse(RelayGroupBinding.objects.exists())
        self.assertManagerReloaded(times=1)

    def test_delete_unknown_group_is_rejected(self):
        self.assertRejected(self.client.get(reverse("delete_relay_group", kwargs={"relay_group_id": 999})))
        self.assertManagerNotReloaded()

    def test_deleting_a_panel_removes_its_bindings(self):
        self.bind(self.panel, 1)

        NSPanel.objects.filter(id=self.panel.id).delete()

        self.assertFalse(RelayGroupBinding.objects.exists())
        self.assertTrue(RelayGroup.objects.exists())

    def test_relay_groups_page_lists_groups_and_panels(self):
        self.bind(self.panel, 2)

        response = self.client.get(reverse("relay_groups"))

        self.assertContains(response, "Hall lights")
        self.assertContains(response, "Hall panel")


class RelayGroupHTMXTests(RelayGroupTestCase):
    def test_create_and_rename(self):
        response = self.client.post(reverse("htmx_save_relay_group"), {"name": "Porch"})

        self.assertEqual(response["HX-Refresh"], "true")
        porch = RelayGroup.objects.get(friendly_name="Porch")

        self.client.post(reverse("htmx_save_relay_group"), {"relay_group_id": porch.id, "name": "Deck"})

        porch.refresh_from_db()
        self.assertEqual(porch.friendly_name, "Deck")

    def test_add_and_remove_relay(self):
        response = self.client.post(reverse("htmx_add_relay_to_relay_group", kwargs={"relay_group_id": self.group.id}), {"nspanel_id": self.panel.id, "relay_selection": "1"})

        self.assertEqual(response["HX-Refresh"], "true")
        binding = RelayGroupBinding.objects.get()
        self.assertManagerReloaded(times=1)

        self.client.post(reverse("htmx_add_relay_to_relay_group", kwargs={"relay_group_id": self.group.id}), {"nspanel_id": self.panel.id, "relay_selection": "1"})
        self.assertEqual(RelayGroupBinding.objects.count(), 1)

        self.client.delete(reverse("htmx_remove_relay_from_relay_group", kwargs={"relay_binding_id": binding.id}))

        self.assertFalse(RelayGroupBinding.objects.exists())
        self.assertManagerReloaded(times=2)

    def test_add_relay_modal_only_offers_accepted_panels(self):
        self.make_panel(self.room, "Pending panel", mac="AA:BB:CC:DD:EE:02")

        response = self.client.get(reverse("htmx_modal_add_relay_to_relay_group", kwargs={"relay_group_id": self.group.id}))

        self.assertContains(response, "Hall panel")
        self.assertNotContains(response, "Pending panel")

    def test_delete_group(self):
        self.bind(self.panel, 1)

        self.client.delete(reverse("htmx_delete_relay_group", kwargs={"relay_group_id": self.group.id}))

        self.assertFalse(RelayGroup.objects.exists())
        self.assertFalse(RelayGroupBinding.objects.exists())

    def test_save_and_delete_group_reload_manager(self):
        self.client.post(reverse("htmx_save_relay_group"), {"name": "Porch"})
        self.client.delete(reverse("htmx_delete_relay_group", kwargs={"relay_group_id": self.group.id}))

        self.assertManagerReloaded(times=2)


class RelayGroupRESTTests(RelayGroupTestCase):
    def test_list_relay_groups(self):
        self.bind(self.panel, 1)
        self.bind(self.other_panel, 2)

        response = self.client.get(reverse("rest_relay_groups"))

        self.assertEqual(response.status_code, 200)
        group = response.json()["relay_groups"][0]
        self.assertEqual((group["relay_group_id"], group["name"]), (self.group.id, "Hall lights"))
        self.assertCountEqual(group["relays"], [{"nspanel_id": self.panel.id, "relay_num": 1}, {"nspanel_id": self.other_panel.id, "relay_num": 2}])

    def test_relay_groups_endpoint_is_read_only(self):
        self.assertEqual(self.client.post(reverse("rest_relay_groups")).status_code, 405)
