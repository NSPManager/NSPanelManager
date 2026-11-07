"""nextion_tools.hmi and hmi_tool.py against the project's own HMIs: the ids, fonts and code the
firmware relies on. The reader itself is tested in nextion-tools."""
import contextlib
import io
import json
import os
import struct
import tempfile
import unittest

from PIL import Image

import hmi_tool
from nextion_tools import hmi, tft
from tft_build import region

PROJECTS = {name: region.hmi(region.path(name)) for name in ('us', 'eu')}


def load(region):
    if region not in load.cache:
        load.cache[region] = hmi.Project.load(PROJECTS[region])
    return load.cache[region]


load.cache = {}


class ParseTest(unittest.TestCase):
    def test_every_component_has_a_name_and_consistent_ends(self):
        for region in PROJECTS:
            project = load(region)
            self.assertGreater(len(project.pages), 10, region)
            for page in project.pages:
                for comp in page.components:
                    self.assertTrue(comp.name, f'{region} {page.name}')
                    for end, (start, size) in hmi.END_ATTRS.items():
                        if end in comp.attrs:
                            self.assertEqual(comp.get(end),
                                             comp.get(start) + comp.get(size) - 1,
                                             f'{region} {page.name}.{comp.name}.{end}')

    def test_event_code_is_not_read_as_attributes(self):
        # Code lines of 16 characters or more look like attribute records.
        for region in PROJECTS:
            for page in load(region).pages:
                for comp in page.components:
                    for attr in comp.attrs:
                        self.assertRegex(attr, r'^[a-z_][a-z0-9_]*$', f'{region} {page.name}.{comp.name}')

    def test_stamp_constant_depends_only_on_length(self):
        # The stamp is core ^ K(length); if the core were wrong, K would vary between
        # same-length pages across the two projects.
        constants = {}
        pages_per_length = {}
        for region in PROJECTS:
            for page in load(region).pages:
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
        for page in load('us').pages:
            self.assertEqual(hmi.restamp(page.block, page.block), page.block)


class IdTest(unittest.TestCase):
    def test_page_ids_are_main_hmi_order(self):
        for region in PROJECTS:
            project = load(region)
            ids = sorted(p.id for p in project.pages)
            self.assertEqual(ids, list(range(len(project.pages))), region)
            for page in project.pages:
                self.assertIs(project.page_by_id(page.id), page)
            with self.assertRaises(KeyError):
                project.page_by_id(len(ids))

    def test_page_ids_the_firmware_relies_on(self):
        # firmware lib/Pages/GUI_data.hpp: GUI_THERMOSTAT_CONTROL_PAGE::page_id = 11. Its
        # GUI_LIGHT_CONTROL_PAGE::page_id = 5 is unused there, and the HMIs say 9.
        for region in PROJECTS:
            project = load(region)
            self.assertEqual(project.page('thermostat').id, 11, region)
            self.assertEqual(project.page('lights').id, 9, region)
            self.assertEqual(project.page('home').id, 2, region)

    def test_component_ids_are_their_positions_and_the_page_comes_first(self):
        for region in PROJECTS:
            for page in load(region).pages:
                first = page.components[0]
                self.assertEqual(first.get('type'), hmi.PAGE_TYPE, f'{region} {page.name}')
                self.assertEqual(first.name, page.name)
                for i, comp in enumerate(page.components):
                    self.assertEqual(comp.get('id'), i, f'{region} {page.name}.{comp.name}')
                    self.assertIn(comp.get('type'), hmi.TYPES)

    def test_component_ids_the_firmware_relies_on(self):
        # firmware lib/Pages/GUI_data.hpp, and the items12 IDs upstream's firmware got wrong.
        expected = {('home', 'h_ceiling'): 22, ('home', 'h_table'): 21, ('home', 'b_scenes'): 17,
                    ('items12', 'slider_save'): 25, ('items12', 'home'): 26,
                    ('items12', 'prev'): 27, ('items12', 'next'): 29}
        for region in PROJECTS:
            project = load(region)
            for (page_name, obj), number in expected.items():
                page = project.page(page_name)
                self.assertEqual(page.component(obj).get('id'), number, f'{region} {page_name}.{obj}')

    def test_sendkey_matches_the_edges_the_firmware_handles(self):
        page = load('us').page('home')
        self.assertEqual(page.component('b_scenes').get('sendkey'), hmi.SENDKEY_RELEASE)
        self.assertEqual(page.component('h_ceiling').get('sendkey'), hmi.SENDKEY_RELEASE | hmi.SENDKEY_PRESS)


