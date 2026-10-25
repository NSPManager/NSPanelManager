#!/usr/bin/env python3
"""Build each theme's TFT from one TFT the Editor compiled, without the Editor.

  build_tfts.py REGION [--template TFT] [--themes 1,2,3,4]

Writes REGION/output_tftN/gui.tft for each theme: the template with the theme's pictures from
REGION/pictures_tftN/all_numbered swapped in (see tft_build/themes.py). By default the template is
REGION/output_tft1/gui.tft, which the Editor compiles from the HMI whenever the HMI changes.

Layouts made from this region's TFTs at another orientation (see ROTATED in tft_build/themes.py)
are written too: building eu also writes us_horizontal_mirrored.
"""
import argparse
import os
import sys

from nextion_tools import tft
from tft_build import region, themes


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('region', help='eu or us, or a directory holding pictures_tft1..4 and output_tft1..4')
    parser.add_argument('--template', help='TFT the Editor compiled (default: REGION/output_tft1/gui.tft)')
    parser.add_argument('--themes', default=','.join(map(str, region.THEMES)))
    args = parser.parse_args(argv)
    region_dir = region.path(args.region)
    template_path = args.template or region.tft(region_dir, region.THEMES[0])
    with open(template_path, 'rb') as f:
        template = f.read()
    if not tft.checksums_ok(template):
        sys.exit(f'{template_path}: checksums do not match; is it a TFT the Editor compiled for this panel?')
    name = region.name(region_dir)
    expected = themes.ORIENTATIONS.get(name)
    if expected is not None and tft.orientation(template) != expected:
        sys.exit(f'{template_path} is compiled for orientation {tft.orientation(template)}, but {name} panels need '
                 f'{expected}: set it in the Editor\'s device settings and compile again')
    try:
        built = themes.build(template, region_dir, [int(t) for t in args.themes.split(',')])
    except ValueError as e:
        sys.exit(str(e))
    for theme, data in built.items():
        write(region.tft(region_dir, theme), data)
        for layout, degrees in themes.rotated(name):
            write(region.tft(region.path(layout), theme), tft.with_orientation(data, degrees))


def write(path, data):
    if os.path.exists(path):
        with open(path, 'rb') as f:
            if f.read() == data:
                print(f'{path}: unchanged')
                return
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'wb') as f:
        f.write(data)
    print(f'{path}: written')


if __name__ == '__main__':
    main()
