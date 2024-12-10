#!/bin/bash

echo "Trying to flash to /dev/ttyUSB0"
esptool.py --baud 921600 --port /dev/ttyUSB1 write_flash 0x0 merged-flash.bin
