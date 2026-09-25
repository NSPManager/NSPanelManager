"""
Panel lifecycle (accept, deny, delete, unblock), the per-panel settings form, and the
legacy panel-facing api/ endpoints.
"""

from unittest import expectedFailure

from django.urls import reverse

from web.models import Entity, NSPanel
from web.settings_helper import get_nspanel_setting_with_default, set_nspanel_setting_value

from .harness import NSPMTestCase


class PanelLifecycleTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")
        self.panel = self.make_panel(self.room, "New panel")

    def test_accept_register_request(self):
        self.panel.denied = True
        self.panel.save()

        response = self.client.post(reverse("htmx_nspanel_accept_register_request", kwargs={"nspanel_id": self.panel.id}))

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response["HX-Refresh"], "true")
        self.panel.refresh_from_db()
        self.assertEqual((self.panel.accepted, self.panel.denied), (True, False))
        self.assertManagerReloaded(times=1)

    def test_deny_register_request(self):
        response = self.client.post(reverse("htmx_nspanel_deny_register_request", kwargs={"nspanel_id": self.panel.id}))

        self.assertEqual(response.status_code, 200)
        self.panel.refresh_from_db()
        self.assertTrue(self.panel.denied)
        self.assertManagerReloaded(times=1)

    def test_denied_panels_page_lists_denied_panel(self):
        self.client.post(reverse("htmx_nspanel_deny_register_request", kwargs={"nspanel_id": self.panel.id}))

        self.assertContains(self.client.get(reverse("denied_nspanels")), "New panel")

    def test_accept_and_deny_require_post(self):
        for name in ("htmx_nspanel_accept_register_request", "htmx_nspanel_deny_register_request"):
            with self.subTest(name=name):
                self.assertEqual(self.client.get(reverse(name, kwargs={"nspanel_id": self.panel.id})).status_code, 405)
        self.panel.refresh_from_db()
        self.assertEqual((self.panel.accepted, self.panel.denied), (False, False))
        self.assertManagerNotReloaded()

    def test_accept_unknown_panel_is_rejected(self):
        self.assertRejected(self.client.post(reverse("htmx_nspanel_accept_register_request", kwargs={"nspanel_id": 999})))
        self.assertManagerNotReloaded()

    def test_delete_panel(self):
        set_nspanel_setting_value(self.panel.id, "screen_dim_level", "50")

        response = self.client.delete(reverse("htmx_nspanel_delete", kwargs={"nspanel_id": self.panel.id}))

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response["HX-Redirect"], "/")
        self.assertFalse(NSPanel.objects.exists())
        self.assertEqual(get_nspanel_setting_with_default(self.panel.id, "screen_dim_level", "gone"), "gone")

    @expectedFailure
    def test_delete_panel_reloads_manager(self):
        # KNOWN BUG: nspanel_delete() never calls send_mqttmanager_reload_command() (deny and
        # unblock both do), so MQTTManager keeps the deleted panel until its next reload.
        self.client.delete(reverse("htmx_nspanel_delete", kwargs={"nspanel_id": self.panel.id}))

        self.assertManagerReloaded(times=1)

    def test_delete_requires_delete_method(self):
        self.assertEqual(self.client.post(reverse("htmx_nspanel_delete", kwargs={"nspanel_id": self.panel.id})).status_code, 405)
        self.assertTrue(NSPanel.objects.exists())

    def test_unblock_removes_the_panel_so_it_can_register_again(self):
        for url_name, method in (("htmx_unblock_nspanel", "delete"), ("unblock_nspanel", "get")):
            with self.subTest(url_name=url_name):
                panel = self.make_panel(self.room, "Blocked", mac=f"AA:BB:CC:DD:EE:{len(url_name):02d}", denied=True)
                getattr(self.client, method)(reverse(url_name, kwargs={"nspanel_id": panel.id}))
                self.assertFalse(NSPanel.objects.filter(id=panel.id).exists())
        self.assertManagerReloaded(times=2)

    def test_deleting_a_room_keeps_its_panels(self):
        other = self.make_room("Lounge")

        self.client.get(reverse("delete_room", kwargs={"room_id": self.room.id}))

        self.panel.refresh_from_db()
        self.assertEqual(self.panel.room, other)


