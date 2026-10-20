"""Where a region's files live. A region directory (eu, us) holds gui.HMI,
and for each theme N its pictures in pictures_tftN/all_numbered/ and its TFT in
output_tftN/gui.tft."""
import os

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
