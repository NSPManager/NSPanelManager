---
title: Flash Sonoff NSPanel
sidebar_position: 4
---
import YouTubePlayer from '@site/src/components/YoutubePlayer';
import CenteredImage from '@site/src/components/CenteredImage';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

# Flash Sonoff NSPanel

## Video guide through

:::note
**Only the first 3 chapters of the video are relevant here**. Do not flash it with ESPHome, this is another firmware that will not work with NSPanelManager.

Instead, follow the below documentation to learn how to [flash the NSPanelManager firmware on your NSPanel](#flash-nspanel-with-nspanelmanager-custom-firmware).
:::

<YouTubePlayer
    videoId="p-AK4o5jOSI"
    author="Mark Watt Tech"
    description="Flash Sonoff NSPanel with ESPHome"
/>

## Detailed guide

### NSPanel disassembly

#### Detach the Power block

:::danger
Make sure to orient the NSPanel the same way as on the picture
It has to be oriented so you can read the text naturally. Doing it this way reduce the risk to hurt the bottom diode.
:::

<CenteredImage src="/images/doc/flash/step1.png" alt="Detach power block" figureNumber="1" />

1. Place flat screw driver behind this little metal part and lift it up
2. Same as previously

Do it softly until you are able to separate the Power block from the rest of the NSPanel. 

You should end up with two parts:

<CenteredImage src="/images/doc/flash/step1-output.png" alt="The detached Power block from the Screen enclosure" figureNumber="2" />

#### Remove plastic cover from Screen enclosure

<CenteredImage src="/images/doc/flash/step2.png" alt="The plastic cover attached to the case" figureNumber="3" />

1. Unscrew it with a cross screwdriver
2. Unscrew it with a cross screwdriver
3. After than you can detach the plastic cover from the PCB

#### Detach the screen from the PCB

:::danger
This operation has to be done very smoothly.
:::

<CenteredImage src="/images/doc/flash/step3.png" alt="The PCB connected to the screen via FFC cable" figureNumber="4" />

1. Push slightly from left to right the black part of the connector (2mm)
2. same as point 1.
3. Point 1 and 2 unlocked the FFC cable, it can now be slowly taken out from the connector.

:::info
The black part of the FFC cable do not contains any electronic nor conductive feature, you can catch it with tweezers without fear to damage it.
:::

#### Prepare for flashing

In order to get the ESP32 enter in flash mode, `GPIO0`, noted `IO0` on the PCB has to be connected to `GND`:

<CenteredImage src="/images/doc/flash/step4.png" alt="The PCB connected to the screen via FFC cable" figureNumber="5" />

1. Connect (or solder temporarily) a cable between `IO0` and `GND` (2).

:::info
Here is the pinout documented by Blackadder more in details [here](https://blakadder.com/nspanel-teardown/):
<CenteredImage src="/images/doc/flash/step4-pinout.png" alt="NSPanel pinout" figureNumber="6" />
:::

### Connect your USB to UART (TTL) converter

:::danger
Make sure to use the 3v3 (not 5V), this can generally be configured with a Jumper up on the USB to UART converter.
:::

1. Connect the USB to UART convert to the PCB:

| USB-UART PIN | NSPanel PCB PIN                         |
|--------------|-----------------------------------------|
| VCC          | VCC (Symbolized by a square on the PCB) |   
| RX           | TX                                      |   
| TX           | RX                                      |  
| GND          | GND                                     |  

:::info
Notice that RX and TX pins have to be inverted between the devices: it is not TX -> TX, RX -> RX (See upper table).
:::

### Flash NSPanel with NSPanelManager custom firmware

First, download the latest firmware `merged-flash.bin` from [here](https://github.com/NSPManager/NSPanelManager/raw/refs/heads/main/firmware/NSPanelManagerFirmware/merged-flash.bin).

Then, flash the firmware with your tool of choice:

<Tabs groupId="flashing-tool" queryString>
  <TabItem value="Espressif" label="Espressif tool (Windows)">

        2. Download Espressif tool from here: https://www.espressif.com/en/support/download/other-tools
           3. Open the tool and choose to flash an `ESP32 chip`.
           4. Click on the `...` button and select the firmware you downloaded at step 1 called `merged-flash.bin`.
           5. enter `0x0` as the upload address.
           6. Check the first checkbox next to the firmware file
           7. Click the `START` button and wait until it writes `FINISH`.

            <CenteredImage src="/images//doc/esp_flash_download_tool.png"/>

     </TabItem>
     <TabItem value="esptool" label="ESPtool">
        1. Open a terminal.
        2. Navigate to the directory where you saved the `merged_flash.bin`-file.
        3. To determine if you have selected the right port, run `esptool.py flash_id --port <port>`.
           You will have to replace `<port>` with the actual port connected to the NSPanel.
           This will do a check and see if the tool can communicate with the NSPanel.
        4. Run `esptool.py --baud 921600 --port /dev/ttyUSB0 write_flash 0x0 merged_flash.bin`.
           You will have to replace `/dev/ttyUSB0` with the actual port connected to the NSPanel.
        
        :::info
        On Windows it might be just `esptool` without the `.py` at the end.
        :::
        
        :::info
        On Windows `/dev/ttyUSB0` will have to be replaced by something like `COM4`. If using MacOS or Linux the
        port will be something similar to `/dev/ttyUSB0`.
        :::
     </TabItem>
</Tabs>







