#!/bin/bash
# Regenerate the committed C++, Python and firmware C protobuf code from the .proto files in this
# directory. Runs compile_all.sh in a throwaway container, so the only requirement is Docker. CI's
# protobuf check runs this same script, so its output is exactly what CI expects.
#
#   docker/protobuf/regenerate.sh
set -euo pipefail

REPO_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)

# Ubuntu 24.04's protobuf-c-compiler is 1.4.1; install_protobuf.sh installs protoc 27.0.
docker run --rm \
    -v "$REPO_ROOT":/repo \
    -e HOST_UID="$(id -u)" -e HOST_GID="$(id -g)" \
    -w /repo/docker/protobuf \
    ubuntu:24.04 \
    bash -euo pipefail -c '
        apt-get update -qq
        DEBIAN_FRONTEND=noninteractive apt-get install -y -qq --no-install-recommends \
            protobuf-c-compiler curl unzip ca-certificates >/dev/null
        (cd /tmp && bash /repo/docker/protobuf/install_protobuf.sh >/dev/null)
        protoc --version
        # -e: compile_all.sh carries on past a failed protoc, which would leave the old files in place.
        DEST_DIR_MQTTMANAGER=/repo/docker/MQTTManager/include/protobuf/ \
        DEST_DIR_DJANGO=/repo/docker/web/nspanelmanager/web/protobuf/ \
        DEST_DIR_FIRMWARE=/repo/protobuf_firmware/ \
            bash -e compile_all.sh
        chown -R "$HOST_UID:$HOST_GID" \
            /repo/docker/MQTTManager/include/protobuf \
            /repo/docker/web/nspanelmanager/web/protobuf \
            /repo/protobuf_firmware
    '
