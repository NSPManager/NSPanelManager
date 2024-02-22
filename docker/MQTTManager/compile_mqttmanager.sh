#!/bin/bash
echo "Starting to compile MQTTManager."
set -e
set -x

if [ ! -z "$1" ]; then
	arch="$1"
else
	arch=$(uname -m)
fi

echo "Compiling for arch '$arch'"
apt -y install libssl-dev openssl libcurl4-openssl-dev

BASEDIR=$(dirname "$0")
pushd "$BASEDIR"
cd /MQTTManager/

rm -rf build
#if [ -e CMakeCache.txt ]; then
#	rm CMakeCache.txt
#fi

#sed -i "s/^arch.*/arch=$build_arch/g" /root/.conan2/profiles/host
# Get build type from conan profile
BUILD_TYPE=$(grep -E "^build_type=" /root/.conan2/profiles/default | cut -d'=' -f 2)

conan install . --build=missing
cd build
source $BUILD_TYPE/generators/conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=$BUILD_TYPE/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . --config $BUILD_TYPE
sed -i "s|/MQTTManager/|/home/tim/NSPanelManager/docker/MQTTManager/|g" compile_commands.json
cp compile_commands.json ../

cp nspm_mqttmanager /usr/src/app/
