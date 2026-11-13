# /// script
# requires-python = ">=3.9"
# dependencies = ["nextion-tools @ git+https://github.com/jamesmulcahy/nextion-tools@ffadfa26c8c0f58dd7df6e28883bdc46ab072d0d"]
# ///
"""Check that the US and EU HMIs differ only where we know they do.

    uv run docker/HMI_files/hmi_compare.py

The two projects lay the same pages out for different screens, so they are compared by name
(page.component), with these differences allowed by rule:

- position and size (x, y, w, h);
- which picture a component uses (each project has its own pictures), but not whether it
  has one;
- a font's index (compared by the font's name and height instead);
- a component's id, unless it reports touches (sendkey), since touch events carry the id;
- comments in event code and Program.s.

Every other difference must be listed in hmi_us_eu_differences.txt, one per line:

    expected  home.s_kelvin  mode      # why it is meant to differ (required)
    todo      dropdown.tc0   on up     # a difference to remove some day

`where` is page.component, a page's name (for `slot` and `exists`), or `*` for the project
(`direction`, `Program.s`). `field` is an attribute, `on <event>`, `id`, `exists` or `slot`.

The check fails on a difference that is not listed, and on a listed one that no longer
exists, so that fixing a `todo` means deleting its line and the list only shrinks.
"""

import difflib
import sys
from pathlib import Path

from hmi_inventory import NO_PICTURE, PICTURE_ATTRS, REPO, Project, resources

HERE = Path(__file__).resolve().parent
US = HERE / "tft_automation/us/gui.HMI"
EU = HERE / "tft_automation/eu/gui.HMI"
KNOWN = HERE / "hmi_us_eu_differences.txt"
STATUSES = ("expected", "todo")
GEOMETRY = {"x", "y", "w", "h", "endx", "endy"}


def strip_comment(line):
    """line without a // comment (one outside a "string") or trailing space."""
    quoted = False
    for i, ch in enumerate(line):
        if ch == '"':
            quoted = not quoted
        elif not quoted and line.startswith("//", i):
            return line[:i].rstrip()
    return line.rstrip()


def code(lines):
    return [s for s in (strip_comment(line) for line in lines) if s.strip()]


def normalised(attr, value, fonts):
    if attr in PICTURE_ATTRS:
        return "none" if value == NO_PICTURE else "set"
    if attr == "font" and isinstance(value, int) and value < len(fonts):
        return fonts[value]
    return value


def code_diff(us, eu):
    return "".join("        " + line + "\n" for line in list(difflib.unified_diff(us, eu, "us", "eu", lineterm="", n=1))[2:])


def differences(us, eu):
    """{(where, field): description} of every difference not allowed by rule."""
    found = {}
    us_fonts, _ = resources(us)
    eu_fonts, _ = resources(eu)
    if us.direction != eu.direction:
        found["*", "direction"] = "US %d, EU %d" % (us.direction, eu.direction)
    if code(us.program) != code(eu.program):
        found["*", "Program.s"] = "\n" + code_diff(code(us.program), code(eu.program))
    for name in sorted({p.name for p in us.pages} | {p.name for p in eu.pages}):
        pu, pe = us.find_page(name), eu.find_page(name)
        if not (pu and pe):
            found[name, "exists"] = "only in %s" % ("US" if pu else "EU")
            continue
        if pu.id != pe.id:
            found[name, "slot"] = "US %d, EU %d" % (pu.id, pe.id)
        for cname in sorted({c.name for c in pu.components} | {c.name for c in pe.components}):
            where = "%s.%s" % (name, cname)
            cu, ce = pu.find(cname), pe.find(cname)
            if not (cu and ce):
                found[where, "exists"] = "only in %s" % ("US" if cu else "EU")
                continue
            if cu.id != ce.id and (cu.attrs.get("sendkey") or ce.attrs.get("sendkey")):
                found[where, "id"] = "US %d, EU %d" % (cu.id, ce.id)
            for attr in sorted(set(cu.attrs) | set(ce.attrs)):
                if attr in GEOMETRY or attr == "id":
                    continue
                a, b = normalised(attr, cu.attrs.get(attr), us_fonts), normalised(attr, ce.attrs.get(attr), eu_fonts)
                if a != b:
                    found[where, attr] = "US %r, EU %r" % (a, b)
            for event in sorted(set(cu.events) | set(ce.events)):
                a, b = code(cu.events.get(event, [])), code(ce.events.get(event, []))
                if a != b:
                    found[where, "on " + event] = "\n" + code_diff(a, b)
    return found


def load_known(path):
    """({(where, field): status}, [problems])."""
    known, problems = {}, []
    for n, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line, _, reason = raw.partition("#")
        words = line.split()
        if not words:
            continue
        here = "%s:%d" % (path.name, n)
        if len(words) < 3 or words[0] not in STATUSES:
            problems.append("%s: expected '<%s> <where> <field>  # reason'" % (here, "|".join(STATUSES)))
            continue
        status, where, field = words[0], words[1], " ".join(words[2:])
        if status == "expected" and not reason.strip():
            problems.append("%s: an 'expected' difference needs a reason after #" % here)
        if (where, field) in known:
            problems.append("%s: %s %s is listed twice" % (here, where, field))
        known[where, field] = status
    return known, problems


def main():
    found = differences(Project.load(str(US)), Project.load(str(EU)))
    known, problems = load_known(KNOWN)
    rel = KNOWN.relative_to(REPO)
    unlisted = [k for k in found if k not in known]
    gone = [k for k in known if k not in found]
    for where, field in unlisted:
        problems.append("US and EU differ, and %s does not list it: %s %s: %s\n    add:  todo  %s  %s" % (rel, where, field, found[where, field], where, field))
    for where, field in gone:
        problems.append("%s lists '%s %s', but the US and EU HMIs no longer differ there; delete the line." % (rel, where, field))
    counts = {s: sum(1 for v in known.values() if v == s) for s in STATUSES}
    if problems:
        print("\n".join(problems))
        return 1
    print("US and EU differ only where %s says: %d expected, %d todo." % (rel, counts["expected"], counts["todo"]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
