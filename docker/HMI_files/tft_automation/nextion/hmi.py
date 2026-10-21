"""Read Nextion Editor .HMI projects, and edit numeric component attributes in place.

`apply_edits` only changes numeric attributes (same width, so no block moves), keeps endx/endy
in step with x/y/w/h, and re-stamps each edited page.

The Editor has opened edited files that hold stale blocks from earlier saves, and files whose
edited page has a different save ID from the rest. But it once rejected an edit ("Version
Mismatch", then "Loading Page Failed") that left the page byte-identical to its own save, for a
reason not yet known; a Save As, which compacts the file, made the same edit open.

Format notes: https://github.com/vladtsvetkov880-hub/nextion-hmi-format (docs/); what
this relies on was checked against files saved by the Editor.
"""
import re
import struct

from . import crc

DIRECTORY_ENTRY = 28
PAGE_HEADER = 56
STALE = 1  # bit 0 of a directory entry's last field

# Attributes stored as strings whatever their length; never edited here.
STRING_ATTRS = {'objname', 'txt', 'path', 'from', 'val0', 'val1'}
# Derived from x/y/w/h by the Editor; recomputed rather than set.
END_ATTRS = {'endx': ('x', 'w'), 'endy': ('y', 'h')}

def _stamp_core(block):
    """The Nextion CRC of block[4:] and 10 zero bytes. The stored stamp is this XOR a constant
    that depends only on the block's length, so a same-length edit can take the constant from
    the old stamp."""
    return crc.nextion(block[4:] + bytes(10))


def restamp(old_block, new_block):
    """Return new_block with the stamp the Editor would give it. Lengths must match."""
    assert len(old_block) == len(new_block)
    k = struct.unpack_from('<I', old_block, 0)[0] ^ _stamp_core(old_block)
    out = bytearray(new_block)
    struct.pack_into('<I', out, 0, _stamp_core(out) ^ k)
    return bytes(out)


class Entry:
    def __init__(self, index, name, offset, size, field):
        self.index, self.name, self.offset, self.size, self.field = index, name, offset, size, field

    @property
    def stale(self):
        return bool(self.field & STALE) or not self.name


def directory(data):
    count = struct.unpack_from('<I', data, 0)[0]
    entries = []
    for i in range(count):
        o = 4 + DIRECTORY_ENTRY * i
        name = data[o:o + 16].split(b'\0')[0].decode('latin1')
        offset, size, field = struct.unpack_from('<III', data, o + 16)
        entries.append(Entry(i, name, offset, size, field))
    return entries


class Component:
    def __init__(self, attrs):
        self.attrs = attrs  # name -> (offset in block, length)

    def raw(self, block, name):
        o, n = self.attrs[name]
        return block[o:o + n]

    def get(self, block, name):
        v = self.raw(block, name)
        if name in STRING_ATTRS:
            return v.split(b'\0')[0].decode('latin1')
        return int.from_bytes(v, 'little')

    def name(self, block):
        return self.get(block, 'objname')


class Page:
    def __init__(self, entry, block):
        self.entry = entry
        self.block = block
        if struct.unpack_from('<I', block, 4)[0] != len(block) or struct.unpack_from('<I', block, 8)[0] != PAGE_HEADER:
            raise ValueError(f'{entry.name}: unexpected page header')
        self.name = block[24:40].split(b'\0')[0].decode('latin1')
        count = struct.unpack_from('<I', block, 12)[0]
        self.components = []
        for i in range(count):
            off, length, _ = struct.unpack_from('<III', block, PAGE_HEADER + 12 * i)
            self.components.append(Component(_records(block, PAGE_HEADER + off, length, entry.name)))

    def component(self, objname):
        found = [c for c in self.components if c.name(self.block) == objname]
        if not found:
            raise KeyError(f'page {self.name} has no component {objname}')
        return found[0]


def _records(block, start, length, where):
    """Attribute records of one component: u32 length, then a 16-byte name and the value
    (length >= 16), a marker string (length < 16), or the end of the component (0). An event
    marker such as `codesdown-7` is followed by that many records of code, one per line."""
    attrs = {}
    p = start
    code_lines = 0
    while True:
        n = struct.unpack_from('<I', block, p)[0]
        p += 4
        if n == 0:
            break
        event = re.match(rb'codes[a-z]+-(\d+)', block[p:p + n]) if not code_lines else None
        if code_lines:
            code_lines -= 1
        elif event:
            code_lines = int(event.group(1))
        elif n >= 16:
            attrs[block[p:p + 16].split(b'\0')[0].decode('latin1')] = (p + 16, n - 16)
        p += n
    if p - start != length:
        raise ValueError(f'{where}: component at {start} did not parse to its recorded length')
    return attrs


