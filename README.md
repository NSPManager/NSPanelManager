# NSPanelManager
## What is it?
This project aims to solve the issue with having multiple Sonoff NSPanels installed around the house and having to make code changes as soon as you want to make a change. The project also aims to be simple to use, work for both Home Assistant and OpenHAB and any other home automation tool that can leverage MQTT.

## The concept
The idea is that by using the same layout on all the different panels all users will quickly become accustomed to the layout and usage of the panel.

## Logging
Logging from NSPanels are done over MQTT to the topic `nspanel/<panel name>/log`. To see everything to and from all the devices, the topic `nspanel/#` can be used.

## MQTT topics
|Topic|Messages|Description|
|---|---|---|
|nspanel/panel_name/screen_cmd| 1 or 0 |Control screen state, 1 or 0|
|nspanel/panel_name/screen_state| 1 or 0 |Current screen state, 1 or 0|
|nspanel/panel_name/r1_cmd| 1 or 0 |Control relay 1 state, 1 or 0|
|nspanel/panel_name/r1_state| 1 or 0 |Current relay 1 state, 1 or 0|
|nspanel/panel_name/r2_cmd| 1 or 0 |Control relay 2 state, 1 or 0|
|nspanel/panel_name/r2_state| 1 or 0 |Current relay 2 state, 1 or 0|
|nspanel/panel_name/temperature_state| temperature reading |Current temperature reading|

# Currently working (but might require more work)
* Integration with Home Assistant and OpenHAB
* Toggle ceiling & table lights in a room separetly
* Dim lights
* Change color temperature of lights
* Special mode where control of only ceiling or table lights are activated

# Setup

## 1. Docker container
### As a standalone container
All configuration for panels are done via a web interface running in a docker container. This container is available in the `docker` directory. Use one of the following scripts to get it running:
|Script|Explination|
|---|---|
|docker-build_and_run.sh|Will build and run the container as a user might expect. This is probably the script you want to get up and running.|
|docker-console.sh|Used to enter a already running NSPanelManager docker container.|
|docker-stop_and_rm.sh|This script will stop and remove and NSPanelManager containers.|
|register_dummy_panel.sh|Used to register a dummy panel in the web interface. This is only used for development purposes.|
|docker-build_and_run_dev.sh|Will build the container and drop you in a shell inside the container. Execute `./run.sh` to start the web interface and MQTTManager application to handle state communication with panels and Home Assistant or OpenHAB. The whole `web` directory is mapped as a volume into the container to aid in development. When exiting the container will be destroyed.|

If you are setting up the container manually using some other tool, make sure to publish port 8000 and 8001 for web interface and websocket. To set the timezone, make sure you pass though the /etc/timezone file.

To access web interface, enter IP-address and port (standard 8000) to where the container is running.
* Press settings for some initial settings:
* Insert your MQTT settings
* Insert API information to Home Assistant or Openhab
* Save
* Now we're ready to add some NSPanels!
* 
### As a Home Assistant addon (only for Home Assistant OS)
In order to install the NSPanel Manager as a Home Assistant addon at the moment, this has to be done manually. This is because this repository is private. To get up and running manually, do the following:  
* add the Samba or SSH addon to Home Assistant.
* Access the "addons"-directory in Home Assistant and create a new directory call "nspanelmanager".
* Copy all the files from the "docker"-directory in the GitHub repo to the new "nspanelmanager"-directory.
* In Home Assistant, navgiate to Settings -> Addons -> Addon store.
* In the upper right corner, press the three dots and choose "Check for update". In a few seconds the "NSPanel Manager" addon show show up under "Local add-ons". If this is not the case, refresh the page.If it still doesn't show, try restarting Home Assistant. Some users has reported that Home Assistant needs to be restarted for it to rekognize the new addon.
* Install the addon, run it up.
* * Press settings for some initial settings:
* Insert your MQTT settings
* Insert API information to Home Assistant or Openhab
* Save
* Now we're ready to add some NSPanels!

**Note:** If you change the port 8001 to something else, live update of the web interface will not work properly. This is a known bug and we are working on fixing it.

