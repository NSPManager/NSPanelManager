#!/usr/bin/env python3
"""Check that the committed HMIs, pictures and TFTs agree, without GIMP or the Editor.

  check_build.py [REGION...]             check what is committed (eu and us by default)
  check_build.py --pictures [REGION...]  check only the pictures (after make_pictures.sh renders,
                                         before the TFTs are rebuilt)
  check_build.py --rendered [REGION...]  also check that freshly rendered pictures (from
                                         make_pictures.sh) match the committed ones

For each region:
- every theme's all_numbered/ runs 0..N with no gaps, the same N in every theme;
- the progress bar pictures are what progress_bars.txt and gui.HMI make now;
- one theme's TFT holds exactly the HMI's pictures, so the TFTs were compiled from this HMI, and
  every theme's TFT has the HMI's picture count and sizes;
- every TFT has the region's display orientation;
- make_tfts.sh rebuilds every theme's TFT byte for byte, including the layouts it makes from the
  region's TFTs at another orientation (us_horizontal_mirrored from eu).

--rendered compares pixels, not files: the BMP headers vary with the ImageMagick version.
"""
import argparse
import os
import subprocess
import sys

import numpy as np

from nextion_tools import hmi, images, tft
from tft_build import bars, region, themes


def check_pictures(region_dir, project):
    problems = region.numbering_problems(region_dir)
    for page, objects in region.progress_bars(region_dir):
        if project.find_page(page) is None:
            continue
        for theme, made in bars.pictures(project, region_dir, page, objects).items():
            committed = region.numbered(region_dir, theme)
            for pic, pixels in made.items():
                if pic not in committed or not np.array_equal(images.read(committed[pic]), pixels):
                    problems.append(f'pictures_tft{theme}/all_numbered/{pic}.bmp is not what {page}\'s bars '
                                    'make now; run make_pictures.sh')
    return problems


def check_tfts(name, region_dir, project):
    """Problems, and the themes whose TFT holds exactly the HMI's pictures."""
    problems = []
    records = project.pictures()
    tfts = {}
    for theme in region.THEMES:
        with open(region.tft(region_dir, theme), 'rb') as f:
            tfts[theme] = f.read()
    held = []
    for theme, data in tfts.items():
        if not tft.checksums_ok(data):
            problems.append(f'output_tft{theme}/gui.tft: checksums do not match')
        expected = themes.ORIENTATIONS.get(name)
        if expected is not None and tft.orientation(data) != expected:
            problems.append(f'output_tft{theme}/gui.tft is compiled for orientation {tft.orientation(data)}, '
                            f'but {name} panels need {expected}')
        theirs = tft.records(data)
        if len(theirs) != len(records):
            problems.append(f'output_tft{theme}/gui.tft has {len(theirs)} pictures but gui.HMI has {len(records)}')
        elif theirs == records:
            held.append(theme)
    if not held:
        problems.append('no theme\'s TFT holds the pictures in gui.HMI, so the TFTs were not compiled from it; '
                        'compile it in the Editor into output_tft1/gui.tft and run make_tfts.sh')

    try:
        built = themes.build(tfts[region.THEMES[0]], region_dir)
    except ValueError as e:
        problems.append(f'make_tfts.sh would fail: {e}')
    else:
        for theme, data in built.items():
            if data != tfts[theme]:
                problems.append(f'output_tft{theme}/gui.tft is not what make_tfts.sh builds; run make_tfts.sh')
    for layout, degrees in themes.rotated(name):
        for theme, data in tfts.items():
            path = region.tft(region.path(layout), theme)
            with open(path, 'rb') as f:
                if f.read() != tft.with_orientation(data, degrees):
                    problems.append(f'{os.path.relpath(path, region.ROOT)} is not output_tft{theme}/gui.tft at '
                                    f'orientation {degrees}; run make_tfts.sh')
    return problems, held


def check_rendered(region_dir):
    """Compare the rendered all_numbered/ pictures with the committed ones, by pixels."""
    problems = []
    repo = subprocess.run(['git', 'rev-parse', '--show-toplevel'], cwd=region_dir, capture_output=True,
                          text=True, check=True).stdout.strip()
    for theme in region.THEMES:
        for pic, path in sorted(region.numbered(region_dir, theme).items()):
            name = os.path.relpath(path, region_dir)
            committed = subprocess.run(['git', 'show', f'HEAD:{os.path.relpath(path, repo)}'], cwd=repo,
                                       capture_output=True)
            if committed.returncode != 0:
                problems.append(f'{name} is rendered but not committed')
                continue
            if path.endswith('.bmp'):
                same = np.array_equal(images.read(path), images.read(committed.stdout))
            else:
                with open(path, 'rb') as f:
                    same = f.read() == committed.stdout
            if not same:
                problems.append(f'{name} renders differently from the committed picture')
    return problems


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--pictures', action='store_true', help='check only the pictures')
    mode.add_argument('--rendered', action='store_true',
                      help='also compare freshly rendered pictures with the committed ones')
    parser.add_argument('regions', nargs='*', default=['eu', 'us'])
    args = parser.parse_args(argv)
    failed = False
    for name in args.regions:
        region_dir = region.path(name)
        project = hmi.Project.load(region.hmi(region_dir))
        problems = check_pictures(region_dir, project)
        held = []
        if not args.pictures:
            tft_problems, held = check_tfts(region.name(region_dir), region_dir, project)
            problems += tft_problems
        if args.rendered:
            problems += check_rendered(region_dir)
        for problem in problems:
            print(f'{name}: {problem}')
        if problems:
            failed = True
        elif held:
            print(f'{name}: OK (gui.HMI holds theme {", ".join(map(str, held))}\'s pictures)')
        else:
            print(f'{name}: OK')
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