# Every field the panel settings form submits, with "use global default" choices.
PANEL_FORM = {
    "name": "Kitchen panel",
    "button1_mode": "0",
    "button1_detached_mode_light": "",
    "button1_custom_mqtt_topic": "",
    "button1_custom_mqtt_payload": "",
    "button2_mode": "0",
    "button2_detached_mode_light": "",
    "button2_custom_mqtt_topic": "",
    "button2_custom_mqtt_payload": "",
    "screen_dim_level": "",
    "screensaver_dim_level": "",
    "screensaver_activation_timeout": "",
    "screensaver_mode": "global",
    "show_screensaver_inside_temperature": "global",
    "show_screensaver_outside_temperature": "global",
    "temperature_calibration": "",
    "relay1_default_mode": "False",
    "relay2_default_mode": "False",
    "default_page": "0",
    "lock_to_default_room": "False",
    "reverse_relays": "False",
    "button1_relay_lower_temperature": "",
    "button1_relay_upper_temperature": "",
    "button2_relay_lower_temperature": "",
    "button2_relay_upper_temperature": "",
    "panel_type": "eu",
    "selected_tft": "tft1",
    "relay1_is_light": "False",
    "relay2_is_light": "False",
}


class PanelSettingsFormTests(NSPMTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")
        self.panel = self.make_panel(self.room, "Panel", accepted=True)

    def save(self, **overrides):
        return self.client.post(reverse("save_panel_settings", kwargs={"panel_id": self.panel.id}), PANEL_FORM | {"room_id": self.room.id} | overrides)

    def panel_setting(self, name, default=None):
        return get_nspanel_setting_with_default(self.panel.id, name, default)

    def test_save_basic_settings(self):
        lounge = self.make_room("Lounge")

        response = self.save(room_id=lounge.id, name="Renamed", default_page="2", temperature_calibration="-1.5")

        self.assertRedirects(response, reverse("edit_nspanel", kwargs={"panel_id": self.panel.id}), fetch_redirect_response=False)
        self.panel.refresh_from_db()
        self.assertEqual((self.panel.friendly_name, self.panel.room), ("Renamed", lounge))
        self.assertEqual(self.panel_setting("default_page"), "2")
        self.assertEqual(self.panel_setting("temperature_calibration"), "-1.5")
        self.assertManagerReloaded(times=1)

    def test_blank_overrides_fall_back_to_global_settings(self):
        set_nspanel_setting_value(self.panel.id, "screen_dim_level", "40")
        set_nspanel_setting_value(self.panel.id, "screensaver_mode", "no_background")

        self.save()

        self.assertIsNone(self.panel_setting("screen_dim_level"))
        self.assertIsNone(self.panel_setting("screensaver_mode"))
        self.assertEqual(self.panel_setting("temperature_calibration"), "0.0")

    def test_per_panel_overrides_are_stored(self):
        self.save(screen_dim_level="40", screensaver_mode="no_background", show_screensaver_inside_temperature="False")

        self.assertEqual(self.panel_setting("screen_dim_level"), "40")
        self.assertEqual(self.panel_setting("screensaver_mode"), "no_background")
        self.assertEqual(self.panel_setting("show_screensaver_inside_temperature"), "False")

    def test_panel_types(self):
        cases = {
            "eu": ("False", None),
            "us_vertical": ("True", "vertical"),
            "us_horizontal": ("True", "horizontal"),
            "us_horizontal_mirrored": ("True", "horizontal_mirrored"),
        }
        for panel_type, expected in cases.items():
            with self.subTest(panel_type=panel_type):
                self.save(panel_type=panel_type)
                is_us = self.panel_setting("is_us_panel")
                self.assertEqual(is_us, expected[0])
                if expected[1]:
                    self.assertEqual(self.panel_setting("us_panel_orientation"), expected[1])

    def test_button_detached_mode_links_an_entity(self):
        light = self.make_light(self.room, "Ceiling")

        self.save(button1_mode="1", button1_detached_mode_light=str(light.id))

        self.panel.refresh_from_db()
        self.assertEqual(self.panel.button1_mode, 1)
        self.assertEqual(self.panel.button1_detached_mode_entity, light)

        self.save(button1_mode="0")

        self.panel.refresh_from_db()
        self.assertIsNone(self.panel.button1_detached_mode_entity)

    def test_deleting_the_detached_entity_unlinks_it(self):
        light = self.make_light(self.room, "Ceiling")
        self.save(button2_mode="1", button2_detached_mode_light=str(light.id))

        Entity.objects.all().delete()

        self.panel.refresh_from_db()
        self.assertIsNone(self.panel.button2_detached_mode_entity)

    def test_button_custom_mqtt_mode(self):
        self.save(button2_mode="2", button2_custom_mqtt_topic="garage/door", button2_custom_mqtt_payload="toggle")

        self.assertEqual((self.panel_setting("button2_mqtt_topic"), self.panel_setting("button2_mqtt_payload")), ("garage/door", "toggle"))

        self.save(button2_mode="0")

        self.assertIsNone(self.panel_setting("button2_mqtt_topic"))

    def test_detached_mode_with_unknown_entity_is_rejected(self):
        self.assertRejected(self.save(button1_mode="1", button1_detached_mode_light="999"))
        self.panel.refresh_from_db()
        self.assertEqual(self.panel.button1_mode, 0)
        self.assertManagerNotReloaded()

    def test_non_numeric_temperature_calibration_is_rejected(self):
        self.assertRejected(self.save(temperature_calibration="warm"))
        self.assertManagerNotReloaded()

    def test_unknown_room_is_rejected(self):
        self.assertRejected(self.save(room_id=999))
        self.panel.refresh_from_db()
        self.assertEqual(self.panel.room, self.room)

    def test_edit_page_shows_saved_values(self):
        self.save(name="Hallway", screen_dim_level="40")

        response = self.client.get(reverse("edit_nspanel", kwargs={"panel_id": self.panel.id}))

        self.assertContains(response, "Hallway")
        self.assertEqual(response.context["settings"]["screen_dim_level"], "40")


class LegacyPanelAPITests(NSPMTestCase):
    """api/* endpoints from before panel config moved to protobuf over MQTT."""

    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")
        self.panel = self.make_panel(self.room, "Kitchen panel", mac="C0:49:EF:00:00:01")

    @expectedFailure
    def test_get_nspanel_config(self):
        # KNOWN BUG: get_nspanel_config() reads nspanel.button1_detached_mode_light, which was
        # renamed to button1_detached_mode_entity, so it always returns 500. (The URL is marked
        # "TODO: Remove" in urls.py; deleting it would also resolve this.)
        response = self.client.get(reverse("get_nspanel_config"), {"mac": self.panel.mac_address})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.json()["name"], "Kitchen panel")

    def test_get_nspanel_config_for_unknown_panel_is_rejected(self):
        self.assertEqual(self.client.get(reverse("get_nspanel_config"), {"mac": "00:00:00:00:00:00"}).status_code, 500)

    def test_online_status_for_unknown_panel_is_rejected(self):
        response = self.client.post(reverse("set_panel_online_status", kwargs={"panel_mac": "00:00:00:00:00:00"}), '{"state": "online"}', content_type="application/json")

        self.assertEqual(response.status_code, 500)

    def test_restart_mqtt_manager(self):
        response = self.client.post(reverse("restart_mqtt_manager"))

        self.assertEqual(response.json(), {"result": "OK"})
        self.assertEqual(self.mqttmanager.restarts, 1)
