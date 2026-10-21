"""Pictures as numpy arrays of RGB pixels (height x width x 3), read and written with Pillow."""
import io

import numpy as np
from PIL import Image


def read(source):
    """RGB pixels of a picture file, given its path or its bytes."""
    with Image.open(io.BytesIO(source) if isinstance(source, bytes) else source) as im:
        return np.asarray(im.convert('RGB'))


def write_bmp(path, pixels):
    # 72 dpi keeps the header the generated pictures have always had.
    Image.fromarray(np.asarray(pixels, dtype=np.uint8)).save(path, 'BMP', dpi=(72, 72))


def rgb565(pixels):
    """The panel's RGB565, little-endian, truncating each channel as the Editor does."""
    p = pixels.astype(np.uint16)
    return ((p[..., 0] >> 3) << 11 | (p[..., 1] >> 2) << 5 | p[..., 2] >> 3).astype('<u2').tobytes()
