"""
Firmware, LittleFS and TFT images: uploading from the UI, and the checksum and download
endpoints panels use to decide whether to update and to fetch the image.
"""

import hashlib
import shutil
import tempfile
from unittest import expectedFailure

from django.core.files.uploadedfile import SimpleUploadedFile
from django.test import override_settings
from django.urls import reverse

from web.settings_helper import set_nspanel_setting_value

from .harness import NSPMTestCase

FIRMWARE = bytes(range(256)) * 4  # 1 KiB with a recognisable pattern


def md5(data):
    return hashlib.md5(data).hexdigest()


class FileTestCase(NSPMTestCase):
    def setUp(self):
        super().setUp()
        media_root = tempfile.mkdtemp(prefix="nspm-media-")
        self.addCleanup(shutil.rmtree, media_root, ignore_errors=True)
        override = override_settings(MEDIA_ROOT=media_root)
        override.enable()
        self.addCleanup(override.disable)

    def upload(self, url_name, field, data, **form):
        return self.client.post(reverse(url_name), {field: SimpleUploadedFile("upload.bin", data), **form})


class FirmwareTests(FileTestCase):
    def test_upload_then_checksum_and_download(self):
        response = self.upload("save_new_firmware", "firmware", FIRMWARE, model="sonoff")

        self.assertRedirects(response, "/", fetch_redirect_response=False)
        self.assertEqual(self.client.get(reverse("checksum_firmware")).content.decode(), md5(FIRMWARE))
        self.assertEqual(self.client.get(reverse("download_firmware")).content, FIRMWARE)
        self.assertManagerReloaded(times=1)

    def test_models_are_stored_separately(self):
        self.upload("save_new_firmware", "firmware", b"sonoff", model="sonoff")
        self.upload("save_new_firmware", "firmware", b"custom", model="custom")

        self.assertEqual(self.client.get(reverse("download_firmware"), {"model": "custom"}).content, b"custom")
        self.assertEqual(self.client.get(reverse("download_firmware"), {"model": "sonoff"}).content, b"sonoff")
        self.assertEqual(self.client.get(reverse("checksum_firmware"), {"model": "custom"}).content.decode(), md5(b"custom"))

    def test_upload_replaces_previous_image(self):
        self.upload("save_new_firmware", "firmware", b"old", model="sonoff")
        self.upload("save_new_firmware", "firmware", b"new", model="sonoff")

        self.assertEqual(self.client.get(reverse("download_firmware")).content, b"new")

    def test_upload_without_model_is_ignored(self):
        self.upload("save_new_firmware", "firmware", FIRMWARE)

        self.assertManagerNotReloaded()
        self.assertRejected(self.client.get(reverse("download_firmware")))

    def test_unknown_model_is_rejected(self):
        for url_name in ("download_firmware", "checksum_firmware", "download_data_file", "checksum_data_file"):
            with self.subTest(url_name=url_name):
                self.assertEqual(self.client.get(reverse(url_name), {"model": "../../etc"}).status_code, 400)

    def test_range_requests_use_an_exclusive_end(self):
        # CONTRACT with the panel firmware (NSPanelManager-firmware, UpdateManager.cpp): the
        # "bytes=start-end" end is treated as EXCLUSIVE (Python slice), not inclusive as RFC 7233
        # says. The firmware deliberately sends ranges in the same convention. Changing either
        # side alone corrupts OTA updates.
        self.upload("save_new_firmware", "firmware", FIRMWARE, model="sonoff")

        response = self.client.get(reverse("download_firmware"), HTTP_RANGE="bytes=100-200")

        self.assertEqual(response.content, FIRMWARE[100:200])
        self.assertEqual(len(response.content), 100)

    def test_open_ended_range(self):
        self.upload("save_new_firmware", "firmware", FIRMWARE, model="sonoff")

        self.assertEqual(self.client.get(reverse("download_firmware"), HTTP_RANGE="bytes=1000-").content, FIRMWARE[1000:])

    def test_merged_flash_upload_does_not_reload_manager(self):
        # MQTTManager does not track the merged image's checksum, so there is nothing to reload.
        self.upload("save_new_merged_flash", "bin", FIRMWARE, model="sonoff")

        self.assertManagerNotReloaded()

    @expectedFailure
    def test_checksum_of_missing_image_is_not_found(self):
        # KNOWN GAP: when no image has been uploaded the checksum endpoints return HTTP 200 with
        # the body "None" (str(None)), which a panel cannot tell apart from a real checksum.
        response = self.client.get(reverse("checksum_firmware"))

        self.assertEqual(response.status_code, 404)


class DataFileTests(FileTestCase):
    def test_upload_then_checksum_and_download(self):
        self.upload("save_new_data_file", "data_file", FIRMWARE, model="sonoff")

        self.assertEqual(self.client.get(reverse("checksum_data_file")).content.decode(), md5(FIRMWARE))
        self.assertEqual(self.client.get(reverse("download_data_file")).content, FIRMWARE)
        self.assertEqual(self.client.get(reverse("download_data_file"), HTTP_RANGE="bytes=0-16").content, FIRMWARE[:16])
        self.assertManagerReloaded(times=1)


class TFTTests(FileTestCase):
    def setUp(self):
        super().setUp()
        self.room = self.make_room()
        self.panel = self.make_panel(self.room)

    def upload_tft(self, tft_file_type, data):
        return self.upload("save_new_tft_file", "tft_file", data, tft_file_type=tft_file_type)

    def panel_tft(self, **headers):
        return self.client.get(reverse("download_tft", kwargs={"panel_id": self.panel.id}), **headers).content

    def configure_panel(self, is_us_panel="False", orientation=None, tft="tft1"):
        set_nspanel_setting_value(self.panel.id, "is_us_panel", is_us_panel)
        set_nspanel_setting_value(self.panel.id, "selected_tft", tft)
        if orientation:
            set_nspanel_setting_value(self.panel.id, "us_panel_orientation", orientation)

    def test_each_panel_gets_the_image_for_its_type_and_selected_tft(self):
        for tft_file_type in ("eu1", "eu3", "us2", "us4_horizontal_mirrored"):
            self.upload_tft(tft_file_type, tft_file_type.encode())

        cases = [
            ({}, b"eu1"),  # defaults: EU panel, tft1
            ({"tft": "tft3"}, b"eu3"),
            ({"is_us_panel": "True", "orientation": "vertical", "tft": "tft2"}, b"us2"),
            # A horizontal US panel has the same screen orientation as an EU panel.
            ({"is_us_panel": "True", "orientation": "horizontal", "tft": "tft3"}, b"eu3"),
            ({"is_us_panel": "True", "orientation": "horizontal_mirrored", "tft": "tft4"}, b"us4_horizontal_mirrored"),
        ]
        for config, expected in cases:
            with self.subTest(config=config):
                self.configure_panel(**config)
                self.assertEqual(self.panel_tft(), expected)
                self.assertEqual(self.client.get(reverse("checksum_tft_file", kwargs={"panel_id": self.panel.id})).content.decode(), md5(expected))

    def test_tft_range_download(self):
        self.upload_tft("eu1", FIRMWARE)

        self.assertEqual(self.panel_tft(HTTP_RANGE="bytes=10-20"), FIRMWARE[10:20])

    def test_upload_reloads_manager(self):
        self.upload_tft("eu1", FIRMWARE)

        self.assertManagerReloaded(times=1)

    def test_unknown_tft_type_is_rejected(self):
        self.assertEqual(self.upload_tft("eu9", FIRMWARE).status_code, 500)
        self.assertManagerNotReloaded()
