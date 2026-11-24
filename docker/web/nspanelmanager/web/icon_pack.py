"""
Custom icons for entities, scenes and thermostat modes.

The user picks any Material Design Icon in the manager. It is registered as an IconPackIcon, which
gives it a code point in Unicode's private use area, and that character is stored where the icon
is used: entity_data["icon"] for entities, Scene.icon for scenes, and the "icon" of each thermostat
mode, in place of the built-in characters ("s", "!", ...) the panels' own icon fonts have. Lights
and switches show their state with their icon, so they have two: entity_data["icon"] when on and
entity_data["icon_off"] when off, in place of the toggle switch on and off icons.

The panels download all custom icons in use as an icon pack from
/download_icon_pack/<panel_id>?sizes=48,28,30, rendered here from the MDI font at each size asked
for. The panel turns each size into a font that falls back to its built-in icon font. The pack
format is described in the firmware's IconPackManager.hpp; render_pack() writes it.
"""

import hashlib
import json
import os
import re
import struct
from functools import lru_cache

from django.http import HttpResponse, JsonResponse

from .models import Entity, IconPackIcon, Scene

PRIVATE_USE_FIRST = 0xE000
PRIVATE_USE_LAST = 0xF8FF
DEFAULT_SIZES = (48, 28, 30)  # What the current firmware asks for
MIN_SIZE, MAX_SIZE, MAX_SIZES = 8, 96, 6

# The Material Design Icons webfont the web interface uses, so the icon picker and the panels show
# the same icons.
_MDI_DIR = os.path.join(os.path.dirname(__file__), "static", "mdi")
_FONT_PATH = os.path.join(_MDI_DIR, "fonts", "materialdesignicons-webfont.ttf")
_CSS_PATH = os.path.join(_MDI_DIR, "css", "materialdesignicons.min.css")
THERMOSTAT_MODE_LISTS = ("hvac_modes", "fan_modes", "preset_modes", "swing_modes", "swingh_modes")


@lru_cache(maxsize=1)
def mdi_code_points() -> dict:
    """MDI icon name (e.g. "lamp") to its code point in the MDI font, read from the webfont's CSS."""
    with open(_CSS_PATH) as file:
        css = file.read()
    return {name: int(code, 16) for name, code in re.findall(r'\.mdi-([a-z0-9-]+)::?before\{content:"\\([0-9A-Fa-f]+)"\}', css)}


def is_custom_icon(icon) -> bool:
    return isinstance(icon, str) and len(icon) == 1 and PRIVATE_USE_FIRST <= ord(icon) <= PRIVATE_USE_LAST


def register_icon(name: str) -> IconPackIcon:
    """The IconPackIcon for an MDI icon, given the lowest free code point if it has none yet."""
    if name not in mdi_code_points():
        raise ValueError(f"Unknown Material Design Icon: {name!r}")
    existing = IconPackIcon.objects.filter(name=name).first()
    if existing is not None:
        return existing
    used = set(IconPackIcon.objects.values_list("code_point", flat=True))
    for code_point in range(PRIVATE_USE_FIRST, PRIVATE_USE_LAST + 1):
        if code_point not in used:
            return IconPackIcon.objects.create(name=name, code_point=code_point)
    raise ValueError("No free code points left for custom icons.")


def clean_icon(icon, allow_builtin=False) -> str:
    """
    An icon as stored: a registered custom icon character, or "" (the default icon). With
    allow_builtin, also a single ASCII character, as thermostat modes use for the built-in icons.
    """
    if is_custom_icon(icon) and IconPackIcon.objects.filter(code_point=ord(icon)).exists():
        return icon
    if allow_builtin and isinstance(icon, str) and len(icon) == 1 and ord(icon) < 128:
        return icon
    return ""


def custom_icons_in_use() -> list:
    """(code_point, name) of every registered custom icon used by an entity, scene or thermostat mode."""
    used = set()
    for entity in Entity.objects.all():
        data = entity.entity_data or {}
        icons = [data.get("icon"), data.get("icon_off")]  # icon_off: lights and switches, see rest.py
        for mode_list in THERMOSTAT_MODE_LISTS:
            icons += [mode.get("icon") for mode in data.get(mode_list, []) if isinstance(mode, dict)]
        used.update(ord(icon) for icon in icons if is_custom_icon(icon))
    used.update(ord(icon) for icon in Scene.objects.values_list("icon", flat=True) if is_custom_icon(icon))
    registered = IconPackIcon.objects.filter(code_point__in=used)
    return sorted((icon.code_point, icon.name) for icon in registered)


