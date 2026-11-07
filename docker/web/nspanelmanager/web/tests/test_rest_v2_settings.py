"""
/rest/v2/settings (web/rest_v2/global_settings.py).

Most important here is compatibility: v2 reads and writes the same Settings rows as the legacy settings
page, the weather page, the first-run wizard (/rest/settings) and MQTTManager, so the stored strings must
stay exactly as those write them. LEGACY_ROWS is a settings table as they leave it.
"""

import importlib
import json
import os
import re
import sys
from pathlib import Path
from unittest import mock, skipUnless

from django.test import Client
from django.urls import reverse
from django.utils.crypto import get_random_string

import web.rest_v2.schema
from web.models import Settings
from web.rest_v2.global_settings import SECRETS, SettingsSerializer, SettingsUpdateSerializer
from web.settings_helper import get_setting_with_default, set_setting_value

from .harness import NSPMTestCase

URL = "/rest/v2/settings"

REPO_DOCKER_DIR = Path(__file__).resolve().parents[4]
MANAGER_CONFIG_HEADER = REPO_DOCKER_DIR / "MQTTManager" / "include" / "mqtt_manager_config" / "mqtt_manager_config.hpp"
TEMPLATES = Path(__file__).resolve().parents[1] / "templates"


def manager_key_map():
    """{db key: default} from MqttManagerConfig::_setting_key_map, or None outside a source checkout."""
    if not MANAGER_CONFIG_HEADER.exists():
        return None
    source = MANAGER_CONFIG_HEADER.read_text()
    body = source[source.index("_setting_key_map = {") : source.index("};", source.index("_setting_key_map = {"))]
    entries = re.findall(r'\{MQTT_MANAGER_SETTING::(\w+),\s*\{"([^"]*)",\s*"([^"]*)"\}\}', body)
    return {key: default for enum, key, default in entries if enum != "LAST"}  # LAST exists only in TEST_MODE


MANAGER_KEYS = manager_key_map()
needs_manager_source = skipUnless(MANAGER_KEYS is not None, f"MQTTManager source not found at {MANAGER_CONFIG_HEADER}")

# Every key in _setting_key_map, stored the way its legacy writer stores it: radio buttons post
# "True"/"False", save_settings stores optimistic_mode as str(bool), MQTTManager writes
# is_home_assistant_addon in lower case and leaves blank defaults blank.
LEGACY_ROWS = {
    "is_home_assistant_addon": "false",
    "manager_address": "192.168.1.10",
    "manager_port": "8000",
    "mqttmanager_log_level": "info",
    "max_live_log_messages": "10",
    "max_log_buffer_size": "10",
    "default_nspanel_type": "us",
    "color_temp_min": "2000",
    "color_temp_max": "6000",
    "reverse_color_temp": "False",
    "turn_on_behavior": "color_temp",
    "raise_to_100_light_level": "95",
    "light_turn_on_brightness": "50",
    "min_button_push_time": "50",
    "button_long_press_time": "5000",
    "special_mode_trigger_time": "300",
    "special_mode_release_time": "5000",
    "optimistic_mode": "True",
    "room_status_backoff_time": "100",
    "all_rooms_status_backoff_time": "250",
    "mqtt_wait_time": "1000",
    "screen_dim_level": "100",
    "screensaver_dim_level": "1",
    "screensaver_activation_timeout": "30000",
    "screensaver_mode": "datetime_with_background",
    "show_screensaver_inside_temperature": "True",
    "show_screensaver_outside_temperature": "False",
    "location_latitude": "-33.8688",
    "location_longitude": "151.2093",
    "weather_update_interval": "10",
    "weather_wind_speed_format": "kmh",
    "weather_precipitation_format": "mm",
    "outside_temp_sensor_provider": "home_assistant",
    "outside_temp_sensor_entity_id": "sensor.outside",
    "date_format": "%a %d/%m %Y",
    "clock_us_style": "True",
    "use_fahrenheit": "True",
    "theme": "dark",
    "buzzer_sound_touch": "",
    "buzzer_sound_scene_activated": "",
    "buzzer_sound_scene_saved": "",
    "mqtt_server": "mqtt.local",
    "mqtt_port": "1883",
    "mqtt_username": "nspm",
    "mqtt_password": "mqtt-secret-value",
    "home_assistant_address": "http://ha.local:8123",
    "home_assistant_token": "ha-secret-value",
    "openhab_address": "",
    "openhab_token": "",
}

