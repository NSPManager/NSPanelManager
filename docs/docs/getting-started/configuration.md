---
title: Configure NSPanel Manager
sidebar_position: 6
---
import CenteredImage from '@site/src/components/CenteredImage';

# Configure NSPanel Manager

## Access NSPanel Manager web interface

For docker users, simply connect to: `http://your-server-ip:8000`

For Home Assistant users having installed the [Home Assistant Add-on](./install/installation-guide-home-assistant), TODO: document

## Initial setup

When first logging into the NSPanel Manager web interface you will be greeted by a configuration wizard:

<CenteredImage src="/images/doc/initial_setup_popup.png" alt="Configuration wizard" figureNumber={1} />

1. Click **next**
2. Fill up requested information

This guide will help you set up the required items in order to have a functional setup.

The following is a walkthrough of what to enter:
* **Manager config**
    * **Manager address** - This address is sent to NSPanel over MQTT when the request to connect with a manager.
      This can be loaded from the URL using the "Load from URL" button. This address needs to be reachable by all
      NSPanels.
    * **Manager port** - This port that will be used to connect to the address above.

* **MQTT config**
    * **MQTT address** - The address to your MQTT broker. If you are running your MQTT broker as an addon to
      Home Assistant, enter you Home Assistant address (only IP).
    * **MQTT port** - The port to connect to your MQTT broker.
    * **MQTT username** - The username to authenticate to your MQTT broker. Leave empty if you don´t use authentication.
    * **MQTT password** - The password to authenticate to your MQTT broker. Leave empty if you don´t use authentication.

* **Home Assistant config (optional)**
    * **Home Assistant address** - The address to your Home Assistant instance. This address should include "http"
      or "https" in the beginning and ":8123" (change to your port) at the end.
    * **Home Assistant token** - The long lived access token used to authenticate to Home Assistant.

* **OpenHAB (optional)**
    * **OpenHAB address** - The address to your OpenHAB instance. This address should include "http" or "https" in
      the beginning and ":8123" (change to your port) at the end.
    * **OpenHAB token** - The access token used to authenticate to OpenHAB.

3. Congrats you have finished the initial setup and you can now use NSPanel Manager.
4. If you are willing to learn more about the Web interface, please read [this](./../web-interface).