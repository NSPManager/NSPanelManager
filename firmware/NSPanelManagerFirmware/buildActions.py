#!/usr/bin/env python3

import shutil
import subprocess

print("Running buildActions.py!")

# Update version file with current version
try:
    print("Creating nspm-bin-version.h")
    current_version_hash_short = subprocess.check_output(
        ['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

    print("Writing version " + current_version_hash_short +
          " to file include/nspm-bin-version.h")

    with open("include/nspm-bin-version.h", "w") as f:
        f.write("#define NSPanelManagerFirmwareVersion \"" +
                current_version_hash_short + "\"")
except Exception as e:
    print("Failed to copy file!")
    print(e)
else:
    print("Copy finished.")

# Copy firmware to docker location
try:
    print("Copying firmware to docker location.")
    shutil.copyfile(".pio/build/esp32dev/firmware.bin",
                    "../../docker/web/nspanelmanager/firmware.bin")
except Exception as e:
    print("Failed to copy file!")
    print(e)
else:
    print("Copy finished.")

# Copy LittleFS file to docker location
try:
    print("Copying data file to docker location.")
    shutil.copyfile(".pio/build/esp32dev/littlefs.bin",
                    "../../docker/web/nspanelmanager/data_file.bin")
except Exception as e:
    print("Failed to copy file!")
    print(e)
else:
    print("Copy finished.")