# The integer settings and the range each accepts.
INTEGER_RANGES = {
    "manager_port": (1, 65535),
    "mqtt_port": (1, 65535),
    "max_live_log_messages": (0, 2**31 - 1),
    "max_log_buffer_size": (0, 2**31 - 1),
    "color_temp_min": (1, 2**31 - 1),
    "color_temp_max": (1, 2**31 - 1),
    "raise_to_100_light_level": (0, 100),
    "light_turn_on_brightness": (1, 100),
    "min_button_push_time": (0, 2**31 - 1),
    "button_long_press_time": (0, 2**31 - 1),
    "special_mode_trigger_time": (0, 2**31 - 1),
    "special_mode_release_time": (0, 2**31 - 1),
    "room_status_backoff_time": (0, 2**31 - 1),
    "all_rooms_status_backoff_time": (0, 2**31 - 1),
    "mqtt_wait_time": (0, 2**31 - 1),
    "screen_dim_level": (1, 100),
    "screensaver_dim_level": (0, 100),
    "screensaver_activation_timeout": (0, 2**31 - 1),
    "weather_update_interval": (1, 2**31 - 1),  # MQTTManager sleeps this many minutes between updates
}

CHOICES = {
    "mqttmanager_log_level": ["error", "warning", "info", "debug", "trace"],
    "default_nspanel_type": ["eu", "us"],
    "turn_on_behavior": ["color_temp", "restore"],
    "screensaver_mode": ["with_background", "without_background", "datetime_with_background", "datetime_without_background", "no_screensaver"],
    "weather_wind_speed_format": ["kmh", "ms", "mph", "knots"],
    "weather_precipitation_format": ["mm", "inch"],
    "outside_temp_sensor_provider": ["", "home_assistant", "openhab"],
}

BOOLEANS = ["reverse_color_temp", "optimistic_mode", "show_screensaver_inside_temperature", "show_screensaver_outside_temperature", "clock_us_style", "use_fahrenheit"]

READ_ONLY = ["is_home_assistant_addon", "mqtt_password_set", "home_assistant_token_set", "openhab_token_set"]


def rows():
    return sorted(Settings.objects.values_list("id", "name", "value"))


def seed(table):
    Settings.objects.all().delete()
    for name, value in table.items():
        Settings.objects.create(name=name, value=value)


def writable(response_data):
    """A GET response as a PATCH body: everything but the read-only fields."""
    return {key: value for key, value in response_data.items() if key not in READ_ONLY}


class RestV2TestCase(NSPMTestCase):
    def setUp(self):
        super().setUp()
        seed(LEGACY_ROWS)

    def get(self):
        response = self.client.get(URL)
        self.assertEqual(response.status_code, 200, response.content)
        return response.json()

    def patch(self, data):
        return self.client.patch(URL, json.dumps(data) if not isinstance(data, str) else data, content_type="application/json")

    def assertPatchOk(self, data):
        response = self.patch(data)
        self.assertEqual(response.status_code, 200, response.content)
        return response.json()

    def assertPatchRejected(self, data, field=None):
        before = rows()
        reloads = self.mqttmanager.reloads
        response = self.patch(data)
        self.assertEqual(response.status_code, 400, f"{data!r} was accepted: {response.content!r}")
        if field is not None:
            self.assertIn(field, response.json(), f"Expected an error for {field}, got {response.content!r}")
        self.assertEqual(rows(), before, "A rejected PATCH changed the stored settings")
        self.assertEqual(self.mqttmanager.reloads, reloads, "A rejected PATCH reloaded MQTTManager")
        return response.json()


