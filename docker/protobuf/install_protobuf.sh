#!/bin/bash
PB_REL="https://github.com/protocolbuffers/protobuf/releases"
REMOTE_FILE="protoc-27.0-linux-x86_64.zip"
curl -LO $PB_REL/download/v27.0/"$REMOTE_FILE"

unzip "$REMOTE_FILE" -d / && rm "$REMOTE_FILE"
