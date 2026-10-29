#!/bin/bash
# Build every theme's TFT (stage 3) without the Editor. Each region's output_tft1/gui.tft,
# compiled by the Editor from the HMI with the theme 1 pictures, is the template; every theme
# is built from it with its pictures from make_pictures.sh swapped in.
#   ./make_tfts.sh       # eu and us
#   ./make_tfts.sh us    # one region
# After changing the HMI, compile it in the Editor once per region into output_tft1/gui.tft.
cd "$(dirname "$0")" || exit 1
regions=("$@")
[ ${#regions[@]} -gt 0 ] || regions=(eu us)
python3 -c 'import numpy, PIL, nextion_tools' 2>/dev/null ||
  { echo "make_tfts.sh needs nextion-tools, numpy and Pillow: python3 -m pip install -r requirements.txt"; exit 1; }
status=0
for region in "${regions[@]}"; do
  echo "--> $region"
  PYTHONDONTWRITEBYTECODE=1 python3 build_tfts.py "$region" || status=1
done
exit "$status"