class CompatibilityTests(RestV2TestCase):
    """v2 must leave the stored settings in the format the legacy views and MQTTManager use."""

    def test_round_trip_leaves_every_row_byte_identical(self):
        before = rows()

        self.assertPatchOk(writable(self.get()))

        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()

    def test_round_trip_keeps_non_canonical_legacy_values(self):
        # Equivalent spellings that MQTTManager reads the same way are left alone, not rewritten.
        seed({**LEGACY_ROWS, "optimistic_mode": "true", "use_fahrenheit": "false", "reverse_color_temp": "FALSE", "manager_port": "", "mqtt_port": "01883"})
        before = rows()

        self.assertPatchOk(writable(self.get()))

        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()

    @needs_manager_source
    def test_round_trip_of_a_fresh_install(self):
        # The table MQTTManager creates on first start (populate_default_and_clean) can be saved back as is.
        seed(MANAGER_KEYS)
        before = rows()

        self.assertPatchOk(writable(self.get()))

        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()

    def test_values_are_stored_as_the_legacy_views_store_them(self):
        cases = [
            ("reverse_color_temp", True, "True"),
            ("reverse_color_temp", False, "False"),
            ("optimistic_mode", False, "False"),
            ("mqtt_port", 1884, "1884"),
            ("color_temp_min", 1500, "1500"),
            ("manager_port", 8001, "8001"),
            ("manager_port", None, ""),
            ("screensaver_mode", "no_screensaver", "no_screensaver"),
            ("outside_temp_sensor_provider", "", ""),
            ("date_format", "%H:%M ", "%H:%M "),
            ("mqtt_username", " spaced ", " spaced "),
            ("location_latitude", "51.5072", "51.5072"),
            ("home_assistant_address", "http://ha.example:8123/", "http://ha.example:8123"),
            ("mqtt_password", "new-password", "new-password"),
            ("buzzer_sound_touch", "Tick:d=32,o=6,b=200:c", "Tick:d=32,o=6,b=200:c"),
        ]
        for name, value, stored in cases:
            with self.subTest(name=name, value=value):
                self.assertPatchOk({name: value})
                self.assertEqual(Settings.objects.get(name=name).value, stored)
                # Every view reads settings through get_setting_with_default.
                self.assertEqual(get_setting_with_default(name), stored)

    def test_stored_values_are_read_as_the_manager_reads_them(self):
        # Booleans follow MqttManagerConfig::get_setting_with_default<bool>: lower-case, then == "true".
        # Integers that aren't plain digits (which std::stoi would half-parse) are reported as null.
        cases = [
            ("reverse_color_temp", "True", True),
            ("reverse_color_temp", "true", True),
            ("reverse_color_temp", "TRUE", True),
            ("reverse_color_temp", "False", False),
            ("reverse_color_temp", "false", False),
            ("reverse_color_temp", "", False),
            ("reverse_color_temp", "yes", False),
            ("reverse_color_temp", " true", False),
            ("mqtt_port", "1883", 1883),
            ("mqtt_port", "01883", 1883),
            ("max_log_buffer_size", "-1", -1),
            ("mqtt_port", "", None),
            ("mqtt_port", "abc", None),
            ("mqtt_port", " 1883", None),
            ("mqtt_port", "1883abc", None),
            ("mqtt_port", "18.83", None),
            ("manager_port", "", None),
            ("screensaver_mode", "with_background", "with_background"),
            ("screensaver_mode", "removed_mode", None),
            ("outside_temp_sensor_provider", "", ""),
            ("theme", "anything", "anything"),
        ]
        for name, raw, expected in cases:
            with self.subTest(name=name, raw=raw):
                set_setting_value(name, raw)
                self.assertEqual(self.get()[name], expected)

    def test_missing_rows_are_reported_as_null(self):
        Settings.objects.filter(name__in=["mqtt_port", "optimistic_mode", "screensaver_mode", "theme"]).delete()

        data = self.get()

        self.assertEqual([data[key] for key in ("mqtt_port", "optimistic_mode", "screensaver_mode", "theme")], [None, None, None, None])

    def test_missing_row_is_created(self):
        Settings.objects.filter(name="mqtt_wait_time").delete()

        self.assertPatchOk({"mqtt_wait_time": 1000})

        self.assertEqual(Settings.objects.get(name="mqtt_wait_time").value, "1000")
        self.assertManagerReloaded(times=1)

    def test_settings_saved_through_v2_read_back_through_v1_and_the_weather_page(self):
        self.assertPatchOk({"reverse_color_temp": True, "optimistic_mode": False, "mqtt_port": 1884, "screensaver_mode": "no_screensaver", "use_fahrenheit": False, "clock_us_style": True})

        v1 = self.client.get(reverse("rest_settings")).json()["settings"]
        self.assertEqual((v1["reverse_color_temp"], v1["optimistic_mode"], v1["mqtt_port"], v1["screensaver_mode"]), ("True", "False", "1884", "no_screensaver"))

        weather_page = self.client.get(reverse("weather_and_time")).context
        self.assertEqual((weather_page["use_fahrenheit"], weather_page["clock_us_style"]), ("False", "True"))

    def test_settings_saved_through_v1_read_back_through_v2(self):
        self.post_json(reverse("rest_settings"), {"settings": {"mqtt_port": "1885", "use_fahrenheit": "False"}})

        data = self.get()

        self.assertEqual((data["mqtt_port"], data["use_fahrenheit"]), (1885, False))

    def test_duplicate_rows_read_the_first_and_update_all(self):
        # Settings.name isn't unique. get_setting_with_default and MQTTManager use the first row.
        Settings.objects.create(name="mqtt_port", value="9999")

        self.assertEqual(self.get()["mqtt_port"], 1883)

        self.assertPatchOk({"mqtt_port": 1884})
        self.assertEqual(sorted(Settings.objects.filter(name="mqtt_port").values_list("value", flat=True)), ["1884", "1884"])
        self.assertEqual(get_setting_with_default("mqtt_port"), "1884")


