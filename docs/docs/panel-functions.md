---
title: Panel functions
sidebar_position: 4
---
import CenteredImage from '@site/src/components/CenteredImage';

# Panel functions

## Main page

<CenteredImage src="/images/doc/main_page_numbers.png" alt="Main page" figureNumber="9" />

1. Ceiling Lights button
   **Short press** - Toggle Ceiling lights ON/OFF.
   **Long press** - Enter lock mode, sliders will now only effect ceiling lights. Short press to exit lock mode.
2. Table Lights button
   **Short press** - Toggle Table lights ON/OFF
   **Long press** - Enter lock mode, sliders will now only effect table lights. Short press to exit lock mode.
3. Brightness Dimmer
   Control over Ceiling and Table Lights group. (More under Lights control logic below)
4. Kelvin Dimmer
   Control over Ceiling and Table Lights group. (More under Lights control logic below)
5. Room toggle button
   **Short press** - Change room.
6. Room button
   **Short press** - Enter room page for individual device control.
7. Lights mode button
   **Short press** - Toggle between Room Lights mode and All Lights mode
8. Scenes button.
   **Short press** - Enter Scenes page. If 7 is in Room Lights mode user will enter Room Scenes. If 7 is in All Lights 
   mode user will enter Global Scenes page.

### Lights control logic

The NSPanel main page might have some behavior that seems odd at first but the logic of it will be described here.
The first page will affect entities in the selected room (if in room-mode) or all the configured lights 
(if in "All lights"-mode).

The two left buttons for ceiling and table lights will always behave the same. 
Pressing a button that is "off" will turn on all the lights of that type. 
Pressing a button that is "on" will turn off all lights of that type.
The sliders will always display an average value of all entities that will be affected of changes.

There are few different scenarios:

#### One or more lights on

When changing the sliders, the changes will only be sent out to the lights currently on. 
If turning on a group of lights or individual lights they will be turned on to the current brightness of the slider. 
I.e. average dimming level in the room.

:::info
If you wish for the light to always turn on with color temperature even though you turned it off from RGB, there is a
setting in the global settings.
:::

#### No lights on

When changing the sliders, the changes will be sent out to all lights selected (depending on room or "all lights"-mode).

#### Lock mode

You can lock which light to affect by pressing and holding either the ceiling or table-lights button. 
This will enter a special mode where changes to the sliders will only affect the selected type of lights. 
By pressing the same button again you can exit the "special mode". 
The "special mode" will also time out after a few seconds.

### Scenes button (top left corner)

The settings icon in the top left corner is for entering the Scenes page. In NSPanel Manager there are both Room Scenes
and Global Scenes. If you’re in Room Lights mode (button in lower right corner) you will enter the Room Scenes page. 
If you’re in All Lights mode you will enter the Global Scenes page. You’ll also see that the settings icon changes when 
toggling between Room Lights and All Lights mode. Standard settings icon leads to Room Scenes page when pressing it. 
Settings icon with a roof on top leads to Global Scenes page.

### Swipe down

You can swipe downwards when on the first page to enter the Smart Home Control page. This page is a work in progress.
Design and functionality is not finished or decided yet.

### Smart Home Control page

Accessed by swiping down on Main page. Work in progress. Design and functionality is not finished or decided yet.

###  Scenes page

To enter Scenes page, press the settings icon in top left corner on Main page. 
Depending on if you’re in Room Lights mode or All Lights mode you will enter Room Scenes page or Global Scenes page.

Scene names that show up here are the ones you have configured in the NSPanel Manager web interface. 

To store a scene simply hold the save button for three seconds and the current values of the lights in the room you are 
in or all lights if in All Lights mode will be saved. 

To activate a scene and send out those saved values you just press the scene name.

### Room page

Enter Room page by pressing the room name on Main page. 
All devices configured for that room will show up here.
To control a device individually press the device name.

### Individual Lights page

All the capabilities of the chosen light will be shown on this page. If the light is RGB capable there will be an icon 
in the top right corner to toggle between Color Temperature mode and Color mode.
