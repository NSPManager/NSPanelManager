#!/usr/bin/env python3
"""Read and edit component attributes in a Nextion Editor .HMI project, outside the Editor.

  hmi_tool.py pages FILE                          list pages
  hmi_tool.py show FILE PAGE [--attrs x,y,w,h]    list a page's components
  hmi_tool.py set IN OUT PAGE.OBJECT.ATTR=VALUE...

`set` only changes numeric attributes, keeps endx/endy in step with x/y/w/h, and re-stamps each
edited page (see nextion/hmi.py). It writes OUT, never IN, and re-reads OUT to check the result.
If the Editor rejects the result ("Version Mismatch", then "Loading Page Failed"), Save As in the
Editor and edit that copy (see nextion/hmi.py).
"""
import argparse
import os
import sys

from nextion import hmi


def parse_edit(text):
    target, _, value = text.partition('=')
    parts = target.split('.')
    if len(parts) != 3 or not value:
        raise ValueError(f'expected PAGE.OBJECT.ATTR=VALUE, got {text!r}')
    return parts[0], parts[1], parts[2], int(value, 0)


def cmd_pages(args):
    project = hmi.Project(args.file)
    for name, page in sorted(project.pages.items(), key=lambda kv: kv[1].entry.index):
        print(f'{name:20} {page.entry.name:8} {len(page.components):3} components')
    if project.stale:
        print(f'\n{len(project.stale)} stale blocks from earlier saves')


def cmd_show(args):
    project = hmi.Project(args.file)
    page = project.page(args.page)
    attrs = args.attrs.split(',')
    print('  '.join(f'{a:>8}' if a != 'objname' else f'{a:16}' for a in ['objname'] + attrs))
    for comp in page.components:
        row = [f'{comp.name(page.block):16}']
        for a in attrs:
            row.append(f'{comp.get(page.block, a) if a in comp.attrs else "-":>8}')
        print('  '.join(row))


def cmd_set(args):
    if os.path.exists(args.out) and os.path.samefile(args.file, args.out):
        sys.exit('OUT must be a different file from IN')
    project = hmi.Project(args.file)
    try:
        edits = [parse_edit(e) for e in args.edits]
        data, changes = hmi.apply_edits(project, edits)
    except (KeyError, ValueError) as e:
        sys.exit(str(e).strip('"\''))
    with open(args.out, 'wb') as f:
        f.write(data)
    hmi.check_written(project, args.out, edits)
    for page_name, obj, attr, old, new in changes:
        print(f'{page_name}.{obj}.{attr}: {old} -> {new}')
    print(f'wrote {args.out}')
    if project.stale:
        print(f'{args.file} holds {len(project.stale)} stale blocks from earlier saves. The Editor has '
              'accepted edits to such files, but once rejected one for a reason not yet known: if it '
              'reports "Version Mismatch", Save As in the Editor and edit that copy.')


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = parser.add_subparsers(dest='command', required=True)
    p = sub.add_parser('pages', help='list pages')
    p.add_argument('file')
    p.set_defaults(func=cmd_pages)
    p = sub.add_parser('show', help="list a page's components")
    p.add_argument('file')
    p.add_argument('page')
    p.add_argument('--attrs', default='type,x,y,w,h')
    p.set_defaults(func=cmd_show)
    p = sub.add_parser('set', help='change numeric attributes, writing a new file')
    p.add_argument('file')
    p.add_argument('out')
    p.add_argument('edits', nargs='+', metavar='PAGE.OBJECT.ATTR=VALUE')
    p.set_defaults(func=cmd_set)
    args = parser.parse_args(argv)
    args.func(args)


if __name__ == '__main__':
    main()
