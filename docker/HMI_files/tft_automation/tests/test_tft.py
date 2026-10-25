"""nextion_tools.tft and tft_build.themes against the TFTs the Editor compiled for each theme."""
import os
import shutil
import struct
import tempfile
import unittest

import build_tfts
from nextion_tools import images, tft
from tft_build import region, themes

REGIONS = {name: region.path(name) for name in ('eu', 'us')}

_built = {}


def built(name):
    if name not in _built:
        _built[name] = themes.build(read(name, 1), REGIONS[name])
    return _built[name]


def read(name, theme):
    with open(region.tft(REGIONS[name], theme), 'rb') as f:
        return f.read()


class ChecksumTest(unittest.TestCase):
    def test_reproduces_every_checksum_in_the_editors_tfts(self):
        for name in REGIONS:
            for theme in region.THEMES:
                self.assertTrue(tft.checksums_ok(read(name, theme)), f'{name} tft{theme}')

    def test_detects_a_changed_byte(self):
        data = bytearray(read('us', 1))
        data[tft.pictures(data)[-1].start + 100] ^= 1
        self.assertFalse(tft.checksums_ok(bytes(data)))


class OrientationTest(unittest.TestCase):
    def test_every_committed_tft_has_its_regions_orientation(self):
        expected = {**themes.ORIENTATIONS, **{layout: degrees for layout, (_, degrees) in themes.ROTATED.items()}}
        for name, degrees in expected.items():
            for theme in region.THEMES:
                with open(region.tft(region.path(name), theme), 'rb') as f:
                    self.assertEqual(tft.orientation(f.read()), degrees, f'{name} tft{theme}')

    def test_rotated_layouts_are_their_source_at_another_orientation(self):
        # Upstream compiled these in the Editor; they differ from the source only in the
        # orientation byte and the checksums that cover it.
        for layout, (source, degrees) in themes.ROTATED.items():
            for theme in region.THEMES:
                with open(region.tft(region.path(layout), theme), 'rb') as f:
                    self.assertEqual(tft.with_orientation(read(source, theme), degrees), f.read(), f'{layout} tft{theme}')

    def test_changing_the_orientation_changes_only_header_1_and_the_file_checksum(self):
        data = read('eu', 1)
        turned = tft.with_orientation(data, 90)
        self.assertTrue(tft.checksums_ok(turned))
        self.assertEqual({i for i in range(len(data)) if data[i] != turned[i]} - {tft.ORIENTATION},
                         set(range(0xC4, 0xC8)) | set(range(len(data) - 4, len(data))))

    def test_build_refuses_a_template_with_the_wrong_orientation(self):
        with self.assertRaises(SystemExit):
            build_tfts.main(['eu', '--template', region.tft(region.path('us_horizontal_mirrored'), 1)])


class BuildTest(unittest.TestCase):
    def test_rebuilds_the_committed_tfts(self):
        for name in REGIONS:
            for theme, data in built(name).items():
                self.assertTrue(tft.checksums_ok(data))
                self.assertEqual(data, read(name, theme), f'{name} tft{theme}')

    def test_refuses_pictures_that_do_not_fit_the_template(self):
        template = read('us', 1)
        with tempfile.TemporaryDirectory() as out:
            for theme in region.THEMES:
                shutil.copytree(region.numbered_dir(REGIONS['us'], theme), region.numbered_dir(out, theme))
            # A picture added since the template was compiled.
            count = len(tft.pictures(template))
            extra = os.path.join(region.numbered_dir(out, 1), f'{count}.bmp')
            shutil.copy(os.path.join(region.numbered_dir(out, 1), '180.bmp'), extra)
            with self.assertRaisesRegex(ValueError, 'compile a new template'):
                themes.build(template, out)
            os.remove(extra)
            # A theme picture of a different size.
            small = os.path.join(region.numbered_dir(out, 2), '74.bmp')
            shutil.copy(os.path.join(region.numbered_dir(out, 2), '73.bmp'), small)
            with open(small, 'r+b') as f:
                f.seek(18)
                f.write(struct.pack('<i', 2))
            with self.assertRaisesRegex(ValueError, 'picture 74'):
                themes.build(template, out)

    def test_refuses_a_record_that_does_not_fill_its_slot(self):
        template = read('us', 1)
        slot = tft.pictures(template)[0]
        with self.assertRaises(ValueError):
            tft.replace_pictures(template, {slot: bytes(slot.size - 1)})

    def test_rgb565_matches_the_editor(self):
        pixels = images.read(os.path.join(region.numbered_dir(REGIONS['eu'], 1), '180.bmp'))
        data = read('eu', 1)
        slot = tft.pictures(data)[180]
        self.assertEqual(pixels.shape[:2], (slot.h, slot.w))
        self.assertEqual(tft.raw_record(images.rgb565(pixels)), data[slot.start:slot.start + slot.size])


if __name__ == '__main__':
    unittest.main()
