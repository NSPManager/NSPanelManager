#!/bin/bash
# Render the panel pictures (stages 1 and 2: GIMP, then dither and renumber, then the progress
# bar pictures listed in each region's progress_bars.txt) in Docker, with no X server needed.
#
#   ./make_pictures.sh            # eu and us
#   ./make_pictures.sh us         # one region
#   ./make_pictures.sh --xvfb us  # run GIMP under a virtual X server instead of gimp-console
#
# Pictures are written into the checkout (<region>/pictures_tftN/ and all_numbered/), owned by
# the invoking user. Output is also written to last_pictures_run.log.
cd "$(dirname "$0")" || exit 1

IMAGE=nspm-tft-pictures
LOG=last_pictures_run.log

XVFB=0
if [ "${1:-}" = --xvfb ]; then
  XVFB=1
  shift
fi

{
  docker build -q -t "$IMAGE" docker &&
    docker run --rm \
      --user "${SUDO_UID:-$(id -u)}:${SUDO_GID:-$(id -g)}" \
      -e NSPM_GIMP_XVFB="$XVFB" \
      -v "$PWD":/work \
      "$IMAGE" bash /work/docker/run_in_container.sh "$@"
} 2>&1 | tee "$LOG"
status=${PIPESTATUS[0]}

# When run with sudo, hand the log back to the invoking user.
if [ -n "${SUDO_UID:-}" ]; then
  chown "$SUDO_UID:$SUDO_GID" "$LOG"
fi
exit "$status"
