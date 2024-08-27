#!/bin/bash
DEST_DIR_MQTTMANAGER=/MQTTManager/include/protobuf/
DEST_DIR_DJANGO=/usr/src/app/nspanelmanager/web/protobuf/
DEST_DIR_FIRMWARE=/firmware_protobuf/
SRC_FILES=('protobuf_mqttmanager.proto' 'protobuf_general.proto' 'protobuf_formats.proto')

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

echo "==> Building protobuf_nspanel.proto for ESP32 NSPanel to $DEST_DIR_FIRMWARE"
#nanopb_generator.py "protobuf_nspanel.proto" -D "$DEST_DIR_FIRMWARE"
protoc --c_out="$DEST_DIR_FIRMWARE" "protobuf_nspanel.proto"
echo "==> Building protobuf_nspanel.proto for MQTTManager to $DEST_DIR_MQTTMANAGER"
protoc --cpp_out="$DEST_DIR_MQTTMANAGER" "protobuf_nspanel.proto"

for file in $(ls $DEST_DIR_DJANGO); do
    if [[ "$file" == *.py ]]; then
        echo "==> Changing import statements in $file"
        sed -i '/^import /s/import /import web.protobuf./g' "$DEST_DIR_DJANGO/$file"
    fi
done
