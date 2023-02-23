#!/usr/bin/env python3
import shutil
import subprocess
Import("env")

# print("Current CLI targets", COMMAND_LINE_TARGETS)
# print("Current Build targets", BUILD_TARGETS)


def pre_program_action(source, target, env):
    try:
        print("Creating version.h")
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


def post_program_action(source, target, env):
    try:
        print("Copying firmware to docker location.")
        shutil.copyfile(".pio/build/esp32dev/firmware.bin",
                        "../../docker/web/nspanelmanager/firmware.bin")
    except Exception as e:
        print("Failed to copy file!")
        print(e)
    else:
        print("Copy finished.")


env.AddPreAction("$PROGPATH", pre_program_action)
env.AddPostAction("$PROGPATH", post_program_action)
print("Running buildActions.py!")
