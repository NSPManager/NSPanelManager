"""
Background image for the panels' LVGL interface.

The panel downloads it from /download_background/<panel_id> at start and
whenever its config is reloaded. The image is sent in the panel's own pixel
format, so the panel needs no image decoder: raw RGB565, little endian, at the
size of the panel's display, row by row: 454x320, or 320x480 for a US panel
mounted vertically. The ETag lets the panel skip the download when its stored
image is current (it sends the ETag back in If-None-Match). 404 means no
background, so the panel removes its stored image.

For now every panel gets the same generated blue gradient. User uploaded
images would be converted to this format when uploaded.
"""

import hashlib
from functools import lru_cache

from django.http import HttpResponse

from web.settings_helper import get_nspanel_setting_with_default

LANDSCAPE_SIZE = (454, 320)  # EU panels, and US panels mounted horizontally
PORTRAIT_SIZE = (320, 480)  # US panels mounted vertically

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


def display_size(panel_id) -> tuple[int, int]:
    # Matches the display orientation the MQTTManager sends the panel in its config.
    is_us_panel = get_nspanel_setting_with_default(panel_id, "is_us_panel", "False") == "True"
    orientation = get_nspanel_setting_with_default(panel_id, "us_panel_orientation", "vertical")
    if is_us_panel and orientation not in ("horizontal", "horizontal_mirrored"):
        return PORTRAIT_SIZE
    return LANDSCAPE_SIZE


@lru_cache(maxsize=2)
def blue_gradient_rgb565(width: int, height: int) -> bytes:
    rows = []
    for y in range(height):
        t = y / (height - 1)
        r, g, b = (top + (bottom - top) * t for top, bottom in zip(GRADIENT_TOP, GRADIENT_BOTTOM))
        pixel = _to_rgb565(r, g, b, (_ROW_DITHER[y % 4] + 0.5) / 4)
        rows.append(pixel.to_bytes(2, "little") * width)
    return b"".join(rows)


@lru_cache(maxsize=2)
def _etag(data: bytes) -> str:
    return '"' + hashlib.sha1(data).hexdigest() + '"'


def download_background(request, panel_id):
    data = blue_gradient_rgb565(*display_size(panel_id))
    etag = _etag(data)
    if request.headers.get("If-None-Match") == etag:
        response = HttpResponse(status=304)
    else:
        response = HttpResponse(data, content_type="application/octet-stream")
    response["ETag"] = etag
    return response
