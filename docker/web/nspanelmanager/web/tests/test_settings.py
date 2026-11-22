"""
Global settings: the settings page, weather & time, theme, the first-run setup
wizard, the settings REST endpoints and web.settings_helper.
"""

from django.urls import reverse

from web.models import NSPanelSettings, Settings
from web.settings_helper import (
    delete_nspanel_setting,
    does_setting_exist,
    get_nspanel_setting_with_default,
    get_setting_with_default,
    set_nspanel_setting_value,
    set_setting_value,
)

from .harness import NSPMTestCase


def setting(name):
    return get_setting_with_default(name)


class SettingsPageTests(NSPMTestCase):
    """The page is the React SettingsPage; what it saves is tested in test_rest_v2_settings.py."""

    def test_settings_page_mounts_the_react_page_without_any_values(self):
        set_setting_value("mqtt_server", "mqtt.local")
        set_setting_value("mqtt_password", "hunter2")

        response = self.client.get(reverse("settings"), headers={"X-Ingress-Path": "/ingress"})

        self.assertContains(response, "data-react-component='SettingsPage'")
        self.assertContains(response, '"logDownloadUrl": "/ingress/download_mqttmanager_log"')
        self.assertNotContains(response, "mqtt.local")
        self.assertNotContains(response, "hunter2")

    def test_legacy_save_url_is_gone(self):
        self.assertEqual(self.client.post("/save_settings", {"mqtt_server": "x"}).status_code, 404)
        self.assertManagerNotReloaded()


WEATHER_FORM = {
    "location_latitude": "-36.85",
    "location_longitude": "174.76",
    "wind_speed_format": "kmh",
    "precipitation_format": "mm",
    "outside_temp_provider": "home_assistant",
    "outside_temp_sensor": "sensor.outside",
    "weather_update_interval": "10",
    "date_format": "%d/%m/%Y",
    "clock_us_style": "False",
    "use_fahrenheit": "True",
}


class WeatherAndThemeTests(NSPMTestCase):
    def test_save_weather_and_time(self):
        response = self.client.post(reverse("weather_and_time"), WEATHER_FORM)

        self.assertRedirects(response, reverse("weather_and_time"), fetch_redirect_response=False)
        self.assertEqual(setting("location_latitude"), "-36.85")
        self.assertEqual(setting("weather_wind_speed_format"), "kmh")
        self.assertEqual(setting("outside_temp_sensor_entity_id"), "sensor.outside")
        self.assertEqual(setting("use_fahrenheit"), "True")
        self.assertManagerReloaded(times=1)

    def test_weather_form_missing_field_is_rejected(self):
        form = {k: v for k, v in WEATHER_FORM.items() if k != "use_fahrenheit"}

        self.assertRejected(self.client.post(reverse("weather_and_time"), form))
        self.assertManagerNotReloaded()

    def test_interface_theme(self):
        response = self.client.post(reverse("htmx_interface_theme"), {"theme-dropdown": "dracula"})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(setting("theme"), "dracula")
        self.assertContains(self.client.get(reverse("index")), "dracula")


# Everything the React first-run wizard submits.
WIZARD_SETTINGS = {
    "manager_address": "192.168.1.10",
    "manager_port": "8000",
    "mqtt_server": "mqtt.local",
    "mqtt_port": "1883",
    "mqtt_username": "u",
    "mqtt_password": "p",
    "home_assistant_address": "http://ha.local",
    "home_assistant_token": "t",
    "openhab_address": "",
    "openhab_token": "",
}


