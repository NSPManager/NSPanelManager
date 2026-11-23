"""
The background image panels download for their LVGL interface.
"""

from django.urls import reverse

from web.background import HEIGHT, WIDTH

from .harness import NSPMTestCase


class BackgroundImageTests(NSPMTestCase):
    def url(self):
        return reverse("download_background", kwargs={"panel_id": 1})

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
