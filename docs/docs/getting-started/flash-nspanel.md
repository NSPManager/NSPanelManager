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

<YouTubePlayer
    videoId="VvbQcUzaS94"
    author="Cables & Coffee"
    description="Flash Sonoff NSPanel with our custom firmware"
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
3. After that you can detach the plastic cover from the PCB


#### Now you have two choices, the Lazy way or the Complicated way:
**1. Lazy way:**

You can flash straight away without dettaching the screen from the PCB and remove PCB completely from cover.
Important is that you insert the cables and tilt them abit (like in GIF below) so the pins does not come in contact with the metall behind the PCB.
Then hold tilted during entire flashing.

![gif](https://github.com/user-attachments/assets/8d2b93c9-9378-44d5-87f4-331c91b2ce76)

**2. Complicated way (dettaching screen from PCB and removing PCB from housing):**

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

In order to get the ESP32 enter in flash mode, `GPIO0`, noted `IO0` on the PCB has to be connected to `GND`. 
Either connect a cable between `IO0` and `GND` (2) as in picture below:
<CenteredImage src="/images/doc/flash/step4.png" alt="The PCB connected to the screen via FFC cable" figureNumber="5" />

or add an extra cable to Ground pin on USB to TTL as in picture below (white cable):

![extraground](https://github.com/user-attachments/assets/29a60b29-a741-403f-9c77-2b5cc5f39e1a)

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
    
    <TabItem value="ESPHOMEWEB" label="ESP Home Web (Chrome or Edge)">
      1. Go to https://web.esphome.io/ (not supported in Firefox)
      2. Connect USB to TTL to computer
      3. Press Connect (choose correct USB device)
      4. Press Install
      5. Choose file...
      5. Choose .bin file to flash
      6. Press Install
      7. Wait for flashing to complete
      
    </TabItem>  
    
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







