---
title: Logs
sidebar_position: 5
---
import CenteredImage from '@site/src/components/CenteredImage';

# Logs

While logs are normally sent over MQTT, any logs that are created before WiFi-connection are sent out on Serial. 
If you wish to see the logs going over MQTT, you can look at the topic `nspanel/<panel name>/log`. 

If you wish to look at the logs going over serial, you can use programs like Putty. 

Connect to the NSPanel with the serial programmer as usual but don’t connect `IO0` to `GND`. 

In Putty enter your serial port in the "Serial line" box and choose baud `115200`. 

You should then be able to connect by pressing the "Open"-button.

:::info
On Windows `/dev/ttyUSB0` will have to be replaced by something like `COM4`. If using MacOS or Linux the port
will be something similar to `/dev/ttyUSB0`.
:::

Example:

<CenteredImage src="/images/doc/putty_serial.png" alt="Connecting to Serial with Putty" figureNumber={10} />