class Project:
    def __init__(self, path):
        self.path = path
        with open(path, 'rb') as f:
            self.data = f.read()
        self.entries = directory(self.data)
        self.stale = [e for e in self.entries if e.stale]
        self.pages = {}
        for e in self.entries:
            if not e.stale and e.name.endswith('.pa'):
                page = Page(e, self.block(e))
                self.pages[page.name] = page

    def page(self, name):
        if name not in self.pages:
            raise KeyError(f'no page {name} (pages: {", ".join(sorted(self.pages))})')
        return self.pages[name]

    def pictures(self):
        """The picture records in ID order, each exactly as a compiled TFT stores it (record
        header and data, compressed or not). main.HMI lists the project's resources in order;
        pictures are the .i blocks, each a header followed by the record."""
        live = {e.name: e for e in self.entries if not e.stale}
        main = self.block(live['main.HMI'])
        count = struct.unpack_from('<I', main, 28)[0]
        names = [main[96 + 16 * i + 8:96 + 16 * i + 16].split(b'\0')[0].decode('latin1') for i in range(count)]
        records = []
        for name in names:
            if name.endswith('.i'):
                block = self.block(live[name])
                records.append(block[struct.unpack_from('<I', block, 8)[0]:])
        return records

    def block(self, entry):
        return self.data[entry.offset:entry.offset + entry.size]


def apply_edits(project, edits):
    """Return (new file bytes, [(page, obj, attr, old, new)]) for edits of
    (page, object, attribute, value). Raises ValueError if any edit is not allowed."""
    data = bytearray(project.data)
    changes = []
    by_page = {}
    for page_name, obj, attr, value in edits:
        by_page.setdefault(page_name, []).append((obj, attr, value))
    for page_name, page_edits in by_page.items():
        page = project.page(page_name)
        block = bytearray(page.block)
        touched = []
        for obj, attr, value in page_edits:
            comp = page.component(obj)
            if attr not in comp.attrs:
                raise ValueError(f'{page_name}.{obj} has no attribute {attr}')
            if attr in STRING_ATTRS or attr in END_ATTRS:
                raise ValueError(f'{page_name}.{obj}.{attr} cannot be set here')
            o, n = comp.attrs[attr]
            if n not in (1, 2, 4) or not 0 <= value < 1 << (8 * n):
                raise ValueError(f'{page_name}.{obj}.{attr}={value} does not fit its {n}-byte field')
            changes.append((page_name, obj, attr, comp.get(block, attr), value))
            block[o:o + n] = value.to_bytes(n, 'little')
            touched.append(comp)
        for comp in touched:
            for end, (start, size) in END_ATTRS.items():
                if end in comp.attrs and start in comp.attrs and size in comp.attrs:
                    o, n = comp.attrs[end]
                    old = comp.get(block, end)
                    new = comp.get(block, start) + comp.get(block, size) - 1
                    if old != new:
                        block[o:o + n] = new.to_bytes(n, 'little')
                        changes.append((page_name, comp.name(block), end, old, new))
        e = page.entry
        data[e.offset:e.offset + e.size] = restamp(page.block, block)
    return bytes(data), changes


def check_written(before, after_path, edits):
    """Re-read the written file: same size, nothing changed outside the edited pages, and
    every edited value reads back."""
    after = Project(after_path)
    if len(after.data) != len(before.data):
        raise RuntimeError('written file changed size')
    edited = {before.page(p).entry for p, _, _, _ in edits}
    spans = sorted((e.offset, e.offset + e.size) for e in edited)
    pos = 0
    for lo, hi in spans + [(len(before.data), len(before.data))]:
        if before.data[pos:lo] != after.data[pos:lo]:
            raise RuntimeError('bytes outside the edited pages changed')
        pos = hi
    for page_name, obj, attr, value in edits:
        page = after.page(page_name)
        if page.component(obj).get(page.block, attr) != value:
            raise RuntimeError(f'{page_name}.{obj}.{attr} did not read back as {value}')
