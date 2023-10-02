!/bin/bash

set -e
set -x

BASEDIR=$(dirname "$0")
pushd "$BASEDIR"

cd /MQTTManager/
echo $PWD

rm -rf build
if [ -e CMakeCache.txt ]; then
	rm CMakeCache.txt
fi

export CONAN_USER_HOME="/test/"

conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
sed -i "s|/usr/src/app/|/home/tim/NSPanelManager/docker/MQTTManager/|g" compile_commands.json
cp compile_commands.json ../

cp nspm_mqttmanager /usr/src/app/