class ManagerParityTests(RestV2TestCase):
    """v2's settings must match what MQTTManager knows, or the manager deletes them at startup."""

    @needs_manager_source
    def test_settings_match_the_manager_key_map(self):
        self.assertEqual(set(SettingsUpdateSerializer().fields) | {"is_home_assistant_addon"}, set(MANAGER_KEYS))

    @needs_manager_source
    def test_fixture_covers_every_manager_setting(self):
        self.assertEqual(set(LEGACY_ROWS), set(MANAGER_KEYS))

    @needs_manager_source
    def test_manager_defaults_are_valid_settings(self):
        seed({})
        for name, default in MANAGER_KEYS.items():
            if name == "is_home_assistant_addon":
                continue
            with self.subTest(name=name, default=default):
                set_setting_value(name, default)
                value = self.get()[f"{name}_set" if name in SECRETS else name]
                if name not in SECRETS:
                    self.assertTrue(value is not None or name == "manager_port", f"Default {default!r} reads as null")
                    self.assertPatchOk({name: value})

    def test_booleans_satisfy_the_manager_parser(self):
        for name in BOOLEANS:
            for value in (True, False):
                with self.subTest(name=name, value=value):
                    self.assertPatchOk({name: value})
                    stored = Settings.objects.get(name=name).value
                    self.assertEqual(stored.lower() == "true", value)  # get_setting_with_default<bool>
                    self.assertIn(stored, ("True", "False"))  # what the settings page radios post

    def test_choices_match_the_weather_page(self):
        # The weather page is still a Django form. (The React settings page's options are typed
        # against openapi.yaml's enums, so tsc checks those.)
        weather_page = (TEMPLATES / "weather_and_time.html").read_text()

        def radio_values(name):
            return re.findall(rf'type="radio" name="{name}" value="([^"]*)"', weather_page)

        for setting, form_name in (("weather_wind_speed_format", "wind_speed_format"), ("weather_precipitation_format", "precipitation_format")):
            with self.subTest(name=setting):
                self.assertTrue(radio_values(form_name), f"Couldn't find the weather page's values for {setting}")
                self.assertEqual(sorted(SettingsUpdateSerializer().fields[setting].choices), sorted(radio_values(form_name)))
                self.assertEqual(sorted(CHOICES[setting]), sorted(radio_values(form_name)))

        for name in ("clock_us_style", "use_fahrenheit"):
            with self.subTest(name=name):
                self.assertEqual(sorted(radio_values(name)), ["False", "True"])


