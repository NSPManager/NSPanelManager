"""A compiled Nextion .tft: its pictures and its checksums.

Format notes, checked against the Editor's own TFTs (see tests/test_tft.py):
- Header 1 byte 0x14 is the display orientation (the Editor's device setting), in 90 degree steps.
- Checksums (nextion.crc) are stored at 0x44 (the resources), 0xC4 (header 1), 0x18C (header 2)
  and the end of the file. Header 2 is encrypted with an unknown key and cannot be rewritten,
  so nothing it describes may move: a picture can be replaced only by one of the same size.
- Pictures are resource 7 in the table at 0x10000: 24-byte entries (marker, index, offset, w, h,
  size, 0), then records of a 20-byte header plus data. A raw record's header is all zeros and
  its data is RGB565 little-endian; the Editor compresses some pictures, with a codec not known.
"""
import struct

from . import crc

RESOURCES = 0x10000
PICTURES = 7
ENTRY = struct.Struct('<IIIHHII')
RECORD_HEADER = 20
ORIENTATION = 0x14


def reseal(tft):
    """Return tft with its resource, header 1 and file checksums recomputed. Header 2 is left as
    it is: its checksum covers only header 2, which picture swaps never change."""
    d = bytearray(tft)
    res_addr, res_size = struct.unpack_from('<I', d, 0x34)[0], struct.unpack_from('<I', d, 0x40)[0]
    struct.pack_into('<I', d, 0x44, crc.nextion(d[res_addr:res_addr + res_size]))
    struct.pack_into('<I', d, 0xC4, crc.nextion(d[0:0xC4]))
    struct.pack_into('<I', d, len(d) - 4, crc.nextion(d[:-4]) ^ d[3] ^ d[0x2E] ^ d[0x3C])
    return bytes(d)


def checksums_ok(tft):
    return reseal(tft) == tft and struct.unpack_from('<I', tft, 0x18C)[0] == crc.nextion(tft[0xC8:0x18C])


def orientation(tft):
    """The display orientation in degrees."""
    return tft[ORIENTATION] * 90


def with_orientation(tft, degrees):
    """tft with its display orientation changed, resealed. Only header 1 records it, so nothing
    else in the file changes."""
    d = bytearray(tft)
    d[ORIENTATION] = degrees // 90
    return reseal(d)


class Picture:
    def __init__(self, index, start, w, h, size):
        self.index, self.start, self.w, self.h, self.size = index, start, w, h, size

    @property
    def raw(self):
        return self.size == RECORD_HEADER + 2 * self.w * self.h


def pictures(tft):
    base = RESOURCES + struct.unpack_from('<I', tft, RESOURCES + 12 * PICTURES)[0]
    count = ENTRY.unpack_from(tft, base)[2] // ENTRY.size
    found = []
    for i in range(count):
        _, index, offset, w, h, size, _ = ENTRY.unpack_from(tft, base + ENTRY.size * i)
        if index != i:
            raise ValueError(f'picture table entry {i} has index {index}')
        found.append(Picture(index, base + offset, w, h, size))
    return found


def records(tft):
    """Each picture's record (header and data) in ID order, as an HMI stores it too."""
    return [tft[p.start:p.start + p.size] for p in pictures(tft)]


def raw_record(rgb565):
    return bytes(RECORD_HEADER) + rgb565


def replace_pictures(tft, new_records):
    """tft with the records in {Picture: record} swapped in, resealed. Each must fill its slot."""
    d = bytearray(tft)
    for slot, record in new_records.items():
        if len(record) != slot.size:
            raise ValueError(f'picture {slot.index}: a {len(record)}-byte record for a {slot.size}-byte slot')
        d[slot.start:slot.start + slot.size] = record
    return reseal(d)