class EventTest(unittest.TestCase):
    EVENTS = {'down', 'up', 'slide', 'timer', 'load', 'loadend', 'unload'}

    def test_event_names_and_where_they_occur(self):
        for region in PROJECTS:
            for page in load(region).pages:
                for comp in page.components:
                    kind = hmi.TYPES[comp.get('type')]
                    where = f'{region} {page.name}.{comp.name}'
                    self.assertLessEqual(set(comp.events), self.EVENTS, where)
                    if {'load', 'loadend', 'unload'} & set(comp.events):
                        self.assertEqual(kind, 'page', where)
                    if 'timer' in comp.events:
                        self.assertEqual(kind, 'timer', where)
                    if 'slide' in comp.events:
                        self.assertEqual(kind, 'slider', where)

    def test_known_code(self):
        for region in PROJECTS:
            page = load(region).page('home')
            self.assertEqual(page.events['load'], ['vis p_lockceiling,0', 'vis p_locktable,0'], region)
            self.assertEqual(page.component('sleep_display').events['timer'], ['printh 92 FF FF FF'], region)
            self.assertEqual(page.component('tc0').events['down'], ['va1.val=tch1', 'tm0.en=1'], region)
            self.assertIn('sleep_display.en=1 //screensaver timer', page.component('tc0').events['up'])

    def test_code_lines_are_text_not_records(self):
        total = 0
        for region in PROJECTS:
            for page in load(region).pages:
                for comp in page.components:
                    for code in comp.events.values():
                        for line in code:
                            total += 1
                            self.assertNotIn('\0', line)
                            self.assertNotRegex(line, r'^codes[a-z]+-\d+$')
        self.assertGreater(total, 1000)

    def test_text_attributes_are_utf8(self):
        for region in PROJECTS:
            page = load(region).page('thermostat')
            self.assertIn('°', page.component('roomTemp').get('txt'))

    def test_values_decodes_every_attribute(self):
        page = load('us').page('home')
        values = page.component('b_ceiling').attrs
        self.assertEqual(values['objname'], 'b_ceiling')
        self.assertEqual((values['type'], values['id']), (53, 3))
        self.assertTrue(all(isinstance(v, (int, str)) for v in values.values()))

    def test_an_edit_keeps_the_code(self):
        project = load('us')
        before = {c.name: c.events for c in project.page('home').components}
        data, _ = hmi.apply_edits(project, [('home', 'b_ceiling', 'x', 1)])
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'edited.HMI')
            with open(path, 'wb') as f:
                f.write(data)
            page = hmi.Project.load(path).page('home')
        self.assertEqual({c.name: c.events for c in page.components}, before)


class ToolTest(unittest.TestCase):
    def run_tool(self, *args):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            hmi_tool.main(list(args))
        return out.getvalue()

    def test_pages_lists_ids_in_order(self):
        lines = self.run_tool('pages', PROJECTS['us']).splitlines()
        self.assertRegex(lines[0], r'^\s*0\s+page0\s')
        self.assertRegex(lines[2], r'^\s*2\s+home\s')

    def test_events_prints_code_by_component(self):
        out = self.run_tool('events', PROJECTS['us'], 'home', 'sleep_display')
        self.assertEqual(out, 'home.sleep_display timer:\n    printh 92 FF FF FF\n')
        self.assertIn('home.tc0 down:', self.run_tool('events', PROJECTS['us'], 'home'))

    def test_fonts_extracts_zi_glyphs_and_metrics(self):
        with tempfile.TemporaryDirectory() as tmp:
            out = self.run_tool('fonts', PROJECTS['us'], '--out', tmp)
            self.assertIn('newfont9', out)
            with open(os.path.join(tmp, 'fonts.json')) as f:
                summary = json.load(f)
            self.assertEqual([s['index'] for s in summary], list(range(18)))
            folder = os.path.join(tmp, '09_newfont9')
            with open(os.path.join(folder, 'newfont9.zi'), 'rb') as f:
                self.assertEqual(f.read(), load('us').font_blocks()[9][1])
            with open(os.path.join(folder, 'glyphs.json')) as f:
                glyphs = json.load(f)
            self.assertEqual(len(glyphs), len(load('us').fonts()[9].glyphs))
            with Image.open(os.path.join(folder, glyphs[0]['file'])) as im:
                self.assertEqual(im.mode, 'LA')
                self.assertEqual(im.height, 50)

    def test_dump_is_json_with_ids_attributes_and_code(self):
        data = json.loads(self.run_tool('dump', PROJECTS['eu']))
        self.assertEqual([p['id'] for p in data['pages']], list(range(len(data['pages']))))
        home = next(p for p in data['pages'] if p['name'] == 'home')
        timer = next(c for c in home['components'] if c['attrs']['objname'] == 'sleep_display')
        self.assertEqual(timer['events'], {'timer': ['printh 92 FF FF FF']})
        self.assertEqual(len(data['fonts']), 18)


class EditTest(unittest.TestCase):
    def setUp(self):
        self.project = load('us')
        self.page = self.project.page('items12')
        self.comp = self.page.component('slider_save')
        self.x = self.comp.get('x')

    def test_edit_changes_only_the_attribute_its_end_and_the_stamp(self):
        data, changes = hmi.apply_edits(self.project, [('items12', 'slider_save', 'x', self.x + 3)])
        self.assertEqual(changes, [('items12', 'slider_save', 'x', self.x, self.x + 3),
                                   ('items12', 'slider_save', 'endx',
                                    self.comp.get('endx'),
                                    self.comp.get('endx') + 3)])
        e = self.page.entry
        changed = {i - e.offset for i in range(len(data)) if data[i] != self.project.data[i]}
        allowed = set(range(4))
        for attr in ('x', 'endx'):
            o, n = self.comp.fields[attr]
            allowed |= set(range(o, o + n))
        self.assertTrue(changed)
        self.assertLessEqual(changed, allowed)

    def test_edit_and_back_restores_the_file(self):
        once, _ = hmi.apply_edits(self.project, [('items12', 'slider_save', 'x', self.x + 3)])
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'once.HMI')
            with open(path, 'wb') as f:
                f.write(once)
            twice, _ = hmi.apply_edits(hmi.Project.load(path), [('items12', 'slider_save', 'x', self.x)])
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
            page = hmi.Project.load(out).page('items12')
            self.assertEqual(page.component('slider_save').get('x'), 1)


if __name__ == '__main__':
    unittest.main()
