"""The project's own fonts, read with nextion_tools.fonts (whose decoder nextion-tools tests)."""
import string
import unittest

from tests.test_hmi import PROJECTS, load


# Text fields whose text the font they name cannot draw, because the text is a placeholder the
# firmware overwrites. Anything new here is worth a look: a font index may be stale.
UNDRAWABLE = {('eu', 'dropdown', 'i4_button')}


class ProjectFontsTest(unittest.TestCase):
    def test_every_project_has_seventeen_fonts_in_main_hmi_order(self):
        for region in PROJECTS:
            project = load(region)
            names = [f.name for f in project.fonts()]
            self.assertEqual(len(names), 17, region)
            self.assertEqual(names[:4], ['hamlin18_latA', 'replace_h23bold_id_1', 'timeandtemp', 'replace_h22_id_3'])
            self.assertEqual([m for m, _ in project.font_blocks()],
                             [n for n in project.resources() if n.endswith('.zi')])

    def test_both_regions_carry_the_same_fonts(self):
        us = {f.name: b for (_, b), f in zip(load('us').font_blocks(), load('us').fonts())}
        eu = {f.name: b for (_, b), f in zip(load('eu').font_blocks(), load('eu').fonts())}
        self.assertEqual(us, eu)

    def test_known_icon_fonts_are_where_components_expect_them(self):
        for region in PROJECTS:
            fs = load(region).fonts()
            self.assertEqual((fs[9].name, fs[9].height), ('newfont9', 50))
            self.assertEqual(fs[9].missing(string.ascii_lowercase[:18]), '')  # a..r: GUI_ICONS
            self.assertEqual((fs[11].name, fs[15].name, fs[16].name), ('stuvwxyz40', 'arrows40', 'thermostat30NEW'))

    def test_no_glyph_codes_more_than_its_box(self):
        # The overhang columns are what make this hold: without them, five glyphs overrun.
        for region in PROJECTS:
            short = 0
            for font in load(region).fonts():
                for g in font.glyphs.values():
                    self.assertEqual(g.coverage.shape, (font.height, g.left + g.advance + g.right))
                    self.assertLessEqual(g.coded, g.coverage.size, f'{font.name} {g.char!r}')
                    short += g.coded < g.coverage.size
            self.assertEqual(short, 13, region)

    def test_right_overhang_holds_the_caron_of_d(self):
        g = load('us').fonts()[0].glyph('ď')  # d with caron
        self.assertEqual((g.left, g.advance, g.right), (0, 13, 1))
        self.assertTrue(g.coverage[:, -1].any(), 'the overhang column should hold ink')

    def test_text_font_draws_printable_ascii(self):
        font = load('us').fonts()[0]
        self.assertEqual(font.missing(string.printable.strip()), '')
        self.assertFalse(font.glyph(' ').coverage.any())
        self.assertTrue(font.glyph('A').coverage.any())

    def test_component_text_is_drawable_in_its_font(self):
        for region in PROJECTS:
            project = load(region)
            fs = project.fonts()
            for page in project.pages:
                for comp in page.components:
                    if 'font' not in comp.attrs or 'txt' not in comp.attrs:
                        continue
                    index = comp.get('font')
                    self.assertLess(index, len(fs))
                    text = comp.get('txt')
                    key = (region, page.name, comp.name)
                    if key in UNDRAWABLE:
                        self.assertTrue(fs[index].missing(text), f'{key} is drawable now; drop it from UNDRAWABLE')
                    else:
                        self.assertEqual(fs[index].missing(text), '', f'{key} font {index} {fs[index].name}')


if __name__ == '__main__':
    unittest.main()
