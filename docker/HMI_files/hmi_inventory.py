# /// script
# requires-python = ">=3.9"
# dependencies = ["nextion-tools @ git+https://github.com/jamesmulcahy/nextion-tools@ffadfa26c8c0f58dd7df6e28883bdc46ab072d0d"]
# ///
"""A text inventory of a Nextion .HMI project, so a change to one shows up in a diff.

    uv run docker/HMI_files/hmi_inventory.py FILE.HMI   # print FILE's inventory
    uv run docker/HMI_files/hmi_inventory.py --write    # regenerate every committed gui.HMI.txt
    uv run docker/HMI_files/hmi_inventory.py --check    # fail if a committed gui.HMI.txt is stale

Each .HMI under docker/HMI_files/ has its inventory committed next to it as <name>.HMI.txt. CI
runs --check, so a PR that changes an HMI has to carry the matching change to its inventory, and
the review shows what changed in the HMI's pages, components, attributes and code.

To get the same view from `git diff` / `git log -p` locally (.gitattributes marks *.HMI
diff=hmi; git only takes the command from your own config):

    git config diff.hmi.textconv "uv run --quiet docker/HMI_files/hmi_inventory.py"

The inventory lists only the attributes in ATTRIBUTES. Those in IGNORED are left out on
purpose (Editor bookkeeping, or the same value on every component); any attribute in neither
set is printed, so one this script has not met yet still shows.
"""

import argparse
import difflib
import struct
import sys
from pathlib import Path

from nextion_tools.hmi import Project

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
SUFFIX = ".txt"  # gui.HMI -> gui.HMI.txt

# Printed, in this order, after the component's header line (id, name, kind, global, sendkey)
# and its position (x, y, w, h on one line).
ATTRIBUTES = [
    # state and content
    "sta",
    "val",
    "txt",
    "txt_maxl",
    "en",
    "tim",
    "minval",
    "maxval",
    "dis",
    "mode",
    "wid",
    "hig",
    # the page's swipe targets
    "up",
    "down",
    "left",
    "right",
    # appearance
    "style",
    "font",
    "pic",
    "pic1",
    "pic2",
    "picc",
    "picc1",
    "picc2",
    "bpic",
    "ppic",
    "bco",
    "bco1",
    "bco2",
    "pco",
    "pco2",
    "xcen",
    "ycen",
    "isbr",
]
HEADER_ATTRS = {"id", "objname", "type", "vscope", "sendkey", "x", "y", "w", "h"}
IGNORED = {
    "endx",
    "endy",  # x + w - 1, y + h - 1
    # The same on every component in our HMIs.
    "aph",
    "borderc",
    "borderw",
    "ch",
    "dez",
    "drag",
    "effect",
    "first",
    "format",
    "groupid0",
    "groupid1",
    "key",
    "lenth",
    "lockobj",
    "movex",
    "movey",
    "psta",
    "pw",
    "spax",
    "spay",
    "time",
    "vvs0",
    "vvs1",
}
FONT_ATTRS = {"font"}
PICTURE_ATTRS = {"pic", "pic1", "pic2", "picc", "picc1", "picc2", "bpic", "ppic"}
NO_PICTURE = 65535

HEADER = """\
# Generated from the .HMI next to this file by docker/HMI_files/hmi_inventory.py. Do not edit.
# Regenerate with: uv run docker/HMI_files/hmi_inventory.py --write
"""


def font_label(block):
    """'<name> <height>' from a .zi font's header (see nextion_tools.fonts)."""
    name_length = struct.unpack_from("<I", block, 0x20)[0]
    return "%s %d" % (block[44 : 44 + name_length].decode("latin1"), block[7])


def resources(project):
    """(font labels by index, picture sizes by id)."""
    fonts = [font_label(block) for _, block in project.font_blocks()]
    pictures = []
    for i in range(project.picture_count):
        p = project.picture(i)
        pictures.append("%dx%d" % (p.width, p.height))
    return fonts, pictures


def describe(attr, value, fonts, pictures):
    if attr in FONT_ATTRS and isinstance(value, int) and value < len(fonts):
        return "%d (%s)" % (value, fonts[value])
    if attr in PICTURE_ATTRS and isinstance(value, int):
        if value == NO_PICTURE:
            return "none"
        if value < len(pictures):
            return "%d (%s)" % (value, pictures[value])
    return repr(value)


def component_header(c):
    flags = [c.kind]
    if c.is_global:
        flags.append("global")
    if c.reports_press or c.reports_release:
        flags.append("sendkey=" + "+".join(e for e, on in (("press", c.reports_press), ("release", c.reports_release)) if on))
    return "[%d] %s  %s" % (c.id, c.name, "  ".join(flags))


def inventory(path):
    project = Project.load(str(path))
    fonts, pictures = resources(project)
    out = [HEADER.rstrip("\n")]
    out.append("direction %d" % project.direction)
    out.append("")
    out.append("## fonts")
    out += ["%d  %s" % (i, f) for i, f in enumerate(fonts)]
    out.append("")
    out.append("## Program.s")
    out += ["    " + line.rstrip() for line in project.program]
    for page in project.pages:
        out.append("")
        out.append("## page %d %s" % (page.id, page.name))
        for c in page.components:
            out.append(component_header(c))
            if c.rect:
                out.append("    at %d,%d  %dx%d" % c.rect)
            known = HEADER_ATTRS | IGNORED | set(ATTRIBUTES)
            for attr in ATTRIBUTES + sorted(a for a in c.attrs if a not in known):
                if attr in c.attrs:
                    out.append("    %s = %s" % (attr, describe(attr, c.attrs[attr], fonts, pictures)))
            for event, lines in sorted(c.events.items()):
                if any(line.strip() for line in lines):
                    out.append("    on %s:" % event)
                    out += ["        " + line.rstrip() for line in lines]
    return "\n".join(out) + "\n"


def hmi_files():
    return sorted(HERE.rglob("*.HMI"))


def inventory_path(hmi):
    return hmi.with_name(hmi.name + SUFFIX)


def write():
    for hmi in hmi_files():
        inventory_path(hmi).write_text(inventory(hmi), encoding="utf-8")
        print("wrote", inventory_path(hmi).relative_to(REPO))
    return 0


def check():
    stale = 0
    for hmi in hmi_files():
        txt = inventory_path(hmi)
        rel = txt.relative_to(REPO)
        want = inventory(hmi)
        have = txt.read_text(encoding="utf-8") if txt.exists() else ""
        if want != have:
            stale += 1
            print("%s does not match %s:" % (rel, hmi.name))
            sys.stdout.writelines(difflib.unified_diff(have.splitlines(True), want.splitlines(True), "committed/%s" % rel, "generated/%s" % rel))
            print()
    orphans = [t for t in HERE.rglob("*.HMI" + SUFFIX) if not t.with_name(t.name[: -len(SUFFIX)]).exists()]
    for t in orphans:
        print("%s has no .HMI next to it; delete it." % t.relative_to(REPO))
    if stale or orphans:
        print("Regenerate with: uv run docker/HMI_files/hmi_inventory.py --write")
        return 1
    print("%d inventories up to date" % len(hmi_files()))
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("hmi", nargs="?", help="print this .HMI's inventory")
    group.add_argument("--write", action="store_true", help="regenerate every committed inventory")
    group.add_argument("--check", action="store_true", help="fail if a committed inventory is stale")
    args = parser.parse_args(argv)
    if args.write:
        return write()
    if args.check:
        return check()
    sys.stdout.write(inventory(args.hmi))
    return 0


if __name__ == "__main__":
    sys.exit(main())
