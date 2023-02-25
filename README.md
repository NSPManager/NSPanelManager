# NSPanelManager
## What is it?
This project aims to solve the issue with having multiple Sonoff NSPanels installed around the house and having to make code changes as soon as you want to make a change. The project also aims to be simple to use, work for both Home Assistant and OpenHAB and any other home automation tool that can leverage MQTT.

## The concept
The idea is that by using the same layout on all the different panels all users will quickly become accustomed to the layout and usage of the panel.

## Logging
Logging from NSPanels are done over MQTT to the topic `nspanel/<panel name>/log`. To everything to and from all devices, the topic `nspanel/#` can be used.
