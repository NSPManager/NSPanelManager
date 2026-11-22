#!/bin/bash
# Build every theme's TFT without the Editor, from one TFT the Editor compiled.
#   ./make_tfts.sh REGION path/to/gui.tft  # after changing REGION/gui.HMI: the Editor's compile of it
#   ./make_tfts.sh [REGION]                # only pictures changed: rebuild from the committed TFTs
# Writes REGION/output_tft1..4/gui.tft, each the compile with that theme's pictures from
# make_pictures.sh swapped in. Building eu also writes us_horizontal_mirrored.
template=()
if [ $# -eq 2 ]; then
  [ -f "$2" ] || { echo "$2: no such file"; exit 1; }
  template=(--template "$(realpath "$2")")
  set -- "$1"
elif [ $# -gt 2 ]; then
  echo "usage: $0 REGION path/to/gui.tft | $0 [REGION]"; exit 1
fi
cd "$(dirname "$0")" || exit 1
regions=("$@")
[ ${#regions[@]} -gt 0 ] || regions=(eu us)
python3 -c 'import numpy, PIL, nextion_tools' 2>/dev/null ||
  { echo "make_tfts.sh needs nextion-tools, numpy and Pillow: python3 -m pip install -r requirements.txt"; exit 1; }
status=0
for region in "${regions[@]}"; do
  echo "--> $region"
  PYTHONDONTWRITEBYTECODE=1 python3 build_tfts.py "$region" "${template[@]}" || status=1
done
exit "$status"
