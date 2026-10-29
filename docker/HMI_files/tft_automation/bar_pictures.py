#!/usr/bin/env python3
"""Make the pictures for the image progress bars a region lists in progress_bars.txt.

  bar_pictures.py REGION --numbered-after N
  bar_pictures.py REGION --out DIR

With --numbered-after, writes each theme's pictures into REGION/pictures_tftN/all_numbered/
beside GIMP's, which end at picture N; with --out, into DIR/tftN/ instead. See tft_build/bars.py.
A page not yet in the HMI is skipped with a note.
"""
import argparse
import sys

from nextion_tools import hmi
from tft_build import bars, region


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('region', help='eu or us, or a directory holding gui.HMI and pictures_tft1..4')
    where = parser.add_mutually_exclusive_group(required=True)
    where.add_argument('--out', help='write DIR/tftN/<picture>.bmp')
    where.add_argument('--numbered-after', type=int, metavar='N',
                       help="write into each theme's pictures_tftN/all_numbered/, whose GIMP pictures end at N")
    args = parser.parse_args(argv)
    region_dir = region.path(args.region)
    project = hmi.Project.load(region.hmi(region_dir))
    try:
        for page, listed in region.progress_bars(region_dir):
            if project.find_page(page) is None:
                print(f'gui.HMI has no page {page} yet; skipping its progress bars')
                continue
            bars.make(project, region_dir, page, listed, args.out, args.numbered_after)
            print(f'progress bars on {page}: {" ".join(obj for obj, _ in listed)}')
    except (KeyError, ValueError, OSError) as e:
        sys.exit(str(e).strip('"\''))


if __name__ == '__main__':
    main()
