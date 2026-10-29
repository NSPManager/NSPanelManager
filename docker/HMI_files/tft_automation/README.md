# TFT automation

Builds the panel GUIs: one TFT per region and theme. The regions are `eu` (landscape) and `us`
(portrait), plus `us_horizontal_mirrored`, which is made from `eu` (below). The four themes are the same HMI with different pictures (see `../readme.txt` for
their names).

```
gimp_scripts/*.scm --GIMP--> pictures_tftN/*.bmp --dither--> pictures_tftN/all_numbered/N.bmp
gui.HMI + progress_bars.txt --bar_pictures.py--> pictures_tftN/all_numbered/181.bmp ...
output_tft1/gui.tft (Editor) + all_numbered --make_tfts.sh--> output_tft1..4/gui.tft
```

## After changing an HMI

1. Save the HMI as `REGION/gui.HMI`. Add new pictures after the existing ones, so no picture ID
   changes.
2. If you added an image progress bar (`sta=1`), list it with its colour in
   `REGION/progress_bars.txt` (see below), so its pictures are generated instead of drawn by hand. A bar already listed there
   needs nothing: moving or resizing it in the HMI is enough.
3. Render the pictures: `sudo ./make_pictures.sh` (or `sudo ./make_pictures.sh us`). See below.
4. Compile the HMI once in the Nextion Editor and save the TFT as `REGION/output_tft1/gui.tft`.
   This is the template for every theme. It doesn't matter which theme's pictures the HMI holds,
   but the display orientation in the Editor's device settings must be the region's: 270 for
   `eu`, 180 for `us`. `make_tfts.sh` refuses a template with any other.
5. Build the themes: `./make_tfts.sh` (or `./make_tfts.sh us`). Building `eu` also writes
   `us_horizontal_mirrored`. See below.
6. Check it: `./check_build.py` (see "Tests and checks").

The Editor is needed only in step 4. `REGION/tftautomation.ahk` is the older way to do steps 4
and 5: it drives the Editor to delete every picture, import `all_numbered/` and compile, once per
theme.

## make_pictures.sh

Renders every theme's pictures in Docker, with no display. The image is `docker/Dockerfile`:
Ubuntu 24.04, for GIMP 2.10, which the Script-Fu in `gimp_scripts/` needs, and ImageMagick 6.9.12,
whose BMP headers match the committed pictures byte for byte. Output and a log go to
`last_pictures_run.log`.

For each region it:

1. runs `REGION/create_pictures_REGION.sh`: GIMP saves each theme's pictures, ImageMagick dithers
   them, and they are numbered into `pictures_tftN/all_numbered/`;
2. checks that every picture the `.scm` scripts save was written by this run;
3. makes the progress bar pictures listed in `REGION/progress_bars.txt` (below);
4. runs `check_build.py --pictures`: each `all_numbered/` must run from 0 to N with no gaps, since
   the Editor numbers pictures by import order and a gap would shift every later ID.

`--xvfb` runs GIMP under a virtual X server instead of `gimp-console`, if that is ever needed.

## progress_bars.txt

An image progress bar (`sta=1`) needs two pictures, empty and full, each exactly the bar's size
and matching the page background in every theme. Drawn by hand, that is 8 pictures per bar per
region, all redrawn whenever the bar moves, changes size or the background changes, and easy to
get subtly wrong. Instead, list the bar here and its pictures are generated from the HMI on every
`make_pictures.sh` run. There is nothing to draw or maintain; `check_build.py` fails if the
committed pictures fall out of step with the HMI.

```
# PAGE OBJECT=#RRGGBB...
mediaplayer p_pos=#DEAA31 p_volume=#DEAA31
```

Each bar is listed with the colour of its full part, which is required: there is no default, so
every bar's colour is chosen and visible here. Everything else comes from the HMI. For each bar,
`bar_pictures.py` reads its `x`, `y`, `w`, `h`, `bpic` and `ppic` from `gui.HMI`, and writes for
every theme:

- `bpic`, the empty bar: the page's background picture (the page's `pic`) cropped to the bar,
  from that theme's dithered `all_numbered/`. It blends into the page in every theme.
- `ppic`, the full bar: a solid fill in the bar's colour. It is the same in every theme, because
  the Editor compresses flat pictures and `make_tfts.sh` can only swap pictures stored raw.

The pictures always match the bar's size, which the Editor requires. To move or resize a bar,
change it in the HMI and run `make_pictures.sh` again. `bpic` and `ppic` must come after GIMP's
last picture; `bar_pictures.py` refuses to overwrite GIMP's pictures. A page not yet in `gui.HMI`
is skipped with a note.

## make_tfts.sh

