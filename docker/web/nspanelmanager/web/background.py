"""
Background image for the panels' LVGL interface.

The panel downloads it from /download_background/<panel_id> at start and
whenever its config is reloaded. The image is sent in the panel's own pixel
format, so the panel needs no image decoder: raw RGB565, little endian,
454x320, row by row. The ETag lets the panel skip the download when its stored
image is current (it sends the ETag back in If-None-Match). 404 means no
background, so the panel removes its stored image.

For now every panel gets the same generated blue gradient. User uploaded
images would be converted to this format when uploaded.
"""

import hashlib
from functools import lru_cache

from django.http import HttpResponse

WIDTH = 454
HEIGHT = 320

# Gradient from the top to the bottom of the screen. Every row is one color: the panel
# compresses each row it sends to the display, and a row of one color costs almost nothing,
# which keeps full screen redraws fast.
GRADIENT_TOP = (8, 22, 52)
GRADIENT_BOTTOM = (34, 104, 196)

# Ordered dither between rows, so the gradient does not show bands in RGB565's 5 and 6 bit
# channels. Dithering within a row would break up the rows the panel compresses.
_ROW_DITHER = (0, 2, 1, 3)


def _to_rgb565(r: float, g: float, b: float, threshold: float) -> int:
    # threshold is 0..1. Adding it before truncating to the channel's step spreads
    # the rounding error over neighbouring pixels.
    r5 = min(31, int(r / 8 + threshold))
    g6 = min(63, int(g / 4 + threshold))
    b5 = min(31, int(b / 8 + threshold))
    return (r5 << 11) | (g6 << 5) | b5


@lru_cache(maxsize=1)
def blue_gradient_rgb565() -> bytes:
    rows = []
    for y in range(HEIGHT):
        t = y / (HEIGHT - 1)
        r, g, b = (top + (bottom - top) * t for top, bottom in zip(GRADIENT_TOP, GRADIENT_BOTTOM))
        pixel = _to_rgb565(r, g, b, (_ROW_DITHER[y % 4] + 0.5) / 4)
        rows.append(pixel.to_bytes(2, "little") * WIDTH)
    return b"".join(rows)


@lru_cache(maxsize=1)
def _etag(data: bytes) -> str:
    return '"' + hashlib.sha1(data).hexdigest() + '"'


def download_background(request, panel_id):
    data = blue_gradient_rgb565()
    etag = _etag(data)
    if request.headers.get("If-None-Match") == etag:
        response = HttpResponse(status=304)
    else:
        response = HttpResponse(data, content_type="application/octet-stream")
    response["ETag"] = etag
    return response
