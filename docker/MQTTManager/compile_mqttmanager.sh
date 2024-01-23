#!/bin/bash
set -e
set -x

if [ ! -z "$1" ]; then
	arch="$1"
else
	arch=$(uname -m)
fi

echo "Compiling for arch '$arch'"

cp /root/.conan2/profiles/default /root/.conan2/profiles/host
sed -i "s/^arch.*/arch=$arch/g" /root/.conan2/profiles/host

if [[ "$arch" == armv7 ]] || [[ "$arch" == armv6 ]]; then
	apt -y install binutils-arm-linux-gnueabi gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
	cat <<EOF >>/root/.conan2/profiles/host

[buildenv]
CC=arm-linux-gnueabi-gcc-12
CXX=arm-linux-gnueabi-g++-12
LD=arm-linux-gnueabi-ld
EOF
elif [[ "$arch" == armv7hf ]]; then
	apt -y install binutils-arm-linux-gnueabihf gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
	cat <<EOF >>/root/.conan2/profiles/host

[buildenv]
CC=arm-linux-gnueabihf-gcc-12
CXX=arm-linux-gnueabihf-g++-12
LD=arm-linux-gnueabihf-ld
EOF
fi

BASEDIR=$(dirname "$0")
pushd "$BASEDIR"

cd /MQTTManager/

rm -rf build
#if [ -e CMakeCache.txt ]; then
#	rm CMakeCache.txt
#fi

# Get build type from conan profile
BUILD_TYPE=$(grep -E "^build_type=" /root/.conan2/profiles/default | cut -d'=' -f 2)

conan install . --output-folder=build --build=missing --profile=host
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . --config $BUILD_TYPE
sed -i "s|/MQTTManager/|/home/tim/NSPanelManager/docker/MQTTManager/|g" compile_commands.json
cp compile_commands.json ../

cp nspm_mqttmanager /usr/src/app/