class InitialSetupTests(NSPMTestCase):
    """The first-run wizard (React InitialSetup) saves everything in one POST /rest/settings."""

    def test_wizard_saves_all_settings(self):
        response = self.post_json(reverse("rest_settings"), {"settings": WIZARD_SETTINGS})

        self.assertEqual(response.status_code, 200)
        for name, value in WIZARD_SETTINGS.items():
            self.assertEqual(setting(name), value)
        self.assertManagerReloaded(times=1)

    def test_only_submitted_settings_change(self):
        set_setting_value("mqtt_password", "keep-me")

        self.post_json(reverse("rest_settings"), {"settings": {"mqtt_server": "mqtt.local"}})

        self.assertEqual(setting("mqtt_server"), "mqtt.local")
        self.assertEqual(setting("mqtt_password"), "keep-me")

    def test_bad_payloads_are_rejected(self):
        before = list(Settings.objects.values_list("name", "value"))
        for body in ({}, {"settings": ["mqtt_server"]}, {"settings": "mqtt_server"}):
            with self.subTest(body=body):
                self.assertEqual(self.post_json(reverse("rest_settings"), body).status_code, 400)
        self.assertEqual(list(Settings.objects.values_list("name", "value")), before)
        self.assertManagerNotReloaded()


class SettingsRESTTests(NSPMTestCase):
    def configure_secrets(self):
        set_setting_value("mqtt_password", "hunter2")
        set_setting_value("home_assistant_token", "ha-secret")
        set_setting_value("openhab_token", "")

    def test_settings_endpoint_masks_secrets(self):
        self.configure_secrets()
        set_setting_value("mqtt_server", "mqtt.local")

        response = self.client.get(reverse("rest_settings"))

        self.assertEqual(response.status_code, 200)
        settings = response.json()["settings"]
        self.assertEqual(settings["mqtt_server"], "mqtt.local")
        for secret in ("mqtt_password", "home_assistant_token", "openhab_token"):
            self.assertNotIn(secret, settings)
        self.assertEqual((settings["mqtt_password_set"], settings["home_assistant_token_set"], settings["openhab_token_set"]), (True, True, False))
        self.assertNotIn("hunter2", response.content.decode())

    def test_settings_endpoint_rejects_unsupported_methods(self):
        self.assertEqual(self.client.put(reverse("rest_settings")).status_code, 405)

    def test_settings_endpoint_works_before_all_secrets_are_set(self):
        set_setting_value("mqtt_password", "hunter2")

        response = self.client.get(reverse("rest_settings"))

        self.assertEqual(response.status_code, 200)
        settings = response.json()["settings"]
        self.assertEqual((settings["mqtt_password_set"], settings["home_assistant_token_set"], settings["openhab_token_set"]), (True, True, False))
        self.assertNotIn("mqtt_password", settings)
        self.assertNotIn("home_assistant_token", settings)


class SettingsHelperTests(NSPMTestCase):
    def test_global_settings(self):
        self.assertEqual(get_setting_with_default("not_set"), "")
        self.assertFalse(does_setting_exist("not_set"))

        set_setting_value("colour", "blue")
        set_setting_value("colour", "green")  # update, not duplicate

        self.assertEqual(get_setting_with_default("colour"), "green")
        self.assertTrue(does_setting_exist("colour"))
        self.assertEqual(Settings.objects.filter(name="colour").count(), 1)

    def test_values_are_returned_as_strings(self):
        set_setting_value("level", 42)

        self.assertEqual(get_setting_with_default("level"), "42")

    def test_panel_settings(self):
        room = self.make_room()
        panel = self.make_panel(room)
        other = self.make_panel(room, mac="AA:BB:CC:DD:EE:01")

        self.assertEqual(get_nspanel_setting_with_default(panel.id, "dim", "default"), "default")
        set_nspanel_setting_value(panel.id, "dim", "50")
        set_nspanel_setting_value(panel.id, "dim", "60")

        self.assertEqual(get_nspanel_setting_with_default(panel.id, "dim", "default"), "60")
        self.assertEqual(get_nspanel_setting_with_default(other.id, "dim", "default"), "default")
        self.assertEqual(NSPanelSettings.objects.filter(nspanel=panel, name="dim").count(), 1)

        delete_nspanel_setting(panel.id, "dim")
        self.assertEqual(get_nspanel_setting_with_default(panel.id, "dim", "default"), "default")

    def test_panel_settings_for_unknown_panel(self):
        self.assertEqual(get_nspanel_setting_with_default(999, "dim", "default"), "default")
        set_nspanel_setting_value(999, "dim", "50")  # silently ignored
        delete_nspanel_setting(999, "dim")
        self.assertFalse(NSPanelSettings.objects.exists())