class GetTests(RestV2TestCase):
    def test_returns_every_setting_typed(self):
        data = self.get()

        self.assertEqual(set(data), set(SettingsSerializer().fields))
        for name in INTEGER_RANGES:
            self.assertIsInstance(data[name], int, name)
        for name in BOOLEANS:
            self.assertIsInstance(data[name], bool, name)
        for name, choices in CHOICES.items():
            self.assertIn(data[name], choices, name)

    def test_secrets_are_never_returned(self):
        secret_values = [LEGACY_ROWS["mqtt_password"], LEGACY_ROWS["home_assistant_token"]]
        responses = [
            self.client.get(URL),
            self.patch({"mqtt_port": 1884}),
            self.patch({"mqtt_password": "x" * 300}),  # a validation error about a secret
            self.patch({"home_assistant_token": "another-secret-value", "mqtt_port": 0}),
        ]
        for response in responses:
            body = response.content.decode()
            for value in [*secret_values, "another-secret-value"]:
                self.assertNotIn(value, body)
            if response.status_code == 200:  # an error may name a secret's field, but not its value
                for secret in SECRETS:
                    self.assertNotIn(secret, response.json())

    def test_secret_flags(self):
        cases = {"missing": None, "blank": "", "set": "value"}
        for case, value in cases.items():
            with self.subTest(case=case):
                Settings.objects.filter(name="openhab_token").delete()
                if value is not None:
                    set_setting_value("openhab_token", value)
                self.assertIs(self.get()["openhab_token_set"], case == "set")

    def test_addon_flag_follows_the_environment_not_the_database(self):
        set_setting_value("is_home_assistant_addon", "true")
        with mock.patch.dict(os.environ, {}, clear=False):
            os.environ.pop("IS_HOME_ASSISTANT_ADDON", None)
            self.assertIs(self.get()["is_home_assistant_addon"], False)

        set_setting_value("is_home_assistant_addon", "false")
        with mock.patch.dict(os.environ, {"IS_HOME_ASSISTANT_ADDON": "true"}):
            self.assertIs(self.get()["is_home_assistant_addon"], True)

    def test_empty_database(self):
        seed({})

        data = self.get()

        self.assertTrue(all(value is None for key, value in data.items() if key not in READ_ONLY))
        self.assertEqual([data[f"{secret}_set"] for secret in SECRETS], [False, False, False])

    def test_get_changes_nothing(self):
        before = rows()

        self.get()
        self.client.head(URL)

        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()


class PatchTests(RestV2TestCase):
    def test_only_the_sent_settings_change(self):
        before = {name: value for _, name, value in rows()}

        data = self.assertPatchOk({"mqtt_port": 1884, "screensaver_mode": "no_screensaver"})

        after = {name: value for _, name, value in rows()}
        self.assertEqual({name for name in after if after[name] != before.get(name)}, {"mqtt_port", "screensaver_mode"})
        self.assertEqual((data["mqtt_port"], data["screensaver_mode"]), (1884, "no_screensaver"))
        self.assertManagerReloaded(times=1)

    def test_unchanged_values_are_not_written_and_do_not_reload(self):
        before = rows()

        self.assertPatchOk({"mqtt_port": 1883, "reverse_color_temp": False, "mqtt_password": LEGACY_ROWS["mqtt_password"]})
        self.assertPatchOk({})
        response = self.client.patch(URL, "", content_type="application/json")
        self.assertEqual(response.status_code, 200)

        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()

    def test_response_is_the_new_state(self):
        data = self.assertPatchOk({"mqtt_port": 1884})

        self.assertEqual(data, self.get())

    def test_secrets(self):
        with self.subTest("absent keeps"):
            self.assertPatchOk({"mqtt_port": 1884})
            self.assertEqual(get_setting_with_default("mqtt_password"), LEGACY_ROWS["mqtt_password"])

        with self.subTest("a value sets"):
            data = self.assertPatchOk({"mqtt_password": "new-password"})
            self.assertEqual(get_setting_with_default("mqtt_password"), "new-password")
            self.assertIs(data["mqtt_password_set"], True)

        with self.subTest("blank clears"):
            data = self.assertPatchOk({"mqtt_password": ""})
            self.assertEqual(get_setting_with_default("mqtt_password"), "")
            self.assertIs(data["mqtt_password_set"], False)

        with self.subTest("clearing a blank secret changes nothing"):
            self.mqttmanager.reset()
            self.assertPatchOk({"openhab_token": ""})
            self.assertManagerNotReloaded()

    def test_a_trailing_slash_is_removed_from_addresses(self):
        for name in ("home_assistant_address", "openhab_address"):
            for sent, stored in (("http://host:8080/", "http://host:8080"), ("http://host//", "http://host/"), ("http://host", "http://host"), ("", "")):
                with self.subTest(name=name, sent=sent):
                    data = self.assertPatchOk({name: sent})
                    self.assertEqual(get_setting_with_default(name), stored)
                    self.assertEqual(data[name], stored)

    def test_home_assistant_settings_are_locked_in_the_addon(self):
        with mock.patch.dict(os.environ, {"IS_HOME_ASSISTANT_ADDON": "true"}):
            self.assertPatchRejected({"home_assistant_address": "http://elsewhere"}, "home_assistant_address")
            self.assertPatchRejected({"home_assistant_token": "t", "mqtt_port": 1884}, "home_assistant_token")
            self.assertPatchOk({"mqtt_port": 1884})

        self.assertPatchOk({"home_assistant_address": "http://elsewhere"})


