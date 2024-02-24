#!/bin/bash

apt -y remove cmake # In case it is already installed
apt -y install build-essential libtool autoconf unzip

# Download and extract CMake
mkdir ~/temp
cd ~/temp
tar -xzf /MQTTManager/cmake-3.28.1.tar.gz
cd cmake-3.28.1/

./bootstrap
make -j$(nproc)
make install
