"""
Every page and htmx partial renders. These catch broken templates, template tags and
view context (e.g. a renamed model field referenced from a template), which otherwise
only show up when someone opens the page.
"""

from unittest import expectedFailure

from django.urls import reverse

from web.models import Message, RelayGroup
from web.settings_helper import set_setting_value

from .harness import NSPMTestCase


class PageRenderTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")
        self.make_light(self.room, "Ceiling")
        self.make_scene(self.room, "Dinner")
        self.make_scene(None, "Goodnight", scene_type="nspm_scene", backend_name="")
        self.panel = self.make_panel(self.room, "Kitchen panel", accepted=True)
        self.denied_panel = self.make_panel(self.room, "Denied panel", mac="AA:BB:CC:DD:EE:01", denied=True)
        self.relay_group = RelayGroup.objects.create(friendly_name="Hall lights")
        Message.objects.create(title="Hello", message="World")

    def assertRenders(self, url):
        response = self.client.get(url)
        self.assertEqual(response.status_code, 200, f"GET {url} returned {response.status_code}")
        return response

    def test_full_pages(self):
        pages = [
            reverse("index"),
            reverse("rooms"),
            reverse("edit_room", kwargs={"room_id": self.room.id}),
            reverse("edit_nspanel", kwargs={"panel_id": self.panel.id}),
            reverse("settings"),
            reverse("relay_groups"),
            reverse("global_scenes"),
            reverse("weather_and_time"),
            reverse("denied_nspanels"),
        ]
        for url in pages:
            with self.subTest(url=url):
                self.assertRenders(url)

    def test_pages_render_with_no_configuration(self):
        # A fresh install: no settings, no rooms, no panels.
        self.room.delete()
        for url in (reverse("index"), reverse("rooms"), reverse("settings"), reverse("weather_and_time"), reverse("global_scenes"), reverse("relay_groups")):
            with self.subTest(url=url):
                self.assertRenders(url)

    def test_index_prompts_for_manager_address_until_configured(self):
        self.assertContains(self.assertRenders(reverse("index")), "No manager address configured")

    def test_index_lists_panels_but_not_denied_ones(self):
        response = self.assertRenders(reverse("htmx_partial_index_nspanels_section"))

        self.assertContains(response, "Kitchen panel")
        self.assertNotContains(response, "Denied panel")

    def test_htmx_partials(self):
        partials = [
            reverse("htmx_partial_index_nspanels_section"),
            reverse("htmx_partial_select_new_entity_type_react", kwargs={"entities_page_id": self.entities_page(self.room).id, "room_view_position": 1}),
            reverse("htmx_partial_select_weather_location"),
            reverse("htmx_partial_select_weather_outside_temperature_sensor"),
            reverse("htmx_show_messages"),
            reverse("htmx_modal_create_new_relay_group"),
            reverse("htmx_modal_edit_relay_group", kwargs={"relay_group_id": self.relay_group.id}),
            reverse("htmx_modal_add_relay_to_relay_group", kwargs={"relay_group_id": self.relay_group.id}),
        ]
        for url in partials:
            with self.subTest(url=url):
                self.assertRenders(url)

    def test_initial_setup_modals(self):
        for name in (
            "htmx_initial_setup_welcome",
            "htmx_initial_setup_manager_settings",
            "htmx_initial_setup_mqtt_settings",
            "htmx_initial_setup_home_assistant_settings",
            "htmx_initial_setup_openhab_settings",
            "htmx_initial_setup_finished",
        ):
            with self.subTest(name=name):
                self.assertRenders(reverse(name))

    def test_sensor_pickers_list_home_assistant_entities(self):
        self.home_assistant.add("sensor.kitchen_temperature", friendly_name="Kitchen temperature")

        for url in (
            reverse("htmx_partial_select_room_temperature_sensor_from_list", kwargs={"entity_source": "home_assistant", "room_id": self.room.id}),
            reverse("htmx_partial_select_weather_outside_temperature_sensor_from_list", kwargs={"entity_source": "home_assistant"}),
        ):
            with self.subTest(url=url):
                self.assertContains(self.assertRenders(url), "Kitchen temperature")

    def test_sensor_provider_picker_skips_straight_to_the_only_configured_source(self):
        room_list = reverse("htmx_partial_select_room_temperature_sensor_from_list", kwargs={"entity_source": "home_assistant", "room_id": self.room.id})
        weather_list = reverse("htmx_partial_select_weather_outside_temperature_sensor_from_list", kwargs={"entity_source": "home_assistant"})

        self.assertRedirects(self.client.get(reverse("htmx_partial_select_room_temperature_sensor_provider", kwargs={"room_id": self.room.id})), room_list, fetch_redirect_response=False)
        self.assertRedirects(self.client.get(reverse("htmx_partial_select_weather_outside_temperature_sensor_provider")), weather_list, fetch_redirect_response=False)

        set_setting_value("home_assistant_token", "")
        set_setting_value("openhab_address", "http://openhab.test")
        set_setting_value("openhab_token", "token")
        self.assertRedirects(
            self.client.get(reverse("htmx_partial_select_weather_outside_temperature_sensor_provider")),
            reverse("htmx_partial_select_weather_outside_temperature_sensor_from_list", kwargs={"entity_source": "openhab"}),
            fetch_redirect_response=False,
        )

    def test_sensor_provider_picker_offers_a_choice_when_both_are_configured(self):
        set_setting_value("openhab_address", "http://openhab.test")
        set_setting_value("openhab_token", "token")

        self.assertRenders(reverse("htmx_partial_select_room_temperature_sensor_provider", kwargs={"room_id": self.room.id}))
        self.assertRenders(reverse("htmx_partial_select_weather_outside_temperature_sensor_provider"))

    def test_sensor_provider_picker_with_nothing_configured_is_an_error(self):
        set_setting_value("home_assistant_token", "")

        self.assertRejected(self.client.get(reverse("htmx_partial_select_weather_outside_temperature_sensor_provider")))

    @expectedFailure
    def test_unused_legacy_partials_render(self):
        # KNOWN BUG: two routes nothing in the UI calls any more are broken. Fix or delete them.
        # - nspanel_index_view: the nspanel_status_header component picks its template by URL
        #   name and has none for this URL, so rendering raises ImproperlyConfigured.
        # - edit_entities_page: its template was removed with the move to React.
        self.assertRenders(reverse("htmx_partial_nspanel_index_view", kwargs={"nspanel_id": self.panel.id}))
        self.assertRenders(reverse("htmx_partial_edit_entities_page", kwargs={"page_id": self.entities_page(self.room).id}))

    def test_unknown_objects_are_rejected(self):
        for url in (
            reverse("edit_nspanel", kwargs={"panel_id": 999}),
            reverse("htmx_partial_nspanel_index_view", kwargs={"nspanel_id": 999}),
            reverse("htmx_modal_edit_relay_group", kwargs={"relay_group_id": 999}),
        ):
            with self.subTest(url=url):
                self.assertRejected(self.client.get(url))
