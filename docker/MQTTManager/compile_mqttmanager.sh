#!/bin/bash
echo "Starting to compile MQTTManager."

TARGETPLATFORM=""
STRIP=""

while true; do
	case "$1" in
	--target-platform)
		TARGETPLATFORM="$2"
		echo "Will compile for platform $TARGETPLATFORM"
		shift # Remove --target-platform
		shift # Remove value for target platform
		;;
	--strip)
		echo "Will strip compiled binaries and .so's"
		STRIP="1"
		shift
		;;
	*) break ;;
	esac
done

if [ -z "$TARGETPLATFORM" ]; then
	echo "No platform given as argument, will assume linux/amd64"
	TARGETPLATFORM="linux/amd64"
fi

echo "Compiling for target '$TARGETPLATFORM'"

set -e
set -x

deb_add_arch=""
strip_bin=""
cp /root/.conan2/profiles/default /root/.conan2/profiles/host
if [ "$TARGETPLATFORM" == "linux/386" ]; then
	deb_add_arch="i386"
	conan_target_arch="x86"
	strip_bin="/usr/bin/strip"
	apt -y install gcc-multilib g++-multilib
elif [ "$TARGETPLATFORM" == "linux/amd64" ]; then
	deb_add_arch="amd64"
	conan_target_arch="x86_64"
	strip_bin="/usr/bin/strip"
elif [ "$TARGETPLATFORM" == "linux/arm/v6" ]; then
	deb_add_arch="armhf"
	conan_target_arch="armv6"
	strip_bin="/usr/bin/arm-linux-gnueabihf-strip"
	apt -y install binutils-arm-linux-gnueabihf gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
	echo "" >>/root/.conan2/profiles/host
	echo "[buildenv]" >>/root/.conan2/profiles/host
	echo "CC=arm-linux-gnueabihf-gcc-12" >>/root/.conan2/profiles/host
	echo "CXX=arm-linux-gnueabihf-g++-12" >>/root/.conan2/profiles/host
	echo "LD=arm-linux-gnueabihf-ld" >>/root/.conan2/profiles/host
elif [ "$TARGETPLATFORM" == "linux/arm/v7" ]; then
	deb_add_arch="armhf"
	conan_target_arch="armv7hf"
	strip_bin="/usr/bin/arm-linux-gnueabihf-strip"
	apt -y install binutils-arm-linux-gnueabihf gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
	echo "" >>/root/.conan2/profiles/host
	echo "[buildenv]" >>/root/.conan2/profiles/host
	echo "CC=arm-linux-gnueabihf-gcc-12" >>/root/.conan2/profiles/host
	echo "CXX=arm-linux-gnueabihf-g++-12" >>/root/.conan2/profiles/host
	echo "LD=arm-linux-gnueabihf-ld" >>/root/.conan2/profiles/host
elif [ "$TARGETPLATFORM" == "linux/arm64" ]; then
	deb_add_arch="arm64"
	conan_target_arch="armv8"
	strip_bin="/usr/bin/aarch64-linux-gnu-strip"
	apt -y install binutils-aarch64-linux-gnu gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
	echo "" >>/root/.conan2/profiles/host
	echo "[buildenv]" >>/root/.conan2/profiles/host
	echo "CC=aarch64-linux-gnu-gcc-12" >>/root/.conan2/profiles/host
	echo "CXX=aarch64-linux-gnu-g++-12" >>/root/.conan2/profiles/host
	echo "LD=aarch64-linux-gnu-ld" >>/root/.conan2/profiles/host
	#echo "CFLAGS=-march=armv8-a" >>/root/.conan2/profiles/host
	#echo "CXXFLAGS=-march=armv8-a" >>/root/.conan2/profiles/host
else
	echo "ERROR !Unknown target platform. Will exit."
	exit 1
fi
eval $(dpkg-architecture) # Load in what the builder machine is

# Setup apt source file to be able to access both architectures
if [ "$deb_add_arch" != "$DEB_HOST_ARCH" ]; then
	if [ ! -f "/etc/apt/sources.list.d/debian.sources.bak" ]; then
		cp /etc/apt/sources.list.d/debian.sources /etc/apt/sources.list.d/debian.sources.bak
	fi
	cp /etc/apt/sources.list.d/debian.sources.bak /etc/apt/sources.list.d/debian.sources
	dpkg --add-architecture $deb_add_arch
	sed -i "/Components.*/a arch=$DEB_HOST_ARCH,$deb_add_arch" /etc/apt/sources.list.d/debian.sources
	apt-get update
fi

apt -y install libssl-dev:$deb_add_arch openssl:$deb_add_arch libcurl4-openssl-dev:$deb_add_arch

BASEDIR=$(dirname "$0")
pushd "$BASEDIR"
cd /MQTTManager/

# rm -rf build
#if [ -e CMakeCache.txt ]; then
#	rm CMakeCache.txt
#fi

#sed -i "s/^arch.*/arch=$build_arch/g" /root/.conan2/profiles/host
# Get build type from conan profile
BUILD_TYPE=$(grep -E "^build_type=" /root/.conan2/profiles/default | cut -d'=' -f 2)
sed -i "s/arch=.*/arch=${conan_target_arch}/g" /root/.conan2/profiles/host

echo "Conan profile: "
cat /root/.conan2/profiles/default

conan install . --build=missing -pr:b default -pr:h host
cd build
source $BUILD_TYPE/generators/conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=$BUILD_TYPE/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . --config $BUILD_TYPE -j $(nproc)
sed -i "s|/MQTTManager/|/home/tim/NSPanelManager/docker/MQTTManager/|g" compile_commands.json
cp compile_commands.json ../

if [ "$STRIP" == "1" ]; then
	echo "Stripping binaries and .so files using '$strip_bin'..."
	"$strip_bin" /MQTTManager/build/*.so
	"$strip_bin" /MQTTManager/build/nspm_mqttmanager
fi