Builds each theme's TFT from the template in `output_tft1/gui.tft`, without the Editor. Runs on
the host, no Docker or sudo needed, but with [nextion-tools](https://github.com/jamesmulcahy/nextion-tools),
numpy and Pillow (`python3 -m pip install -r requirements.txt`). The themes differ only in pictures, so
`build_tfts.py` puts each theme's pictures from `all_numbered/` into the template's picture slots
and recomputes the TFT's checksums. Each picture keeps its slot size, so nothing else in the file moves.

It refuses, and asks for a new template, when:

- the template has a different number of pictures from `all_numbered/`;
- a picture's size differs from its slot;
- a picture differs between themes, but the Editor compressed it in the template. The
  compression format is unknown, so only pictures stored raw can change.

It also refuses a template compiled for the wrong display orientation.

`output_tft1/gui.tft` is then theme 1's build, which serves as the template from then on.

The theme whose pictures the HMI holds comes out byte-identical to the Editor's compile. The
other themes can differ from the Editor's own compile of them only where the Editor happened to
compress a picture, which this stores raw; both work on a panel.

## hmi_tool.py

Reads and edits Nextion `.HMI` files, for checking a layout or nudging it without the Editor:

```
./hmi_tool.py pages FILE                          # with each page's ID
./hmi_tool.py show FILE PAGE [--attrs type,x,y,w,h]
./hmi_tool.py events FILE PAGE [OBJECT]           # event code (touch, timer, page load)
./hmi_tool.py fonts FILE [--out DIR]              # with --out: each .zi, a PNG per glyph, metrics
./hmi_tool.py dump FILE                           # everything above as JSON
./hmi_tool.py set IN OUT PAGE.OBJECT.ATTR=VALUE...
```

IDs are what the display uses on the wire. A page's ID (`page N`, and the page byte of a touch
event) and a font's index (a component's `font` attribute) are both positions in `main.HMI`'s
resource list, not the numbers in the block names (`1.pa`, `3.zi`). A component's ID is its `id`
attribute, which is also its position on the page, and the page itself is component 0.

`set` changes numeric attributes only (position, size, picture IDs, flags). It keeps `endx` and
`endy` in step with `x`/`y`/`w`/`h`, writes a new file, and checks it. The Editor has opened such
edits, including to files still holding stale blocks from earlier saves, but once rejected one for
a reason not yet known. If it reports "Version Mismatch", Save As in the Editor and edit that copy.
Text, code, names and pictures still need the Editor.

## Tests and checks

```
python3 -m unittest discover -s tests -t .   # the build, against the committed files
./check_build.py                             # the committed HMIs, pictures and TFTs agree
./check_build.py --rendered                  # after make_pictures.sh: the rendering matches git
```

`check_build.py` fails, for each region, if:

- an `all_numbered/` has gaps, or the themes have different numbers of pictures;
- a progress bar's pictures are not what its bar in `gui.HMI` makes now;
- no theme's TFT holds exactly the HMI's pictures, which means the TFTs were not compiled from
  this HMI, or a TFT has a different picture count or sizes from the HMI;
- a TFT has the wrong display orientation;
- a TFT is not what `make_tfts.sh` builds, including `us_horizontal_mirrored`.

The HMI stores each picture exactly as the TFT does, so the third check covers every picture,
compressed or not. It cannot see changes to pages or code that leave the pictures alone.

## Code

The scripts here are thin command lines over two packages. The Nextion file formats are
[nextion-tools](https://github.com/jamesmulcahy/nextion-tools) (`nextion_tools`, pinned in
`requirements.txt`), which has its own tests and format notes:

| Module | What it holds |
| --- | --- |
| `nextion_tools.crc` | the CRC behind TFT checksums and HMI page stamps (CRC-32/MPEG-2 over widened bytes) |
| `nextion_tools.hmi` | reading `.HMI` projects (pages, components, event code, page and font IDs), and editing numeric attributes |
| `nextion_tools.fonts` | decoding `.zi` fonts to per-glyph coverage |
| `nextion_tools.tft` | a `.tft`'s picture table, picture swaps, display orientation, and checksums |
| `nextion_tools.images` | reading and writing pictures, and RGB565 (numpy and Pillow) |

What is particular to this project's build is `tft_build/`:

| Module | What it holds |
| --- | --- |
| `region` | where a region's HMI, pictures, TFTs and `progress_bars.txt` live |
| `bars` | progress bar pictures |
| `themes` | building every theme's TFT from the template; each region's orientation, and the layouts made by turning another region's TFTs |

The tests here check this project's files: the ids, fonts and code the firmware relies on, and
that the committed TFTs are what the build makes.

`.github/workflows/tft-build.yml` runs the tests and `check_build.py` on every change under
`docker/HMI_files/`. A second job then renders everything with `make_pictures.sh` and runs
`check_build.py --rendered`.

## us_horizontal_mirrored

US panels mounted in landscape, the manager's "US Horizontal (Mirrored)". (For "US Horizontal", the
other landscape mounting, it serves the `eu` TFTs as they are.) This is the EU layout turned the
other way: the same TFT with display orientation 90 instead of 270. The
orientation is one byte in the TFT's first header, so `make_tfts.sh eu` writes these from the EU
TFTs, changing only that byte and the checksums. There is no HMI to compile; `ROTATED` in
`tft_build/themes.py` lists such layouts.

Upstream compiled these in the Editor, and those TFTs differ from the EU ones in exactly that byte
and its checksums; `tests/test_tft.py` checks that.
