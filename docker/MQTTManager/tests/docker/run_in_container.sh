#!/bin/bash
# Runs inside the test container (see ../../run_tests.sh). The source tree is mounted
# read-only at /src and copied to /MQTTManager, where CMakeLists.txt expects it; the build
# directory and Conan package cache are Docker volumes so rebuilds are incremental.
set -euo pipefail

mkdir -p /MQTTManager
cp -a /src/CMakeLists.txt /src/conanfile.py /src/include /src/src /src/tests /MQTTManager/
cd /MQTTManager

echo "--> Installing dependencies with Conan (slow the first time, cached afterwards)"
conan install . --build=missing

echo "--> Building tests"
cmake -S . -B build/Debug \
  -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DNSPM_BUILD_TESTS=ON
cmake --build build/Debug --target nspm_mqttmanager_tests -j "$(nproc)"

echo "--> Running tests"
exec build/Debug/tests/nspm_mqttmanager_tests "$@"
