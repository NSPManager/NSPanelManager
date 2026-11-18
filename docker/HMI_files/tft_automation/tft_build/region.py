"""Where a region's files live. A region directory (eu, us) holds gui.HMI, progress_bars.txt,
and for each theme N its pictures in pictures_tftN/all_numbered/ and its TFT in
output_tftN/gui.tft."""
import os
import re

THEMES = (1, 2, 3, 4)
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def path(region):
    """A region's directory, from its name (eu, us) or a path."""
    return region if os.path.isdir(region) else os.path.join(ROOT, region)


def name(region_dir):
    return os.path.basename(os.path.normpath(region_dir))


def hmi(region_dir):
    return os.path.join(region_dir, 'gui.HMI')


def tft(region_dir, theme):
    return os.path.join(region_dir, f'output_tft{theme}', 'gui.tft')


def numbered_dir(region_dir, theme):
    return os.path.join(region_dir, f'pictures_tft{theme}', 'all_numbered')


def numbered(region_dir, theme):
    """{picture ID: path} of a theme's all_numbered/, which holds N.bmp or N.png."""
    folder = numbered_dir(region_dir, theme)
    return {int(f.split('.')[0]): os.path.join(folder, f)
            for f in os.listdir(folder) if f.endswith(('.bmp', '.png'))}


def numbering_problems(region_dir):
    """The Editor numbers pictures by import order, so each theme's IDs must run 0..N with no
    gaps, the same N in every theme."""
    problems = []
    counts = set()
    for theme in THEMES:
        ids = sorted(numbered(region_dir, theme))
        if ids != list(range(len(ids))):
            problems.append(f'pictures_tft{theme}/all_numbered/ has gaps; IDs must run 0..N')
        counts.add(len(ids))
    if len(counts) > 1:
        problems.append(f'the themes have different numbers of pictures: {sorted(counts)}')
    return problems


def progress_bars(region_dir):
    """[(page, [(object, (r, g, b))...])] from progress_bars.txt, if there is one. Each line is
    PAGE OBJECT=#RRGGBB..., and every bar must name its fill colour."""
    listed = os.path.join(region_dir, 'progress_bars.txt')
    if not os.path.exists(listed):
        return []
    found = []
    with open(listed) as f:
        for number, line in enumerate(f, 1):
            if not line.strip() or line.startswith('#'):
                continue
            page, *words = line.split()
            if not words:
                raise ValueError(f'{listed}:{number}: page {page} lists no bars')
            bars = []
            for word in words:
                obj, _, colour = word.partition('=')
                if not re.fullmatch(r'#[0-9A-Fa-f]{6}', colour):
                    raise ValueError(f'{listed}:{number}: {word}: expected OBJECT=#RRGGBB, with the bar\'s fill colour')
                bars.append((obj, tuple(int(colour[i:i + 2], 16) for i in (1, 3, 5))))
            found.append((page, bars))
    return found
