"""Tests for nextion.bars and nextion.images, using upstream's own slider (items12.slider_save)."""
import os
import shutil
import tempfile
import unittest

import numpy as np

from nextion import bars, images, region
from tests.test_hmi import load

REGION = region.path('us')


class BarPicturesTest(unittest.TestCase):
    def test_pictures_fit_the_bar_and_the_track_is_the_background(self):
        project = load('us')
        page = project.page('items12')
        background = page.component('items12').get(page.block, 'pic')
        (_, x, y, w, h, bpic, ppic), = bars.bars(project, 'items12', ['slider_save'])
        with tempfile.TemporaryDirectory() as out:
            bars.make(project, REGION, 'items12', [('slider_save', (1, 2, 3))], out)
            for theme in region.THEMES:
                track = images.read(os.path.join(out, f'tft{theme}', f'{bpic}.bmp'))
                fill = images.read(os.path.join(out, f'tft{theme}', f'{ppic}.bmp'))
                bg = images.read(os.path.join(region.numbered_dir(REGION, theme), f'{background}.bmp'))
                np.testing.assert_array_equal(track, bg[y:y + h, x:x + w])
                np.testing.assert_array_equal(fill, np.full((h, w, 3), (1, 2, 3)))

    def test_numbered_writes_beside_the_theme_pictures_and_never_over_its_sources(self):
        project = load('us')
        page = project.page('items12')
        background = page.component('items12').get(page.block, 'pic')
        with tempfile.TemporaryDirectory() as out:
            for theme in region.THEMES:
                numbered = region.numbered_dir(out, theme)
                os.makedirs(numbered)
                shutil.copy(os.path.join(region.numbered_dir(REGION, theme), f'{background}.bmp'), numbered)
            # slider_save's own pictures (73/74) are GIMP's, so writing them into all_numbered
            # would replace them.
            with self.assertRaises(ValueError):
                bars.make(project, out, 'items12', [('slider_save', (1, 2, 3))], numbered_after=180)
            # A bar whose pictures come after the background is written beside it.
            original = bars.bars
            bars.bars = lambda *a: [('bar', 0, 0, 4, 2, background + 1, background + 2)]
            try:
                written = bars.make(project, out, 'items12', [('bar', (1, 2, 3))], numbered_after=background)
            finally:
                bars.bars = original
            self.assertEqual(sorted(path for _, path in written),
                             sorted(os.path.join(region.numbered_dir(out, t), f'{background + n}.bmp')
                                    for t in region.THEMES for n in (1, 2)))

    def test_progress_bars_txt_gives_every_bar_a_colour(self):
        with tempfile.TemporaryDirectory() as region_dir:
            listed = os.path.join(region_dir, 'progress_bars.txt')
            with open(listed, 'w') as f:
                f.write('# comment\n\nmediaplayer p_pos=#DEAA31 p_volume=#00ff80\n')
            self.assertEqual(region.progress_bars(region_dir),
                             [('mediaplayer', [('p_pos', (0xde, 0xaa, 0x31)), ('p_volume', (0x00, 0xff, 0x80))])])
            for line in ('mediaplayer p_pos', 'mediaplayer p_pos=DEAA31', 'mediaplayer p_pos=#DEA', 'mediaplayer'):
                with open(listed, 'w') as f:
                    f.write(line + '\n')
                with self.assertRaises(ValueError, msg=line):
                    region.progress_bars(region_dir)

    def test_refuses_objects_that_are_not_progress_bars(self):
        with self.assertRaises(ValueError):
            bars.bars(load('us'), 'items12', ['items12'])
        with self.assertRaises(KeyError):
            bars.bars(load('us'), 'items12', ['nonesuch'])

    def test_committed_bar_pictures_are_byte_identical_to_a_fresh_build(self):
        # Including the BMP header, so a make_pictures.sh run leaves them unchanged in git.
        for name in ('eu', 'us'):
            region_dir = region.path(name)
            project = load(name)
            for page, listed in region.progress_bars(region_dir):
                if page not in project.pages:
                    continue
                with tempfile.TemporaryDirectory() as out:
                    for theme, path in bars.make(project, region_dir, page, listed, out):
                        committed = os.path.join(region.numbered_dir(region_dir, theme), os.path.basename(path))
                        with open(path, 'rb') as a, open(committed, 'rb') as b:
                            self.assertEqual(a.read(), b.read(), committed)


class ImagesTest(unittest.TestCase):
    def test_bmp_round_trip(self):
        pixels = np.array([[(i, j, i ^ j) for i in range(5)] for j in range(3)], dtype=np.uint8)  # odd width pads rows
        with tempfile.TemporaryDirectory() as out:
            path = os.path.join(out, 'x.bmp')
            images.write_bmp(path, pixels)
            np.testing.assert_array_equal(images.read(path), pixels)

    def test_rgb565_truncates_each_channel(self):
        pixels = np.array([[(0xFF, 0x00, 0x00), (0x07, 0xFF, 0x07), (0x08, 0x04, 0xF8)]], dtype=np.uint8)
        self.assertEqual(images.rgb565(pixels), bytes([0x00, 0xF8, 0xE0, 0x07, 0x3F, 0x08]))


if __name__ == '__main__':
    unittest.main()
