#!/bin/bash

gdb /MQTTManager/build/nspm_mqttmanager $(pgrep -f "/nspm_mqttmanager") -ex "thread apply all bt" -ex detach -ex quit
