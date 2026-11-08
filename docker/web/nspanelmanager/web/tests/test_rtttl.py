"""
web/rest_v2/rtttl.py, the port of the panel firmware's RTTTL parser, and its use in /rest/v2/settings.

rtttl_vectors.json is the firmware parser's own output, so these tests check the port against the firmware
rather than against a reading of it. The UI's src/lib/rtttl.ts is tested against the same file.
"""

import json
from pathlib import Path
from unittest import TestCase

from web.models import Settings
from web.rest_v2 import rtttl

from .harness import NSPMTestCase
from .test_rest_v2_settings import URL

VECTORS = json.loads((Path(__file__).resolve().parent / "rtttl_vectors.json").read_text())["vectors"]
BUZZER_SETTINGS = ("buzzer_sound_touch", "buzzer_sound_scene_activated", "buzzer_sound_scene_saved")


class RtttlParserTests(TestCase):
    def test_matches_firmware(self):
        self.assertGreater(len(VECTORS), 50)
        for vector in VECTORS:
            with self.subTest(rtttl=vector["rtttl"]):
                if "steps" in vector:
                    self.assertEqual([list(step) for step in rtttl.parse(vector["rtttl"])], vector["steps"])
                else:
                    expected = rtttl.RtttlTooLong if vector["error"] == "too_long" else rtttl.RtttlError
                    with self.assertRaises(rtttl.RtttlError) as caught:
                        rtttl.parse(vector["rtttl"])
                    self.assertIs(type(caught.exception), expected)

    def test_vectors_cover_both_errors(self):
        errors = {vector.get("error") for vector in VECTORS}
        self.assertEqual(errors, {None, "invalid", "too_long"})

    def test_unicode_is_not_folded(self):
        # The firmware only knows ASCII: a non-breaking space isn't whitespace there, and no Unicode digit is a digit.
        for text in ("::c ", "::١c", "::c٢", "：：c"):
            with self.subTest(text=text), self.assertRaises(rtttl.RtttlError):
                rtttl.parse(text)

    def test_messages(self):
        cases = {
            "c,d": "name:defaults:notes",
            ":b=901:c": "b must be 1 to 900",
            ":o=9:c": "o must be 1 to 8",
            ":d=3:c": "d must be 1, 2, 4",
            ":x=1:c": "Unknown default 'x'",
            "::c,e,q": "Note 3 ('q')",
            "::c,,d": "Note 2 ('')",
            "x:d=1,o=6,b=8:c,d": "at most 30 seconds",
        }
        for text, message in cases.items():
            with self.subTest(text=text), self.assertRaises(rtttl.RtttlError) as caught:
                rtttl.parse(text)
            self.assertIn(message, str(caught.exception))


class BuzzerSettingsApiTests(NSPMTestCase):
    def patch(self, data):
        return self.client.patch(URL, json.dumps(data), content_type="application/json")

    def test_valid_rtttl_is_stored_as_sent(self):
        for name in BUZZER_SETTINGS:
            for value in ("tick:d=64,o=7,b=625:b", " Chime : d=32, o=6, b=190 : g6,16p,32c7 ", ""):
                with self.subTest(name=name, value=value):
                    response = self.patch({name: value})
                    self.assertEqual(response.status_code, 200, response.content)
                    self.assertEqual(Settings.objects.get(name=name).value, value)

    def test_invalid_rtttl_is_rejected_with_a_field_error(self):
        Settings.objects.update_or_create(name="buzzer_sound_touch", defaults={"value": "tick:d=64,o=7,b=625:b"})
        for value in ("beep", ":b=999:c", "::c,,d", "x:d=1,o=6,b=8:c,d", " "):
            with self.subTest(value=value):
                response = self.patch({"buzzer_sound_touch": value})
                self.assertEqual(response.status_code, 400, response.content)
                self.assertEqual(list(response.json()), ["buzzer_sound_touch"])
                self.assertEqual(Settings.objects.get(name="buzzer_sound_touch").value, "tick:d=64,o=7,b=625:b")

    def test_invalid_rtttl_writes_nothing_else(self):
        response = self.patch({"buzzer_sound_scene_saved": "saved:d=16,o=6,b=200:c,e,g,8c7", "buzzer_sound_touch": "::x"})
        self.assertEqual(response.status_code, 400)
        self.assertFalse(Settings.objects.filter(name="buzzer_sound_scene_saved").exclude(value="").exists())

    def test_stored_invalid_rtttl_is_still_returned(self):
        # Written before validation existed, or through /rest/settings. GET shows it so the UI can flag it.
        Settings.objects.update_or_create(name="buzzer_sound_touch", defaults={"value": "not rtttl"})
        self.assertEqual(self.client.get(URL).json()["buzzer_sound_touch"], "not rtttl")
