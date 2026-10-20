"""Build every theme's TFT from one TFT the Editor compiled, without the Editor.

The themes differ only in pictures, so each theme's TFT is the template with that theme's
pictures from all_numbered/ swapped in. Each picture keeps its slot: a picture the template
stores raw is replaced by the theme's picture in RGB565; a picture the Editor compressed must be
the same in every theme, since its codec is not known.

A region's display orientation is set in the HMI's device settings, so the Editor compiles it
into the template; ORIENTATIONS holds what each region's TFTs must have. Layouts that are another
region's TFTs at another orientation are not compiled at all: ROTATED makes them from that
region's TFTs, changing only the orientation byte.
"""
from . import images, region, tft

ORIENTATIONS = {'eu': 270, 'us': 180}
# The manager's "US Horizontal (Mirrored)": the EU layout turned the other way. (For "US
# Horizontal" it serves the EU TFTs as they are.)
ROTATED = {'us_horizontal_mirrored': ('eu', 90)}


def rotated(source):
    """[(layout, degrees)] built from the named region's TFTs."""
    return [(layout, degrees) for layout, (src, degrees) in ROTATED.items() if src == source]


def build(template, region_dir, themes=region.THEMES):
    """{theme: TFT bytes}. Raises ValueError if a picture cannot be swapped into its slot."""
    slots = tft.pictures(template)
    sources = {theme: region.numbered(region_dir, theme) for theme in region.THEMES}
    first = sources[region.THEMES[0]]
    if len(first) != len(slots):
        raise ValueError(f'the template has {len(slots)} pictures but '
                         f'{region.numbered_dir(region_dir, region.THEMES[0])} has {len(first)}; '
                         'compile a new template from the HMI')
    swaps = {theme: {} for theme in themes}
    for slot in slots:
        paths = {theme: sources[theme].get(slot.index) for theme in region.THEMES}
        missing = [theme for theme, path in paths.items() if path is None]
        if missing:
            raise ValueError(f'picture {slot.index} is missing from theme {missing[0]}')
        if first[slot.index].endswith('.png'):
            # Static pictures, which the Editor converts itself.
            contents = set()
            for path in paths.values():
                with open(path, 'rb') as f:
                    contents.add(f.read())
            if len(contents) > 1:
                raise ValueError(f'picture {slot.index} differs between themes but is a PNG')
            continue
        pixels = {theme: images.read(path) for theme, path in paths.items()}
        for theme, p in pixels.items():
            if p.shape[:2] != (slot.h, slot.w):
                raise ValueError(f'picture {slot.index} is {p.shape[1]}x{p.shape[0]} in theme {theme} but '
                                 f'{slot.w}x{slot.h} in the template; compile a new template')
        data = {theme: images.rgb565(p) for theme, p in pixels.items()}
        if slot.raw:
            for theme in themes:
                swaps[theme][slot] = tft.raw_record(data[theme])
        elif len(set(data.values())) > 1:
            raise ValueError(f'picture {slot.index} differs between themes, but the Editor compressed it '
                             'in the template, so it cannot be swapped')
    return {theme: tft.replace_pictures(template, swaps[theme]) for theme in themes}