class ValidationTests(RestV2TestCase):
    def test_integer_ranges(self):
        self.assertEqual({name for name, field in SettingsUpdateSerializer().fields.items() if type(field).__name__ == "StrictIntegerField"}, set(INTEGER_RANGES))
        for name, (low, high) in INTEGER_RANGES.items():
            with self.subTest(name=name):
                seed({})  # so the min < max pairs don't interfere
                self.assertPatchOk({name: low})
                self.assertPatchOk({name: high})
                self.assertPatchRejected({name: low - 1}, name)
                self.assertPatchRejected({name: high + 1}, name)

    def test_wrong_types(self):
        cases = [
            ("mqtt_port", "1883"),
            ("mqtt_port", 1883.0),
            ("mqtt_port", True),
            ("mqtt_port", None),
            ("mqtt_port", [1883]),
            ("reverse_color_temp", "True"),
            ("reverse_color_temp", 1),
            ("reverse_color_temp", "yes"),
            ("reverse_color_temp", None),
            ("mqtt_server", 5),
            ("mqtt_server", None),
            ("mqtt_server", ["mqtt.local"]),
            ("mqtt_password", 1234),
            ("screensaver_mode", 1),
            ("screensaver_mode", None),
        ]
        for name, value in cases:
            with self.subTest(name=name, value=value):
                self.assertPatchRejected({name: value}, name)

    def test_choices(self):
        self.assertEqual({name for name, field in SettingsUpdateSerializer().fields.items() if type(field).__name__ == "StrictChoiceField"}, set(CHOICES))
        for name, choices in CHOICES.items():
            with self.subTest(name=name):
                for choice in choices:
                    self.assertPatchOk({name: choice})
                self.assertPatchRejected({name: "bogus"}, name)
                if "" not in choices:
                    self.assertPatchRejected({name: ""}, name)

    def test_blank_strings(self):
        for name in ("date_format", "theme"):
            with self.subTest(name=name):
                self.assertPatchRejected({name: ""}, name)
        for name in ("mqtt_server", "mqtt_username", "manager_address", "outside_temp_sensor_entity_id", "buzzer_sound_touch", "location_latitude"):
            with self.subTest(name=name):
                self.assertPatchOk({name: ""})

    def test_coordinates(self):
        for name, limit in (("location_latitude", 90), ("location_longitude", 180)):
            for value in ("0", "-33.8688", f"{limit}", f"-{limit}", "12.000001"):
                with self.subTest(name=name, value=value):
                    self.assertPatchOk({name: value})
            for value in ("abc", f"{limit}.1", f"-{limit}.0001", "nan", "inf", "1e3", "12,5"):
                with self.subTest(name=name, value=value):
                    self.assertPatchRejected({name: value}, name)

    def test_string_length(self):
        self.assertPatchOk({"mqtt_server": "a" * 255})
        self.assertPatchRejected({"mqtt_server": "a" * 256}, "mqtt_server")

    def test_pairs_are_checked_against_the_stored_value(self):
        # Stored: color_temp_min 2000, color_temp_max 6000.
        self.assertPatchRejected({"color_temp_min": 6000}, "color_temp_min")
        self.assertPatchRejected({"color_temp_max": 2000}, "color_temp_max")
        self.assertPatchRejected({"color_temp_min": 7000, "color_temp_max": 6500}, "color_temp_min")
        self.assertPatchOk({"color_temp_min": 5999})
        self.assertPatchOk({"color_temp_min": 7000, "color_temp_max": 8000})

    def test_pairs_with_an_unreadable_stored_half_are_not_checked(self):
        set_setting_value("color_temp_max", "garbage")

        self.assertPatchOk({"color_temp_min": 9000})

    def test_one_bad_setting_saves_nothing(self):
        errors = self.assertPatchRejected({"mqtt_port": 1884, "screen_dim_level": 0, "reverse_color_temp": True})

        self.assertEqual(set(errors), {"screen_dim_level"})

    def test_every_bad_setting_is_reported(self):
        errors = self.assertPatchRejected({"mqtt_port": 0, "screen_dim_level": 0, "screensaver_mode": "bogus"})

        self.assertEqual(set(errors), {"mqtt_port", "screen_dim_level", "screensaver_mode"})

    def test_unknown_and_read_only_settings(self):
        errors = self.assertPatchRejected({"mqtt_port": 1884, "mqtt_sever": "typo", "dark_theme": "true"})
        self.assertEqual(errors, {"mqtt_sever": ["Unknown setting."], "dark_theme": ["Unknown setting."]})

        for name in READ_ONLY:
            with self.subTest(name=name):
                errors = self.assertPatchRejected({name: True})
                self.assertEqual(errors, {name: ["This setting is read-only."]})

    def test_request_shapes(self):
        for body in ("[]", '["mqtt_port"]', '"mqtt_port"', "null", "5"):
            with self.subTest(body=body):
                self.assertIn("non_field_errors", self.assertPatchRejected(body))

        errors = self.assertPatchRejected("{not json")
        self.assertIn("detail", errors)

    def test_only_json_is_accepted(self):
        before = rows()

        response = self.client.patch(URL, "mqtt_port=1884", content_type="application/x-www-form-urlencoded")

        self.assertEqual(response.status_code, 415)
        self.assertEqual(rows(), before)

    def test_unsupported_methods(self):
        before = rows()
        for method in ("put", "post", "delete"):
            with self.subTest(method=method):
                response = getattr(self.client, method)(URL, json.dumps({"mqtt_port": 1884}), content_type="application/json")
                self.assertEqual(response.status_code, 405)
        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()