## 2. Flash firmware to NSPanel
The NSPanelManager firmware is written as a PlatformIO-project and this is by far the easiest method to flash the firmware. Perform the following steps to flash you NSPanel: (if not using PlatformIO or unable to run the below scripts you can use whatever tool you prefer to flash the panel. File to flash is: `merged-flash.bin` in the `firmware/NSPanelManagerFirmware/`)

* Install PlatformIO for you platform.
* Navigate to the `firmware/NSPanelManagerFirmware/` directory.
* Execute `./upload_image.sh` while connected to the NSPanel with serial programmer. Repeat for all panels.
* Power up one panel at a time. 
* On boot the newly flahsed panel will start a WIFI Access point called `NSPMPanel`
* Connect to the access point with the password `password`.
* On successfull connect to accesspoint, enter panels web interface on 192.168.1.1 and enter settings as follows.
* Enter a friendly name for this panel.
* Enter the IP address and port to where the docker-container with the web-interface is running.
* Enter your Wifi SSID (Name) and password.
* Enter your MQTT address and port (enter username and password if used).
* Press the save button. The panel will restart and try to connect to the given SSID, MQTT and manager address.
  
* Switch back your home WIFI to check if panel successfully connected to the manager by doing the following steps:
* Go to web interface on docker container.
* Update page.
* Panel should appear in first page. Panel is added to a dummy room if no room exists.

For more information on how to connect to the NSPanel to flash it, see [this tutorial](https://www.youtube.com/watch?v=sCrdiCzxMOQ).


## 3. Upload TFT file
Before uploading:
EU or US version tft file i chosen based on the panel settings you have chosen. There is a 'Is US panel' flag to activate on the settings pages for each panel. Do that on all your US panels before proceeding. 

Upload:
* Go to web interface
* Actions column ro the right
* Press 'Actions' button on the row of the panel you want to upload the tft file to.
* Press 'Update firmware' to start.
* Watch the magic and pray to god

The Upload button in the main menu bar in the web interface is only used if you want to upload another tft or firmware file than the one included in the current NSPanel Manager version.

## 4. Build your home and start controlling stuff!
* Go to web interface
* Rooms
* Add new room
* Enter room
* Add new light (if API connection was successfully setup pop up window should be filled with entities from your smart home)
* Choose light
* Light is automactically given a place below on the room page.
* Press room page rows to edit the order of the lights on the room page.
* New lights and changes is sent out to all panels instantly.
* Try controling your newly added lights through the NSPanel. Hope it works!

# Further questions or discussion?
Head over to our [Discord](https://discord.gg/RwXvAH56fE)!


# Web interface
## Navigation
|Path|Description|
|----|----|
|NSPanels|The default page so see status and information about all your NSPanels registered to this controller. By pressing the name of the NSPanel you will navgiate to more in depth information about the panel, live logs and also panel specific settings that you may wish to apply. To set a panel to a US version, use this page.|
|Rooms|Here you can manage rooms. By pressing "Rooms" you will get a list of all available rooms.|
|Upload|If given a new firmware, data file or GUI file, this is where to upload them. They will usally be bundled with the container though.|
|Settings|Global settings that apply to everything|

### Room
When on the "Room settings" page, you can change the name of the room and manage all entities assigned to that room.  
**Scenes**  
Here you can create, edit (rename) and delete scenes assigned to the room. Saving and recalling scenes are done though the panel.  
**Lights**  
Here you can assign, edit, and remove lights assigned to the room. Each light is of type Ceiling or Table, have certain capabilities and a friendly name.  
**Individual light control**  
The *Picture* shows a preview of what the panel will look like when on the "Individual light control"-page. Here you can assign up to 12 lights. Each light will get assigned to the first free slot on the page when assigned to the room. To manually assign a light to a slot, press the slot on the picture. Each light may only be assigned once, so when all lights assigned to this room has been assigned a place, the list will be empty. By pressing the *clear*-button you may remove a light from the indivivudal light control page but still have it assigned to the room and controlled by the sliders and button on the front page.

