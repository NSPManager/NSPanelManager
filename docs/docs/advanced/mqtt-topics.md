---
title: MQTT Topics
sidebar_position: 10
---
import Centered from '@site/src/components/Centered';

# MQTT Topics

Below table is a description of all MQTT topics that might be of use by a user. Replace `<panel_name>` with the friendly
name of your NSPanel:

| Topic                                         | Payload             | Description                                                                                                                                                                                             |
|-----------------------------------------------|---------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| nspanel/\<panel_name\>/screen_cmd             | 1 or 0              | Send a 1 or 0 to turn on/off the display.                                                                                                                                                               |
| nspanel/\<panel_name\>/screen_state           | 1 or 0              | Current state of the screen.                                                                                                                                                                            |
| nspanel/\<panel_name\>/brightness             | 1 to 100            | Control the brightness of the screen.                                                                                                                                                                   |
| nspanel/\<panel_name\>/brightness_screensaver | 0 to 100            | Control the brightness of the screensaver.                                                                                                                                                              |
| nspanel/\<panel_name\>/r1_cmd                 | 1 or 0              | Send a 1 or 0 to turn on/off relay 1.                                                                                                                                                                   |
| nspanel/\<panel_name\>/r1_state               | 1 or 0              | The current state of relay 1.                                                                                                                                                                           |
| nspanel/\<panel_name\>/r2_cmd                 | 1 or 0              | Send a 1 or 0 to turn on/off relay 2.                                                                                                                                                                   |
| nspanel/\<panel_name\>/r2_state               | 1 or 0              | The current state of relay 2.                                                                                                                                                                           |
| nspanel/\<panel_name\>/temperature_state      | Current temperature | The current temperature reading.                                                                                                                                                                        |
| nspanel/\<panel_name\>/screensaver_mode       | screensaver mode    | Select what screensaver to display <br/> Choose from the following: <br/>- with_background<br/>-without_background<br/>-datetime_with_background<br/>-datetime_without_background<br/>or no_screensaver |
| nspanel/\<panel_name\>/log                    | Log message         | The panel will send live logs on this topic.                                                                                                                                                            |

There are more topics that are used internally, these are:


| Topic                                                | Payload                    | Description                                                                                             |
|------------------------------------------------------|----------------------------|---------------------------------------------------------------------------------------------------------|
| nspanel/entities/\<type\>/\<id\>/state_\<attribute\> | The value of the attribute | An update of entity state value sent out by MQTTManager. Example:nspanel/entities/light/42/state_kelvin |
| nspanel/status/time                                  | Time as a string           | Current time sent by MQTTManager.                                                                       |
| nspanel/status/weather                               | JSON                       | A JSON representation of the current weather and weather forecast.                                      |
| nspanel/\<panel_name\>/status_report                 | JSON                       | JSON payload with current state of the panel.                                                           |
| nspanel/\<panel_name\>/status                        | JSON                       | JSON payload with current online/offline state of the panel.                                            |
| nspanel/\<panel_name\>/command                       | JSON                       | JSON payload with a command for the panel to execute.                                                   |
| nspanel/mqttmanager/command                          | JSON                       | JSON payload from panel with a command for MQTTManager to perform.                                      |