def _render_glyph(font, character: str, size: int):
    """The glyph as (bitmap, adv_w, box_w, box_h, ofs_x, ofs_y), 4 bits per pixel, like the firmware's fonts."""
    from PIL import Image, ImageDraw  # Only needed when rendering a pack

    origin_x, baseline_y = size, 2 * size
    canvas = Image.new("L", (3 * size, 3 * size), 0)
    ImageDraw.Draw(canvas).text((origin_x, baseline_y), character, font=font, fill=255, anchor="ls")
    ink = canvas.getbbox()
    if not ink:
        return b"", round(font.getlength(character) * 16), 0, 0, 0, 0
    x0, y0, x1, y1 = ink
    box_w, box_h = x1 - x0, y1 - y0
    img = canvas.crop(ink)
    pixels = [img.getpixel((x, y)) >> 4 for y in range(box_h) for x in range(box_w)]
    if len(pixels) % 2:
        pixels.append(0)
    bitmap = bytes((pixels[i] << 4) | pixels[i + 1] for i in range(0, len(pixels), 2))
    return bitmap, round(font.getlength(character) * 16), box_w, box_h, x0 - origin_x, -(y1 - baseline_y)


@lru_cache(maxsize=8)
def render_pack(icons: tuple, sizes: tuple) -> bytes:
    """
    The icon pack for these icons, ((code point on the panel, MDI name), ...) ascending by code
    point, at these sizes in pixels. See the firmware's IconPackManager.hpp for the format.
    """
    from PIL import ImageFont

    names = mdi_code_points()
    out = bytearray(b"NSIP" + struct.pack("<BBHI", 1, len(sizes), len(icons), 0))
    for size in sizes:
        font = ImageFont.truetype(_FONT_PATH, size)
        ascent, descent = font.getmetrics()
        out += struct.pack("<HHhH", size, ascent + descent, descent, 0)
        glyph_table = bytearray()
        bitmaps = bytearray()
        for code_point, name in icons:
            bitmap, adv_w, box_w, box_h, ofs_x, ofs_y = _render_glyph(font, chr(names[name]), size)
            glyph_table += struct.pack("<IIHBBbbH", code_point, len(bitmaps), min(adv_w, 4095), box_w, box_h, ofs_x, ofs_y, 0)
            bitmaps += bitmap
        out += glyph_table + struct.pack("<I", len(bitmaps)) + bitmaps
    return bytes(out)


def _parse_sizes(value) -> tuple:
    if not value:
        return DEFAULT_SIZES
    sizes = tuple(int(size) for size in value.split(","))
    if not 1 <= len(sizes) <= MAX_SIZES or any(not MIN_SIZE <= size <= MAX_SIZE for size in sizes):
        raise ValueError("Bad sizes")
    return sizes


def download_icon_pack(request, panel_id):
    try:
        sizes = _parse_sizes(request.GET.get("sizes"))
    except ValueError:
        return HttpResponse(f"sizes must be 1 to {MAX_SIZES} sizes from {MIN_SIZE} to {MAX_SIZE}, e.g. 48,28", status=400)

    # Every panel gets the same pack: all custom icons in use. Unknown names (the MDI data changed) are left out.
    icons = tuple((code_point, name) for code_point, name in custom_icons_in_use() if name in mdi_code_points())
    if not icons:
        return HttpResponse(status=404)

    data = render_pack(icons, sizes)
    etag = '"' + hashlib.sha1(data).hexdigest() + '"'
    if request.headers.get("If-None-Match") == etag:
        response = HttpResponse(status=304)
    else:
        response = HttpResponse(data, content_type="application/octet-stream")
    response["ETag"] = etag
    return response


def available_icons(request):
    """GET: the names of all icons that can be chosen, for the icon picker."""
    return JsonResponse({"names": sorted(mdi_code_points())})


def icons(request):
    """
    GET: the registered custom icons. POST {"name": "lamp"}: register an MDI icon (idempotent) and
    return it, with the character to store where the icon is used.
    """

    def representation(icon):
        return {"name": icon.name, "code_point": icon.code_point, "character": chr(icon.code_point)}

    if request.method == "GET":
        return JsonResponse({"icons": [representation(icon) for icon in IconPackIcon.objects.order_by("code_point")]})
    if request.method == "POST":
        try:
            name = json.loads(request.body).get("name", "")
            return JsonResponse(representation(register_icon(name)))
        except (ValueError, AttributeError) as ex:
            return JsonResponse({"status": "error", "message": str(ex)}, status=400)
    return JsonResponse({"status": "error", "error": "Unsupported method"}, status=405)
