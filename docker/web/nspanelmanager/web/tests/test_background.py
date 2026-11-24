"""
The background image panels download for their LVGL interface.
"""

from django.urls import reverse

from web.background import LANDSCAPE_SIZE, PORTRAIT_SIZE
from web.settings_helper import set_nspanel_setting_value

from .harness import NSPMTestCase


WIDTH, HEIGHT = LANDSCAPE_SIZE


class BackgroundImageTests(NSPMTestCase):
    def url(self, panel_id=1):
        return reverse("download_background", kwargs={"panel_id": panel_id})

    def test_image_is_raw_rgb565_at_display_size(self):
        response = self.client.get(self.url())

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response["Content-Type"], "application/octet-stream")
        self.assertEqual(len(response.content), WIDTH * HEIGHT * 2)
        self.assertTrue(response["ETag"].startswith('"'))

    def test_gradient_is_blue_and_gets_lighter(self):
        data = self.client.get(self.url()).content

        def pixel(x, y):
            value = data[(y * WIDTH + x) * 2] | (data[(y * WIDTH + x) * 2 + 1] << 8)
            return value >> 11, (value >> 5) & 0x3F, value & 0x1F  # r5, g6, b5

        top_left, bottom_right = pixel(0, 0), pixel(WIDTH - 1, HEIGHT - 1)
        self.assertGreater(bottom_right[2], top_left[2])
        self.assertGreater(bottom_right[2], bottom_right[0])

    def test_unchanged_image_is_not_sent_again(self):
        etag = self.client.get(self.url())["ETag"]

        response = self.client.get(self.url(), headers={"If-None-Match": etag})

        self.assertEqual(response.status_code, 304)
        self.assertEqual(response.content, b"")
        self.assertEqual(response["ETag"], etag)

    def test_stale_etag_gets_the_image(self):
        response = self.client.get(self.url(), headers={"If-None-Match": '"stale"'})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(response.content), WIDTH * HEIGHT * 2)

    def test_vertical_us_panel_gets_a_portrait_image(self):
        panel = self.make_panel(self.make_room("Kitchen"), "US panel")
        set_nspanel_setting_value(panel.id, "is_us_panel", "True")
        set_nspanel_setting_value(panel.id, "us_panel_orientation", "vertical")

        response = self.client.get(self.url(panel.id))

        self.assertEqual(len(response.content), PORTRAIT_SIZE[0] * PORTRAIT_SIZE[1] * 2)

    def test_horizontal_us_panel_gets_the_landscape_image(self):
        panel = self.make_panel(self.make_room("Kitchen"), "US panel")
        set_nspanel_setting_value(panel.id, "is_us_panel", "True")
        set_nspanel_setting_value(panel.id, "us_panel_orientation", "horizontal")

        response = self.client.get(self.url(panel.id))

        self.assertEqual(len(response.content), WIDTH * HEIGHT * 2)
