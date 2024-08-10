#!/bin/bash
DEST_DIR_MQTTMANAGER=/MQTTManager/include/protobuf/
DEST_DIR_DJANGO=/usr/src/app/nspanelmanager/web/protobuf/
SRC_FILES=('mqttmanager.proto' 'general.proto')

for SRC_FILE in ${SRC_FILES[@]}; do
    echo "==> Creating destination directory $DEST_DIR_MQTTMANAGER"
    mkdir -p "$DEST_DIR_MQTTMANAGER"
    echo "==> Building $SRC_FILE for MQTTManager to $DEST_DIR_MQTTMANAGER"
    protoc --cpp_out="$DEST_DIR_MQTTMANAGER" "$SRC_FILE"

    echo "==> Creating destination directory $DEST_DIR_DJANGO"
    mkdir -p "$DEST_DIR_DJANGO"
    echo "==> Building $SRC_FILE for Django to $DEST_DIR_DJANGO"
    protoc --python_out="$DEST_DIR_DJANGO" "$SRC_FILE"
done
