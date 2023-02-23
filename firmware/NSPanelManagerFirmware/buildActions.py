#!/usr/bin/env python3
import shutil
Import("env")

# print("Current CLI targets", COMMAND_LINE_TARGETS)
# print("Current Build targets", BUILD_TARGETS)


def copyFirmwareToDocker(source, target, env):
    try:
        print("Copying firmware to docker location")
        shutil.copy(".pio/build/esp32dev/firmware.bin",
                    "../../docker/web/nspanelmanager/firmware.bin")
    except:
        print("Failed to copy file!")
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


env.AddPostAction("$PROGPATH", post_program_action)
print("Running buildActions.py!")
