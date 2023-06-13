# NSPanelManager
## What is it?
This project aims to solve the issue with having multiple Sonoff NSPanels installed around the house and having to make code changes as soon as you want to make a change. The project also aims to be simple to use, work for both Home Assistant and OpenHAB and any other home automation tool that can leverage MQTT.

## The concept
The idea is that by using the same layout on all the different panels all users will quickly become accustomed to the layout and usage of the panel.

## Logging
Logging from NSPanels are done over MQTT to the topic `nspanel/<panel name>/log`. To see everything to and from all the devices, the topic `nspanel/#` can be used.

# MQTT topics
|Topic|Messages|Description|
|---|---|---|
|nspanel/panel_name/screen_cmd| 1 or 0 |Control screen state, 1 or 0|
|nspanel/panel_name/screen_state| 1 or 0 |Current screen state, 1 or 0|
|nspanel/panel_name/r1_cmd| 1 or 0 |Control relay 1 state, 1 or 0|
|nspanel/panel_name/r1_state| 1 or 0 |Current relay 1 state, 1 or 0|
|nspanel/panel_name/r2_cmd| 1 or 0 |Control relay 2 state, 1 or 0|
|nspanel/panel_name/r2_state| 1 or 0 |Current relay 2 state, 1 or 0|

# Currently working (but might require more work)
* Integration with Home Assistant and OpenHAB
* Toggle ceiling & table lights in a room separetly
* Dim lights
* Change color temperature of lights
* Special mode where control of only ceiling or table lights are activated

# Setup
## Docker container
All configuration for panels are done via a web interface running in a docker container. This container is available in the `docker` directory. Use one of the following scripts to get it running:
|Script|Explination|
|---|---|
|docker-build_and_run.sh|Will build and run the container as a user might expect. This is probably the script you want to get up and running.|
|docker-console.sh|Used to enter a already running NSPanelManager docker container.|
|docker-stop_and_rm.sh|This script will stop and remove and NSPanelManager containers.|
|register_dummy_panel.sh|Used to register a dummy panel in the web interface. This is only used for development purposes.|
|docker-build_and_run_dev.sh|Will build the container and drop you in a shell inside the container. Execute `./run.sh` to start the web interface and MQTTManager application to handle state communication with panels and Home Assistant or OpenHAB. The whole `web` directory is mapped as a volume into the container to aid in development. When exiting the container will be destroyed.|

## NSPanel
The NSPanelManager firmware is written as a PlatformIO-project and this is by far the easiest method to flash the firmware. Perform the following steps to flash you NSPanel:
* Install PlatformIO for you platform.
* Navigate to the `firmware/NSPanelManagerFirmware/` directory.
* Execute `platformio run --target upload --environment esp32dev` to upload the firmware.
* Restart the panel by removing power and applying it again.
* Execute `platformio run --target uploadfs --environment esp32dev`.
* Restart the panel by removing power and applying it again.
* Connect to the access point named `NSPMPanel` with the password `password`.
* Enter a friendly name for this panel.
* Enter the IP address and port to where the docker-container with the web-interface is running.
* Enter your Wifi SSID (Name) and password .
* Enter your MQTT address and port (enter username and password if used).
* Press the save button and the panel will restart and try to connect to the given SSID, MQTT and manager address.

For more information on how to connect to the NSPanel to flash it, see [this tutorial](https://www.youtube.com/watch?v=sCrdiCzxMOQ).
