"""Tests for nextion.hmi and hmi_tool.py against the project's own HMIs."""
import contextlib
import io
import os
import struct
import tempfile
import unittest

import hmi_tool
from nextion import hmi, region, tft

PROJECTS = {name: region.hmi(region.path(name)) for name in ('us', 'eu')}


def load(region):
    if region not in load.cache:
        load.cache[region] = hmi.Project(PROJECTS[region])
    return load.cache[region]


load.cache = {}


class ParseTest(unittest.TestCase):
    def test_every_component_has_a_name_and_consistent_ends(self):
        for region in PROJECTS:
            project = load(region)
            self.assertGreater(len(project.pages), 10, region)
            for page in project.pages.values():
                for comp in page.components:
                    self.assertTrue(comp.name(page.block), f'{region} {page.name}')
                    for end, (start, size) in hmi.END_ATTRS.items():
                        if end in comp.attrs:
                            self.assertEqual(comp.get(page.block, end),
                                             comp.get(page.block, start) + comp.get(page.block, size) - 1,
                                             f'{region} {page.name}.{comp.name(page.block)}.{end}')

    def test_event_code_is_not_read_as_attributes(self):
        # Code lines of 16 characters or more look like attribute records.
        for region in PROJECTS:
            for page in load(region).pages.values():
                for comp in page.components:
                    for attr in comp.attrs:
                        self.assertRegex(attr, r'^[a-z_][a-z0-9_]*$', f'{region} {page.name}.{comp.name(page.block)}')

    def test_stamp_constant_depends_only_on_length(self):
        # The stamp is core ^ K(length); if the core were wrong, K would vary between
        # same-length pages across the two projects.
        constants = {}
        pages_per_length = {}
        for region in PROJECTS:
            for page in load(region).pages.values():
                k = struct.unpack_from('<I', page.block, 0)[0] ^ hmi._stamp_core(page.block)
                constants.setdefault(len(page.block), set()).add(k)
                pages_per_length.setdefault(len(page.block), set()).add(page.block)
        self.assertGreater(max(len(blocks) for blocks in pages_per_length.values()), 1,
                           'need two different pages of one length for this to test anything')
        for length, ks in constants.items():
            self.assertEqual(len(ks), 1, f'length {length}')

    def test_pictures_are_the_records_the_editor_compiled(self):
        # The HMI holds one theme's pictures, so one theme's TFT holds the same records.
        for name in PROJECTS:
            compiled = []
            for theme in region.THEMES:
                with open(region.tft(region.path(name), theme), 'rb') as f:
                    compiled.append(tft.records(f.read()))
            self.assertIn(load(name).pictures(), compiled, name)

    def test_restamping_an_unchanged_page_is_a_no_op(self):
        for page in load('us').pages.values():
            self.assertEqual(hmi.restamp(page.block, page.block), page.block)


class EditTest(unittest.TestCase):
    def setUp(self):
        self.project = load('us')
        self.page = self.project.page('items12')
        self.comp = self.page.component('slider_save')
        self.x = self.comp.get(self.page.block, 'x')

    def test_edit_changes_only_the_attribute_its_end_and_the_stamp(self):
        data, changes = hmi.apply_edits(self.project, [('items12', 'slider_save', 'x', self.x + 3)])
        self.assertEqual(changes, [('items12', 'slider_save', 'x', self.x, self.x + 3),
                                   ('items12', 'slider_save', 'endx',
                                    self.comp.get(self.page.block, 'endx'),
                                    self.comp.get(self.page.block, 'endx') + 3)])
        e = self.page.entry
        changed = {i - e.offset for i in range(len(data)) if data[i] != self.project.data[i]}
        allowed = set(range(4))
        for attr in ('x', 'endx'):
            o, n = self.comp.attrs[attr]
            allowed |= set(range(o, o + n))
        self.assertTrue(changed)
        self.assertLessEqual(changed, allowed)

    def test_edit_and_back_restores_the_file(self):
        once, _ = hmi.apply_edits(self.project, [('items12', 'slider_save', 'x', self.x + 3)])
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'once.HMI')
            with open(path, 'wb') as f:
                f.write(once)
            twice, _ = hmi.apply_edits(hmi.Project(path), [('items12', 'slider_save', 'x', self.x)])
        self.assertEqual(twice, self.project.data)

    def test_refuses_strings_derived_attributes_and_values_that_do_not_fit(self):
        for attr, value in (('objname', 1), ('endx', 10), ('x', 1 << 16), ('x', -1), ('nonesuch', 1)):
            with self.assertRaises(ValueError, msg=attr):
                hmi.apply_edits(self.project, [('items12', 'slider_save', attr, value)])
        with self.assertRaises(KeyError):
            hmi.apply_edits(self.project, [('items12', 'nonesuch', 'x', 1)])
        with self.assertRaises(KeyError):
            hmi.apply_edits(self.project, [('nonesuch', 'slider_save', 'x', 1)])

    def test_set_edits_files_with_stale_blocks(self):
        self.assertTrue(self.project.stale, 'expected the committed HMI to hold stale blocks')
        with tempfile.TemporaryDirectory() as tmp:
            out = os.path.join(tmp, 'out.HMI')
            with contextlib.redirect_stdout(io.StringIO()):
                hmi_tool.main(['set', PROJECTS['us'], out, 'items12.slider_save.x=1'])
            page = hmi.Project(out).page('items12')
            self.assertEqual(page.component('slider_save').get(page.block, 'x'), 1)


if __name__ == '__main__':
    unittest.main()
