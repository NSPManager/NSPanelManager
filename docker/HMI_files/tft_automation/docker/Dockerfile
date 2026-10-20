# Environment for rendering the panel pictures (create_pictures_{eu,us}.sh) without a display.
# Pinned to Ubuntu 24.04 for two versions:
# - GIMP 2.10: the Script-Fu in gimp_scripts/ uses 2.10 procedures (gimp-image-get-active-layer,
#   the five-argument gimp-file-save) that GIMP 3 removed.
# - ImageMagick 6.9.12, which made the committed pictures. Their BMP headers carry colour space
#   endpoints that differ between versions: 6.9.11 (Debian bookworm) copies GIMP's zero endpoints
#   and writes 1.0 into three of them, so every picture would differ from git in three bytes.
FROM ubuntu:24.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends gimp imagemagick xvfb xauth \
       python3 python3-numpy python3-pil \
    && rm -rf /var/lib/apt/lists/*

COPY gimp /usr/local/bin/gimp
