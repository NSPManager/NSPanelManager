"""Pictures for image progress bars (sta=1), made from the HMI and the page's themed background.

For each bar, reads x, y, w, h, bpic and ppic from the HMI, and for each theme makes:
  bpic  the page background (the page's `pic`) cropped to the bar, from the theme's dithered
        all_numbered/, so the empty part of the bar matches what is around it pixel for pixel.
  ppic  a solid fill in the bar's colour from progress_bars.txt, the same in every theme: the
        Editor compresses flat pictures, and the themes may differ only in pictures it stores raw
        (see nextion/themes.py).
The picture sizes always match the bar, which the Editor requires.
"""
import os

import numpy as np

from . import images, region

def bars(project, page_name, objects):
    """[(object, x, y, w, h, bpic, ppic)] for the named progress bars."""
    page = project.page(page_name)
    found = []
    for obj in objects:
        comp = page.component(obj)
        missing = [a for a in ('x', 'y', 'w', 'h', 'bpic', 'ppic') if a not in comp.attrs]
        if missing:
            raise ValueError(f'{page_name}.{obj} has no {", ".join(missing)}; is it a progress bar?')
        found.append((obj, *(comp.get(page.block, a) for a in ('x', 'y', 'w', 'h', 'bpic', 'ppic'))))
    return found


def pictures(project, region_dir, page_name, listed):
    """{theme: {picture ID: pixels}} for the bars listed as [(object, fill colour)]."""
    page = project.page(page_name)
    background = page.component(page_name).get(page.block, 'pic')
    fills = dict(listed)
    found = bars(project, page_name, [obj for obj, _ in listed])
    made = {}
    for theme in region.THEMES:
        bg = images.read(os.path.join(region.numbered_dir(region_dir, theme), f'{background}.bmp'))
        made[theme] = {}
        for obj, x, y, w, h, bpic, ppic in found:
            if y + h > bg.shape[0] or x + w > bg.shape[1]:
                raise ValueError(f'{page_name}.{obj} ({x},{y} {w}x{h}) runs off background '
                                 f'{background} ({bg.shape[1]}x{bg.shape[0]})')
            made[theme][bpic] = bg[y:y + h, x:x + w]
            made[theme][ppic] = np.full((h, w, 3), fills[obj], dtype=np.uint8)
    return made


def make(project, region_dir, page_name, listed, out=None, numbered_after=None):
    """Write the pictures to out/tftN/, or if out is None to each theme's all_numbered/, where
    they must come after picture numbered_after, the last one GIMP makes; return [(theme, path)]."""
    if out is None and numbered_after is None:
        raise ValueError('writing into all_numbered needs the last GIMP picture number')
    made = pictures(project, region_dir, page_name, listed)
    if out is None and min(made[region.THEMES[0]]) <= numbered_after:
        raise ValueError(f'{page_name}\'s bars use pictures {sorted(made[region.THEMES[0]])}, which would '
                         f'overwrite GIMP\'s pictures (0..{numbered_after})')
    written = []
    for theme, by_id in made.items():
        target = region.numbered_dir(region_dir, theme) if out is None else os.path.join(out, f'tft{theme}')
        os.makedirs(target, exist_ok=True)
        for pic, pixels in by_id.items():
            path = os.path.join(target, f'{pic}.bmp')
            images.write_bmp(path, pixels)
            written.append((theme, path))
    return written
