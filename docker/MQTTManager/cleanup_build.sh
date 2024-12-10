#!/bin/bash
# This script is ment to clean up and remove things that are not needed after the MQTTManager
# has been compiled. Basicly, it should remove compilation software and cache stuff.

apt -y remove build-essential

# Remove cmake which is compiled in ~/temp
rm -rf ~/temp

# Remove cached files used during building of MQTTManager
rm -rf /MQTTManager/conan_cache/
