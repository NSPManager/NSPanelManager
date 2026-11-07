#!/bin/bash
# Runs inside the picture container (see ../make_pictures.sh). tft_automation/ is mounted
# read-write at /work, so the rendered pictures land in the checkout like a local run.
set -euo pipefail

regions=("$@")
[ ${#regions[@]} -gt 0 ] || regions=(eu us)

export PYTHONDONTWRITEBYTECODE=1

# GIMP only finds functions registered with script-fu-register in its scripts folder.
export HOME=/tmp/home
# The profile is new every run; GIMP saves its built-in data here on quit and complains if
# the folders are missing.
for dir in brushes dynamics gradients palettes patterns tool-presets; do
  mkdir -p "$HOME/.config/GIMP/2.10/$dir"
done
# Without a tag cache GIMP reports "Failed to parse tag cache" on start.
printf '<?xml version="1.0" encoding="UTF-8"?>\n<tags>\n</tags>\n' >"$HOME/.config/GIMP/2.10/tags.xml"
ln -sfn /work/gimp_scripts "$HOME/.config/GIMP/2.10/scripts"

echo "--> $(gimp-console-2.10 --version), xvfb=${NSPM_GIMP_XVFB:-0}"

status=0
for region in "${regions[@]}"; do
  cd "/work/$region"
  # create_pictures_*.sh dithers into dithered_numbered/, which git can't keep while empty.
  for t in 1 2 3 4; do mkdir -p "pictures_tft$t/dithered_numbered" "pictures_tft$t/all_numbered"; done

  marker=$(mktemp)
  echo "--> Rendering $region"
  bash "./create_pictures_$region.sh"

  # The script runs GIMP in the background and ignores its exit status, so check that every
  # picture the Script-Fu saves was written by this run.
  for t in 1 2 3 4; do
    expected=$(grep -o "pictures_tft$t/[^\"]*\.bmp" "/work/gimp_scripts/nspanel-$region-tft$t.scm" | sort -u | wc -l)
    written=$(find "pictures_tft$t" -maxdepth 1 -name '*.bmp' -newer "$marker" | wc -l)
    numbered=$(find "pictures_tft$t/all_numbered" -maxdepth 1 -name '*.bmp' -newer "$marker" | wc -l)
    echo "    $region tft$t: GIMP wrote $written of $expected, $numbered dithered into all_numbered"
    [ "$written" -eq "$expected" ] && [ "$numbered" -eq "$expected" ] || status=1
  done
  gimp_last=$(find pictures_tft1/all_numbered -maxdepth 1 -name '*.bmp' -newer "$marker" -printf '%f\n' |
    sed 's/\..*//' | sort -n | tail -1)
  rm -f "$marker"

  # Progress bar pictures are cut from the backgrounds GIMP just rendered, so they come after.
  python3 /work/bar_pictures.py "$region" --numbered-after "$gimp_last" | sed 's/^/    /' || status=1
  # The AHK script imports all_numbered/ in order, so a gap would shift every later picture ID.
  python3 /work/check_build.py --pictures "$region" | sed 's/^/    /' || status=1
done

if [ "$status" -ne 0 ]; then
  echo "--> FAILED: see the output above"
else
  echo "--> Done. Compare with the committed pictures: git status docker/HMI_files/tft_automation"
fi
exit "$status"
