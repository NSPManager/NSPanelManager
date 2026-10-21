"""The CRC behind TFT checksums and HMI page stamps.

Both are CRC-32/MPEG-2 (poly 0x04C11DB7, MSB first, init 0xFFFFFFFF, no final XOR) over the
data with each byte widened to a big-endian 32-bit word, which is how an STM32's CRC unit
checksums a byte stream. See https://github.com/UNUF/TFTTool for the TFT side.
"""
import zlib

_REVERSED = bytes(int(f'{i:08b}'[::-1], 2) for i in range(256))


def crc32_mpeg2(data):
    """CRC-32/MPEG-2. zlib computes the bit-reflected CRC-32 with the same polynomial, so this is
    zlib's CRC of the bit-reversed input, bit-reversed, without zlib's final XOR."""
    return int(f'{zlib.crc32(data.translate(_REVERSED)) ^ 0xFFFFFFFF:032b}'[::-1], 2)


def widen(data):
    """Each byte as a big-endian 32-bit word."""
    words = bytearray(4 * len(data))
    words[3::4] = data
    return bytes(words)


def nextion(data):
    return crc32_mpeg2(widen(data))
