#!/usr/bin/env python3
"""Read and edit component attributes in a Nextion Editor .HMI project, outside the Editor.

  hmi_tool.py pages FILE                          list pages, with their IDs
  hmi_tool.py show FILE PAGE [--attrs x,y,w,h]    list a page's components
  hmi_tool.py events FILE PAGE [OBJECT]           print event code
  hmi_tool.py fonts FILE [--out DIR]              list fonts; with --out, extract them
  hmi_tool.py dump FILE                           pages, components, attributes and code as JSON
  hmi_tool.py set IN OUT PAGE.OBJECT.ATTR=VALUE...

`set` only changes numeric attributes, keeps endx/endy in step with x/y/w/h, and re-stamps each
edited page (see nextion_tools.hmi). It writes OUT, never IN, and re-reads OUT to check the result.
If the Editor rejects the result ("Version Mismatch", then "Loading Page Failed"), Save As in the
Editor and edit that copy (see nextion_tools.hmi).
"""
import argparse
import json
import os
import sys

from PIL import Image

from nextion_tools import hmi


def parse_edit(text):
    target, _, value = text.partition('=')
    parts = target.split('.')
    if len(parts) != 3 or not value:
        raise ValueError(f'expected PAGE.OBJECT.ATTR=VALUE, got {text!r}')
    return parts[0], parts[1], parts[2], int(value, 0)


def cmd_pages(args):
    project = hmi.Project.load(args.file)
    for name, page in ((p.name, p) for p in project.pages):
        print(f'{page.id:3}  {name:20} {page.entry.name:8} {len(page.components):3} components')
    if project.stale:
        print(f'\n{len(project.stale)} stale blocks from earlier saves')


def cmd_show(args):
    project = hmi.Project.load(args.file)
    page = project.page(args.page)
    attrs = args.attrs.split(',')
    print('  '.join(f'{a:>8}' if a != 'objname' else f'{a:16}' for a in ['objname'] + attrs))
    for comp in page.components:
        row = [f'{comp.name:16}']
        for a in attrs:
            row.append(f'{comp.get(a) if a in comp.attrs else "-":>8}')
        print('  '.join(row))


def cmd_events(args):
    project = hmi.Project.load(args.file)
    page = project.page(args.page)
    comps = [page.component(args.object)] if args.object else page.components
    for comp in comps:
        for event, code in comp.events.items():
            if code:
                print(f'{page.name}.{comp.name} {event}:')
                for line in code:
                    print(f'    {line}')


def cmd_fonts(args):
    project = hmi.Project.load(args.file)
    blocks = project.font_blocks()
    for index, ((member, block), font) in enumerate(zip(blocks, project.fonts())):
        print(f'{index:3}  {font.name:28} {member:7} height {font.height:3}  {len(font.glyphs):4} glyphs')
        if args.out:
            _write_font(os.path.join(args.out, f'{index:02}_{font.name}'), font, block)
    if args.out:
        with open(os.path.join(args.out, 'fonts.json'), 'w') as f:
            json.dump([_font_summary(i, m, font) for i, ((m, _), font) in enumerate(zip(blocks, project.fonts()))],
                      f, indent=1)
        print(f'wrote {args.out}')


def _font_summary(index, member, font):
    return {'index': index, 'member': member, 'name': font.name, 'encoding': font.encoding,
            'height': font.height, 'version': font.version, 'glyphs': len(font.glyphs)}


def _write_font(folder, font, block):
    """The .zi as stored, one PNG per glyph (white ink, coverage as alpha, the whole box) and a
    glyphs.json of each glyph's metrics."""
    os.makedirs(folder, exist_ok=True)
    with open(os.path.join(folder, f'{font.name}.zi'), 'wb') as f:
        f.write(block)
    glyphs = []
    for g in font.glyphs.values():
        name = f'U+{g.code:04X}.png'
        alpha = Image.fromarray(g.coverage, 'L')
        Image.merge('LA', (Image.new('L', alpha.size, 255), alpha)).save(os.path.join(folder, name))
        glyphs.append({'code': g.code, 'char': g.char if g.code >= 0x20 else None, 'advance': g.advance,
                       'left': g.left, 'right': g.right, 'file': name})
    with open(os.path.join(folder, 'glyphs.json'), 'w') as f:
        json.dump(glyphs, f, indent=1)


def dump(project):
    """The project as plain data: pages in ID order with their components, every attribute
    decoded, and event code; and the font list."""
    pages = []
    for page in project.pages:
        comps = [{'attrs': c.attrs, 'events': {e: code for e, code in c.events.items() if code}}
                 for c in page.components]
        pages.append({'id': page.id, 'name': page.name, 'block': page.entry.name, 'components': comps})
    fonts = [_font_summary(i, m, font) for i, ((m, _), font) in enumerate(zip(project.font_blocks(), project.fonts()))]
    return {'pages': pages, 'fonts': fonts}


def cmd_dump(args):
    json.dump(dump(hmi.Project.load(args.file)), sys.stdout, indent=1, ensure_ascii=False)
    print()


def cmd_set(args):
    if os.path.exists(args.out) and os.path.samefile(args.file, args.out):
        sys.exit('OUT must be a different file from IN')
    project = hmi.Project.load(args.file)
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
    p = sub.add_parser('events', help='print event code')
    p.add_argument('file')
    p.add_argument('page')
    p.add_argument('object', nargs='?')
    p.set_defaults(func=cmd_events)
    p = sub.add_parser('fonts', help='list fonts, or extract them with --out')
    p.add_argument('file')
    p.add_argument('--out', metavar='DIR', help='write each font\'s .zi, a PNG per glyph and its metrics here')
    p.set_defaults(func=cmd_fonts)
    p = sub.add_parser('dump', help='print pages, components, attributes and event code as JSON')
    p.add_argument('file')
    p.set_defaults(func=cmd_dump)
    p = sub.add_parser('set', help='change numeric attributes, writing a new file')
    p.add_argument('file')
    p.add_argument('out')
    p.add_argument('edits', nargs='+', metavar='PAGE.OBJECT.ATTR=VALUE')
    p.set_defaults(func=cmd_set)
    args = parser.parse_args(argv)
    args.func(args)


if __name__ == '__main__':
    main()
