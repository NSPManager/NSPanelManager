"""
Global settings: the settings page form, weather & time, theme, the first-run setup
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

# Every field the settings page form submits.
SETTINGS_FORM = {
    "mqtt_server": "mqtt.local",
    "mqtt_port": "1883",
    "mqtt_username": "nspm",
    "mqtt_password": "",
    "home_assistant_address": "http://ha.local:8123",
    "home_assistant_token": "",
    "openhab_address": "",
    "openhab_token": "",
    "raise_to_100_light_level": "95",
    "default_nspanel_type": "eu",
    "color_temp_min": "2000",
    "color_temp_max": "6000",
    "reverse_color_temp": "False",
    "min_button_push_time": "50",
    "button_long_press_time": "300",
    "special_mode_trigger_time": "300",
    "special_mode_release_time": "5000",
    "screensaver_activation_timeout": "30000",
    "screen_dim_level": "100",
    "screensaver_dim_level": "0",
    "screensaver_mode": "with_background",
    "show_screensaver_inside_temperature": "True",
    "show_screensaver_outside_temperature": "True",
    "turn_on_behavior": "color_temp",
    "max_live_log_messages": "10",
    "max_log_buffer_size": "10",
    "mqttmanager_log_level": "info",
    "manager_address": "192.168.1.10",
    "manager_port": "8000",
    "optimistic_mode": "optimistic",
    "all_rooms_status_backoff_time": "250",
    "room_status_backoff_time": "250",
    "light_turn_on_brightness": "50",
}


def setting(name):
    return get_setting_with_default(name)


class SettingsPageTests(NSPMTestCase):
    def save(self, **overrides):
        return self.client.post(reverse("save_settings"), SETTINGS_FORM | overrides)

    def test_save_settings(self):
        response = self.save()

        self.assertRedirects(response, reverse("settings"), fetch_redirect_response=False)
        for name in ("mqtt_server", "mqtt_port", "color_temp_min", "screensaver_mode", "manager_address", "light_turn_on_brightness"):
            with self.subTest(name=name):
                self.assertEqual(setting(name), SETTINGS_FORM[name])
        self.assertEqual(setting("optimistic_mode"), "True")
        self.assertManagerReloaded(times=1)

    def test_trailing_slash_is_stripped_from_addresses(self):
        self.save(home_assistant_address="http://ha.local:8123/", openhab_address="http://openhab.local/")

        self.assertEqual(setting("home_assistant_address"), "http://ha.local:8123")
        self.assertEqual(setting("openhab_address"), "http://openhab.local")

    def test_blank_secret_fields_keep_the_stored_secret(self):
        # The form never echoes secrets back, so a blank field means "unchanged".
        set_setting_value("mqtt_password", "hunter2")
        set_setting_value("home_assistant_token", "ha-token")
        set_setting_value("openhab_token", "oh-token")

        self.save()

        self.assertEqual((setting("mqtt_password"), setting("home_assistant_token"), setting("openhab_token")), ("hunter2", "ha-token", "oh-token"))

    def test_new_secrets_replace_the_stored_ones(self):
        set_setting_value("mqtt_password", "hunter2")

        self.save(mqtt_password="correct horse", home_assistant_token="new-token")

        self.assertEqual((setting("mqtt_password"), setting("home_assistant_token")), ("correct horse", "new-token"))

    def test_clear_checkboxes_remove_secrets(self):
        set_setting_value("mqtt_password", "hunter2")
        set_setting_value("openhab_token", "oh-token")

        self.save(clear_mqtt_password="on", clear_home_assistant_token="on", clear_openhab_token="on", mqtt_password="ignored")

        self.assertEqual((setting("mqtt_password"), setting("home_assistant_token"), setting("openhab_token")), ("", "", ""))

    def test_optional_fields_fall_back_to_defaults(self):
        form = {k: v for k, v in SETTINGS_FORM.items() if k not in ("show_screensaver_inside_temperature", "turn_on_behavior", "optimistic_mode")}
        set_setting_value("optimistic_mode", "False")

        self.client.post(reverse("save_settings"), form)

        self.assertEqual(setting("show_screensaver_inside_temperature"), "True")
        self.assertEqual(setting("turn_on_behavior"), "color_temp")
        self.assertEqual(setting("optimistic_mode"), "False")  # left alone when not submitted

    def test_missing_required_field_is_rejected(self):
        form = {k: v for k, v in SETTINGS_FORM.items() if k != "manager_port"}

        self.assertRejected(self.client.post(reverse("save_settings"), form))
        self.assertManagerNotReloaded()

    def test_settings_page_shows_saved_values_but_not_secrets(self):
        self.save(mqtt_password="hunter2", home_assistant_token="ha-secret-token")

        response = self.client.get(reverse("settings"))

        self.assertContains(response, "mqtt.local")
        self.assertNotContains(response, "hunter2")
        self.assertNotContains(response, "ha-secret-token")


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

    def test_mqttmanager_can_read_a_setting(self):
        set_setting_value("color_temp_min", "2000")

        response = self.client.get(reverse("rest_mqttmanager_get_setting", kwargs={"setting_key": "color_temp_min"}))

        self.assertEqual(response.json(), {"status": "ok", "settings": {"color_temp_min": "2000"}})

    def test_mqttmanager_settings_endpoint_checks_method(self):
        self.assertEqual(self.client.post(reverse("rest_mqttmanager_get_setting", kwargs={"setting_key": "x"})).status_code, 405)

    def test_mqttmanager_settings_endpoint_refuses_banned_keys_as_listed(self):
        for key in ("MQTT_PASSWORD", "HOME_ASSISTANT_TOKEN"):
            with self.subTest(key=key):
                self.assertEqual(self.client.get(reverse("rest_mqttmanager_get_setting", kwargs={"setting_key": key})).status_code, 403)

    def test_mqttmanager_settings_endpoint_refuses_secrets(self):
        # banned_setting_keys is upper case but settings are stored lower case, so the check
        # must be case-insensitive or the secrets it protects are returned.
        self.configure_secrets()

        for key in ("mqtt_password", "home_assistant_token", "openhab_token"):
            with self.subTest(key=key):
                self.assertEqual(self.client.get(reverse("rest_mqttmanager_get_setting", kwargs={"setting_key": key})).status_code, 403)


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
