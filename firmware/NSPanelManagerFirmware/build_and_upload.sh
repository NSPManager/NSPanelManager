#!/bin/bash
# This script will build the .bin file and the LittleFS .bin file
# It will also upload the files to the NSPanelManager

NSPanelManager_address="127.0.0.1"
NSPanelManager_port="8000"

function update_panel_version() {
	# Update version file.
	echo "Updating version number"
	current_version="$(grep -oE "[0-9\.]+" include/nspm-bin-version.h)"
	current_major_version="$(echo $current_version | cut -d . -f 1,2)"
	current_minor_version="$(echo $current_version | cut -d . -f 3)"
	current_minor_version="$((current_minor_version+1))"
	echo "#define NSPanelManagerFirmwareVersion \"$current_major_version.$current_minor_version\"" > include/nspm-bin-version.h
}

if [ $# -ne 0 ]; then
	if [ "$1" != "--no-ver" ]; then
		update_panel_version
	else
		echo "Will not increase version number."
	fi
else
	update_panel_version
fi

# Build firmware and LittleFS
platformio run --environment esp32dev
firmware_build_result="$?"

if [ "$firmware_build_result" -ne 0 ]; then
    echo "Firmware build failed. Will not upload to NSPanelManager"
    exit 1
fi

touch littlefs.md5
current_littlefs_md5="$(cat littlefs.md5)"
new_littlefs_md5="$(find data/ -type f -exec md5sum {} \; | sort | md5sum | cut -d ' ' -f 1)"

if [ "$current_littlefs_md5" != "$new_littlefs_md5" ] || [ ! -f ".pio/build/esp32dev/littlefs.bin" ]; then
	platformio run --target buildfs --environment esp32dev

	if [ "$?" -ne 0 ]; then
		echo "--- LittleFS build failed. Will not upload to NSPanelManager ---"
		exit 2
	fi

	echo $new_littlefs_md5 > littlefs.md5
else
	echo "LittleFS has not changed. Will not build."
fi

# Upload firmware and LittleFS to NSPanelManager
curl http://"$NSPanelManager_address":"$NSPanelManager_port"/save_new_firmware -F firmware=@.pio/build/esp32dev/firmware.bin
firmware_status="$?"
curl http://"$NSPanelManager_address":"$NSPanelManager_port"/save_new_data_file -F data_file=@.pio/build/esp32dev/littlefs.bin
data_file_status="$?"

# Wait for Django to process files.
sleep 5

if [ "$firmware_status" -eq 0 ] && [ "$data_file_status" -eq 0 ]; then
	if [ -e "nspanels_to_ota.txt" ]; then
		while read nspanel; do
			echo "Calling OTA for $nspanel"
			curl -X POST http://"$nspanel"/start_ota_update &> /dev/null
			if [ "$?" -eq 0 ]; then
				echo "OTA called for $nspanel!"
			else
				echo "Failed to call OTA for $nspanel"
			fi
		done < nspanels_to_ota.txt
	else
		echo "ERROR: No 'nspanels_to_ota.txt' file, will not auto-call OTA for any panels."
		echo "Enter one IP-address per line in 'nspanels_to_ota.txt' to automatically start OTA flash when compilation and upload is complete."
	fi
else
	echo "Something went wrong during upload, will not call OTA!"
fi
