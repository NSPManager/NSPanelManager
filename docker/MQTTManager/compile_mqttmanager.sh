!/bin/bash

set -e
set -x

BASEDIR=$(dirname "$0")
pushd "$BASEDIR"

cd /MQTTManager/

rm -rf build
if [ -e CMakeCache.txt ]; then
	rm CMakeCache.txt
fi

# Get build type from conan profile
BUILD_TYPE=$(grep -E "^build_type=" /root/.conan2/profiles/default | cut -d'=' -f 2)

conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . --config $BUILD_TYPE
sed -i "s|/MQTTManager/|/home/tim/NSPanelManager/docker/MQTTManager/|g" compile_commands.json
cp compile_commands.json ../

cp nspm_mqttmanager /usr/src/app/