class CsrfTests(RestV2TestCase):
    """v2 keeps the CSRF protection CsrfViewMiddleware gives the plain views (the UI sends X-CSRFToken)."""

    def setUp(self):
        super().setUp()
        self.client = Client(enforce_csrf_checks=True, raise_request_exception=False)
        self.token = get_random_string(32)

    def test_patch_without_a_token_is_rejected(self):
        before = rows()

        response = self.patch({"mqtt_port": 1884})

        self.assertEqual(response.status_code, 403)
        self.assertIn("CSRF", response.json()["detail"])
        self.assertEqual(rows(), before)
        self.assertManagerNotReloaded()

    def test_patch_with_a_token_is_accepted(self):
        self.client.cookies["csrftoken"] = self.token

        response = self.client.patch(URL, json.dumps({"mqtt_port": 1884}), content_type="application/json", headers={"X-CSRFToken": self.token})

        self.assertEqual(response.status_code, 200, response.content)
        self.assertEqual(get_setting_with_default("mqtt_port"), "1884")

    def test_patch_with_the_wrong_token_is_rejected(self):
        self.client.cookies["csrftoken"] = self.token

        response = self.client.patch(URL, json.dumps({"mqtt_port": 1884}), content_type="application/json", headers={"X-CSRFToken": get_random_string(32)})

        self.assertEqual(response.status_code, 403)

    def test_get_needs_no_token(self):
        self.assertEqual(self.client.get(URL).status_code, 200)

    def test_v1_is_protected_the_same_way(self):
        self.assertEqual(self.post_json(reverse("rest_settings"), {"settings": {"mqtt_port": "1884"}}).status_code, 403)


class SchemaShimTests(NSPMTestCase):
    """The release image doesn't install drf-spectacular; the views must import without it."""

    def test_extend_schema_is_a_no_op_without_drf_spectacular(self):
        def view():
            return "response"

        try:
            with mock.patch.dict(sys.modules, {"drf_spectacular": None, "drf_spectacular.utils": None}):
                shim = importlib.reload(web.rest_v2.schema)
                self.assertIs(shim.extend_schema(responses=object())(view), view)
        finally:
            importlib.reload(web.rest_v2.schema)
