#!/bin/bash

set -e
set -x

BASEDIR=$(dirname "$0")
pushd "$BASEDIR"

rm -rf build
if [ -e CMakeCache.txt ]; then
	rm CMakeCache.txt
fi

if [ -e "env.sh" ]; then
	source ./env.sh # Load environment variables if manually set
fi

if [ ! -z "$TZ" ]; then
	echo "$TZ" >/etc/localtime
fi

export CONAN_USER_HOME="/test/"

conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
sed -i "s|/usr/src/app/|/home/tim/NSPanelManager/MQTTManager-CPP/|g" compile_commands.json
cp compile_commands.json ../
# Update compile commands with overrides for your specific computer in order to have clangd rekognize libraries:
./nspm_mqttmanager
