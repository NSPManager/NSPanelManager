#!/bin/bash

version=3.28
build=1

apt -y remove cmake # In case it is already installed
apt -y install build-essential libtool autoconf unzip wget

# Download and extract CMake
mkdir ~/temp
cd ~/temp
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
cd cmake-$version.$build/

./bootstrap
make -j$(nproc)
make install
