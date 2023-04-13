#!/bin/bash

pio_core_path="$(pio system info | grep "PlatformIO Core Directory" | grep -oEe "/.+$")"
boot_app0_bin_path="$pio_core_path/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"

if [ ! -e "$boot_app0_bin_path" ]; then
	echo "boot_app0.bin file does not exist as $boot_app0_bin_path!"
	echo "Have you installed the esp32 platform in PlatformIO"
	exit 1
fi

echo "Compile firmware"
platformio run --environment esp32dev

echo "Build LittleFS image"
platformio run --target buildfs --environment esp32dev

echo "Building image"
esptool.py --chip esp32 merge_bin -o merged-flash.bin --flash_mode dio --flash_size 4MB 0x1000 .pio/build/esp32dev/bootloader.bin 0x8000 .pio/build/esp32dev/partitions.bin 0xe000 "$boot_app0_bin_path" 0x10000 .pio/build/esp32dev/firmware.bin 2686976 .pio/build/esp32dev/littlefs.bin
